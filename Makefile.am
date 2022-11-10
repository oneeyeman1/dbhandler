# Process this file with autoreconf

ACLOCAL_AMFLAGS = -I auto-m4 -I m4

DISTCLEANFILES =
EXTRA_DIST =
EXTRA_LTLIBRARIES =
bin_PROGRAMS =
dist_doc_DATA =
lib_LTLIBRARIES =
noinst_LTLIBRARIES =

SUBDIRS = . po

include Makefile-files
include m4/Makefile-files

# Keeping the sequence from the old SUBDIRS definition so that "make
# install" installs the libraries in the proper sequence, where
# prerequisite libraries are installed before the binaries which link
# to them.
include dbinterface/Makefile-files
include libmysql/Makefile-files
include libodbc/Makefile-files
include libpostgres/Makefile-files
include libsqlite/Makefile-files
include libshapeframework/Makefile-files
include libdbloader/Makefile-files
include libfieldswindow/Makefile-files
include libpropertypages/Makefile-files
include libdialogs/Makefile-files
include libpropertieshandlers/Makefile-files
include libtabledataedit/Makefile-files
include libdbwindow/Makefile-files
include libtablewindow/Makefile-files
include dbhandler/Makefile-files
