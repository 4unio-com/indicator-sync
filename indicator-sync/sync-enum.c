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

#include "sync-enum.h"

GType
sync_state_get_type (void)
{
  static GType etype = 0;

  if (G_UNLIKELY(etype == 0))
    {
      static const GEnumValue values[] =
        {
          { SYNC_STATE_IDLE, "SYNC_STATE_IDLE", "idle" },
          { SYNC_STATE_SYNCING, "SYNC_STATE_SYNCING", "syncing" },
          { SYNC_STATE_ERROR, "SYNC_STATE_ERROR", "error" },
          { 0, NULL, NULL }
        };

      etype = g_enum_register_static ("SyncState", values);
    }

  return etype;
}
