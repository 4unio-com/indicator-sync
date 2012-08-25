
check_PROGRAMS = sync-app-example

sync_app_example_SOURCES = \
  sync-app-example.c

sync_app_example_CPPFLAGS = \
  -I$(top_srcdir)/src \
  $(SYNC_LIB_DEPS_CFLAGS)

sync_app_example_CFLAGS = \
  -Wall -Werror

sync_app_example_LDADD = \
  $(top_builddir)/src/sync-menu/libsync-menu.la \
  $(SYNC_LIB_DEPS_LIBS)

examplesdir = $(docdir)/examples/

