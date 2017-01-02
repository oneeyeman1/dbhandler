## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpropertypages.la

libpropertypages_la_SOURCES =  \
	tablegeneral.cpp \
	tablegeneral.h \
	fontpropertypagebase.cpp \
	fontpropertypagebase.h \
	gtk/fontpropertypage.cpp \
	gtk/fontpropertypage.h

libpropertypages_la_CXXFLAGS = -I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libpropertypages_la_LDFLAGS = `/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
