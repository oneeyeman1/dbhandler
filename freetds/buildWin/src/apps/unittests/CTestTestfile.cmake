# CMake generated Testfile for 
# Source directory: C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests
# Build directory: C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(a_defncopy "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests/Debug/defncopy.exe")
  set_tests_properties(a_defncopy PROPERTIES  WORKING_DIRECTORY "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests" _BACKTRACE_TRIPLES "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;7;add_test;C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(a_defncopy "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests/Release/defncopy.exe")
  set_tests_properties(a_defncopy PROPERTIES  WORKING_DIRECTORY "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests" _BACKTRACE_TRIPLES "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;7;add_test;C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(a_defncopy "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests/MinSizeRel/defncopy.exe")
  set_tests_properties(a_defncopy PROPERTIES  WORKING_DIRECTORY "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests" _BACKTRACE_TRIPLES "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;7;add_test;C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(a_defncopy "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests/RelWithDebInfo/defncopy.exe")
  set_tests_properties(a_defncopy PROPERTIES  WORKING_DIRECTORY "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/buildWin/src/apps/unittests" _BACKTRACE_TRIPLES "C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;7;add_test;C:/Users/Igor/OneDrive/Documents/dbhandler_app/freetds/src/apps/unittests/CMakeLists.txt;0;")
else()
  add_test(a_defncopy NOT_AVAILABLE)
endif()
