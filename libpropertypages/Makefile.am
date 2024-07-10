## Process this file with automake to produce Makefile.in

QT_CXXFLAGS = @QT_CXXFLAGS@
QT_DIR = @QT_DIR@
QT_LIBS = @QT_LIBS@
QT_LRELEASE = @QT_LRELEASE@
QT_LUPDATE = @QT_LUPDATE@
QT_MOC = @QT_MOC@
QT_RCC = @QT_RCC@
QT_UIC = @QT_UIC@

lib_LTLIBRARIES =  \
	libpropertypages.la

libpropertypages_la_SOURCES =  \
	tablegeneral.cpp \
	tablegeneral.h \
	fontpropertypagebase.cpp \
	fontpropertypagebase.h \
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
	pointerproperty.h \
	tableprimarykey.cpp \
	tableprimarykey.h \
	designgeneral.cpp \
	designgeneral.h \
	printspec.cpp \
	printspec.h \
	qt/fontpropertypageQt.cpp \
	qt/fontpropertypageQt.h \
	gtk/fontpropertypageGTK.cpp \
	gtk/fontpropertypageGTK.h \
	databasefielddisplay.cpp \
	databasefielddisplay.h

libpropertypages_la_CXXFLAGS = -I../../dbinterface \
	-I../../libshapeframework \
	-I../../libfieldswindow \
	-I../../libhelpers \
	`pkg-config \
	--cflags \
	gtk+-3.0` \
	`wx-config \
	--cxxflags` \
	$(QT_CXXFLAGS)

libpropertypages_la_LDFLAGS = `pkg-config \
	--libs \
	gtk+-3.0` \
	-L../libhelpers \
	-lhelpers \
	`wx-config \
	--libs`  $(QT_LIBS)
