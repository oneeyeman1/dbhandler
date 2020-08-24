## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpropertieshandlers.la

libpropertieshandlers_la_SOURCES =  \
	propertieshandler.cpp \
	propertieshandler.h \
	fieldpropertieshandler.cpp \
	fieldpropertieshandler.h \
	propertieshandlerbase.h

libpropertieshandlers_la_CXXFLAGS = -I../../dbinterface \
	-I../../libpropertypages \
	-I../../libshapeframework \
	-I../../libfieldswindow \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libpropertieshandlers_la_LDFLAGS = -L../libpropertypages \
	-lpropertypages \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
