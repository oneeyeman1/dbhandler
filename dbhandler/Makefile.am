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

dbhandler_CXXFLAGS = -I../../dbinterface \
	-I../../libdbwindow/res/gui \
	-I/home/igor/wxWidgets/buildGTK/lib/wx/include/gtk2-unicode-3.1 \
	-I/home/igor/wxWidgets/include \
	-D_FILE_OFFSET_BITS=64 \
	-DWXUSINGDLL \
	-D__WXGTK__ \
	-pthread 

dbhandler_LDFLAGS = -Wl,-rpath \
	-Wl,/home/igor/dbhandler/Debug/libdialogs \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` 
