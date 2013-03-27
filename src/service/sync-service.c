/*
   A service which aggregates all the SyncMenuApps' data together
   for consumption by the Sync Indicator

   Copyright 2012 Canonical Ltd.

   Authors:
     Charles Kerr <charles.kerr@canonical.com>

   This program is free software: you can redistribute it and/or modify it 
   under the terms of the GNU General Public License version 3,
   as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranties of
   MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along 
   with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include <locale.h>
#include <libintl.h>

#include <gio/gio.h>
#include <glib/gi18n.h>

#include <libdbusmenu-glib/client.h>
#include <libdbusmenu-glib/menuitem.h>
#include <libdbusmenu-glib/server.h>

#include <libindicator/indicator-service.h>

#include "sync-menu/sync-app.h"
#include "sync-menu/sync-enum.h"

#include "app-menu-item.h"
#include "dbus-shared.h"
#include "sync-app-dbus.h"
#include "sync-service-dbus.h"

/***
****
***/

/* bookkeeping for each SyncMenuApp */
typedef struct ClientEntry
{
  gint watch_id;

  AppMenuItem * app_menu_item;

  DbusmenuClient * menu_client;
  gulong menu_client_root_handler_id;

  DbusSyncMenuApp * sync_menu_app;
  gulong sync_menu_app_menu_handler_id;
  gulong sync_menu_app_state_handler_id;
  gulong sync_menu_app_paused_handler_id;
}
ClientEntry;

/* the main service struct */
typedef struct SyncService
{
  GMainLoop * mainloop;

  IndicatorService * indicator_service;
  DbusmenuServer * menu_server;
  DbusSyncService * skeleton;
  guint signal_subscription;

  GSList * client_entries;
}
SyncService;

static SyncService sync_service;

static void entry_clear_menu_client (ClientEntry * entry);


/****
*****  Various ways to look up a ClientEntry
****/

typedef gint (entry_compare_func)(const ClientEntry * entry, gconstpointer key);

static gint
entry_compare_to_object_path (const ClientEntry * entry, gconstpointer path)
{
  GDBusProxy * proxy = G_DBUS_PROXY (entry->sync_menu_app);

  return g_strcmp0 (g_dbus_proxy_get_object_path(proxy), path);
}
static gint
entry_compare_to_dbus_name (const ClientEntry * entry, gconstpointer name)
{
  GDBusProxy * proxy = G_DBUS_PROXY (entry->sync_menu_app);

  return g_strcmp0 (g_dbus_proxy_get_name(proxy), name);
}
static gint
entry_compare_to_menu_client (const ClientEntry * entry, gconstpointer client)
{
  const ptrdiff_t diff = (gconstpointer)(entry->menu_client) - client;
  if (diff < 0) return -1;
  if (diff > 0) return  1;
  return 0;
}

struct name_and_path
{
  const gchar * name;
  const gchar * path;
};

static gint
entry_compare_to_name_and_path (const ClientEntry * entry, gconstpointer key)
{
  int i;
  const struct name_and_path * nap = key;

  /* primary key */
  if (( i = entry_compare_to_dbus_name (entry, nap->name)))
    return i;

  /* secondary key */
  return entry_compare_to_object_path (entry, nap->path);
}

static ClientEntry *
entry_find (SyncService * service, entry_compare_func func, gconstpointer key)
{
  GSList * l;
  ClientEntry * match = NULL;

  for (l=service->client_entries; l!=NULL; l=l->next)
    {
      if (func (l->data, key) == 0)
        {
          match = l->data;
          break;
        }
    }

  return match;
}
static ClientEntry *
entry_find_from_dbus_name (SyncService * service, const gchar * name)
{
  return entry_find (service, entry_compare_to_dbus_name, name);
}
static ClientEntry *
entry_find_from_menu_client (SyncService * service, DbusmenuClient * client)
{
  return entry_find (service, entry_compare_to_menu_client, client);
}
static ClientEntry *
entry_find_from_name_and_path (SyncService  * service,
                               const gchar  * name,
                               const gchar  * path)
{
  struct name_and_path nap;
  nap.name = name;
  nap.path = path;
  return entry_find (service, entry_compare_to_name_and_path, &nap);
}

