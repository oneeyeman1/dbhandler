## Process this file with automake to produce Makefile.in


lib_LIBRARIES =  \
	libhelpers.a

libhelpers_a_SOURCES =  \
	painterobjects.h \
	typecombobox.cpp \
	typecombobox.h \
	colorcombobox.cpp \
	colorcombobox.h \
	ablbaseview.cpp \
	ablbaseview.h \
	maskededit.cpp \
	maskededit.h \
	mycombocelleditor.cpp \
	mycombocelleditor.h \
	mycombocellrenderer.cpp \
	mycombocellrenderer.h \
	mytabletypeeditor.cpp \
	mytabletypeeditor.h

libhelpers_a_CXXFLAGS = -DwxUSE_MASKED_EDIT -fPIC \
	-ggdb3 @WX_CXXFLAGS@

libhelpers_a_LDFLAGS = @WX_LDFLAGS@
