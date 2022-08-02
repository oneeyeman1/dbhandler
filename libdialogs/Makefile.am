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
	typecombobox.cpp \
	typecombobox.h \
	gotoline.cpp \
	gotoline.h \
	attachdb.cpp \
	attachdb.h \
	detachdb.cpp \
	detachdb.h \
	datasource.cpp \
	datasource.h

libdialogs_la_CPPFLAGS = \
	-I$(top_srcdir)/dbinterface \
	-I$(top_srcdir)/libfieldswindow \
	-I$(top_srcdir)/libshapeframework \
	-I$(top_srcdir)/libpropertypages \
	-I$(top_srcdir)/libpropertieshandlers \
	$(GTK3_CFLAGS) \
	$(WX_CPPFLAGS)

libdialogs_la_CXXFLAGS = \
	$(WX_CXXFLAGS_ONLY)

libdialogs_la_LIBADD = \
	$(top_builddir)/dbinterface/libdbinterface.la \
	$(top_builddir)/libsqlite/libsqlite_lib.la \
	$(top_builddir)/libodbc/libodbc_lib.la \
	$(top_builddir)/libshapeframework/libshapeframework.la \
	$(top_builddir)/libfieldswindow/libfieldswindow.la \
	$(top_builddir)/libpropertypages/libpropertypages.la \
	$(top_builddir)/libpostgres/libpostgres.la \
	$(WX_LIBS)
