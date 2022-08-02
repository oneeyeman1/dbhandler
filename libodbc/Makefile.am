## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libodbc_lib.la

libodbc_lib_la_SOURCES =  \
	database_odbc.cpp \
	database_odbc.h

libodbc_lib_la_CXXFLAGS = \
	-I$(top_srcdir)/dbinterface \
	-DUNICODE \
	-DUNIXODBC \
	@ODBC_CFLAGS@ @ODBCINST_CFLAGS@

libodbc_lib_la_LIBADD = \
	$(top_builddir)/dbinterface/libdbinterface.la \
	@ODBC_LIBS@ @ODBCINST_LIBS@
