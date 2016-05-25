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
	viewbase.h

dbhandler_CXXFLAGS = -I/home/igor/wxWidgets/buildGTK2/lib/wx/include/gtk2-unicode-3.1 \
	-I/home/igor/wxWidgets/include \
	-D_FILE_OFFSET_BITS=64 \
	-DWXUSINGDLL \
	-D__WXGTK__ \
	-pthread 

dbhandler_LDFLAGS = -L/home/igor/wxWidgets/buildGTK2/lib \
	-pthread \
	-Wl,-rpath,/home/igor/wxWidgets/buildGTK2/lib \
	-lwx_gtk2u_xrc-3.1 \
	-lwx_gtk2u_html-3.1 \
	-lwx_gtk2u_qa-3.1 \
	-lwx_gtk2u_adv-3.1 \
	-lwx_gtk2u_core-3.1 \
	-lwx_baseu_xml-3.1 \
	-lwx_baseu_net-3.1 \
	-lwx_baseu-3.1 
