## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdbwindow.la

libdbwindow_la_SOURCES =  \
	ErdForeignKey.cpp \
	ErdForeignKey.h \
	ErdTable.cpp \
	ErdTable.h \
	IDbAdapter.h \
	IDbType.h \
	NArrow.cpp \
	NArrow.h \
	OneArrow.cpp \
	OneArrow.h \
	column.cpp \
	column.h \
	constraint.cpp \
	constraint.h \
	table.cpp \
	table.h

libdbwindow_la_CXXFLAGS = -I../../libshapeframework \
	-I../../libdatabaseview \
	-I../../dbinterface \
	-I/home/igor/wxWidgets/buildGTK/lib/wx/include/gtk2-unicode-3.1 \
	-I/home/igor/wxWidgets/include \
	-D_FILE_OFFSET_BITS=64 \
	-DWXUSINGDLL \
	-D__WXGTK__ \
	-pthread 

libdbwindow_la_LDFLAGS = -L../libshapeframework \
	 \
	-lshapeframework \
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
