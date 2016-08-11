## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdatabaseview.la

libdatabaseview_la_SOURCES =  \
	dbview.cpp \
	databasedoc.cpp \
	databasedoc.h \
	databaseview.cpp \
	databaseview.h \
	databasecanvas.cpp \
	databasecanvas.h

libdatabaseview_la_CXXFLAGS = -I../../libshapeframework \
	-I../../libdbwindow \
	-I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libdatabaseview_la_LDFLAGS = -L../libshapeframework \
	 \
	-lshapeframework \
	-L../libdbwindow \
	-ldbwindow \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs`
	 
