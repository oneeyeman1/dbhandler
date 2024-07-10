## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libtableedit.la

libtableedit_la_SOURCES =  \
	tableeditview.h \
	tableeditview.cpp \
	tableeditdocument.cpp \
	tableeditdocument.h \
	dataedit.cpp \
	dataeditdoctemplate.cpp \
	dataeditdoctemplate.h \
	imagecellrenderer.cpp \
	imagecellrenderer.h \
	tableattributes.cpp \
	tableattributes.h

libtableedit_la_CXXFLAGS = `wx-config \
	--cxxflags` \
	-I../../dbinterface \
	-I../../libhelpers 

libtableedit_la_LDFLAGS = `wx-config \
	--libs` \
	-L../libhelpers \
	-lhelpers 
