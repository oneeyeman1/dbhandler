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

libtablewindow_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	$(WX_CPPFLAGS)

libtablewindow_la_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

libtablewindow_la_LIBADD = \
	$(top_builddir)/dbinterface/libdbinterface.la \
	$(WX_LIBS)
