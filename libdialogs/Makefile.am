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
	selecttables.h \
	createindex.cpp \
	createindex.h \
	properties.cpp \
	properties.h \
	foreignkey.cpp \
	foreignkey.h \
	getobjectname.cpp \
	getobjectname.h \
	jointype.cpp \
	jointype.h \
	addcolumnsdialog.cpp \
	addcolumnsdialog.h \
	odbccredentials.cpp \
	odbccredentials.h \
	bitmappanel.cpp \
	bitmappanel.h \
	newquery.cpp \
	newquery.h \
	quickselect.cpp \
	quickselect.h \
	retrievalarguments.cpp \
	retrievalarguments.h \
	gotoline.cpp \
	gotoline.h \
	attachdb.cpp \
	attachdb.h \
	detachdb.cpp \
	detachdb.h \
	datasource.cpp \
	datasource.h \
	createviewoptions.cpp \
	createviewoptions.h \
	saveview.cpp \
	saveview.h

libdialogs_la_CXXFLAGS = -I../../dbinterface \
	-I../../libfieldswindow \
	-I../../libshapeframework \
	-I../../libpropertypages \
	-I../../libpropertieshandlers \
	`pkg-config \
	--cflags \
	gtk+-3.0` \
	-I../../libhelpers \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags`

libdialogs_la_LDFLAGS = -L../libsqlite \
	-L../libshapeframework \
	-L../dbinterface \
	-L../libfieldswindow \
	-L../libodbc \
	-L../libpostgres \
	-L../libpropertypages \
	-L../libpropertieshamdlers \
	-L../libhelpers \
	-lhelpers \
	-ldbinterface \
	-lsqlite_lib \
	-lodbc_lib \
	-lshapeframework \
	-lfieldswindow \
	-lpropertypages \
	-lpropertieshandlers \
	-lpostgres \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs`
