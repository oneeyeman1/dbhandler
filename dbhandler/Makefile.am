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
	newtablehandler.h

dbhandler_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	-I$(top_srcdir)/libdbwindow/res/gui \
	$(WX_CPPFLAGS)

dbhandler_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

dbhandler_LDFLAGS = \
	$(WX_LIBS)
