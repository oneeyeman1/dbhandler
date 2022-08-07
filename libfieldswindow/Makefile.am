## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libfieldswindow.la

libfieldswindow_la_SOURCES =  \
	field.cpp \
	field.h \
	fieldswindow.cpp \
	fieldwindow.cpp \
	fieldwindow.h

libfieldswindow_la_CXXFLAGS = -I../../libshapeframework \
	`wx-config \
	--cxxflags` 

libfieldswindow_la_LDFLAGS = -L../libshapeframework \
	-lshapeframework \
	`wx-config \
	--libs` 
