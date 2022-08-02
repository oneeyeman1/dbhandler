## Process this file with automake to produce Makefile.in


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
	colorcombobox.cpp \
	colorcombobox.h \
	printspec.cpp \
	printspec.h \
	qt/fontpropertypageQt.cpp \
	qt/fontpropertypageQt.h \
	gtk/fontpropertypageGTK.cpp \
	gtk/fontpropertypageGTK.h

libpropertypages_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	-I$(top_srcdir)/libshapeframework \
	-I$(top_srcdir)/libfieldswindow \
	$(GTK3_CFLAGS) \
	$(WX_CPPFLAGS)

libpropertypages_la_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

libpropertypages_la_LIBADD = \
	$(GTK3_LIBS) \
	$(WX_LIBS)
