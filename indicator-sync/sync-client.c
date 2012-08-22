/*
   The way for apps to interact with the Sync Indicator

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

#include <string.h> /* strstr() */

#include <glib.h>

#include <libdbusmenu-glib/dbusmenu-glib.h>

#include "dbus-shared.h"
#include "sync-client.h"
#include "sync-client-dbus.h"
#include "sync-enum.h"

struct _SyncClientPriv
{
  guint             watch_id;
  GDBusConnection * session_bus;
  guint             signal_subscription;
  DbusSyncClient  * skeleton;
  DbusmenuServer  * menu_server;
  GBinding        * menu_binding;
  gchar           * desktop_id;
  SyncState         state;
  gboolean          paused;
  GCancellable    * cancellable;
};

enum
{
  PROP_0,
  PROP_STATE,
  PROP_PAUSED,
  PROP_DESKTOP_ID,
  PROP_DBUSMENU,
  N_PROPERTIES
};

static GParamSpec * properties[N_PROPERTIES];

/* GObject stuff */
static void sync_client_class_init (SyncClientClass * klass);
static void sync_client_init       (SyncClient *self);
static void sync_client_dispose    (GObject *object);
static void sync_client_finalize   (GObject *object);
static void set_property (GObject*, guint prop_id, const GValue*, GParamSpec* );
static void get_property (GObject*, guint prop_id,       GValue*, GParamSpec* );

G_DEFINE_TYPE (SyncClient, sync_client, G_TYPE_OBJECT);

