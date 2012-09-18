BUILT_SOURCES =
EXTRA_DIST =
CLEANFILES =

##
##

SUBDIRS = sync-menu service indicator

noinst_HEADERS = dbus-shared.h

##
##

sync_service_dbus_sources = \
  sync-service-dbus.c \
  sync-service-dbus.h

$(sync_service_dbus_sources): sync-service.xml
	$(AM_V_GEN) gdbus-codegen \
	  --c-namespace Dbus \
	  --interface-prefix com.canonical.indicator \
	  --generate-c-code sync-service-dbus \
	  $^

BUILT_SOURCES += $(sync_service_dbus_sources)
CLEANFILES += $(sync_service_dbus_sources)
EXTRA_DIST += sync-service.xml

##
##

sync_app_dbus_sources = \
  sync-app-dbus.c \
  sync-app-dbus.h

$(sync_app_dbus_sources): sync-app.xml
	$(AM_V_GEN) gdbus-codegen \
	  --c-namespace DbusSyncMenu \
	  --interface-prefix com.canonical.indicator.sync \
	  --generate-c-code sync-app-dbus \
	  $^

BUILT_SOURCES += $(sync_app_dbus_sources)
CLEANFILES += $(sync_app_dbus_sources)
EXTRA_DIST += sync-app.xml

