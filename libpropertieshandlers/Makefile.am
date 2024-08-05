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
	dividerpropertieshandler.h \
	databaseoptionshandler.cpp \
	databaseoptionshandler.h \
	labelpropertieshandler.cpp \
	labelpropertieshandler.h


libpropertieshandlers_la_CXXFLAGS = -I../../dbinterface \
	-I../../libpropertypages \
	-I../../libdbwindow \
	-I../../libshapeframework \
	-I../../libfieldswindow \
	-I../../libhelpers \
	@WX_CXXFLAGS@

libpropertieshandlers_la_LDFLAGS = -L../libpropertypages \
	-L../libdbwindow \
	-ldbwindow \
	-lpropertypages \
	@WX_LIBS@
