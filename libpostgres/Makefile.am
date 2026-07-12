## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpostgres.la

libpostgres_la_SOURCES =  \
	database_postgres.cpp \
	database_postgres.h \
	postgres.cpp

libpostgres_la_CXXFLAGS = @DBTOOLKIT@ \
	-I../../dbinterface \
	@POSTGRES_CXXFLAGS@

libpostgres_la_LDFLAGS = -L../dbinterface \
	-ldbinterface \
	@POSTGRES_LIBS@

