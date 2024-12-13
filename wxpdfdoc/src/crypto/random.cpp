/*
** Name:        random.cpp
** Purpose:     Random bytes generator based on ChaCha20 cipher stream
** Copyright:   (c) 2024-2024 Ulrich Telle
** SPDX-License-Identifier: MIT
**
** The code was taken from the public domain implementation
** of the sqleet project (https://github.com/resilar/sqleet)
*/

#include "random.h"
#include <string.h>

#define ROL32(x, c) (((x) << (c)) | ((x) >> (32-(c))))
#define ROR32(x, c) (((x) >> (c)) | ((x) << (32-(c))))

#define LOAD32_LE(p)            \
  ( ((uint32_t)((p)[0]) <<  0)  \
  | ((uint32_t)((p)[1]) <<  8)  \
  | ((uint32_t)((p)[2]) << 16)  \
  | ((uint32_t)((p)[3]) << 24)  \
  )

#define STORE32_LE(p, v)        \
  (p)[0] = ((v) >>  0) & 0xFF;  \
  (p)[1] = ((v) >>  8) & 0xFF;  \
  (p)[2] = ((v) >> 16) & 0xFF;  \
  (p)[3] = ((v) >> 24) & 0xFF;

/*
 * ChaCha20 stream cipher
 */
static void
chacha20_block(uint32_t x[16])
{
  int i;
  /* Macro renamed from QR to CC20QR to avoid name clashes. */
  #define CC20QR(x, a, b, c, d)                       \
  x[a] += x[b]; x[d] ^= x[a]; x[d] = ROL32(x[d], 16); \
  x[c] += x[d]; x[b] ^= x[c]; x[b] = ROL32(x[b], 12); \
  x[a] += x[b]; x[d] ^= x[a]; x[d] = ROL32(x[d],  8); \
  x[c] += x[d]; x[b] ^= x[c]; x[b] = ROL32(x[b],  7);
  for (i = 0; i < 10; i++)
  {
    /* Column round */
    CC20QR(x, 0, 4, 8, 12)
    CC20QR(x, 1, 5, 9, 13)
    CC20QR(x, 2, 6, 10, 14)
    CC20QR(x, 3, 7, 11, 15)
    /* Diagonal round */
    CC20QR(x, 0, 5, 10, 15)
    CC20QR(x, 1, 6, 11, 12)
    CC20QR(x, 2, 7, 8, 13)
    CC20QR(x, 3, 4, 9, 14)
  }
  #undef CC20QR
}

static void
chacha20_xor(uint8_t* buffer, size_t n, const uint8_t key[32],
             const uint8_t nonce[12], uint32_t counter)
{
  size_t i;
  union {
    uint8_t bytes[64];
    uint32_t words[16];
  } block;
  uint32_t state[16];
  uint8_t* buf = buffer;

  state[ 0] = 0x61707865; /* 'expa' */
  state[ 1] = 0x3320646e; /* 'nd 3' */
  state[ 2] = 0x79622d32; /* '2-by' */
  state[ 3] = 0x6b206574; /* 'te k' */

  state[ 4] = LOAD32_LE(key +  0);
  state[ 5] = LOAD32_LE(key +  4);
  state[ 6] = LOAD32_LE(key +  8);
  state[ 7] = LOAD32_LE(key + 12);
  state[ 8] = LOAD32_LE(key + 16);
  state[ 9] = LOAD32_LE(key + 20);
  state[10] = LOAD32_LE(key + 24);
  state[11] = LOAD32_LE(key + 28);

  state[12] = counter;
  state[13] = LOAD32_LE(nonce + 0);
  state[14] = LOAD32_LE(nonce + 4);
  state[15] = LOAD32_LE(nonce + 8);

  while (n > 64)
  {
    for (i = 0; i < 16; ++i)
    {
      block.words[i] = state[i];
    }
    chacha20_block(block.words);
    for (i = 0; i < 16; ++i)
    {
      block.words[i] += state[i];
      block.words[i] ^= LOAD32_LE(buf);
      STORE32_LE(buf, block.words[i]);
      buf += 4;
    }
    ++state[12];
    n -= 64;
  }

  for (i = 0; i < 16; ++i)
  {
    block.words[i] = state[i];
  }
  chacha20_block(state);
  for (i = 0; i < 16; ++i)
  {
    state[i] += block.words[i];
    STORE32_LE(&block.bytes[4*i], state[i]);
  }
  for (i = 0; i < n; i++)
  {
    buf[i] ^= block.bytes[i];
  }
}

/*
 * Platform-specific entropy functions for seeding RNG
 */

#if defined(_WIN32) || defined(__CYGWIN__)

#ifndef WXPDFDOC_USE_RAND_S
#define WXPDFDOC_USE_RAND_S 1
#endif

