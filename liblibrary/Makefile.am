## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES =  \
	liblibrary.la

liblibrary_la_SOURCES =  \
	library.cpp \
	libraryview.cpp \
	libraryview.h \
	librarydocument.cpp \
	librarydocument.h \
	librarydoctemplate.cpp \
	librarydoctemplate.h

liblibrary_la_CXXFLAGS = -I../../libhelpers \
	-I../../dbinterface \
	-I../../dbhandler/ \
	@WX_CXXFLAGS@

liblibrary_la_LDFLAGS = -L../libhelpers \
	-lhelpers \
	@WX_LIBS@

