CLEANFILES =
EXTRA_DIST =

###
###  installed library
###

lib_LTLIBRARIES = libsync-menu.la

libsync_menu_la_LIBADD = -lm

libsync_menu_la_LDFLAGS = \
  $(SYNC_LIB_DEPS_LIBS) \
  $(COVERAGE_LDFLAGS) \
  -version-info 1:0:0 \
  -no-undefined \
  -export-symbols-regex "^[^_d].*"

libsync_menu_la_CFLAGS = \
  -I$(top_srcdir)/src \
  -I$(top_builddir)/src \
  $(SYNC_LIB_DEPS_CFLAGS) \
  $(COVERAGE_CFLAGS) \
  -Wall -Werror \
  -Wl,-Bsymbolic-functions \
  -Wl,-z,defs \
  -Wl,--as-needed \
  -DG_LOG_DOMAIN=\"libsync-menu\"

libsync_menu_la_SOURCES = \
  $(top_builddir)/src/sync-app-dbus.c \
  $(top_builddir)/src/sync-app-dbus.h \
  $(top_srcdir)/src/dbus-shared.h \
  sync-app.c \
  sync-enum.c


###
###  installed headers
###

libsync_menu_la_includedir = \
  $(includedir)/sync-menu-0.1/sync-menu

libsync_menu_la_include_HEADERS = \
  sync-app.h \
  sync-enum.h


###
###  pkgconfig
###

pcfile = sync-menu-0.1.pc
libsync_menu_la_pcdir = $(libdir)/pkgconfig
libsync_menu_la_pc_DATA = $(pcfile)
CLEANFILES += $(pcfile)
EXTRA_DIST += $(pcfile).in


###
###  GIR
###


-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(libsync_menu_la_SOURCES)

SyncMenu-12.10.gir: libsync-menu.la
SyncMenu_12_10_gir_VERSION = @GIR_VERSION@
SyncMenu_12_10_gir_INCLUDES = GObject-2.0 Dbusmenu-0.4
SyncMenu_12_10_gir_CFLAGS = $(INCLUDES) -I$(srcdir)/..
SyncMenu_12_10_gir_LIBS = libsync-menu.la
SyncMenu_12_10_gir_PACKAGES = dbusmenu-glib-0.4 glib-2.0 
SyncMenu_12_10_gir_FILES = $(introspection_sources)
INTROSPECTION_GIRS += SyncMenu-12.10.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