#if WXPDFDOC_USE_RAND_S

/* Force header stdlib.h to define rand_s() */
#if !defined(_CRT_RAND_S)
#define _CRT_RAND_S
#endif
#include <stdlib.h>

/*
  Provide declaration of rand_s() for MinGW-32 (not 64).
  MinGW-32 didn't declare it prior to version 5.3.0.
*/
#if defined(__MINGW32__) && defined(__MINGW32_VERSION) && __MINGW32_VERSION < 5003000L && !defined(__MINGW64_VERSION_MAJOR)
__declspec(dllimport) int rand_s(unsigned int *);
#endif

static size_t
entropy(void* buf, size_t n)
{
  size_t totalBytes = 0;
  while (totalBytes < n)
  {
    unsigned int random32 = 0;
    size_t j = 0;

    if (rand_s(&random32))
    {
      /* rand_s failed */
      return 0;
    }

    for (; (j < sizeof(random32)) && (totalBytes < n); j++, totalBytes++)
    {
      const uint8_t random8 = (uint8_t)(random32 >> (j * 8));
      ((uint8_t*) buf)[totalBytes] = random8;
    }
  }
  return n;
}

#else
  
#include <windows.h>
#define RtlGenRandom SystemFunction036
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);
#pragma comment(lib, "advapi32.lib")
static size_t entropy(void* buf, size_t n)
{
  return RtlGenRandom(buf, (ULONG) n) ? n : 0;
}

#endif

#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__) || defined(__QNX__)

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/ioctl.h>
/* musl does not have <linux/random.h> so let's define RNDGETENTCNT here */
#ifndef RNDGETENTCNT
#define RNDGETENTCNT _IOR('R', 0x00, int)
#endif
#endif

/* Returns the number of urandom bytes read (either 0 or n) */
static size_t
read_urandom(void* buf, size_t n)
{
  size_t i;
  ssize_t ret;
  int fd, count;
  struct stat st;
  int errnold = errno;

  do
  {
    fd = open("/dev/urandom", O_RDONLY, 0);
  }
  while (fd == -1 && errno == EINTR);
  if (fd == -1)
    goto fail;
  fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);

  /* Check the sanity of the device node */
  if (fstat(fd, &st) == -1 || !S_ISCHR(st.st_mode)
                         #ifdef __linux__
                           || ioctl(fd, RNDGETENTCNT, &count) == -1
                         #endif
     )
  {
    close(fd);
    goto fail;
  }

  /* Read bytes */
  for (i = 0; i < n; i += ret)
  {
    while ((ret = read(fd, (char *)buf + i, n - i)) == -1)
    {
      if (errno != EAGAIN && errno != EINTR)
      {
        close(fd);
        goto fail;
      }
    }
  }
  close(fd);

  /* Verify that the random device returned non-zero data */
  for (i = 0; i < n; i++)
  {
    if (((uint8_t*) buf)[i] != 0)
    {
      errno = errnold;
      return n;
    }
  }

  /* Tiny n may unintentionally fall through! */
fail:
  fprintf(stderr, "bad /dev/urandom RNG\n");
  abort(); /* PANIC! */
  return 0;
}

#if defined(__APPLE__)
#include <Security/SecRandom.h>
#endif

static size_t
entropy(void* buf, size_t n)
{
#if defined(__APPLE__)
  if (SecRandomCopyBytes(kSecRandomDefault, n, (uint8_t*) buf) == 0)
    return n;
#elif defined(__linux__) && defined(SYS_getrandom)
  if (syscall(SYS_getrandom, buf, n, 0) == n)
    return n;
#elif defined(SYS_getentropy)
  if (syscall(SYS_getentropy, buf, n) == 0)
    return n;
#endif
  return read_urandom(buf, n);
}

#else
# error "Secure pseudorandom number generator not implemented for this OS"
#endif

namespace wxpdfdoc {
namespace crypto {

/*
 * ChaCha20 random number generator
 */
void
RandomBytesGenerator::GetRandomBytes(uint8_t* out, size_t n)
{
  while (n > 0)
  {
    size_t m;
    if (available == 0)
    {
      if (counter == 0)
      {
        if (entropy(key, sizeof(key)) != sizeof(key))
          abort();
        if (entropy(nonce, sizeof(nonce)) != sizeof(nonce))
          abort();
      }
      chacha20_xor(buffer, sizeof(buffer), key, nonce, counter++);
      available = sizeof(buffer);
    }
    m = (available < n) ? available : n;
    memcpy(out, buffer + (sizeof(buffer) - available), m);
    out = (uint8_t*) out + m;
    available -= m;
    n -= m;
  }
}

} // namespace crypto
} // namespace wxpdfdoc
