## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpropertieshandlers.la

libpropertieshandlers_la_SOURCES =  \
	propertieshandler.cpp \
	propertieshandler.h \
	fieldpropertieshandler.cpp \
	fieldpropertieshandler.h \
	propertieshandlerbase.h \
	designpropertieshandler.cpp \
	designpropertieshandler.h \
	dividerpropertieshandler.cpp \
	dividerpropertieshandler.h

libpropertieshandlers_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	-I$(top_srcdir)/libpropertypages \
	-I$(top_srcdir)/libdbwindow \
	-I$(top_srcdir)/libshapeframework \
	-I$(top_srcdir)/libfieldswindow \
	$(WX_CPPFLAGS)

libpropertieshandlers_la_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

libpropertieshandlers_la_LIBADD = \
	$(top_builddir)/libpropertypages/libpropertypages.la \
	$(WX_LIBS)
