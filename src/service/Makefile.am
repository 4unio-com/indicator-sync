
libexec_PROGRAMS = indicator-sync-service

indicator_sync_service_SOURCES = \
  ../sync-service-dbus.c \
  ../sync-service-dbus.h \
  ../sync-app-dbus.c \
  ../sync-app-dbus.h \
  ../sync-menu/sync-enum.c \
  ../sync-menu/sync-enum.h \
  app-menu-item.c \
  app-menu-item.h \
  dbus-shared.h \
  sync-service.c

indicator_sync_service_CFLAGS = \
  $(SERVICE_DEPS_CFLAGS) \
  $(COVERAGE_CFLAGS) \
  -I$(srcdir)/.. \
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

