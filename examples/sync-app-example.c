/*
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

#include <glib.h>

#include <libdbusmenu-glib/server.h>

#include <sync-menu/sync-app.h>

static gboolean paused = FALSE;
static gboolean pretend_error = FALSE;
static int syncing_file_count = 0;
static SyncMenuApp * sync_menu_app = NULL;

/***
****
***/

static void
state_refresh (void)
{
  if (pretend_error)
    {
      sync_menu_app_set_state (sync_menu_app, SYNC_MENU_STATE_ERROR);
    }
  else if (!paused && (syncing_file_count > 0))
    {
      sync_menu_app_set_state (sync_menu_app, SYNC_MENU_STATE_SYNCING);
    }
  else
    {
      sync_menu_app_set_state (sync_menu_app, SYNC_MENU_STATE_IDLE);
    }
}

/***
****
***/

/* Called when one of our self-created menuitems is activated,
   such as when a human clicks on it in the indicator */
static void
on_menuitem_clicked (DbusmenuMenuitem  * mi,
                     guint               timestamp,
                     gpointer            user_data   G_GNUC_UNUSED)
{
  g_message (G_STRLOC" Menuitem '%s' clicked at %d",
             dbusmenu_menuitem_property_get (mi, DBUSMENU_MENUITEM_PROP_LABEL),
             timestamp);
}

/* Called when the sync app's 'paused' state changes,
 * such as when a human clicks on our app's name in the sync indicator */
static void
on_sync_menu_app_paused_changed (SyncMenuApp  * sync_menu_app,
                            GParamSpec   * pspec         G_GNUC_UNUSED,
                            const gchar  * name          G_GNUC_UNUSED)
{
  paused = sync_menu_app_get_paused (sync_menu_app);

  if (paused)
    {
      g_message (G_STRLOC" The user paused %s", name);
    }
  else
    {
      g_message (G_STRLOC" The user resumed %s", name);
    }

  state_refresh ();
}

/* This demo simulates a handful of files being synchronized.
   This timer func is called periodically to increment our progress */
static gboolean
on_file_sync_timer (DbusmenuMenuitem * mi)
{
  const gchar * key = SYNC_MENU_PROGRESS_MENUITEM_PROP_PERCENT_DONE;
  int percent = dbusmenu_menuitem_property_get_int (mi, key);
  gboolean done = percent == 100;

  /* if the app's not paused, pretend we got a little further in our sync */
  if (!paused && !pretend_error && !done)
    {
      const int increment = 1 + g_random_int()%10;
      percent = CLAMP (percent+increment, 0, 100);
      dbusmenu_menuitem_property_set_int (mi, key, percent);

      done = percent == 100;
      if (done)
        {
          syncing_file_count--;
          state_refresh ();
        }
    }
     
  /* this file's timer can end when its sync simulation finishes */
  return !done;
}

/* Called when the user clicks on the 'pretend error state' checkbox */
static void
on_pretend_error_toggled (DbusmenuMenuitem  * mi,
                          guint               timestamp   G_GNUC_UNUSED,
                          gpointer            user_data   G_GNUC_UNUSED)
{
  /* toggle the checkbox */
  const gchar * key = DBUSMENU_MENUITEM_PROP_TOGGLE_STATE;
  int checked = dbusmenu_menuitem_property_get_int (mi, key);
  checked = !checked;
  dbusmenu_menuitem_property_set_int (mi, key, checked);
 
  /* update our app's state */
  pretend_error = checked;
  state_refresh ();
}

/***
****
***/

static void
create_u1_sync_menu_app (void)
{
  DbusmenuMenuitem *root, *mi, *files;

  /**
  ***  Build the menu
  **/

  root = dbusmenu_menuitem_new();

  mi = dbusmenu_menuitem_new();
  dbusmenu_menuitem_property_set(mi, DBUSMENU_MENUITEM_PROP_LABEL, "Pretend Error");
  dbusmenu_menuitem_property_set_bool(mi, DBUSMENU_MENUITEM_PROP_VISIBLE, TRUE);
  dbusmenu_menuitem_property_set_bool(mi, DBUSMENU_MENUITEM_PROP_ENABLED, TRUE);
  //dbusmenu_menuitem_property_set (mi, DBUSMENU_MENUITEM_PROP_TYPE, DBUSMENU_MENUITEM_PROP_TOGGLE_TYPE);
  dbusmenu_menuitem_property_set(mi, DBUSMENU_MENUITEM_PROP_TOGGLE_TYPE, DBUSMENU_MENUITEM_TOGGLE_CHECK);
  dbusmenu_menuitem_property_set_int(mi, DBUSMENU_MENUITEM_PROP_TOGGLE_STATE, DBUSMENU_MENUITEM_TOGGLE_STATE_UNCHECKED);
  g_signal_connect (mi, DBUSMENU_MENUITEM_SIGNAL_ITEM_ACTIVATED, G_CALLBACK(on_pretend_error_toggled), NULL);
  dbusmenu_menuitem_child_append (root, mi);

  files = dbusmenu_menuitem_new();
  dbusmenu_menuitem_property_set (files, DBUSMENU_MENUITEM_PROP_LABEL, "Files");
  dbusmenu_menuitem_child_append (root, files);

  const gchar * filenames[] =
  {
    "ubuntu-12.10-alternate-amd64.iso",
    "ubuntu-12.10-alternate-i386.iso",
    "ubuntu-12.10-desktop-amd64.iso",
    "ubuntu-12.10-desktop-i386.iso",
    "ubuntu-12.10-server-amd64.iso",
    "ubuntu-12.10-server-i386.iso"
  };
  const int n = G_N_ELEMENTS(filenames);

  int i;
  for (i=0; i<n; ++i)
    {
      DbusmenuMenuitem * file = dbusmenu_menuitem_new ();
      dbusmenu_menuitem_property_set (file,
                                      DBUSMENU_MENUITEM_PROP_TYPE,
                                      SYNC_MENU_PROGRESS_MENUITEM_TYPE);
      dbusmenu_menuitem_property_set (file,
                                      DBUSMENU_MENUITEM_PROP_LABEL,
                                      filenames[i]);
      dbusmenu_menuitem_property_set_int (file,
                                          SYNC_MENU_PROGRESS_MENUITEM_PROP_PERCENT_DONE,
                                          g_random_int() % 100);
      g_signal_connect (file, DBUSMENU_MENUITEM_SIGNAL_ITEM_ACTIVATED,
                        G_CALLBACK(on_menuitem_clicked), NULL);
      g_timeout_add_seconds (1+g_random_int()%3,
                             (GSourceFunc)on_file_sync_timer,
                             file);
      dbusmenu_menuitem_child_append (files, file);
      syncing_file_count++;
    }

  DbusmenuServer * dmserver = dbusmenu_server_new ("/dbusmenu/ubuntuone");
  dbusmenu_server_set_root(dmserver, root);

  /**
  ***  Build the Sync Client
  **/

  const gchar * desktop = "ubuntuone-installer.desktop";
  sync_menu_app = sync_menu_app_new (desktop);
  sync_menu_app_set_menu (sync_menu_app, dmserver);
  state_refresh ();
  g_signal_connect (sync_menu_app, "notify::paused",
                    G_CALLBACK(on_sync_menu_app_paused_changed), (gpointer)desktop);
}

int
main (void)
{
  create_u1_sync_menu_app ();
  g_main_loop_run (g_main_loop_new(NULL, FALSE));
  return 0;
}
