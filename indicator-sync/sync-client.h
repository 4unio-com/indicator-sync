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

#ifndef __SYNC_CLIENT_H__
#define __SYNC_CLIENT_H__

#include <glib.h>
#include <glib-object.h>
#include <libdbusmenu-glib/dbusmenu-glib.h>
#include <indicator-sync/sync-enum.h>

G_BEGIN_DECLS

/***
****  GObject boilerplate
***/

GType sync_client_get_type (void) G_GNUC_CONST;

typedef struct _SyncMenuClient SyncMenuClient;

#define SYNC_CLIENT_TYPE            (sync_client_get_type ())
#define SYNC_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SYNC_CLIENT_TYPE, SyncClient))
#define SYNC_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SYNC_CLIENT_TYPE, SyncClientClass))
#define IS_SYNC_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SYNC_CLIENT_TYPE))
#define IS_SYNC_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SYNC_CLIENT_TYPE))
#define SYNC_CLIENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), SYNC_CLIENT_TYPE, SyncClientClass))

#define SYNC_CLIENT_PROP_DESKTOP_ID  "desktop_id"
#define SYNC_CLIENT_PROP_STATE       "state"
#define SYNC_CLIENT_PROP_PAUSED      "paused"
#define SYNC_CLIENT_PROP_DBUSMENU    "dbusmenu"

typedef struct _SyncClientClass    SyncClientClass;
typedef struct _SyncClient         SyncClient;
typedef struct _SyncClientPriv     SyncClientPriv;

struct _SyncClientClass
{
  /*< private >*/
  GObjectClass parent;

  /* reserved for future use */
  void (*sync_client_reserved1)(void);
  void (*sync_client_reserved2)(void);
  void (*sync_client_reserved3)(void);
  void (*sync_client_reserved4)(void);
};

struct _SyncClient
{
  /*< private >*/
  GObject parent;
  SyncClientPriv * priv;
};

/***
****
***/

/**
 * sync_client_new:
 * @desktop : A desktop id as described by g_desktop_app_info_new(),
 *            such as "transmission-gtk.desktop"
 *
 * Creates a new #SyncClient object. Applications wanting to interact
 * with the Sync Indicator should instantiate one of these and use it.
 *
 * The initial state is #SYNC_STATE_IDLE, unpaused, and with no menu.
 *
 * Returns (transfer full): a new SyncClient for the desktop id.
 *                          Free the returned object with g_object_unref().
 **/
SyncClient * sync_client_new (const char * desktop_id);

/**
 * sync_client_set_state:
 * @client : A #SyncClient object
 * @state : The #SyncState such as #SYNC_STATE_IDLE
 *
 * Sets the SyncClient's state property.
 **/
void sync_client_set_state (SyncClient * client, SyncState state);

/**
 * sync_client_set_state:
 * @client : A #SyncClient object
 * @paused : A boolean representing whether the client is paused or not
 *
 * Sets the SyncClient's paused property.
 **/
void sync_client_set_paused (SyncClient * client, gboolean paused);

/**
 * sync_client_set_menu:
 * @client : A #SyncClient object
 * @paused : A #DbusmenuServer of the menu to be exported by the #SyncClient
 *
 * Sets the SyncClient's menu.
 **/
void sync_client_set_menu (SyncClient * client, DbusmenuServer * menu);

/**
 * sync_client_get_state:
 * @client : A #SyncClient object
 *
 * Returns : the client's current #SyncState property, such as #SYNC_STATE_IDLE
 **/
SyncState sync_client_get_state (SyncClient * client);

/**
 * sync_client_get_state:
 * @client : A #SyncClient object
 *
 * Returns : the client's current 'paused' property.
 **/
gboolean sync_client_get_paused (SyncClient * client);

/**
 * sync_client_get_state:
 * @client : A #SyncClient object
 *
 * Returns (transfer none): the client's current #DbusmenuServer
 **/
DbusmenuServer * sync_client_get_menu (SyncClient * client);

/**
 * sync_client_get_desktop_id:
 * @client : A #SyncClient object
 *
 * Returns (transfer none): the client's desktop id
 **/
const gchar * sync_client_get_desktop_id (SyncClient * client);


/**
 * SYNC_PROGRESS_MENUITEM_TYPE:
 *
 * A value for #DBUSMENU_MENUITEM_PROP_TYPE to create a #DbusmenuMenuitem
 * that represents some task that has a progress state associated with it.
 * For example, this could be used for showing a file while it's in the
 * process of being synchronized.
 **/
#define SYNC_PROGRESS_MENUITEM_TYPE               "sync-progress-item"

/**
 * SYNC_PROGRESS_MENUITEM_PROP_PERCENT_DONE:
 *
 * #DbusmenuMenuitem property that says what percent done this item is.
 * Should be an integer in the range [0..100]
 * Type: #G_VARIANT_TYPE_INT32
 */
#define SYNC_PROGRESS_MENUITEM_PROP_PERCENT_DONE  "percent-done"

G_END_DECLS

#endif

