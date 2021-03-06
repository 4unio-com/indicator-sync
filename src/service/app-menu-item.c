/*
   A DbusmenuMenuitem which takes a .desktop file
   and displays an application name and icon.

   Copyright 2012 Canonical Ltd.

   Authors:
     Charles Kerr <charles.kerr@canonical.com>
     Ted Gould <ted@canonical.com>

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

#include <glib/gi18n.h>
#include <gio/gdesktopappinfo.h>

#include <libdbusmenu-glib/client.h>

#include "sync-menu/sync-enum.h"

#include "app-menu-item.h"
#include "dbus-shared.h"
#include "sync-app-dbus.h"

#define SYNC_MENU_ICON_KEY  "X-Ayatana-Sync-Menu-Icon"

#define PROP_ICON         APPLICATION_MENUITEM_PROP_ICON
#define PROP_NAME         APPLICATION_MENUITEM_PROP_NAME
#define PROP_STATE        APPLICATION_MENUITEM_PROP_STATE
#define PROP_TYPE         DBUSMENU_MENUITEM_PROP_TYPE
#define PROP_TOGGLE       DBUSMENU_MENUITEM_PROP_TOGGLE_STATE

#define TOGGLE_UNCHECKED  DBUSMENU_MENUITEM_TOGGLE_STATE_UNCHECKED
#define TOGGLE_CHECKED    DBUSMENU_MENUITEM_TOGGLE_STATE_CHECKED

struct _AppMenuItemPriv
{
  GDBusProxy * sync_menu_app;
  GAppInfo * appinfo;
  gchar * desktop;
};

static void app_menu_item_class_init (AppMenuItemClass *klass);
static void app_menu_item_init       (AppMenuItem *self);
static void app_menu_item_dispose    (GObject *object);
static void app_menu_item_finalize   (GObject *object);

static void update_checked (AppMenuItem * self);
static void update_label (AppMenuItem * self);

static void on_menuitem_activated (AppMenuItem * self, guint timestamp, gpointer data);
static void on_client_state_changed (GObject * o, GParamSpec * pspec, gpointer user_data);
static void on_client_paused_changed (GObject * o, GParamSpec * pspec, gpointer user_data);

static gchar* get_iconstr (const gchar * desktop_filename, GAppInfo * appinfo);

/***
****  GObject Boilerplate
***/

G_DEFINE_TYPE (AppMenuItem, app_menu_item, DBUSMENU_TYPE_MENUITEM);

static void
app_menu_item_class_init (AppMenuItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (AppMenuItemPriv));

  object_class->dispose = app_menu_item_dispose;
  object_class->finalize = app_menu_item_finalize;
}

static void
app_menu_item_init (AppMenuItem *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            APP_MENU_ITEM_TYPE,
                                            AppMenuItemPriv);
}

static void
app_menu_item_dispose (GObject *object)
{
  AppMenuItem * self = APP_MENU_ITEM(object);
  AppMenuItemPriv * p = self->priv;

  g_clear_object (&p->appinfo);

  g_signal_handlers_disconnect_by_func (self, on_menuitem_activated, NULL);

  if (p->sync_menu_app)
    {
      g_signal_handlers_disconnect_by_func (p->sync_menu_app, on_client_state_changed, self);
      g_signal_handlers_disconnect_by_func (p->sync_menu_app, on_client_paused_changed, self);
      g_clear_object (&p->sync_menu_app);
    }

  G_OBJECT_CLASS (app_menu_item_parent_class)->dispose (object);
}

static void
app_menu_item_finalize (GObject *object)
{
  AppMenuItem * self = APP_MENU_ITEM(object);
  AppMenuItemPriv * p = self->priv;

  g_clear_pointer (&p->desktop, g_free);

  G_OBJECT_CLASS (app_menu_item_parent_class)->finalize (object);
}

