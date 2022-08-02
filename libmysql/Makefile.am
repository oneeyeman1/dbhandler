## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libmysql_lib.la

libmysql_lib_la_SOURCES = \
	database_mysql.cpp \
	database_mysql.h

libmysql_lib_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	$(MYSQL_CFLAGS)

libmysql_lib_la_CXXFLAGS = \
	-DUNICODE

libmysql_lib_la_LIBADD = \
	$(MYSQL_LIBS)
