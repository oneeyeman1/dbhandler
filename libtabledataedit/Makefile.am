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

libtableedit_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	$(WX_CPPFLAGS)

libtableedit_la_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

libtableedit_la_LIBADD = \
	$(WX_LIBS)