static gint
entry_compare_by_appname (gconstpointer ga, gconstpointer gb)
{
  const ClientEntry * const a = ga;
  const ClientEntry * const b = gb;
  return g_strcmp0 (app_menu_item_get_name(a->app_menu_item),
                    app_menu_item_get_name(b->app_menu_item));
}

static SyncMenuState
entry_get_state (ClientEntry * entry)
{
  g_return_val_if_fail (entry->sync_menu_app != NULL, SYNC_MENU_STATE_IDLE);

  return dbus_sync_menu_app_get_state (entry->sync_menu_app);
}

static gboolean
entry_get_paused (ClientEntry * entry)
{
  g_return_val_if_fail (entry->sync_menu_app != NULL, FALSE);

  return dbus_sync_menu_app_get_paused (entry->sync_menu_app);
}


/****
*****
****/

static inline void
menuitem_set_visible (DbusmenuMenuitem * mi, gboolean v)
{
  dbusmenu_menuitem_property_set_bool (mi, DBUSMENU_MENUITEM_PROP_VISIBLE, v);
}

static void
menuitem_unparent (DbusmenuMenuitem * mi)
{
  DbusmenuMenuitem * parent = dbusmenu_menuitem_get_parent (mi);
  if (parent != NULL)
    {
      dbusmenu_menuitem_child_delete (parent, mi);
    }
}

static void
service_menu_append_client_menu (DbusmenuMenuitem  * root,
                                 DbusmenuClient    * menu_client)
{
  g_return_if_fail (menu_client != NULL);

  DbusmenuMenuitem * mi = dbusmenu_client_get_root (menu_client);
  if (mi != NULL)
    {
      GList * l;
      GList * children = dbusmenu_menuitem_get_children (mi);
      if (children == NULL)
        {
          mi = DBUSMENU_MENUITEM(dbusmenu_menuitem_proxy_new (mi));
          dbusmenu_menuitem_child_append (root, mi);
        }
      else for (l=children; l!=NULL; l=l->next)
        {
          mi = DBUSMENU_MENUITEM(l->data);
          mi = DBUSMENU_MENUITEM(dbusmenu_menuitem_proxy_new (mi));
          dbusmenu_menuitem_child_append (root, mi);
        }
    }
}

static void
service_refresh_menu (SyncService * service)
{
  GSList * l;
  GSList * entries;
  g_debug (G_STRLOC" rebuilding the menu");

  /* get an alphabetically sorted list of SyncMenuApps */
  entries = g_slist_copy (service->client_entries);
  entries = g_slist_sort (entries, entry_compare_by_appname);

  /* build the new menu */
  DbusmenuMenuitem * root = dbusmenu_menuitem_new ();
  for (l=entries; l!=NULL; l=l->next)
    {
      ClientEntry * entry = l->data;

      /* add the client's app menuitem */
      DbusmenuMenuitem * mi = DBUSMENU_MENUITEM (entry->app_menu_item);
      menuitem_unparent (mi);
      menuitem_set_visible (mi, TRUE);
      dbusmenu_menuitem_child_append (root, mi);

      /* add the client's custom menuitems */
      if (entry->menu_client != NULL)
        service_menu_append_client_menu (root, entry->menu_client);

      /* add a separator before the next client */
      if (l->next != NULL)
        {
          DbusmenuMenuitem * sep = dbusmenu_menuitem_new ();
          menuitem_set_visible (mi, TRUE);
          dbusmenu_menuitem_property_set (sep,
                                          DBUSMENU_MENUITEM_PROP_TYPE,
                                          DBUSMENU_CLIENT_TYPES_SEPARATOR);
          dbusmenu_menuitem_child_append (root, sep);
        }
    }

  dbusmenu_server_set_root (service->menu_server, root);

  /* cleanup */
  g_slist_free (entries);
  g_object_unref (root);
}

