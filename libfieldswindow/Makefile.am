## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libfieldswindow.la

libfieldswindow_la_SOURCES =  \
	field.cpp \
	field.h \
	fieldswindow.cpp \
	fieldwindow.cpp \
	fieldwindow.h

libfieldswindow_la_CXXFLAGS = \
	$(WX_CXXFLAGS)

libfieldswindow_la_CPPFLAGS = \
	-I$(top_srcdir)/libshapeframework \
	$(WX_CPPFLAGS)

libfieldswindow_la_LIBADD = \
	$(top_builddir)/libshapeframework/libshapeframework.la \
	$(WX_LIBS)
