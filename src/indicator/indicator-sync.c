/*
   An indicator to show SyncMenuApps' states and menus

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

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include <libdbusmenu-gtk/menu.h>
#include <libdbusmenu-gtk/menuitem.h>

#include <libindicator/indicator.h>
#include <libindicator/indicator-object.h>
#include <libindicator/indicator-service-manager.h>

#include <libido/idoswitchmenuitem.h>

#include "dbus-shared.h"
#include "sync-menu/sync-app.h"
#include "sync-menu/sync-enum.h"
#include "sync-service-dbus.h"

#define INDICATOR_SYNC_TYPE            (indicator_sync_get_type ())
#define INDICATOR_SYNC(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_SYNC_TYPE, IndicatorSync))
#define INDICATOR_SYNC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_SYNC_TYPE, IndicatorSyncClass))
#define IS_INDICATOR_SYNC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_SYNC_TYPE))
#define IS_INDICATOR_SYNC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_SYNC_TYPE))
#define INDICATOR_SYNC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_SYNC_TYPE, IndicatorSyncClass))

typedef struct _IndicatorSync      IndicatorSync;
typedef struct _IndicatorSyncClass IndicatorSyncClass;

struct _IndicatorSyncClass
{
  IndicatorObjectClass parent_class;
};

struct _IndicatorSync
{
  IndicatorObject            parent;
  IndicatorObjectEntry       entry;
  IndicatorServiceManager  * service_manager;
  DbusSyncService          * sync_service_proxy;
  DbusmenuGtkClient        * menu_client;
};

GType indicator_sync_get_type (void);

/* Indicator Module Config */
INDICATOR_SET_VERSION
INDICATOR_SET_TYPE(INDICATOR_SYNC_TYPE)

static void indicator_sync_class_init (IndicatorSyncClass *klass);
static void indicator_sync_init       (IndicatorSync *self);
static void indicator_sync_dispose    (GObject *object);
static void indicator_sync_finalize   (GObject *object);

static gboolean new_item_app (DbusmenuMenuitem * newitem,
                              DbusmenuMenuitem * parent,
                              DbusmenuClient   * client,
                              gpointer           user_data);

static gboolean new_item_prog (DbusmenuMenuitem * newitem,
                               DbusmenuMenuitem * parent,
                               DbusmenuClient   * client,
                               gpointer           user_data);

static const gchar * calculate_icon_name (IndicatorSync * self);

static void on_service_manager_connection_changed (IndicatorServiceManager * sm,
                                                   gboolean connected,
                                                   gpointer user_data);

/***
****
***/

static GList*
get_entries (IndicatorObject * io)
{
  return g_list_prepend (NULL, &INDICATOR_SYNC(io)->entry);
}

G_DEFINE_TYPE (IndicatorSync, indicator_sync, INDICATOR_OBJECT_TYPE);

static void
indicator_sync_class_init (IndicatorSyncClass *klass)
{
  GObjectClass * object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = indicator_sync_dispose;
  object_class->finalize = indicator_sync_finalize;

  IndicatorObjectClass * io_class = INDICATOR_OBJECT_CLASS(klass);
  io_class->get_entries = get_entries;
}
  
