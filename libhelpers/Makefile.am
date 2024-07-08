## Process this file with automake to produce Makefile.in


lib_LIBRARIES =  \
	libhelpers.a

libhelpers_a_SOURCES =  \
	typecombobox.cpp \
	typecombobox.h \
	colorcombobox.cpp \
	colorcombobox.h

libhelpers_a_CXXFLAGS = `wx-config \
	--cxxflags` \
	-fPIC 

libhelpers_a_LDFLAGS = `wx-config \
	--libs` 
