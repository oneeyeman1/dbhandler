## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdbwindow.la

libdbwindow_la_SOURCES =  \
	ErdForeignKey.cpp \
	ErdForeignKey.h \
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
	GridTableShape.h \
	HeaderGrid.cpp \
	HeaderGrid.h \
	databasetemplate.cpp \
	databasetemplate.h \
	constraintsign.cpp \
	constraintsign.h \
	commentfieldshape.h \
	commentfieldshape.cpp \
	commenttableshape.cpp \
	designcanvas.cpp \
	designcanvas.h \
	designfield.cpp \
	designfield.h \
	designlabel.cpp \
	designlabel.h \
	divider.cpp \
	divider.h \
	fontcombobox.cpp \
	fontcombobox.h \
	taborder.cpp \
	taborder.h \
	commenttableshape.h \
	fieldtypeshape.cpp \
	fieldtypeshape.h \
	bold.h \
	italic.h \
	underline.h \
	nametableshape.cpp \
	nametableshape.h \
	dbview.h

libdbwindow_la_CXXFLAGS = -I../../libshapeframework \
	-I../../libfieldswindow \
	-I../../dbinterface \
	-I../../libpropertypages \
	-I../../libpropertieshandlers \
	-I../../libhelpers \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` \
	`pkg-config \
	--cflags \
	pango` 

libdbwindow_la_LDFLAGS = -L../libpropertypages \
	-lpropertypages \
	-L../libpropertieshandlers \
	-lpropertieshandlers \
	-L../libshapeframework \
	-L../libfieldswindow \
	-lfieldswindow \
	-lshapeframework \
	-L../libhelpers \
	-lhelpers \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs \
	stc`
	

