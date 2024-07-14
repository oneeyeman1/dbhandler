## Process this file with automake to produce Makefile.in

QT_CXXFLAGS = @QT_CXXFLAGS@
QT_DIR = @QT_DIR@
QT_LIBS = @QT_LIBS@
QT_LRELEASE = @QT_LRELEASE@
QT_LUPDATE = @QT_LUPDATE@
QT_MOC = @QT_MOC@
QT_RCC = @QT_RCC@
QT_UIC = @QT_UIC@


lib_LTLIBRARIES =  \
	libpropertieshandlers.la

libpropertieshandlers_la_SOURCES =  \
	propertieshandler.cpp \
	propertieshandler.h \
	fieldpropertieshandler.cpp \
	fieldpropertieshandler.h \
	propertieshandlerbase.h \
	designpropertieshandler.cpp \
	designpropertieshandler.h \
	dividerpropertieshandler.cpp \
	dividerpropertieshandler.h

libpropertieshandlers_la_CXXFLAGS = -I../../dbinterface \
	-I../../libpropertypages \
	-I../../libdbwindow \
	-I../../libshapeframework \
	-I../../libfieldswindow \
	-I../../libhelpers \
	`wx-config \
	--cxxflags` \
	$(QT_CXXFLAGS)

libpropertieshandlers_la_LDFLAGS = -L../libpropertypages \
	-lpropertypages \
	`wx-config \
	--libs`    $(QT_LIBS)
