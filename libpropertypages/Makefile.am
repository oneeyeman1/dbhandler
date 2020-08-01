## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libpropertypages.la

libpropertypages_la_SOURCES =  \
	tablegeneral.cpp \
	tablegeneral.h \
	fontpropertypagebase.cpp \
	fontpropertypagebase.h \
	gtk/fontpropertypage.cpp \
	gtk/fontpropertypage.h \
	syntaxproppage.cpp \
	syntaxproppage.h \
	wherehavingpage.cpp \
	wherehavingpage.h \
	fieldgeneral.cpp \
	fieldgeneral.h \
	fieldheader.cpp \
	fieldheader.h \
	propertypagebase.h \
	designlabelgeneral.cpp \
	designlabelgeneral.h \
	sortgroupbypage.cpp \
	sortgroupbypage.h \
	bandgeneral.cpp \
	bandgeneral.h \
	pointerproperty.cpp \
	pointerproperty.h

libpropertypages_la_CXXFLAGS = -I../../dbinterface \
	`pkg-config \
	--cflags \
	gtk+-3.0` \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libpropertypages_la_LDFLAGS = `pkg-config \
	--libs \
	gtk+-3.0` \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
