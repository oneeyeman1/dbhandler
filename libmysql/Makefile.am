## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libmysql_lib.la

libmysql_lib_la_SOURCES =  \
	database_mysql.cpp \
	database_mysql.h

libmysql_lib_la_CXXFLAGS = -I../../mysql-connector/include \
	-I../../dbinterface \
	-DUNICODE 

AM_CXXFLAGS = -I../../mysql-connector 
