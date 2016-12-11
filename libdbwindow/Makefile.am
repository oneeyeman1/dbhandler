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
	table.h \
	GUIDatabaseTable.cpp \
	GUIDatabaseTable.h \
	databasecanvas.cpp \
	databasecanvas.h \
	databasedoc.cpp \
	databasedoc.h \
	databaseview.cpp \
	databaseview.h \
	dbview.cpp \
	MyErdTable.cpp \
	MyErdTable.h \
	ErdLineShape.cpp \
	ErdLineShape.h \
	FieldShape.cpp \
	FieldShape.h \
	GridTableShape.cpp \
	GridTableShape.h

libdbwindow_la_CXXFLAGS = -I../../libshapeframework \
	-I../../dbinterface \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libdbwindow_la_LDFLAGS = -L../libshapeframework \
	-L../libfieldswindow \
	-lfieldswindow \
	-lshapeframework \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs`
	