static void
indicator_sync_init (IndicatorSync *self)
{
  g_object_set (self, INDICATOR_OBJECT_DEFAULT_VISIBILITY, FALSE, NULL);

  /* init the menu */
  DbusmenuGtkMenu * menu = dbusmenu_gtkmenu_new (SYNC_SERVICE_DBUS_NAME,
                                                 SYNC_SERVICE_DBUS_MENU_OBJECT);
  self->menu_client = dbusmenu_gtkmenu_get_client (menu);
  dbusmenu_client_add_type_handler (DBUSMENU_CLIENT(self->menu_client),
                                    APPLICATION_MENUITEM_TYPE,
                                    new_item_app);
  dbusmenu_client_add_type_handler (DBUSMENU_CLIENT(self->menu_client),
                                    SYNC_MENU_PROGRESS_MENUITEM_TYPE,
                                    new_item_prog);

  /* init the entry */
  self->entry.label = NULL; /* no label */
  self->entry.image = g_object_ref_sink (gtk_image_new_from_icon_name (calculate_icon_name (self), GTK_ICON_SIZE_BUTTON));
  self->entry.menu = g_object_ref_sink (menu);
  self->entry.name_hint = PACKAGE;
  self->entry.accessible_desc = NULL;
  gtk_widget_show (GTK_WIDGET(self->entry.image));

  /* init the service manager */
  self->service_manager = indicator_service_manager_new_version (
                            SYNC_SERVICE_DBUS_NAME, 1);
  g_signal_connect (self->service_manager,
                    INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE,
                    G_CALLBACK(on_service_manager_connection_changed), self);
}

static void
indicator_sync_dispose (GObject *object)
{
  IndicatorSync * self = INDICATOR_SYNC(object);
  g_return_if_fail (self != NULL);

  g_clear_object (&self->sync_service_proxy);
  g_clear_object (&self->service_manager);
  g_clear_object (&self->entry.image);

  if (self->entry.menu)
    {
      self->menu_client = NULL;
      g_clear_object (&self->entry.menu);
    }

  G_OBJECT_CLASS (indicator_sync_parent_class)->dispose (object);
}

static void
indicator_sync_finalize (GObject *object)
{
  G_OBJECT_CLASS (indicator_sync_parent_class)->finalize (object);
}

/****
*****
****/

static SyncMenuState
get_service_state (IndicatorSync * self)
{
  if (self->sync_service_proxy == NULL)
    {
      return SYNC_MENU_STATE_IDLE;
    }

  return dbus_sync_service_get_state (self->sync_service_proxy);
}

static gboolean
get_service_paused (IndicatorSync * self)
{
  return (self->sync_service_proxy != NULL)
      && (dbus_sync_service_get_paused (self->sync_service_proxy));
}

static gboolean
get_service_count (IndicatorSync * self)
{
  int count = 0;

  if (self->sync_service_proxy != NULL)
    {
      count = dbus_sync_service_get_client_count (self->sync_service_proxy);
    }

  return count;
}

/* show the indicator only if the service has SyncMenuApps */
static void
update_visibility (IndicatorSync * self)
{
  const gboolean new_visible = get_service_count (self) > 0;
  g_debug (G_STRLOC" setting visibility flag to %d", (int)new_visible);
  indicator_object_set_visible (INDICATOR_OBJECT(self), new_visible);
}

/* update our a11y text based on the sync service's state */
static void
update_accessible_title (IndicatorSync * self)
{
  const SyncMenuState state = get_service_state (self);
  const gboolean paused = get_service_paused (self);

  const gchar * desc = NULL;

  if (state == SYNC_MENU_STATE_ERROR)
    {
      desc = _("Sync (error)");
    }
  else if (state == SYNC_MENU_STATE_SYNCING)
    {
      desc = _("Sync (syncing)");
    }
  else if (paused)
    {
      desc = _("Sync (paused)");
    }
  else
    {
      desc = _("Sync");
    }

  if (g_strcmp0 (self->entry.accessible_desc, desc))
    {
      g_debug (G_STRLOC" setting accessible_desc to '%s'", desc);
      self->entry.accessible_desc = desc;
      g_signal_emit (self,
                     INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE_ID,
                     0,
                     &self->entry);
    }
}

/* figure out what icon to use */
static const gchar *
calculate_icon_name (IndicatorSync * self)
{
  const gchar * icon_name;
  const SyncMenuState state = get_service_state (self);
  const gboolean paused = get_service_paused (self);

  if (state == SYNC_MENU_STATE_ERROR)
    {
      icon_name = "sync-error";
    }
  else if (state == SYNC_MENU_STATE_SYNCING)
    {
      icon_name = "sync-syncing";
    }
  else if (paused)
    {
      icon_name = "sync-paused";
    }
  else
    {
      icon_name = "sync-idle";
    }

  return icon_name;
}

