## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libfieldswindow.la

libfieldswindow_la_SOURCES =  \
	fieldswindow.cpp \
	fieldwindow.cpp \
	fieldwindow.h

libfieldswindow_la_CXXFLAGS = -I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libfieldswindow_la_LDFLAGS = -L/home/igor/dbhandler/Debug/dbinterface \
	-ldbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
