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
	query.h \
	table.h

dbhandler_CXXFLAGS = -I../../dbinterface \
	-I../../libhelpers \
	-I../../libdbwindow/res/gui \
	@WX_CPPFLAGS@

dbhandler_LDFLAGS = @WX_LIBS@
