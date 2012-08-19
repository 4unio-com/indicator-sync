
check_PROGRAMS = sync-client-demo

sync_client_demo_SOURCES = \
  sync-client-demo.c

sync_client_demo_CFLAGS = \
  -I $(top_srcdir) \
  -Wall \
  $(SYNC_CLIENT_CFLAGS)

sync_client_demo_LDADD = \
  $(top_builddir)/indicator-sync/libindicator-sync-client.la \
  $(SYNC_CLIENT_LIBS)

python_demos = \
  sync-client-demo.py

examplesdir = $(docdir)/examples/

EXTRA_DIST = $(python_examples)

