
pkglibexec_PROGRAMS = indicator-sync-service

indicator_sync_service_SOURCES = \
  $(top_builddir)/src/sync-service-dbus.c \
  $(top_builddir)/src/sync-service-dbus.h \
  $(top_builddir)/src/sync-app-dbus.c \
  $(top_builddir)/src/sync-app-dbus.h \
  $(top_srcdir)/src/sync-menu/sync-enum.c \
  $(top_srcdir)/src/sync-menu/sync-enum.h \
  $(top_srcdir)/src/dbus-shared.h \
  app-menu-item.c \
  app-menu-item.h \
  sync-service.c

indicator_sync_service_CFLAGS = \
  -I$(top_srcdir)/src \
  -I$(top_builddir)/src \
  $(SERVICE_DEPS_CFLAGS) \
  $(COVERAGE_CFLAGS) \
  -Wall -Werror \
  -Wl,-Bsymbolic-functions \
  -Wl,-z,defs \
  -Wl,--as-needed \
  -DG_LOG_DOMAIN=\"Sync-Service\"

indicator_sync_service_LDADD = \
  $(SERVICE_DEPS_LIBS)

indicator_sync_service_LDFLAGS = \
  $(COVERAGE_LDFLAGS) \
  -module \
  -avoid-version

