## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpostgres.la

libpostgres_la_SOURCES =  \
	database_postgres.cpp \
	database_postgres.h \
	postgres.cpp

libpostgres_la_CPPFLAGS = \
	-D__WXGTK__ \
	-I$(top_srcdir)/dbinterface \
	$(LIBPQ_CFLAGS)

libpostgres_la_LIBADD = \
	$(top_builddir)/dbinterface/libdbinterface.la \
	$(LIBPQ_LIBS)
