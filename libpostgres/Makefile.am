## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpostgres_lib.la

libpostgres_lib_la_SOURCES =  \
	database_postgres.cpp \
	database_postgres.h

libpostgres_lib_la_CXXFLAGS = -I../../dbinterface 

libpostgres_lib_la_LDFLAGS = -L../dbinterface \
	-ldbinterface 

AM_CXXFLAGS = -I../../dbinterface 