AppMenuItem *
app_menu_item_new (GDBusProxy * sync_menu_app)
{
  AppMenuItem * self = g_object_new(APP_MENU_ITEM_TYPE, NULL);
  DbusmenuMenuitem * mi = DBUSMENU_MENUITEM(self);
  AppMenuItemPriv * p = self->priv;

  dbusmenu_menuitem_property_set (mi, PROP_TYPE, APPLICATION_MENUITEM_TYPE);

  /* Set up listener signals */
  p->sync_menu_app = g_object_ref (sync_menu_app);
  g_signal_connect (p->sync_menu_app, "notify::state",
                    G_CALLBACK(on_client_state_changed), self);
  g_signal_connect (p->sync_menu_app, "notify::paused",
                    G_CALLBACK(on_client_paused_changed), self);

  /* initialize self from the current properties */
  const gchar * desktop =
                   dbus_sync_menu_app_get_desktop (DBUS_SYNC_MENU_APP(sync_menu_app));
  p->desktop = g_strdup (desktop);

  /* handle both filenames and desktop ids... */
  p->appinfo = g_file_test (desktop, G_FILE_TEST_EXISTS)
             ? G_APP_INFO (g_desktop_app_info_new_from_filename (desktop))
             : G_APP_INFO (g_desktop_app_info_new (desktop));

  const gchar * filename = G_IS_DESKTOP_APP_INFO(p->appinfo)
                         ? g_desktop_app_info_get_filename (G_DESKTOP_APP_INFO(p->appinfo))
                         : NULL;
  gchar * iconstr = get_iconstr (filename, p->appinfo);
  dbusmenu_menuitem_property_set (mi, PROP_ICON, iconstr);
  g_free (iconstr);

  update_label (self);
  update_checked (self);

  g_signal_connect (self, DBUSMENU_MENUITEM_SIGNAL_ITEM_ACTIVATED,
                    G_CALLBACK(on_menuitem_activated), NULL);

  return self;
}

/***
****
***/

static SyncMenuState
get_client_state (AppMenuItem * self)
{
  DbusSyncMenuApp * client = DBUS_SYNC_MENU_APP (self->priv->sync_menu_app);

  return client == NULL ? SYNC_MENU_STATE_IDLE
                        : dbus_sync_menu_app_get_state (client);
}

static gboolean
get_client_paused (AppMenuItem * self)
{
  DbusSyncMenuApp * client = DBUS_SYNC_MENU_APP (self->priv->sync_menu_app);

  return (client != NULL) && dbus_sync_menu_app_get_paused(client);
}

static void
update_checked (AppMenuItem * self)
{
  DbusmenuMenuitem * mi = DBUSMENU_MENUITEM(self);
  const gboolean paused = get_client_paused (self);
  const gint old_checked = dbusmenu_menuitem_property_get_int (mi, PROP_TOGGLE);
  const gint new_checked = paused ? TOGGLE_UNCHECKED : TOGGLE_CHECKED;

  if (old_checked != new_checked)
    {
      g_debug (G_STRLOC" setting mi checked property to '%d'", new_checked);
      dbusmenu_menuitem_property_set_int (mi, PROP_TOGGLE, new_checked);
    }
}

static void
update_label (AppMenuItem * self)
{
  const SyncMenuState state = get_client_state (self);
  const gboolean paused = get_client_paused (self);

  gchar * name = NULL;
  const gchar * const app_name = app_menu_item_get_name (self);
  if (state == SYNC_MENU_STATE_ERROR)
    {
      name = g_strdup_printf (_("%s (Error)"), app_name);
    }
  else if (state == SYNC_MENU_STATE_SYNCING)
    {
      name = g_strdup_printf (_("%s (Syncing)"), app_name);
    }
  else if (paused)
    {
      name = g_strdup_printf (_("%s (Paused)"), app_name);
    }
  else
    {
      name = g_strdup (app_name);
    }

  g_debug (G_STRLOC" setting name to '%s'", name);
  DbusmenuMenuitem * mi = DBUSMENU_MENUITEM(self);
  dbusmenu_menuitem_property_set (mi, PROP_NAME, name);
  dbusmenu_menuitem_property_set_int (mi, PROP_STATE, state);
  g_free (name);
}

/***
****
***/

static void
on_client_state_changed (GObject    * o          G_GNUC_UNUSED,
                         GParamSpec * pspec      G_GNUC_UNUSED,
                         gpointer     user_data)
{
  AppMenuItem * self = APP_MENU_ITEM(user_data);

  update_label (self);
}

static void
on_client_paused_changed (GObject    * o          G_GNUC_UNUSED,
                          GParamSpec * pspec      G_GNUC_UNUSED,
                          gpointer     user_data)
{
  AppMenuItem * self = APP_MENU_ITEM(user_data);

  update_label (self);
  update_checked (self);
}

