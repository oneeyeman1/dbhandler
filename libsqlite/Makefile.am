## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libsqlite_lib.la

libsqlite_lib_la_SOURCES =  \
	database_sqlite.cpp \
	database_sqlite.h \
	sqlite3.c \
	sqlite3.h \
	sqlite3ext.h

libsqlite_lib_la_CXXFLAGS = -I../../dbinterface \
	@DBTOOLKIT@

libsqlite_lib_la_LDFLAGS = -L../dbinterface \
	-ldbinterface 

AM_CXXFLAGS = -I../../dbinterface 
