## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libtableedit.la

libtableedit_la_SOURCES =  \
	tableeditview.h \
	tableeditview.cpp \
	tableeditdocument.cpp \
	tableeditdocument.h \
	dataedit.cpp \
	dataretriever.cpp \
	dataretriever.h \
	dbtableedit.cpp \
	dbtableedit.h \
	dataeditdoctemplate.cpp \
	dataeditdoctemplate.h \
	imagecellrenderer.cpp \
	imagecellrenderer.h

libtableedit_la_CXXFLAGS = -I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libtableedit_la_LDFLAGS = `/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
