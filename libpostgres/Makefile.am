## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpostgres.la

libpostgres_la_SOURCES =  \
	database_postgres.cpp \
	database_postgres.h \
	postgres.cpp

libpostgres_la_CXXFLAGS = -D__WXGTK__ \
	-I$(top_srcdir)/dbinterface \
	-I`pg_config \
	--includedir` 

libpostgres_la_LDFLAGS = \
	$(top_builddir)/dbinterface/libdbinterface.la \
	-L`pg_config \
	--libdir` \
	-lpq 
