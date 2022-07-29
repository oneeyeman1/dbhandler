dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_PREREQ([2.69])
AC_INIT([dbhandler], [0.1])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([
  1.11
  foreign
  subdir-objects
])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

AC_ARG_VAR([ODBC_CONFIG], [odbc_config tool to use])
AS_VAR_IF([ODBC_CONFIG], [], [dnl
    AC_PATH_PROG([ODBC_CONFIG], [odbc_config], [no])
])
AC_ARG_VAR([PKG_CONFIG], [pkg-config tool to use])
AS_VAR_IF([PKG_CONFIG], [], [dnl
    AC_PATH_TOOL([PKG_CONFIG], [pkg-config], [no])
])
AS_IF([test "x$ODBC_CONFIG" != xno], [dnl
    ODBC_CFLAGS="`$ODBC_CONFIG --cflags`"
    ODBC_LIBS="`$ODBC_CONFIG --libs` -lodbcinst"
], [test "x$PKG_CONFIG" != xno], [dnl
    ODBC_CFLAGS="`$PKG_CONFIG odbc --cflags` `$PKG_CONFIG odbcinst --cflags`"
    ODBC_LIBS="`$PKG_CONFIG odbc --libs` `$PKG_CONFIG odbcinst --libs`"
])
AC_SUBST([ODBC_CFLAGS])
AC_SUBST([ODBC_LIBS])

WXCONFIG=wx-config
AC_ARG_WITH([wx-config],
    [AS_HELP_STRING([--with-wx-config=FILE],
        [Use the given path to wx-config when determining
         wxWidgets configuration; defaults to "wx-config"])],
    [dnl
        AS_IF([test "x$withval" != xyes && test "x$withval" != x], [dnl
            WXCONFIG="$withval"
        ])
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
WX_LIBS="`$WXCONFIG --libs`"

AC_SUBST([WX_CPPFLAGS])
AC_SUBST([WX_CXXFLAGS])
AC_SUBST([WX_LIBS])


dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
m4_pattern_forbid([^IT_PROG_INTLTOOL])dnl
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=dbhandler
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [GETTEXT package name])
m4_pattern_forbid([^AM_GLIB_GNU_GETTEXT])
AM_GLIB_GNU_GETTEXT



m4_pattern_forbid([^LT_INIT])dnl
LT_INIT




AC_CONFIG_FILES([
  Makefile
  dbhandler/Makefile
  dbinterface/Makefile
  libdbloader/Makefile
  libdbwindow/Makefile
  libdialogs/Makefile
  libfieldswindow/Makefile
  libmysql/Makefile
  libodbc/Makefile
  libpostgres/Makefile
  libpropertieshandlers/Makefile
  libpropertypages/Makefile
  libshapeframework/Makefile
  libsqlite/Makefile
  libtabledataedit/Makefile
  libtablewindow/Makefile
  po/Makefile.in
])
AC_OUTPUT
