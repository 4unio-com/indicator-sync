
synclib_LTLIBRARIES = libsyncindicator.la

synclibdir = $(INDICATORDIR)

libsyncindicator_la_SOURCES = \
  $(top_builddir)/src/sync-service-dbus.c \
  $(top_builddir)/src/sync-service-dbus.h \
  indicator-sync.c

libsyncindicator_la_CFLAGS = \
  -I$(top_srcdir)/src \
  -I$(top_builddir)/src \
  $(INDICATOR_DEPS_CFLAGS) \
  $(COVERAGE_CFLAGS) \
  -Wall -Werror \
  -Wl,-Bsymbolic-functions \
  -Wl,-z,defs \
  -Wl,--as-needed \
  -DG_LOG_DOMAIN=\"Sync-Indicator\"

libsyncindicator_la_LIBADD = \
  $(INDICATOR_DEPS_LIBS)

libsyncindicator_la_LDFLAGS = \
  $(COVERAGE_LDFLAGS) \
  -module \
  -avoid-version