/* update our icon based on the service's state */
static void
update_icon (IndicatorSync * self)
{
  g_return_if_fail (IS_INDICATOR_SYNC(self));

  const gchar * const icon_name = calculate_icon_name (self);
  g_debug (G_STRLOC" setting icon to '%s'", icon_name);
  gtk_image_set_from_icon_name (GTK_IMAGE(self->entry.image), icon_name, GTK_ICON_SIZE_MENU);
}

static void
on_service_client_count_changed (GObject     * o           G_GNUC_UNUSED,
                                 GParamSpec  * pspec       G_GNUC_UNUSED,
                                 gpointer      user_data)
{
  IndicatorSync * self = INDICATOR_SYNC(user_data);
  g_return_if_fail (self != NULL);

  update_visibility (self);
}

static void
on_service_state_changed (GObject     * o          G_GNUC_UNUSED,
                          GParamSpec  * pspec      G_GNUC_UNUSED,
                          gpointer      user_data)
{
  IndicatorSync * self = INDICATOR_SYNC(user_data);
  g_return_if_fail (self != NULL);

  update_icon (self);
  update_accessible_title (self);
}

static void
on_service_paused_changed (GObject     * o          G_GNUC_UNUSED,
                           GParamSpec  * pspec      G_GNUC_UNUSED,
                           gpointer      user_data)
{
  IndicatorSync * self = INDICATOR_SYNC(user_data);
  g_return_if_fail (self != NULL);

  update_icon (self);
  update_accessible_title (self);
}
 

/* manage our service proxy based on whether or not the manager's connected */
static void 
on_service_manager_connection_changed (IndicatorServiceManager  * sm,
                                       gboolean                   connected,
                                       gpointer                   user_data)
{
  IndicatorSync * self = INDICATOR_SYNC(user_data);
  g_return_if_fail (self != NULL);

  if (!connected)
    {
      g_clear_object (&self->sync_service_proxy);
      update_visibility (self);
    }
  else if (self->sync_service_proxy == NULL)
    {
      GError * err = NULL;
      self->sync_service_proxy = dbus_sync_service_proxy_new_for_bus_sync (
                                  G_BUS_TYPE_SESSION,
                                  G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                  SYNC_SERVICE_DBUS_NAME,
                                  SYNC_SERVICE_DBUS_OBJECT,
                                  NULL,
                                  &err);
      if (err != NULL)
        {
          g_warning ("Indicator couldn't create SyncService proxy: %s", err->message);
          g_clear_error (&err);
        }
      else
        {
          GObject * o = G_OBJECT (self->sync_service_proxy);
          g_signal_connect (o, "notify::state",
                            G_CALLBACK(on_service_state_changed), self);
          g_signal_connect (o, "notify::paused",
                            G_CALLBACK(on_service_paused_changed), self);
          g_signal_connect (o, "notify::client-count",
                            G_CALLBACK(on_service_client_count_changed), self);
          on_service_state_changed (o, NULL, self);
          on_service_paused_changed (o, NULL, self);
          on_service_client_count_changed (o, NULL, self);
        }
    }
}

/***
****
****  APPLICATION_MENUITEM_TYPE handler
****
***/

#define PROP_TYPE                DBUSMENU_MENUITEM_PROP_TYPE
#define PROP_TOGGLE_STATE        DBUSMENU_MENUITEM_PROP_TOGGLE_STATE
#define TOGGLE_STATE_CHECKED     DBUSMENU_MENUITEM_TOGGLE_STATE_CHECKED
#define SIGNAL_PROPERTY_CHANGED  DBUSMENU_MENUITEM_SIGNAL_PROPERTY_CHANGED

static int aligned_left_margin = 0;

