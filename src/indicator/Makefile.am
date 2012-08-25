
synclib_LTLIBRARIES = libsyncindicator.la

synclibdir = $(INDICATORDIR)

libsyncindicator_la_SOURCES = \
  ../sync-service-dbus.c \
  ../sync-service-dbus.h \
  indicator-sync.c

libsyncindicator_la_CFLAGS = \
  $(INDICATOR_DEPS_CFLAGS) \
  -I$(srcdir)/.. \
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

