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

#ifndef __SYNC_MENU_APP_H__
#define __SYNC_MENU_APP_H__

#include <glib.h>
#include <glib-object.h>
#include <libdbusmenu-glib/dbusmenu-glib.h>
#include <sync-menu/sync-enum.h>

G_BEGIN_DECLS

/***
****  GObject boilerplate
***/

GType sync_menu_app_get_type (void) G_GNUC_CONST;

#define SYNC_MENU_APP_TYPE            (sync_menu_app_get_type ())
#define SYNC_MENU_APP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SYNC_MENU_APP_TYPE, SyncMenuApp))
#define SYNC_MENU_APP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  SYNC_MENU_APP_TYPE, SyncMenuAppClass))
#define IS_SYNC_MENU_APP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SYNC_MENU_APP_TYPE))
#define IS_SYNC_MENU_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  SYNC_MENU_APP_TYPE))
#define SYNC_MENU_APP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  SYNC_MENU_APP_TYPE, SyncMenuAppClass))

#define SYNC_MENU_APP_PROP_DESKTOP_ID  "desktop-id"
#define SYNC_MENU_APP_PROP_STATE       "state"
#define SYNC_MENU_APP_PROP_PAUSED      "paused"
#define SYNC_MENU_APP_PROP_DBUSMENU    "dbusmenu"

typedef struct _SyncMenuAppClass    SyncMenuAppClass;
typedef struct _SyncMenuApp         SyncMenuApp;
typedef struct _SyncMenuAppPriv     SyncMenuAppPriv;

struct _SyncMenuAppClass
{
  /*< private >*/
  GObjectClass parent;

  /* reserved for future use */
  void (*sync_menu_app_reserved1)(void);
  void (*sync_menu_app_reserved2)(void);
  void (*sync_menu_app_reserved3)(void);
  void (*sync_menu_app_reserved4)(void);
};

struct _SyncMenuApp
{
  /*< private >*/
  GObject parent;
  SyncMenuAppPriv * priv;
};

/***
****
***/

SyncMenuApp * sync_menu_app_new (const char * desktop_id);

void sync_menu_app_set_state (SyncMenuApp * client, SyncMenuState state);

void sync_menu_app_set_paused (SyncMenuApp * client, gboolean paused);

void sync_menu_app_set_menu (SyncMenuApp * client, DbusmenuServer * menu_server);

SyncMenuState sync_menu_app_get_state (SyncMenuApp * client);

gboolean sync_menu_app_get_paused (SyncMenuApp * client);

DbusmenuServer * sync_menu_app_get_menu (SyncMenuApp * client);

const gchar * sync_menu_app_get_desktop_id (SyncMenuApp * client);

/**
 * SYNC_MENU_PROGRESS_MENUITEM_TYPE:
 *
 * A value for %DBUSMENU_MENUITEM_PROP_TYPE to create a #DbusmenuMenuitem
 * that represents some task that has a progress state associated with it.
 * For example, this could be used for showing a file while it's in the
 * process of being synchronized.
 **/
#define SYNC_MENU_PROGRESS_MENUITEM_TYPE               "sync-progress-item"

/**
 * SYNC_MENU_PROGRESS_MENUITEM_PROP_PERCENT_DONE:
 *
 * #DbusmenuMenuitem property that says what percent done this item is.
 * Should be an integer in the range [0..100]
 * Type: %G_VARIANT_TYPE_INT32
 */
#define SYNC_MENU_PROGRESS_MENUITEM_PROP_PERCENT_DONE  "percent-done"

G_END_DECLS

#endif

