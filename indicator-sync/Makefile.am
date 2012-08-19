CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =
EXTRA_DIST =
lib_LTLIBRARIES =

SHARED_LDFLAGS = \
  $(COVERAGE_LDFLAGS) \
  -module -avoid-version

SHARED_CFLAGS = \
  $(COVERAGE_CFLAGS) \
  -Wall -Werror \
  -Wl,-Bsymbolic-functions \
  -Wl,-z,defs \
  -Wl,--as-needed \
  -DG_LOG_DOMAIN=\"Indicator-Sync\"


######################################
# the client library
######################################

SYNC_CLIENT_API_VERSION = 1
SYNC_CLIENT_DIR = $(libexecdir)
statusprovidersdir = $(SYNC_SOURCE_DIR)

sync_client_dbus_sources = \
        sync-client-dbus.c \
        sync-client-dbus.h

BUILT_SOURCES += $(sync_client_dbus_sources)

$(sync_client_dbus_sources): sync-client.xml
	gdbus-codegen \
	  --c-namespace Dbus \
	  --interface-prefix com.canonical.indicator \
	  --generate-c-code sync-client-dbus \
	  $^

EXTRA_DIST += sync-client.xml

indicator_sync_client_libdir = $(INDICATORDIR)
lib_LTLIBRARIES += libindicator-sync-client.la

libindicator_sync_client_la_SOURCES = \
  dbus-shared.h \
  sync-client.c \
  sync-client-dbus.c \
  sync-client-dbus.h \
  sync-enum.c

libindicator_sync_client_la_LIBADD = -lm

libindicator_sync_client_la_CFLAGS = \
  $(SYNC_CLIENT_CFLAGS) \
  $(SHARED_CFLAGS)

libindicator_sync_client_la_LDFLAGS = \
  $(SYNC_CLIENT_LIBS) \
  $(SHARED_LDFLAGS)

libindicator_sync_client_la_includedir = \
  $(includedir)/indicator-sync-0.1/indicator-sync

libindicator_sync_client_la_headers = \
  sync-client.h \
  sync-enum.h

EXTRA_DIST += indicator-sync-client.pc.in.in
CLEANFILES += indicator-sync-client.pc

pkgconfig_DATA = indicator-sync-client.pc
pkgconfigdir = $(libdir)/pkgconfig

%.pc: %.pc.in
	sed \
		-e "s|\@sync_client_dir\@|$(SYNC_CLIENT_DIR)|" \
		-e "s|\@sync_client_api_version\@|$(SYNC_CLIENT_API_VERSION)|" \
		$< > $@


######################################
# the service
######################################

libexec_PROGRAMS = indicator-sync-service

sync_service_dbus_sources = \
  sync-service-dbus.c \
  sync-service-dbus.h

$(sync_service_dbus_sources): sync-service.xml
	gdbus-codegen \
	  --c-namespace Dbus \
	  --interface-prefix com.canonical.indicator \
	  --generate-c-code sync-service-dbus \
	  $^

indicator_sync_service_SOURCES = \
	app-menu-item.c \
	app-menu-item.h \
	dbus-shared.h \
	sync-service.c \
	sync-enum.c \
	sync-enum.h \
	$(sync_client_dbus_sources) \
	$(sync_service_dbus_sources)

indicator_sync_service_CFLAGS = \
	$(SYNC_SERVICE_CFLAGS) \
	$(SHARED_CFLAGS)
indicator_sync_service_LDADD = \
	$(SYNC_SERVICE_LIBS)
indicator_sync_service_LDFLAGS = \
	$(SHARED_LDFLAGS)

BUILT_SOURCES += $(sync_service_dbus_sources)
EXTRA_DIST += sync-service.xml

######################################
# the indicator
######################################

synclibdir = $(INDICATORDIR)
synclib_LTLIBRARIES = libsyncindicator.la
libsyncindicator_la_SOURCES = \
	$(sync_service_dbus_sources) \
	indicator-sync.c
libsyncindicator_la_CFLAGS = \
	$(SYNC_INDICATOR_CFLAGS) \
	$(SHARED_CFLAGS)
libsyncindicator_la_LIBADD = \
	$(SYNC_INDICATOR_LIBS)
libsyncindicator_la_LDFLAGS = \
	$(SHARED_LDFLAGS)

######################################
# Extras
######################################

CLEANFILES += \
  $(BUILT_SOURCES)

