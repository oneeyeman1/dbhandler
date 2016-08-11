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
	selecttables.h

libdialogs_la_CXXFLAGS = -I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libdialogs_la_LDFLAGS = -Wl,-rpath,/home/igor/dbhandler/Debug/libsqlite/.libs \
	-L/home/igor/dbhandler/Debug/libsqlite/.libs \
	-L/home/igor/dbhandler/Debug/dbinterface \
	-ldbinterface \
	-lsqlite_lib \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
