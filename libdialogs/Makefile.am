## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdialogs.la

libdialogs_la_SOURCES =  \
	databasetype.cpp \
	databasetype.h \
	dialogs.cpp \
	odbcconfigure.cpp \
	odbcconfigure.h \
	selecttables.cpp \
	selecttables.h \
	createindex.cpp \
	createindex.h

libdialogs_la_CXXFLAGS = -I../../dbinterface \
	-I../../libfieldswindow \
	-I../../libshapeframework \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libdialogs_la_LDFLAGS = -L../libsqlite \
	-L../libshapeframework \
	-L../dbinterface \
	-L../libfieldswindow \
	-L../libodbc \
	-Wl,-rpath,/home/igor/dbhandler/libfieldswindow/.lib \
	-ldbinterface \
	-lsqlite_lib \
	-lodbc_lib \
	-lshapeframework \
	-lfieldswindow \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