/*
 * This ugly little hack aligns the SyncMenuApp's exported GtkMenuItems'
 * text with the AppMenuItem's GtkMenuItem text.
 *
 * Ideally we'd do this by setting the widgets' left margins when
 * they are created, but that happens deep inside dbusmenu-gtk.
 * So we need a way to get the widgets after they're created but
 * before the users see them.
 *
 * The approach used here is to listen for the "map" signal from the
 * widgets we /do/ create (in new_item_app()) so that we know when
 * the menu is being popped up. When mapping happens, we walk through
 * the menuitems and align them.
 */
static void
on_app_widget_shown (GtkWidget * unused, DbusmenuClient * client)
{
  /* init the tweak key */
  static GQuark tweak_quark = 0;
  if (G_UNLIKELY(tweak_quark == 0))
    {
      tweak_quark = g_quark_from_static_string ("margin tweaked by i-sync");
    }

  DbusmenuMenuitem * root = dbusmenu_client_get_root (client);
  if (root == NULL)
    {
      return;
    }

  GList * l;
  GList * children = dbusmenu_menuitem_get_children (root);
  DbusmenuGtkClient * gtk_client = DBUSMENU_GTKCLIENT (client);
  for (l=children; l!=NULL; l=l->next)
    {
      DbusmenuMenuitem * child = DBUSMENU_MENUITEM(l->data);
    
      /* AppMenuItems are already indented by their icons */
      const gchar * type = dbusmenu_menuitem_property_get (child, PROP_TYPE);
      if (!g_strcmp0 (type, APPLICATION_MENUITEM_TYPE))
        {
          continue;
        }

      GtkMenuItem * gmi = dbusmenu_gtkclient_menuitem_get (gtk_client, child);
       
      GObject * o = G_OBJECT(gmi);
      if (g_object_get_qdata (o, tweak_quark) == NULL)
        {
          g_object_set_qdata (o, tweak_quark, GINT_TO_POINTER(TRUE));
          gtk_widget_set_margin_left (GTK_WIDGET(o), aligned_left_margin);
        }
    }
}

struct AppWidgets
{
  gboolean stop_activation;
  GtkWidget * label;
  GtkWidget * icon;
  GtkWidget * gmi;
};

static void
on_app_widget_activated (GtkWidget * gmi, struct AppWidgets * widgets)
{
  /* If this is being called because we just synchronized our widget's
     state from the DbusmenuMenuitem's TOGGLE state in app_update_check(),
     stop the "activate" event here to keep it from getting back to
     DbusmenuMenuitem and creating a feedback loop */
  if (widgets->stop_activation)
    {
      widgets->stop_activation = FALSE;
      g_signal_stop_emission_by_name (gmi, "activate");
    }
}

static void
app_update_check (DbusmenuMenuitem * mi, struct AppWidgets * widgets)
{
  GtkCheckMenuItem * cmi = GTK_CHECK_MENU_ITEM (widgets->gmi);
  const gint old_active = gtk_check_menu_item_get_active (cmi);
  const gint new_active = dbusmenu_menuitem_property_get_int (mi, PROP_TOGGLE_STATE) == TOGGLE_STATE_CHECKED;

  if (old_active != new_active)
    {
      widgets->stop_activation = TRUE;
      gtk_check_menu_item_set_active (cmi, new_active);
    }
}

static void
app_update_icon (DbusmenuMenuitem * mi, struct AppWidgets * w)
{
  const gchar * icon_name = dbusmenu_menuitem_property_get (mi, APPLICATION_MENUITEM_PROP_ICON);

  gtk_image_set_from_icon_name (GTK_IMAGE(w->icon), icon_name, GTK_ICON_SIZE_MENU);
}