static void
on_menuitem_activated (AppMenuItem * self, guint timestamp, gpointer data)
{
  DbusmenuMenuitem * mi = DBUSMENU_MENUITEM(self);
  DbusSyncMenuApp * sync_menu_app = DBUS_SYNC_MENU_APP(self->priv->sync_menu_app);

  /* tell the client that the user has manually paused it*/
  const gint old_checked = dbusmenu_menuitem_property_get_int (mi, PROP_TOGGLE);
  const gint new_checked = old_checked == TOGGLE_UNCHECKED ? TOGGLE_CHECKED
                                                           : TOGGLE_UNCHECKED;
  const gboolean old_paused = dbus_sync_menu_app_get_paused (sync_menu_app);
  const gboolean new_paused = new_checked == TOGGLE_UNCHECKED;
  if (old_paused != new_paused)
    {
      g_debug (G_STRLOC" changing SyncMenuApp's paused property from %d to %d", (int)old_paused, (int)new_paused);
      dbus_sync_menu_app_set_paused (sync_menu_app, new_paused);
    }
}

/*
**
*/

const gchar *
app_menu_item_get_name (AppMenuItem * appitem)
{
  const gchar * name = NULL;
  g_return_val_if_fail (IS_APP_MENU_ITEM(appitem), name);
  AppMenuItemPriv * p = appitem->priv;

  if ((name == NULL) && (p->appinfo != NULL))
    {
      name = g_app_info_get_name (p->appinfo);
    }

  if ((name == NULL) && (p->sync_menu_app != NULL))
    {
      name = g_dbus_proxy_get_name (p->sync_menu_app);
    }

  if (name == NULL)
    {
      name = _("Unknown");
    }

  return name;
}

/*
**
*/

static gchar*
get_iconstr (const gchar * desktop_filename, GAppInfo * app_info)
{
  GIcon * icon = NULL;
  gchar * iconstr = NULL;

  /* If the .desktop file has an Icon entry, use that */
  if (desktop_filename != NULL)
    {
      /* see if the .desktop file has an entry specifying its sync menu icon */
      GKeyFile * keyfile = g_key_file_new ();
      gchar * str = NULL;

      if (g_key_file_load_from_file (keyfile, desktop_filename, G_KEY_FILE_NONE, NULL))
        {
          str = g_key_file_get_string (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                       SYNC_MENU_ICON_KEY, NULL);
          if (str == NULL)
            str = g_key_file_get_string (keyfile, G_KEY_FILE_DESKTOP_GROUP,
                                         G_KEY_FILE_DESKTOP_KEY_ICON, NULL);
        }

      if (str != NULL)
        {
          GError * error = NULL;
          icon = g_icon_new_for_string (str, &error);
          if (error != NULL)
            {
              g_warning ("Unable to load icon '%s': %s", str, error->message);
              g_error_free (error);
            }
          g_free (str);
        }

      g_key_file_free (keyfile);
    }

  /* as a fallback, use the app_info's icon */
  if (icon == NULL)
    icon = g_app_info_get_icon (app_info);

    
  /* if the icon provided is a themed one, 
     let symbolic name(s) override the one(s) provided above */
  if (G_IS_THEMED_ICON (icon))
    {
      guint i, n;
      const gchar * const * names;
      GPtrArray * new_names;

      names = g_themed_icon_get_names (G_THEMED_ICON(icon));
      n = g_strv_length ((gchar**)names);
      new_names = g_ptr_array_new_full (n*2, g_free);

      for (i=0; i<n; i++)
        {
          const gchar * name = names[i];

          if (!g_str_has_suffix (name, "-symbolic"))
            g_ptr_array_add (new_names, g_strdup_printf ("%s-symbolic", name));

          g_ptr_array_add (new_names, g_strdup (name));
        }

      g_object_unref (icon);
      icon = g_themed_icon_new_from_names ((char**)new_names->pdata, new_names->len);

      g_ptr_array_unref (new_names);
    }

  if (icon != NULL)
    {
      iconstr = g_icon_to_string (icon);
      g_object_unref (icon);
    }

  if (iconstr == NULL)
    iconstr = g_strdup ("gtk-missing-image");

  return iconstr;
}
