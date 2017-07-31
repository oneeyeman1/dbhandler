## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = dbinterface libsqlite libodbc libpostgres libmysql libshapeframework libdbloader libfieldswindow libpropertypages libdialogs libdbwindow libtablewindow po dbhandler

dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(INTLTOOL_FILES)

AM_CXXFLAGS = -I/home/igor/wxWidgets/buildGTK2/lib/wx/include/gtk2-unicode-3.1 \
	-I/home/igor/wxWidgets/include \
	-D_FILE_OFFSET_BITS=64 \
	-DWXUSINGDLL \
	-D__WXGTK__ \
	-pthread 

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
