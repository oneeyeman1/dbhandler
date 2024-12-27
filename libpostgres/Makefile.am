## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpostgres.la

libpostgres_la_SOURCES =  \
	database_postgres.cpp \
	database_postgres.h \
	postgres.cpp

libpostgres_la_CXXFLAGS = @DBTOOLKIT@ \
	-I../../dbinterface \
	-I`pg_config \
	--includedir` 

libpostgres_la_LDFLAGS = -L../dbinterface \
	-ldbinterface \
	-L`pg_config \
	--libdir` \
	-lpq 