static SyncMenuState
service_calculate_state (SyncService * service)
{
  GSList * l;

  /* if any service is in error state... */
  for (l=service->client_entries; l!=NULL; l=l->next)
    if (entry_get_state (l->data) == SYNC_MENU_STATE_ERROR)
      return SYNC_MENU_STATE_ERROR;

  /* otherwise if any service is syncing... */
  for (l=service->client_entries; l!=NULL; l=l->next)
    if (entry_get_state (l->data) == SYNC_MENU_STATE_SYNCING)
      return SYNC_MENU_STATE_SYNCING;

  return SYNC_MENU_STATE_IDLE;
}

static void
service_refresh_state (SyncService * service)
{
  const SyncMenuState new_state = service_calculate_state (service);

  dbus_sync_service_set_state (service->skeleton, new_state);
}

static gboolean
service_calculate_paused (SyncService * service)
{
  GSList * l;

  for (l=service->client_entries; l!=NULL; l=l->next)
    if (entry_get_paused (l->data))
      return TRUE;

  return FALSE;
}

static void
service_refresh_paused (SyncService * service)
{
  const gboolean old_paused = dbus_sync_service_get_paused (service->skeleton);
  const gboolean new_paused = service_calculate_paused (service);

  if (old_paused != new_paused)
    dbus_sync_service_set_paused (service->skeleton, new_paused);
}

static void
service_refresh_count (SyncService * service)
{
  const guint new_count = g_slist_length (service->client_entries);

  dbus_sync_service_set_client_count (service->skeleton, new_count);
}

static void
service_refresh (SyncService * service)
{
  service_refresh_menu (service);
  service_refresh_state (service);
  service_refresh_paused (service);
  service_refresh_count (service);
}

/****
*****
****/

static void
signal_handler_clear (gpointer instance, gulong * handler_id)
{
  g_return_if_fail (handler_id != NULL);

  if ((instance != NULL) && *handler_id)
    {
      g_signal_handler_disconnect (instance, *handler_id);
    }

  *handler_id = 0;
}

static void
entry_free (ClientEntry * entry)
{
  g_debug (G_STRLOC " freeing entry %p", entry);

  if (entry->watch_id != 0)
    {
      g_bus_unwatch_name (entry->watch_id);
      entry->watch_id = 0;
    }

  signal_handler_clear (entry->sync_menu_app, &entry->sync_menu_app_menu_handler_id);
  signal_handler_clear (entry->sync_menu_app, &entry->sync_menu_app_state_handler_id);
  signal_handler_clear (entry->sync_menu_app, &entry->sync_menu_app_paused_handler_id);
  g_clear_object (&entry->app_menu_item);
  g_clear_object (&entry->sync_menu_app);

  entry_clear_menu_client (entry);

  g_free (entry);
}

static void
service_remove_entry (SyncService * service, ClientEntry * entry)
{
  g_return_if_fail (service != NULL);
  g_return_if_fail (entry != NULL);

  service->client_entries = g_slist_remove (service->client_entries, entry);
  service_refresh (service);

  entry_free (entry);
}

static void
service_add_entry (SyncService * service, ClientEntry * entry)
{
  service->client_entries = g_slist_prepend (service->client_entries, entry);
  service_refresh (service);
}

static void
on_client_menu_root_changed (DbusmenuClient    * client,
                             DbusmenuMenuitem  * newroot     G_GNUC_UNUSED,
                             gpointer            user_data)
{
  SyncService * service = user_data;
  ClientEntry * entry = entry_find_from_menu_client (service, client);
  g_return_if_fail (entry != NULL);

  g_debug (G_STRLOC " SyncMenuApp %s changed its menu root",
           app_menu_item_get_name(entry->app_menu_item));
  service_refresh_menu (service);
}

