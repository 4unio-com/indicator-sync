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

#ifndef __SYNC_ENUM_H__
#define __SYNC_ENUM_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * SyncState:
 * @SYNC_STATE_IDLE:    The sync client is not active and has no errors.
 * @SYNC_STATE_SYNCING: The sync client is actively synchronizing.
 * @SYNC_STATE_ERROR:   The sync client has encountered an error that stopped activity.
 */
typedef enum
  {
    SYNC_STATE_IDLE,
    SYNC_STATE_SYNCING,
    SYNC_STATE_ERROR
  }
SyncState;

GType sync_state_get_type(void);
#define SYNC_TYPE_STATE (sync_state_get_type())

G_END_DECLS

#endif
