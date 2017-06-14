# Install script for directory: /home/igor/mysql-connector/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/mysql")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/igor/mysql-connector/include/mysql.h"
    "/home/igor/mysql-connector/include/mysql_com.h"
    "/home/igor/mysql-connector/include/my_command.h"
    "/home/igor/mysql-connector/include/mysql_time.h"
    "/home/igor/mysql-connector/include/my_list.h"
    "/home/igor/mysql-connector/include/my_alloc.h"
    "/home/igor/mysql-connector/include/typelib.h"
    "/home/igor/mysql-connector/include/binary_log_types.h"
    "/home/igor/mysql-connector/include/my_dbug.h"
    "/home/igor/mysql-connector/include/m_string.h"
    "/home/igor/mysql-connector/include/my_sys.h"
    "/home/igor/mysql-connector/include/my_xml.h"
    "/home/igor/mysql-connector/include/mysql_embed.h"
    "/home/igor/mysql-connector/include/my_thread.h"
    "/home/igor/mysql-connector/include/my_thread_local.h"
    "/home/igor/mysql-connector/include/decimal.h"
    "/home/igor/mysql-connector/include/errmsg.h"
    "/home/igor/mysql-connector/include/my_global.h"
    "/home/igor/mysql-connector/include/my_getopt.h"
    "/home/igor/mysql-connector/include/sslopt-longopts.h"
    "/home/igor/mysql-connector/include/my_dir.h"
    "/home/igor/mysql-connector/include/sslopt-vars.h"
    "/home/igor/mysql-connector/include/sslopt-case.h"
    "/home/igor/mysql-connector/include/sql_common.h"
    "/home/igor/mysql-connector/include/keycache.h"
    "/home/igor/mysql-connector/include/m_ctype.h"
    "/home/igor/mysql-connector/include/my_compiler.h"
    "/home/igor/mysql-connector/include/mysql_com_server.h"
    "/home/igor/mysql-connector/include/my_byteorder.h"
    "/home/igor/mysql-connector/include/byte_order_generic.h"
    "/home/igor/mysql-connector/include/byte_order_generic_x86.h"
    "/home/igor/mysql-connector/include/little_endian.h"
    "/home/igor/mysql-connector/include/big_endian.h"
    "/home/igor/mysql-connector/include/thr_cond.h"
    "/home/igor/mysql-connector/include/thr_mutex.h"
    "/home/igor/mysql-connector/include/thr_rwlock.h"
    "/home/igor/mysql-connector/include/mysql_version.h"
    "/home/igor/mysql-connector/include/my_config.h"
    "/home/igor/mysql-connector/include/mysqld_ername.h"
    "/home/igor/mysql-connector/include/mysqld_error.h"
    "/home/igor/mysql-connector/include/sql_state.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mysql" TYPE DIRECTORY FILES "/home/igor/mysql-connector/include/mysql/" REGEX "/[^/]*\\.h$" REGEX "/psi\\_abi[^/]*$" EXCLUDE)
endif()

