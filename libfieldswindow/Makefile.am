## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libfieldswindow.la

libfieldswindow_la_SOURCES =  \
	field.cpp \
	field.h \
	fieldswindow.cpp \
	fieldwindow.cpp \
	fieldwindow.h

libfieldswindow_la_CXXFLAGS = -I../../libshapeframework \
	@WX_CXXFLAGS@

libfieldswindow_la_LDFLAGS = -L../libshapeframework \
	-lshapeframework \
	@WX_LIBS@