static void
app_update_label (DbusmenuMenuitem * mi, struct AppWidgets * w)
{
  const gchar * name = dbusmenu_menuitem_property_get (mi, APPLICATION_MENUITEM_PROP_NAME);
  const SyncMenuState state = dbusmenu_menuitem_property_get_int (mi, APPLICATION_MENUITEM_PROP_STATE);

  if (state != SYNC_MENU_STATE_ERROR)
    {
      gtk_label_set_text (GTK_LABEL(w->label), name);
    }
  else
    {
      /* FIXME: it would be nice if the error color played nicely with themes */
      gchar * escaped = g_markup_escape_text (name, -1);
      gchar * markup = g_strdup_printf ("<span foreground=\"red\">%s</span>", escaped);
      gtk_label_set_markup (GTK_LABEL(w->label), markup);
      g_free (markup);
      g_free (escaped);
    }
}

static void
on_app_property_changed (DbusmenuMenuitem  * mi,
                         gchar             * prop,
                         GVariant          * value,
                         struct AppWidgets * w)
{
  if (!g_strcmp0 (prop, APPLICATION_MENUITEM_PROP_STATE) ||
      !g_strcmp0 (prop, APPLICATION_MENUITEM_PROP_NAME))
    {
      app_update_label (mi, w);
    }
  else if (!g_strcmp0(prop, APPLICATION_MENUITEM_PROP_ICON))
    {
      app_update_icon (mi, w);
    }
  else if (!g_strcmp0(prop, PROP_TOGGLE_STATE))
    {
      app_update_check (mi, w);
    }
}

/* build a gtkmenuitem to represent an AppMenuItem */
static gboolean
new_item_app (DbusmenuMenuitem  * newitem,
              DbusmenuMenuitem  * parent,
              DbusmenuClient    * client,
              gpointer            user_data)
{
  struct AppWidgets * w;

  /* build the display widgets and initialize them */
  w = g_new0 (struct AppWidgets, 1);
  w->gmi = ido_switch_menu_item_new ();
  w->icon = gtk_image_new ();
  w->label = gtk_label_new (NULL);
  app_update_icon  (newitem, w);
  app_update_label (newitem, w);
  app_update_check (newitem, w);

  /* layout & styling */
  g_signal_connect (w->gmi, "map", G_CALLBACK(on_app_widget_shown), client);
  g_signal_connect (w->gmi, "activate", G_CALLBACK(on_app_widget_activated), w);

  gint padding = 4;
  gtk_widget_style_get(GTK_WIDGET(w->gmi), "toggle-spacing", &padding, NULL);
  GtkWidget * hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, padding);
  int icon_width = 0;
  gtk_widget_get_preferred_width (w->icon, NULL, &icon_width);
  aligned_left_margin = padding + icon_width;
  gtk_misc_set_alignment(GTK_MISC(w->icon), 1.0 /* right aligned */, 0.5);
  gtk_box_pack_start (GTK_BOX (hbox), w->icon, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC(w->label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX(hbox), w->label, TRUE, TRUE, 0);
  gtk_container_add (ido_switch_menu_item_get_content_area(IDO_SWITCH_MENU_ITEM(w->gmi)), hbox);
  gtk_widget_show_all (w->gmi);

  /* let dbusmenu attach its standard handlers */
  dbusmenu_gtkclient_newitem_base (DBUSMENU_GTKCLIENT(client),
                                   newitem,
                                   GTK_MENU_ITEM(w->gmi),
                                   parent);

  /* listen for property changes that would affect how we draw this item */
  g_signal_connect_data (newitem, SIGNAL_PROPERTY_CHANGED,
                         G_CALLBACK(on_app_property_changed),
                         w, (GClosureNotify)g_free, 0);

  return TRUE; /* success */
}


/***
****  SYNC_MENU_PROGRESS_MENUITEM_TYPE handler
****
****  FIXME: this would be a good candiate for promotion into IDO
****  if any other indicators need this in the future.
***/


struct ProgWidgets
{
  GtkWidget * gmi;
  GtkWidget * label;
  GtkWidget * progress_bar;
};

#define PROP_PERCENT  SYNC_MENU_PROGRESS_MENUITEM_PROP_PERCENT_DONE
#define PROP_LABEL    DBUSMENU_MENUITEM_PROP_LABEL

