## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = dbinterface libsqlite libodbc libpostgres libmysql libshapeframework libhelpers libdbloader libfieldswindow libpropertypages libpropertieshandlers  libdialogs libtabledataedit libdbwindow libtablewindow po dbhandler

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

AM_CXXFLAGS = `/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
