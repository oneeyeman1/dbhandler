## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libfieldswindow.la

libfieldswindow_la_SOURCES =  \
	fieldswindow.cpp \
	fieldwindow.cpp \
	fieldwindow.h \
	field.cpp \
	field.h

libfieldswindow_la_CXXFLAGS = -I../../dbinterface \
	-I../../libshapeframework \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libfieldswindow_la_LDFLAGS = -L/home/igor/dbhandler/Debug/dbinterface \
	-L/home/igor/dbhandler/Debug/libshapeframework/.libs \
	-lshapeframework \
	-ldbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