static inline int
get_percent_done (DbusmenuMenuitem * mi)
{
  return CLAMP (dbusmenu_menuitem_property_get_int (mi, PROP_PERCENT), 0, 100);
}

static void
prog_update_name (DbusmenuMenuitem * mi, struct ProgWidgets * w)
{
  const int percent_done = get_percent_done (mi);
  const gchar * name = dbusmenu_menuitem_property_get (mi, PROP_LABEL);

  char * text = g_strdup_printf ("%s ... %d%%", name, percent_done);
  gtk_label_set_text (GTK_LABEL(w->label), text);
  g_free (text);
}

static void
prog_update_percent (DbusmenuMenuitem * mi, struct ProgWidgets * w)
{
  const int percent_done = get_percent_done (mi);

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(w->progress_bar),
                                 percent_done/100.0);
}

/* update our display when some of the menuitem's properties change */
static void
on_prog_property_changed (DbusmenuMenuitem    * mi,
                          gchar               * prop,
                          GVariant            * value,
                          struct ProgWidgets  * w)
{
  if (!g_strcmp0 (prop, PROP_PERCENT))
    {
      prog_update_name (mi, w);
      prog_update_percent (mi, w);
    }
  else if (!g_strcmp0 (prop, PROP_LABEL))
    {
      prog_update_name (mi, w);
    }
}

/* Calculate a uniform width for the progmenus' progress bars & labels.
   This is kind of a fuzzy guesswork of wanting to ellipsize filenames
   after they get about 40 characters or so wide. */
static gint
calculate_prog_menuitem_preferred_width (GtkWidget * w)
{
   int width;
   const gchar * const test_case = "THIS STRING HAS FORTY CHARACTERS IN IT..";

   PangoLayout * pl;
   pl = gtk_widget_create_pango_layout (w, test_case);
   pango_layout_get_pixel_size (pl, &width, NULL);
   g_clear_object (&pl);

   return width;
}

static gboolean
new_item_prog (DbusmenuMenuitem  * newitem,
               DbusmenuMenuitem  * parent,
               DbusmenuClient    * client,
               gpointer            user_data)
{
  struct ProgWidgets * w;

  /* build the display widgets and initialize them */
  w = g_new0 (struct ProgWidgets, 1);
  w->label = gtk_label_new (NULL);
  w->progress_bar = gtk_progress_bar_new ();
  w->gmi = gtk_menu_item_new ();
  prog_update_name (newitem, w);
  prog_update_percent (newitem, w);

  /* layout & styling */
  GtkWidget * grid = gtk_grid_new ();
  const int width = calculate_prog_menuitem_preferred_width (w->label);
  gtk_widget_set_size_request (w->label, width, -1);
  gtk_label_set_ellipsize (GTK_LABEL(w->label), PANGO_ELLIPSIZE_MIDDLE);
  gtk_misc_set_alignment(GTK_MISC(w->label), 0.0, 0.5);
  gtk_grid_attach (GTK_GRID(grid), w->label, 0, 0, 1, 1);
  gtk_widget_set_size_request (w->progress_bar, width, -1);
  gtk_grid_attach (GTK_GRID(grid), w->progress_bar, 0, 1, 1, 1);
  gtk_container_add (GTK_CONTAINER(w->gmi), grid);
  gtk_container_set_border_width (GTK_CONTAINER(w->gmi), 4);
  gtk_widget_show_all (w->gmi);

  /* let dbusmenu attach its standard handlers */
  dbusmenu_gtkclient_newitem_base (DBUSMENU_GTKCLIENT(client),
                                   newitem,
                                   GTK_MENU_ITEM(w->gmi),
                                   parent);

  /* listen for property changes that would affect how we draw this item */
  g_signal_connect_data (newitem, SIGNAL_PROPERTY_CHANGED,
                         G_CALLBACK(on_prog_property_changed),
                         w, (GClosureNotify)g_free, 0);


  return TRUE; /* success */
}
