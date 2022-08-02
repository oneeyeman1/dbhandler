## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdbloader.la

libdbloader_la_SOURCES =  \
	dbloader.cpp

libdbloader_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	-I$(top_srcdir)/libsqlite \
	-I$(top_srcdir)/libodbc \
	-I$(top_srcdir)/libpostgres \
	-I$(top_srcdir)/libmysql \
	$(ODBC_CFLAGS) \
	$(ODBCINST_CFLAGS) \
	$(LIBPQ_CFLAGS) \
	$(MYSQL_CFLAGS) \
	$(WX_CPPFLAGS)

libdbloader_la_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

libdbloader_la_LIBADD = \
	$(top_builddir)/dbinterface/libdbinterface.la \
	$(top_builddir)/libsqlite/libsqlite_lib.la \
	$(top_builddir)/libodbc/libodbc_lib.la \
	$(top_builddir)/libpostgres/libpostgres.la \
	$(top_builddir)/libmysql/libmysql_lib.la \
	$(ODBC_LIBS) \
	$(ODBCINST_LIBS) \
	$(LIBPQ_LIBS) \
	$(MYSQL_LIBS) \
	$(WX_LIBS)
