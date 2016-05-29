## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdialogs.la

libdialogs_la_SOURCES =  \
	databasetype.cpp \
	databasetype.h \
	dialogs.cpp \
	odbcconfigure.cpp \
	odbcconfigure.h \
	selecttables.cpp \
	selecttables.h

libdialogs_la_CXXFLAGS = -I../../dbinterface \
	-I/home/igor/wxWidgets/buildGTK2/lib/wx/include/gtk2-unicode-3.1 \
	-I/home/igor/wxWidgets/include \
	-D_FILE_OFFSET_BITS=64 \
	-DWXUSINGDLL \
	-D__WXGTK__ \
	-pthread \
	-DUNICODE 

libdialogs_la_LDFLAGS = -Wl,-rpath,/home/igor/dbhandler/Debug/libsqlite/.libs \
	-L/home/igor/dbhandler/Debug/libsqlite/.libs \
	-L/home/igor/dbhandler/Debug/dbinterface \
	-ldbinterface \
	-lsqlite_lib \
	-L/home/igor/wxWidgets/buildGTK2/lib \
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
