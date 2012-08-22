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

#ifndef __APP_MENU_ITEM_H__
#define __APP_MENU_ITEM_H__

#include <glib.h>
#include <glib-object.h>

#include <libdbusmenu-glib/menuitem.h>

G_BEGIN_DECLS

#define APP_MENU_ITEM_TYPE            (app_menu_item_get_type ())
#define APP_MENU_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), APP_MENU_ITEM_TYPE, AppMenuItem))
#define APP_MENU_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), APP_MENU_ITEM_TYPE, AppMenuItemClass))
#define IS_APP_MENU_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), APP_MENU_ITEM_TYPE))
#define IS_APP_MENU_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), APP_MENU_ITEM_TYPE))
#define APP_MENU_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), APP_MENU_ITEM_TYPE, AppMenuItemClass))

typedef struct _AppMenuItem      AppMenuItem;
typedef struct _AppMenuItemPriv  AppMenuItemPriv;
typedef struct _AppMenuItemClass AppMenuItemClass;

struct _AppMenuItemClass
{
  DbusmenuMenuitemClass parent_class;
};

struct _AppMenuItem
{
  DbusmenuMenuitem parent;
  AppMenuItemPriv * priv;
};

GType          app_menu_item_get_type    (void);
AppMenuItem  * app_menu_item_new         (GDBusProxy * sync_source_proxy);
const gchar  * app_menu_item_get_name    (AppMenuItem * appitem);

G_END_DECLS

#endif /* __APP_MENU_ITEM_H__ */