static void
entry_create_menu_client (SyncService * service,
                          ClientEntry * entry)
{
  g_return_if_fail (entry != NULL);
  g_return_if_fail (entry->sync_menu_app != NULL);

  const gchar * name = g_dbus_proxy_get_name (G_DBUS_PROXY(entry->sync_menu_app));
  const gchar * path = dbus_sync_menu_app_get_menu_path (entry->sync_menu_app);

  if (name && *name && path && *path)
    {
      DbusmenuClient * client;
      gulong id;

      client = dbusmenu_client_new (name, path);
      id = g_signal_connect (client,
                             DBUSMENU_CLIENT_SIGNAL_ROOT_CHANGED,
                             G_CALLBACK(on_client_menu_root_changed),
                             service);

      entry->menu_client = client;
      entry->menu_client_root_handler_id = id;
    }
}

static void
entry_clear_menu_client (ClientEntry * entry)
{
  signal_handler_clear (entry->menu_client, &entry->menu_client_root_handler_id);
  g_clear_object (&entry->menu_client);
}

static void
on_sync_menu_app_state_changed (GObject * o, GParamSpec * ps, gpointer service)
{
  service_refresh_state (service);
}

static void
on_sync_menu_app_paused_changed (GObject * o, GParamSpec * ps, gpointer service)
{
  service_refresh_paused (service);
}

static void
on_sync_menu_app_menu_path_changed (GObject * o, GParamSpec * ps, gpointer gentry)
{
  SyncService * service = &sync_service;
  DbusSyncMenuApp * sync_menu_app = DBUS_SYNC_MENU_APP(o);
  ClientEntry * entry = gentry;
  g_return_if_fail (sync_menu_app != NULL);

  entry_clear_menu_client (entry);
  entry_create_menu_client (service, entry);
  service_refresh_menu (service);
}

static void
on_sync_menu_app_vanished (GDBusConnection * connection  G_GNUC_UNUSED,
                           const gchar     * dbus_name_in,
                           gpointer          user_data)
{
  ClientEntry * entry;
  SyncService * self = user_data;
  gchar * dbus_name;

  /* service_remove_entry() calls g_bus_unwatch_name() which frees dbus_name_in.
     Make a temporary copy of dbus_name_in to ensure we don't pass a dangling
     pointer to entry_find_from_dbus_name()... */
  dbus_name = g_strdup (dbus_name_in);

  while ((entry = entry_find_from_dbus_name (self, dbus_name)))
    service_remove_entry (self, entry);

  g_free (dbus_name);
}

static ClientEntry *
entry_new (SyncService * service, DbusSyncMenuApp * sync_menu_app)
{
  GDBusProxy * proxy = G_DBUS_PROXY(sync_menu_app);
  ClientEntry * entry = g_new0 (ClientEntry, 1);

  entry->sync_menu_app = sync_menu_app;

  entry->app_menu_item = app_menu_item_new (proxy);

  entry->sync_menu_app_menu_handler_id = g_signal_connect (
    proxy, "notify::menu-path",
    G_CALLBACK(on_sync_menu_app_menu_path_changed), entry);

  entry->sync_menu_app_state_handler_id = g_signal_connect (
    proxy, "notify::state",
    G_CALLBACK(on_sync_menu_app_state_changed), service);

  entry->sync_menu_app_paused_handler_id = g_signal_connect (
    proxy, "notify::paused",
    G_CALLBACK(on_sync_menu_app_paused_changed), service);

  entry_create_menu_client (service, entry);

  entry->watch_id = g_bus_watch_name_on_connection (
    g_dbus_proxy_get_connection (proxy),
    g_dbus_proxy_get_name (proxy),
    G_BUS_NAME_WATCHER_FLAGS_NONE,
    NULL, on_sync_menu_app_vanished,
    service, NULL);

  g_debug (G_STRLOC" created a new proxy for '%s', watch id is %d",
           g_dbus_proxy_get_name(proxy), entry->watch_id);

  return entry;
}

