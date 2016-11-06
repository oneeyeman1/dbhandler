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

libdialogs_la_LDFLAGS = -Wl,-rpath,/home/igor/dbhandler/Debug/libsqlite/.libs \
	-L/home/igor/dbhandler/Debug/libsqlite/.libs \
	-L/home/igor/dbhandler/Debug/libfieldswindow/.libs \
	-L/home/igor/dbhandler/libshapeframework/.libs \
	-ldbinterface \
	-lsqlite_lib \
	-lfieldswindow \
	-lshapeframework \
	-L/home/igor/dbhandler/Debug/dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
