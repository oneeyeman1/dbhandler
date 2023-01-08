## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libtablewindow.la

libtablewindow_la_SOURCES =  \
	tabledoc.cpp \
	tabledoc.h \
	tableview.cpp \
	tableview.h \
	tablewindow.cpp \
	tablecanvas.cpp \
	tablecanvas.h \
	typecombobox.cpp \
	typecombobox.h

libtablewindow_la_CXXFLAGS = -I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libtablewindow_la_LDFLAGS = -L../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