static void
on_sync_menu_app_exists (GDBusConnection * connection,
                       const gchar     * sender,
                       const gchar     * object,
                       const gchar     * interface,
                       const gchar     * signal      G_GNUC_UNUSED,
                       GVariant        * params      G_GNUC_UNUSED,
                       gpointer          user_data)
{
  g_debug (G_STRLOC" got an Exists signal from"
                   " sender {%s}"
                   " object {%s}"
                   " interface {%s}",
                   sender, object, interface);

  SyncService * service = user_data;

  if (entry_find_from_name_and_path (service, sender, object) != NULL)
    {
      g_debug (G_STRLOC" ...which we're already tracking");
      return;
    }
 
  GError * err = NULL;
  g_debug (G_STRLOC" ...which is new to us! Let's add it to our list.");
  DbusSyncMenuApp * proxy = dbus_sync_menu_app_proxy_new_sync (
                             connection,
                             G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                             sender,
                             object,
                             NULL,
                             &err);
  if (err == NULL)
    {
      service_add_entry (service, entry_new (service, proxy));
    }
  else
    {
      g_warning ("couldn't create a proxy for '%s': %s", object, err->message);
      g_clear_error (&err);
    }
}

/****
*****
****/

static void
on_got_bus (GObject * o, GAsyncResult * res, gpointer user_data)
{
  GError * err = NULL;
  SyncService * service = user_data;
  GDBusConnection * connection = g_bus_get_finish (res, &err);

  if (err != NULL)
    {
      g_error ("unable to get bus: %s", err->message);
      g_clear_error (&err);
      g_main_loop_quit (sync_service.mainloop);
    }
  else
    {
      service->indicator_service = indicator_service_new_version (SYNC_SERVICE_DBUS_NAME,
                                                                  1);
      g_signal_connect_swapped (sync_service.indicator_service,
                                INDICATOR_SERVICE_SIGNAL_SHUTDOWN,
                                G_CALLBACK(g_main_loop_quit),
                                sync_service.mainloop);

      g_dbus_interface_skeleton_export (
          G_DBUS_INTERFACE_SKELETON(service->skeleton),
          connection,
          SYNC_SERVICE_DBUS_OBJECT,
          &err);

      if (err != NULL)
        {
          g_error ("unable to get bus: %s", err->message);
          g_clear_error (&err);
        }

      /* listen for SyncMenuApps to show up on the bus */
      g_debug (G_STRLOC" listening to Exists from %s", SYNC_MENU_APP_DBUS_IFACE);
      service->signal_subscription = g_dbus_connection_signal_subscribe (
                                       connection,
                                       NULL, /* sender */
                                       SYNC_MENU_APP_DBUS_IFACE,
                                       "Exists",
                                       NULL, /* path */
                                       NULL, /* arg0 */
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       on_sync_menu_app_exists,
                                       service,
                                       NULL); /* destroy notify */

      /* cleanup */
      g_object_unref (connection);
    }
}

int
main (int argc, char ** argv)
{
  memset (&sync_service, 0, sizeof(sync_service));

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
  textdomain (GETTEXT_PACKAGE);

  sync_service.skeleton = dbus_sync_service_skeleton_new ();

  g_bus_get (G_BUS_TYPE_SESSION, NULL, on_got_bus, &sync_service);

  sync_service.menu_server = dbusmenu_server_new(SYNC_SERVICE_DBUS_MENU_OBJECT);

  /* the main loop */
  sync_service.mainloop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (sync_service.mainloop);

  /* cleanup */
  g_clear_pointer (&sync_service.mainloop, g_main_loop_unref);
  g_slist_free_full (sync_service.client_entries, (GDestroyNotify)entry_free);
  g_clear_object (&sync_service.menu_server);
  g_clear_object (&sync_service.skeleton);
  g_clear_object (&sync_service.indicator_service);
  return 0;
}
