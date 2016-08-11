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
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libdbwindow_la_LDFLAGS = `/home/igor/wxWidgets/buildGTK/wx-config \
	--libs`-L../libshapeframework \
	-lshapeframework

