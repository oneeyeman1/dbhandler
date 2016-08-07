## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdatabaseview.la

libdatabaseview_la_SOURCES =  \
	dbview.cpp \
	databasedoc.cpp \
	databasedoc.h \
	databaseview.cpp \
	databaseview.h \
	databasecanvas.cpp \
	databasecanvas.h

libdatabaseview_la_CXXFLAGS = -I../../libshapeframework \
	-I../../libdbwindow \
	-I../../dbinterface \
	-I/home/igor/wxWidgets/buildGTK/lib/wx/include/gtk2-unicode-3.1 \
	-I/home/igor/wxWidgets/include \
	-D_FILE_OFFSET_BITS=64 \
	-DWXUSINGDLL \
	-D__WXGTK__ \
	-pthread 

libdatabaseview_la_LDFLAGS = -L../libshapeframework \
	 \
	-lshapeframework \
	-L../libdbwindow \
	-ldbwindow \
	-L/home/igor/wxWidgets/buildGTK/lib \
	-pthread \
	-Wl,-rpath,/home/igor/wxWidgets/buildGTK/lib \
	-lwx_gtk2u_xrc-3.1 \
	-lwx_gtk2u_html-3.1 \
	-lwx_gtk2u_qa-3.1 \
	-lwx_gtk2u_adv-3.1 \
	-lwx_gtk2u_core-3.1 \
	-lwx_baseu_xml-3.1 \
	-lwx_baseu_net-3.1 \
	-lwx_baseu-3.1
	 
