# Install script for directory: C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/FreeTDS")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/bkpublic.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/cspublic.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/cstypes.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/ctpublic.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/sqldb.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/sqlfront.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/sybdb.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/sybfront.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/include/syberror.h"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/include/tds_sysdep_public.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/freetds" TYPE FILE FILES "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/include/freetds/version.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/FreeTDS/etc/freetds.conf;C:/Program Files (x86)/FreeTDS/etc/locales.conf;C:/Program Files (x86)/FreeTDS/etc/pool.conf")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/FreeTDS/etc" TYPE FILE FILES
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/freetds.conf"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/locales.conf"
    "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/pool/pool.conf"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/utils/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/replacements/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/tds/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/ctlib/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/dblib/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/odbc/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/server/cmake_install.cmake")
  include("C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/pool/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
