## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libsqlany.la

libsqlany_la_SOURCES =  \
	database_sqlany.cpp \
	database_sqlany.h

libsqlany_la_CXXFLAGS = -I../../dbinterface 

libsqlany_lib_la_CXXFLAGS = -I../../dbinterface \
	-I`SQLANY17/sdk/include`

libsqlany_lib_la_LDFLAGS =
	-L`SQLANY17/lib64` -llibdbcapi
