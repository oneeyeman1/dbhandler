## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libmysql_lib.la

libmysql_lib_la_SOURCES =  \
	database_mysql.cpp \
	database_mysql.h

libmysql_lib_la_CXXFLAGS = `mysql_config \
	--cflags` \
	-I../../dbinterface \
	-DUNICODE \
	@DBTOOLKIT@

libmysql_lib_la_LDFLAGS = `mysql_config \
	--libs` 

AM_CXXFLAGS = -I../../mysql-connector 
