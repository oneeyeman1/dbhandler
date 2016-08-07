## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libodbc_lib.la

libodbc_lib_la_SOURCES =  \
	database_odbc.cpp \
	database_odbc.h

libodbc_lib_la_CXXFLAGS = -I../../dbinterface \
	-DUNICODE 

libodbc_lib_la_LDFLAGS = -L../dbinterface \
	-ldbinterface \
	-lodbc \
	-lodbcinst 
