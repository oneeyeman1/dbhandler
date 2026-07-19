## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libsqlany.la

libsqlany_la_SOURCES =  \
	database_sqlany.cpp \
	database_sqlany.h \
	sacapidll.c \
	sacapidll.h \
	sacapi.h

libsqlany_la_CXXFLAGS = -I../../dbinterface \
	-DUNICODE \
	-I@SQLANY@/sdk/include

libsqlany_la_LDFLAGS =
	-L@SQLANY@/lib64 -ldbcapi
