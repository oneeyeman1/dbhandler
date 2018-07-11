## Process this file with automake to produce Makefile.in


bin_PROGRAMS =  \
	dbhandler

dbhandler_SOURCES =  \
	doc.cpp \
	doc.h \
	docview.cpp \
	docview.h \
	mainframe.cpp \
	mainframe.h \
	view.cpp \
	view.h \
	viewbase.cpp \
	viewbase.h \
	newtablehandler.cpp \
	newtablehandler.h \
	newtablehandler.cpp \
	newtablehandler.h

dbhandler_CXXFLAGS = -I../../dbinterface \
	-I../../libdbwindow/res/gui \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

dbhandler_LDFLAGS = -Wl,-rpath \
	-Wl,/home/igor/dbhandler/Debug/libdialogs \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
