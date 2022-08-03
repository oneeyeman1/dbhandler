dnl Process this file with autoreconf to produce a configure script

AC_PREREQ([2.69])
AC_INIT([dbhandler], [0.1])

AC_CONFIG_SRCDIR([dbhandler/mainframe.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([auto-m4])

AM_INIT_AUTOMAKE([
  1.14
  foreign
  subdir-objects
])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

m4_pattern_forbid([^PKG_CHECK_MODULES])dnl
PKG_CHECK_MODULES([GTK3],     [gtk+-3.0])
PKG_CHECK_MODULES([ODBC],     [odbc])
PKG_CHECK_MODULES([ODBCINST], [odbcinst])

m4_pattern_forbid([^WX_CONFIG_OPTIONS])dnl
m4_pattern_forbid([^WX_CONFIG_CHECK])dnl
WX_CONFIG_OPTIONS
dnl 3.1.0 introduces wxOVERRIDE
dnl 3.1.6 introduces wxBitmapBundle
WX_CONFIG_CHECK([3.1.6], [wxWin=1])
AS_IF([test "x$wxWin" != x1], [dnl
  AC_MSG_ERROR([Could not find appropriate wxWidgets.])
])
AC_SUBST([WX_LIBS_STC], ["`$WX_CONFIG_PATH --libs stc`"])


dnl ***************************************************************************
dnl Check for ttt_config tools and define variables with them:
dnl   * mysql_config: MYSQL_CFLAGS and MYSQL_LIBS (standard names)
dnl   * pg_config:    LIBPQ_CFLAGS and LIBPQ_LIBS (standard names)
dnl ***************************************************************************

AC_DEFUN([DBH_CONFIG_TOOL], [dnl
  AC_ARG_VAR([$1], [$2 library config tool])
  AS_VAR_IF([$1], [], [dnl
    AC_PATH_PROG([$1], [$2], [no])
  ])
  AS_IF([test "x$$1" = x || test "x$$1" = xno], [dnl
    AC_MSG_ERROR([Library config tool ][$2][ not found.])
  ])
])dnl

m4_pattern_forbid([^DBH_CONFIG_TOOL])dnl

DBH_CONFIG_TOOL([MYSQL_CONFIG], [mysql_config])
AC_SUBST([MYSQL_CFLAGS],        ["`$MYSQL_CONFIG --cflags`"])
AC_SUBST([MYSQL_LIBS],          ["`$MYSQL_CONFIG --libs`"])

DBH_CONFIG_TOOL([PG_CONFIG],    [pg_config])
AC_SUBST([LIBPQ_CFLAGS],        ["-I`$PG_CONFIG --includedir`"])
AC_SUBST([LIBPQ_LIBS],          ["-L`$PG_CONFIG --libdir` -lpq"])


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
  po/Makefile.in
])
AC_OUTPUT