static void
sync_client_class_init (SyncClientClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (SyncClientPriv));

  object_class->dispose = sync_client_dispose;
  object_class->finalize = sync_client_finalize;
  object_class->set_property = set_property;
  object_class->get_property = get_property;

  properties[PROP_STATE] = g_param_spec_enum (
    SYNC_CLIENT_PROP_STATE,
    "State",
    "The SyncState that represents this client's state",
    sync_state_get_type(),
    SYNC_STATE_IDLE,
    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_PAUSED] = g_param_spec_boolean (
    SYNC_CLIENT_PROP_PAUSED,
    "Paused",
    "Whether or not this client is paused",
    FALSE,
    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  properties[PROP_DESKTOP_ID] = g_param_spec_string (
    SYNC_CLIENT_PROP_DESKTOP_ID,
    "Desktop Id",
    "The name of the .desktop file that belongs to the client app",
    NULL,
    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  properties[PROP_DBUSMENU] = g_param_spec_object (
    SYNC_CLIENT_PROP_DBUSMENU,
    "MenuItems",
    "The extra menuitems to display in the client's section in the Sync Indicator",
    DBUSMENU_TYPE_SERVER,
    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
sync_client_dispose (GObject *object)
{
  SyncClient * self = SYNC_CLIENT(object);
  g_return_if_fail (self != NULL);
  SyncClientPriv * p = self->priv;

  sync_client_set_menu (self, NULL);

  if (p->signal_subscription)
    {
      g_dbus_connection_signal_unsubscribe (p->session_bus,
                                            p->signal_subscription);
      p->signal_subscription = 0;
    }

  if (p->session_bus)
    {
      GDBusInterfaceSkeleton * s = G_DBUS_INTERFACE_SKELETON(p->skeleton);

      if (g_dbus_interface_skeleton_has_connection (s, p->session_bus))
        {
          g_dbus_interface_skeleton_unexport (s);
        }
    }

  if (p->cancellable != NULL)
    {
      g_cancellable_cancel (p->cancellable);
      g_clear_object (&p->cancellable);
    }

  if (p->watch_id != 0 )
    {
      g_bus_unwatch_name (p->watch_id);
      p->watch_id = 0;
    }

  g_clear_object (&p->session_bus);
  g_clear_object (&p->skeleton);

  G_OBJECT_CLASS (sync_client_parent_class)->dispose (object);
}

static void
sync_client_finalize (GObject *object)
{
  SyncClient * self = SYNC_CLIENT(object);
  g_return_if_fail (self != NULL);
  SyncClientPriv * p = self->priv;

  g_clear_pointer (&p->desktop_id, g_free);

  G_OBJECT_CLASS (sync_client_parent_class)->finalize (object);
}

static gchar*
build_path_from_desktop_id (const gchar * desktop_id)
{
  /* get the basename in case they passed a full filename
     instead of just the desktop id */
  gchar * base = g_path_get_basename (desktop_id);

  /* trim the .desktop off the end */
  gchar * p = strstr (base, ".desktop");
  if (p != NULL)
    {
      *p = '\0';
    }

  /* dbus names only allow alnum + underscores */
  for (p=base; p && *p; ++p)
    {
      if (!g_ascii_isalnum(*p))
        {
          *p = '_';
        }
    }

  p = g_strdup_printf( "/com/canonical/indicator/sync/source/%s", base);
  g_free (base);
  g_debug (G_STRLOC" built path {%s} from desktop id {%s}", p, desktop_id);
  return p;
}

static void
export_if_ready (SyncClient * client)
{
  SyncClientPriv * p = client->priv;
  GDBusInterfaceSkeleton * s = G_DBUS_INTERFACE_SKELETON(p->skeleton);
  const gchar * const desktop_id = p->desktop_id;

  if ((desktop_id != NULL) &&
      (p->session_bus != NULL) &&
      (s != NULL) &&
      (!g_dbus_interface_skeleton_has_connection (s, p->session_bus)))
    {
      GError * err = NULL;
      gchar * path = build_path_from_desktop_id (desktop_id);
      g_dbus_interface_skeleton_export (s, p->session_bus, path, &err);

      if (err == NULL)
        {
          /* tell the world that we're here */
          dbus_sync_client_emit_exists (p->skeleton);
        }
      else
        { 
          g_error ("unable to export skeleton: %s", err->message);
          g_clear_error (&err);
        }
    }
}

static void
on_sync_service_name_appeared (GDBusConnection * connection, const gchar * name, const gchar * name_owner, gpointer user_data)
{
  export_if_ready (SYNC_CLIENT(user_data));
}

static void
on_sync_service_name_vanished (GDBusConnection * connection, const gchar * name, gpointer user_data)
{
  SyncClient * client = SYNC_CLIENT(user_data);
  SyncClientPriv * p = client->priv;
  GDBusInterfaceSkeleton * s = G_DBUS_INTERFACE_SKELETON(p->skeleton);

  if (g_dbus_interface_skeleton_has_connection (s, p->session_bus))
    {
      g_dbus_interface_skeleton_unexport (s);
    }
}

static void
on_got_bus (GObject * o, GAsyncResult * res, gpointer user_data)
{
  SyncClient * client = SYNC_CLIENT(user_data);
  SyncClientPriv * p = client->priv;

  GError * err = NULL;
  p->session_bus = g_bus_get_finish (res, &err);
  if (err != NULL)
    { 
      g_error ("unable to get bus: %s", err->message);
      g_clear_error (&err);
    }
  else
    {
      p->watch_id = g_bus_watch_name_on_connection (p->session_bus,
                                                    SYNC_SERVICE_DBUS_NAME,
                                                    G_BUS_NAME_WATCHER_FLAGS_AUTO_START,
                                                    on_sync_service_name_appeared,
                                                    on_sync_service_name_vanished,
                                                    client, NULL);
      export_if_ready (client);
    }
}

static void
sync_client_init (SyncClient * client)
{
  SyncClientPriv * p;

  p = G_TYPE_INSTANCE_GET_PRIVATE (client, SYNC_CLIENT_TYPE, SyncClientPriv);
  client->priv = p;

  p->paused = FALSE;
  p->menu_server = NULL;
  p->desktop_id = NULL;
  p->skeleton = dbus_sync_client_skeleton_new ();
  p->state = SYNC_STATE_IDLE;
  p->cancellable = g_cancellable_new();

  g_object_bind_property (client,      SYNC_CLIENT_PROP_PAUSED,
                          p->skeleton, SYNC_CLIENT_PROP_PAUSED,
                          G_BINDING_BIDIRECTIONAL|G_BINDING_SYNC_CREATE);

  g_object_bind_property (client,      SYNC_CLIENT_PROP_STATE,
                          p->skeleton, SYNC_CLIENT_PROP_STATE,
                          G_BINDING_SYNC_CREATE);

  g_bus_get (G_BUS_TYPE_SESSION, p->cancellable, on_got_bus, client);
}

/**
 * sync_client_new:
 * @desktop_id: A desktop id as described by g_desktop_app_info_new(),
 *              such as "transmission-gtk.desktop"
 *
 * Creates a new #SyncClient object. Applications wanting to interact
 * with the Sync Indicator should instantiate one of these and use it.
 *
 * The initial state is %SYNC_STATE_IDLE, unpaused, and with no menu.
 *
 * Returns: transfer full: a new SyncClient for the desktop id.
 *                         Free the returned object with g_object_unref().
 */
SyncClient *
sync_client_new (const char * desktop_id)
{
  GObject * o = g_object_new (SYNC_CLIENT_TYPE,
                              SYNC_CLIENT_PROP_DESKTOP_ID, desktop_id,
                              NULL);

  return SYNC_CLIENT(o);
}

/***
****
***/

static void
get_property (GObject     * o,
              guint         prop_id,
              GValue      * value,
              GParamSpec  * pspec)
{
  SyncClient * client = SYNC_CLIENT(o);

  switch (prop_id)
    {
      case PROP_STATE:
        g_value_set_enum (value, client->priv->state);
        break;

      case PROP_PAUSED:
        g_value_set_boolean (value, client->priv->paused);
        break;

      case PROP_DESKTOP_ID:
        g_value_set_string (value, client->priv->desktop_id);
        break;

      case PROP_DBUSMENU:
        g_value_set_object (value, sync_client_get_menu(client));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(o, prop_id, pspec);
        break;
    }
}

static void
set_property (GObject       * o,
              guint           prop_id,
              const GValue  * value,
              GParamSpec    * pspec)
{
  SyncClient * client = SYNC_CLIENT(o);
  SyncClientPriv * p = client->priv;

  switch (prop_id)
    {
      case PROP_STATE:
        sync_client_set_state (client, g_value_get_enum(value));
        break;

      case PROP_PAUSED:
        sync_client_set_paused (client, g_value_get_boolean (value));
        break;

      case PROP_DBUSMENU:
        sync_client_set_menu (client, DBUSMENU_SERVER(g_value_get_object(value)));
        break;

      case PROP_DESKTOP_ID:
        g_return_if_fail (p->desktop_id == NULL); /* ctor only */
        p->desktop_id = g_value_dup_string (value);
        g_debug (G_STRLOC" setting desktop_id to '%s'", p->desktop_id);
        dbus_sync_client_set_desktop (p->skeleton, p->desktop_id);
        export_if_ready (client);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(o, prop_id, pspec);
        break;
    }
}

/**
 * sync_client_get_paused:
 * @client: a #SyncClient
 *
 * Returns: the client's current 'paused' property.
 */
gboolean
sync_client_get_paused (SyncClient * client)
{
  g_return_val_if_fail (IS_SYNC_CLIENT(client), FALSE);

  return client->priv->paused;
}

/**
 * sync_client_get_state:
 * @client: a #SyncClient
 *
 * Returns: the client's current #SyncState, such as %SYNC_STATE_IDLE
 */
SyncState
sync_client_get_state (SyncClient * client)
{
  g_return_val_if_fail (IS_SYNC_CLIENT(client), SYNC_STATE_ERROR);

  return client->priv->state;
}

/**
 * sync_client_get_desktop_id:
 * @client: a #SyncClient
 *
 * Returns: (transfer none): the client's desktop id
 */
const gchar*
sync_client_get_desktop_id (SyncClient * client)
{
  g_return_val_if_fail (IS_SYNC_CLIENT(client), NULL);

  return client->priv->desktop_id;
}

/**
 * sync_client_get_menu:
 * @client: a #SyncClient
 *
 * Returns: (transfer none): the client's #DbusmenuServer
 */
DbusmenuServer*
sync_client_get_menu (SyncClient * client)
{
  g_return_val_if_fail (IS_SYNC_CLIENT(client), NULL);

  return client->priv->menu_server;
}

/**
 * sync_client_set_paused:
 * @client: a #SyncClient
 * @paused: a boolean of whether or not the client is paused
 *
 * Sets the client's SyncClient:paused property
 */
void
sync_client_set_paused (SyncClient * client, gboolean paused)
{
  g_return_if_fail (IS_SYNC_CLIENT(client));
  SyncClientPriv * p = client->priv;

  if (p->paused != paused)
    {
      p->paused = paused;
      g_object_notify_by_pspec (G_OBJECT(client), properties[PROP_PAUSED]);
    }
}

/**
 * sync_client_set_state:
 * @client: a #SyncClient
 * @state: the client's new #SyncState, such as %SYNC_STATE_IDLE
 *
 * Sets the client's SyncClient:paused property
 */
void
sync_client_set_state (SyncClient * client, SyncState state)
{
  g_return_if_fail (IS_SYNC_CLIENT(client));
  SyncClientPriv * p = client->priv;

  if (p->state != state)
    {
      p->state = state;
      g_object_notify_by_pspec (G_OBJECT(client), properties[PROP_STATE]);
    }
}

/**
 * sync_client_set_menu:
 * @client: a #SyncClient
 * @menu_server: a #DbusmenuServer of the menu to be exported by the #SyncClient
 *
 * Sets the client's SyncClient:menu-path property specifying which menu
 * to export to the sync indicator.
 */
void
sync_client_set_menu (SyncClient * client, DbusmenuServer * menu_server)
{
  g_return_if_fail (IS_SYNC_CLIENT(client));
  SyncClientPriv * p = client->priv;

  if (p->menu_server != menu_server)
    {
      g_clear_object (&p->menu_binding);
      g_clear_object (&p->menu_server);

      if (menu_server != NULL)
        {
          p->menu_server = g_object_ref (menu_server);
          p->menu_binding = g_object_bind_property (
                               p->menu_server, DBUSMENU_SERVER_PROP_DBUS_OBJECT,
                               p->skeleton, "menu-path",
                               G_BINDING_SYNC_CREATE);
        }

      g_object_notify_by_pspec (G_OBJECT(client), properties[PROP_DBUSMENU]);
    }
}
