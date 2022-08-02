## Process this file with automake to produce Makefile.in
## Created by Anjuta

ACLOCAL_AMFLAGS = -I auto-m4 -I m4

SUBDIRS = dbinterface libsqlite libodbc libpostgres libmysql libshapeframework libdbloader libfieldswindow libpropertypages libdialogs libpropertieshandlers libtabledataedit libdbwindow libtablewindow po dbhandler

dist_doc_DATA = COPYING

INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

include m4/Makefile-files
