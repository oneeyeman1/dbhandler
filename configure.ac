dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([dbhandler],[0.1])

AM_MAINTAINER_MODE([enable])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 subdir-objects])

AM_SILENT_RULES([yes])

AC_PROG_CC

AC_CHECK_PROG(ODBC,odbc_config,yes)
if test x"${ODBC}" == x"yes" ; then
    ODBC_CFLAGS = `odbc_config --cflags`
    ODBC_LIBS = `odbc_config --libs` -lodbcinst
else
    ODBC_CFLAGS = `pkg-config odbc --cflags`
    ODBC_LIBS = `pkg-config odbc --libs` -lodbcinst
fi

AC_SUBST(ODBC_CFLAGS)
AC_SUBST(ODBC_LIBS)

WXCONFIG=wx-config
AC_ARG_WITH(wx-config,
[[  --with-wx-config=FILE
    Use the given path to wx-config when determining
    wxWidgets configuration; defaults to "wx-config"]],
[
    if test "$withval" != "yes" -a "$withval" != ""; then
        WXCONFIG=$withval
    fi
])
AC_MSG_CHECKING([wxWidgets version])
if wxversion=`$WXCONFIG --version`; then
    AC_MSG_RESULT([$wxversion])
else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([wxWidgets is required. Try --with-wx-config.])
fi

WX_CPPFLAGS="`$WXCONFIG --cppflags`"
WX_CXXFLAGS="`$WXCONFIG --cxxflags`"
WX_LIBS="`$WXCONFIG --libs std,stc,propgrid`"
DBTOOLKIT=""

if wx-config --list | grep -i default | grep -i qt ; then
    AC_MSG_NOTICE([Qt build found])
    AX_HAVE_QT()
    AS_IF([test "$have_qt" = yes], [
        WX_CXXFLAGS="$WX_CXXFLAGS $QT_CXXFLAGS"
        WX_LIBS="$WX_LIBS $QT_LIBS"
    ])
    DBTOOLKIT="-D__DBQT__"
fi 

if wx-config --list | grep -i default | grep -i gtk ; then
    AC_MSG_NOTICE([GTK build found])
    WX_CXXFLAGS="$WX_CXXFLAGS `pkg-config --cflags gtk+-3.0`"
    WX_LIBS="$WX_LIBS `pkg-config --libs gtk+-3.9`"
    DBTOOLKIT="-D__DBGTK__"
fi

AC_SUBST(WX_CPPFLAGS)
AC_SUBST(WX_CXXFLAGS)
AC_SUBST(WX_LIBS)
AC_SUBST(DBTOOLKIT)

AC_PROG_CPP
AC_PROG_CXX


dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=dbhandler
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT



LT_INIT




AC_CONFIG_FILES([
Makefile
po/Makefile.in
dbinterface/Makefile
libsqlite/Makefile
libodbc/Makefile
libshapeframework/Makefile
libfieldswindow/Makefile
libdialogs/Makefile
libdbloader/Makefile
libdbwindow/Makefile
dbhandler/Makefile
libhelpers/Makefile
libtabledataedit/Makefile
libmysql/Makefile
libpostgres/Makefile
libtablewindow/Makefile
libpropertypages/Makefile
liblibrary/Makefile
])
AC_OUTPUT
