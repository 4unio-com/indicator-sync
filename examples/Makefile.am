
check_PROGRAMS = sync-app-example

sync_app_example_SOURCES = \
  sync-app-example.c

sync_app_example_CPPFLAGS = \
  $(SYNC_LIB_DEPS_CFLAGS) \
  -I$(top_srcdir)/src

sync_app_example_CFLAGS = \
  -Wall

sync_app_example_LDADD = \
  $(top_builddir)/src/libsync-menu/libsync-menu.la \
  $(SYNC_LIB_DEPS_LIBS)

examplesdir = $(docdir)/examples/

