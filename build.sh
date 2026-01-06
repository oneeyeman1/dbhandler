export CFLAGS="-O2 -fsanitize=address"
export LDFLAGS="-fsanitize=address"
rm -rf Debug
autoreconf -i
mkdir Debug
cd Debug
../configure --enable-maintainer-mode 'CFLAGS=-g -O0' 'CXXFLAGS=-g -O0' 'JFLAGS=-g -O0' 'FFLAGS=-g -O0' --with-wx-config
make V=1 -j3
