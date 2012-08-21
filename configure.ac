AC_INIT(indicator-sync, 12.10.0)

AC_PREREQ(2.53)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE()

AM_MAINTAINER_MODE

IT_PROG_INTLTOOL([0.35.0])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_C_O
AC_STDC_HEADERS
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(VERSION)
AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])

# the Google Test targets are cpp
AC_PROG_CXX


###########################
# GObject Introspection
###########################

AC_MSG_CHECKING([for gobject-introspection m4 macros])
m4_ifdef([GOBJECT_INTROSPECTION_CHECK], , AC_MSG_FAILURE([failed. Please install the package 'gobject-introspection' in order to compile]))
AC_MSG_RESULT([yes])

GOBJECT_INTROSPECTION_CHECK([0.10.0])

GIR_MAJOR=1
GIR_MINOR=0
AC_SUBST([GIR_VERSION],[$GIR_MAJOR.$GIR_MINOR])
AC_DEFINE_UNQUOTED(GIR_VERSION, [$GIR_VERSION], [GIR version])

###########################
# Dependencies 
###########################

GTK_REQUIRED_VERSION=3.0
GIO_UNIX_REQUIRED_VERSION=2.18
PANEL_REQUIRED_VERSION=2.0.0
INDICATOR_REQUIRED_VERSION=0.3.19
DBUSMENUGLIB_REQUIRED_VERSION=.0.6.2
DBUSMENUGTK_REQUIRED_VERSION=0.5.90
GLIB_REQUIRED_VERSION=2.32.0

PKG_CHECK_MODULES(SYNC_CLIENT, [dbusmenu-glib-0.4 >= $DBUSMENUGLIB_REQUIRED_VERSION
                                gio-unix-2.0      >= $GIO_UNIX_REQUIRED_VERSION
                                glib-2.0          >= $GLIB_REQUIRED_VERSION
                                gmodule-2.0       >= $GLIB_REQUIRED_VERSION])
AC_SUBST(SYNC_CLIENT_CFLAGS)
AC_SUBST(SYNC_CLIENT_LIBS)

PKG_CHECK_MODULES(SYNC_SERVICE, [indicator3-0.4    >= $INDICATOR_REQUIRED_VERSION
                                 dbusmenu-glib-0.4 >= $DBUSMENUGLIB_REQUIRED_VERSION
                                 gio-unix-2.0      >= $GIO_UNIX_REQUIRED_VERSION
                                 glib-2.0          >= $GLIB_REQUIRED_VERSION
                                 gmodule-2.0       >= $GLIB_REQUIRED_VERSION])
AC_SUBST(SYNC_SERVICE_CFLAGS)
AC_SUBST(SYNC_SERVICE_LIBS)

PKG_CHECK_MODULES(SYNC_INDICATOR, [indicator3-0.4       >= $INDICATOR_REQUIRED_VERSION
                                   dbusmenu-gtk3-0.4    >= $DBUSMENUGTK_REQUIRED_VERSION
                                   gtk+-3.0             >= $GTK_REQUIRED_VERSION
                                   glib-2.0             >= $GLIB_REQUIRED_VERSION
                                   gmodule-2.0          >= $GLIB_REQUIRED_VERSION])
AC_SUBST(SYNC_INDICATOR_CFLAGS)
AC_SUBST(SYNC_INDICATOR_LIBS)

GTK_DOC_CHECK([1.18], [])

###########################
# gcov coverage reporting
###########################

m4_include([m4/gcov.m4])
AC_TDD_GCOV
AM_CONDITIONAL([HAVE_GCOV], [test "x$ac_cv_check_gcov" = xyes])
AM_CONDITIONAL([HAVE_LCOV], [test "x$ac_cv_check_lcov" = xyes])
AM_CONDITIONAL([HAVE_GCOVR], [test "x$ac_cv_check_gcovr" = xyes])
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_CXXFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

###########################
# Google Test framework
###########################

AC_ARG_ENABLE([tests],
    [AS_HELP_STRING([--disable-tests], [Disable test scripts and tools (default=auto)])],
    [enable_tests=${enableval}],
    [enable_tests=auto])
if test "x$enable_tests" != "xno"; then
    m4_include([m4/gtest.m4])
    CHECK_GTEST
    if test "x$enable_tests" = "xauto"; then
        enable_tests=${have_gtest}
    elif test "x$enable_tests" = "xyes" && test "x$have_gtest" != "xyes"; then
        AC_MSG_ERROR([tests were requested but gtest is not installed.])
    fi
fi
AM_CONDITIONAL([BUILD_TESTS],[test "x$enable_tests" = "xyes"])

###########################
# Check to see if we're local
###########################

with_localinstall="no"
AC_ARG_ENABLE(localinstall, AS_HELP_STRING([--enable-localinstall], [install all of the files localy instead of system directories (for distcheck)]), with_localinstall=$enableval, with_localinstall=no)

###########################
# Indicator Info
###########################

if test "x$with_localinstall" = "xyes"; then
	INDICATORDIR="${libdir}/indicators/2/"
	INDICATORICONSDIR="${datadir}/icons/"
else
	INDICATORDIR=`$PKG_CONFIG --variable=indicatordir indicator3-0.4`
	INDICATORICONSDIR=`$PKG_CONFIG --variable=iconsdir indicator3-0.4`
fi
AC_SUBST(INDICATORDIR)
AC_SUBST(INDICATORICONSDIR)

###########################
# DBus Service Info
###########################

if test "x$with_localinstall" = "xyes"; then
	DBUSSERVICEDIR="${datadir}/dbus-1/services/"
else
	DBUSSERVICEDIR=`$PKG_CONFIG --variable=session_bus_services_dir dbus-1`
fi
AC_SUBST(DBUSSERVICEDIR)

##############################
# Custom Junk
##############################

AC_DEFUN([AC_DEFINE_PATH], [
	test "x$prefix" = xNONE && prefix="$ac_default_prefix"
	test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
	ac_define_path=`eval echo [$]$2`
	ac_define_path=`eval echo [$]ac_define_path`
	$1="$ac_define_path"
	AC_SUBST($1)
	ifelse($3, ,
		AC_DEFINE_UNQUOTED($1, "$ac_define_path"),
		AC_DEFINE_UNQUOTED($1, "$ac_define_path", $3))
])

###########################
# Internationalization
###########################

GETTEXT_PACKAGE=indicator-sync
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of the default get text domain])
AC_DEFINE_PATH(GNOMELOCALEDIR, "${datadir}/locale", [locale directory])

AM_GLIB_GNU_GETTEXT

###########################
# Files
###########################

AM_CONDITIONAL([HAS_PYTHON], [test "x$have_python" = "xyes"])

AC_OUTPUT([Makefile
           indicator-sync/Makefile
           indicator-sync/indicator-sync-client.pc.in
           data/Makefile
           data/icons/Makefile
           data/icons/22x22/Makefile
           data/icons/22x22/status/Makefile
           docs/Makefile
           docs/reference/Makefile
           docs/reference/libindicator-sync-client/Makefile
           examples/Makefile
           test/Makefile
           po/Makefile.in])

###########################
# Results
###########################

AC_MSG_NOTICE([

Sync Indicator Configuration:

	Prefix:           $prefix
	Indicator Dir:    $INDICATORDIR
	Python Bindings:  $have_python
	gtest:            $enable_tests
	gcov:             $use_gcov
])
