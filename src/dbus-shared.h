/*
   Some package-visible symbols 

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

#ifndef SYNC_INDICATOR_DBUS_SHARED
#define SYNC_INDICATOR_DBUS_SHARED


#define  SYNC_SERVICE_DBUS_NAME          "com.canonical.indicator.sync"
#define  SYNC_SERVICE_DBUS_IFACE         "com.canonical.indicator.sync.service"
#define  SYNC_SERVICE_DBUS_OBJECT       "/com/canonical/indicator/sync/service"
#define  SYNC_SERVICE_DBUS_MENU_OBJECT  "/com/canonical/indicator/sync/menu"

#define  SYNC_MENU_APP_DBUS_IFACE        "com.canonical.indicator.sync.app"

#define APPLICATION_MENUITEM_TYPE        "application-item"
#define APPLICATION_MENUITEM_PROP_NAME   "label"
#define APPLICATION_MENUITEM_PROP_ICON   "icon-name"
#define APPLICATION_MENUITEM_PROP_STATE  "sync-state"


#endif /* SYNC_INDICATOR_DBUS_SHARED */
