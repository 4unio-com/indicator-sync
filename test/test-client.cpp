
#include <glib-object.h>
#include <gio/gio.h>
#include <glib.h>

#include <gtest/gtest.h>

#include "dbus-shared.h"
#include "sync-service-dbus.h"
#include "sync-menu/sync-app.h"

#define INDICATOR_SERVICE_OBJECT_PATH "/org/ayatana/indicator/service"
#define INDICATOR_SERVICE_INTERFACE_NAME "org.ayatana.indicator.service"

class ClientTest : public ::testing::Test
{
  protected:

    GTestDBus * test_dbus;
    GDBusConnection * session_bus;
    GMainLoop * main_loop;
    DbusSyncService * service_proxy;

    virtual void SetUp()
    {
      test_dbus = NULL;
      session_bus = NULL;
      main_loop = NULL;
      service_proxy = NULL;

      static bool ran_once_init = false;
      if (!ran_once_init)
        {
          g_type_init();
          g_setenv ("INDICATOR_SERVICE_SHUTDOWN_TIMEOUT", "5000", TRUE);
          g_unsetenv ("INDICATOR_ALLOW_NO_WATCHERS");
          g_unsetenv ("INDICATOR_SERVICE_REPLACE_MODE");
          ran_once_init = true;
        }

      main_loop = g_main_loop_new (NULL, FALSE);
      // pull up a test dbus that's pointed at our test .service file
      test_dbus = g_test_dbus_new (G_TEST_DBUS_NONE);
      g_debug (G_STRLOC" service dir path is \"%s\"", INDICATOR_SERVICE_DIR);
      g_test_dbus_add_service_dir (test_dbus, INDICATOR_SERVICE_DIR);

      // allow the service to exist w/o a sync indicator
      g_setenv ("INDICATOR_ALLOW_NO_WATCHERS", "1", TRUE);

      g_test_dbus_up (test_dbus);
      g_debug (G_STRLOC" this test bus' address is \"%s\"", g_test_dbus_get_bus_address(test_dbus));
      session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
      g_debug (G_STRLOC" the dbus connection %p unique name is \"%s\"", session_bus, g_dbus_connection_get_unique_name(session_bus));
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
    }

    // undo SetUp
    virtual void TearDown()
    {
      g_clear_object (&session_bus);
      g_debug (G_STRLOC" tearing down the bus");
      g_test_dbus_down (test_dbus);
      g_clear_object (&test_dbus);
      g_clear_pointer (&main_loop, g_main_loop_unref);
    }

    void SetUpServiceProxy (bool autostart=false)
    {
      GError * err;

      ASSERT_TRUE (session_bus != NULL);

      int flags = G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES;
      if (!autostart)
        flags |= G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START;

      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
      err = NULL;
      service_proxy = dbus_sync_service_proxy_new_sync (
        session_bus, 
        GDBusProxyFlags(flags),
        SYNC_SERVICE_DBUS_NAME,
        SYNC_SERVICE_DBUS_OBJECT,
        NULL, &err);
      if (err != NULL)
        g_error ("unable to create service proxy: %s", err->message);
      g_debug (G_STRLOC" the service proxy %p refcount is %d", service_proxy, G_OBJECT(service_proxy)->ref_count);
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
      g_debug (G_STRLOC" proxy's name owner is \"%s\"", g_dbus_proxy_get_name_owner(G_DBUS_PROXY(service_proxy)));

      ASSERT_TRUE (err == NULL);
      ASSERT_EQ (dbus_sync_service_get_client_count (service_proxy), 0);
    }

    bool ServiceProxyIsOwned () const
    {
      g_return_val_if_fail (service_proxy != NULL, FALSE);

      gchar * owner = g_dbus_proxy_get_name_owner (G_DBUS_PROXY(service_proxy));
      const bool is_owned = owner && *owner;
      g_free (owner);
      return is_owned;
    }

    void CallIndicatorServiceMethod (const gchar * method)
    {
      GVariant * ret;

      ASSERT_TRUE (session_bus != NULL);
      ASSERT_TRUE (service_proxy != NULL);
      ASSERT_TRUE (ServiceProxyIsOwned ());

      ret = g_dbus_connection_call_sync (
              session_bus,
              SYNC_SERVICE_DBUS_NAME,
              INDICATOR_SERVICE_OBJECT_PATH,
              INDICATOR_SERVICE_INTERFACE_NAME,
              method, NULL,
              NULL,
              G_DBUS_CALL_FLAGS_NONE,
              -1, NULL, NULL);

      g_clear_pointer (&ret, g_variant_unref);
    }

    void ServiceProxyShutdown ()
    {
      CallIndicatorServiceMethod ("Shutdown");
      WaitForSignal (service_proxy, "notify::g-name-owner");

      ASSERT_FALSE (ServiceProxyIsOwned ());
    }

    // undo SetUpServiceProxy
    void TearDownServiceProxy ()
    {
      ASSERT_TRUE (service_proxy != NULL);

      if (ServiceProxyIsOwned ())
          ServiceProxyShutdown ();

      g_clear_object (&service_proxy);
    }

    static gboolean
    on_wait_timeout (gpointer main_loop)
    {
      g_main_loop_quit (static_cast<GMainLoop*>(main_loop));
      return G_SOURCE_REMOVE;
    }

    void
    WaitForSignal (gpointer instance, const gchar * detailed_signal)
    {
      guint timeout_id;
      gulong handler_id;
      const int timeout_seconds = 5;

      ASSERT_TRUE (instance != NULL);
      ASSERT_TRUE (main_loop != NULL);


      handler_id = g_signal_connect_swapped (instance,
                                             detailed_signal,
                                             G_CALLBACK(g_main_loop_quit),
                                             main_loop);

      timeout_id = g_timeout_add_seconds (timeout_seconds,
                                          on_wait_timeout,
                                          main_loop);

      // wait for the signal or for timeout, whichever comes first
      g_main_loop_run (main_loop);
      ASSERT_TRUE (g_main_context_find_source_by_id(NULL,timeout_id) != NULL);
      g_signal_handler_disconnect (instance, handler_id);
      g_source_remove (timeout_id);
    }

    static void log_counter_func (const gchar *log_domain,
                                  GLogLevelFlags log_level,
                                  const gchar *message,
                                  gpointer counter)
    {
      ++*static_cast<guint*>(counter);
    }
};

/***
****
***/

TEST_F (ClientTest, TestNullDesktop)
{
  guint count;
  guint handler;
  GLogLevelFlags level;
  SyncMenuApp * app;
  const gchar * log_domain = "libsync-menu";

  // passing a .desktop ID of NULL should generate a warning message...
  count = 0;
  level = G_LOG_LEVEL_WARNING;
  handler = g_log_set_handler (log_domain, level, log_counter_func, &count);
  app = sync_menu_app_new (NULL);
  ASSERT_EQ (1, count);
  ASSERT_TRUE (sync_menu_app_get_desktop_id (app) == NULL);
  g_log_remove_handler (log_domain, handler);

  // ... and it should also generate a critical after we get the bus
  // and try to use the desktop-id to generate an object path
  count = 0;
  level = G_LOG_LEVEL_CRITICAL;
  handler = g_log_set_handler (log_domain, level, log_counter_func, &count);
  while (!count)
    {
      g_timeout_add (100, on_wait_timeout, main_loop);
      g_main_loop_run (main_loop);
    }
  ASSERT_EQ (1, count);
  g_log_remove_handler (log_domain, handler);

  // cleanup
  g_clear_object (&app);
}


TEST_F (ClientTest, TestAccessors)
{
  SyncMenuApp * app;
  DbusmenuServer * menu_server;
  const gboolean paused = TRUE;
  const gchar * const desktop_id = "transmission-gtk.desktop";
  const SyncMenuState state = SYNC_MENU_STATE_ERROR;
 
  // create a new SyncMenuApp 
  app = sync_menu_app_new (desktop_id);

  // test its setters & getters
  menu_server = dbusmenu_server_new ("/dbusmenu/ubuntuone");
  sync_menu_app_set_menu (app, menu_server);
  sync_menu_app_set_state (app, state);
  sync_menu_app_set_paused (app, paused);
  ASSERT_EQ (state, sync_menu_app_get_state (app));
  ASSERT_EQ (paused, sync_menu_app_get_paused (app));
  ASSERT_EQ (menu_server, sync_menu_app_get_menu (app));
  ASSERT_STREQ (desktop_id, sync_menu_app_get_desktop_id (app));

  // cleanup
  g_clear_object (&app);
  g_clear_object (&menu_server);
}

TEST_F (ClientTest, TestGObjectAccessors)
{
  gchar * d;
  gboolean p;
  SyncMenuState s;
  DbusmenuServer * ms;
  SyncMenuApp * app;
  DbusmenuServer * menu_server;
  const gboolean paused = TRUE;
  const gchar * const desktop_id = "transmission-gtk.desktop";
  const SyncMenuState state = SYNC_MENU_STATE_ERROR;
 
  // create a new SyncMenuApp 
  menu_server = dbusmenu_server_new ("/dbusmenu/ubuntuone");
  app = SYNC_MENU_APP (g_object_new (SYNC_MENU_APP_TYPE,
                                     SYNC_MENU_APP_PROP_DESKTOP_ID, desktop_id,
                                     SYNC_MENU_APP_PROP_STATE, state,
                                     SYNC_MENU_APP_PROP_PAUSED, paused,
                                     SYNC_MENU_APP_PROP_DBUSMENU, menu_server,
                                     NULL));

  // test its properties via g_object_get
  g_object_get (G_OBJECT(app), SYNC_MENU_APP_PROP_STATE, &s,
                               SYNC_MENU_APP_PROP_PAUSED, &p,
                               SYNC_MENU_APP_PROP_DESKTOP_ID, &d,
                               SYNC_MENU_APP_PROP_DBUSMENU, &ms,
                               NULL);
  ASSERT_EQ (state, s);
  ASSERT_EQ (paused, p);
  ASSERT_EQ (menu_server, ms);
  ASSERT_STREQ (desktop_id, d);
  g_clear_object (&ms);
  g_free (d);

  g_clear_object (&app);
  g_clear_object (&menu_server);
}

/***
****
***/

TEST_F (ClientTest, TestCanStartService)
{
  ASSERT_TRUE (service_proxy == NULL);
  SetUpServiceProxy (true);
  ASSERT_TRUE (ServiceProxyIsOwned ());
  TearDownServiceProxy ();
}

TEST_F (ClientTest, AppCanStartService)
{
  SyncMenuApp * app;
  SetUpServiceProxy (false);

  app = sync_menu_app_new ("transmission-gtk.desktop");
  WaitForSignal (service_proxy, "notify::g-name-owner");
  ASSERT_TRUE (ServiceProxyIsOwned ());

  WaitForSignal (service_proxy, "notify::client-count");
  ASSERT_EQ (1, dbus_sync_service_get_client_count (service_proxy));

  TearDownServiceProxy ();
  g_clear_object (&app);
}

/***
****  Confirm that changing our menu triggers the service to layout-update
***/

TEST_F (ClientTest, TestMenu)
{
  SyncMenuApp * app;
  DbusmenuMenuitem * root;
  DbusmenuServer * menu_server;
  DbusmenuClient * menu_client;
  SetUpServiceProxy (false);

  app = sync_menu_app_new ("transmission-gtk.desktop");
  WaitForSignal (service_proxy, "notify::g-name-owner");
  ASSERT_TRUE (ServiceProxyIsOwned ());

  WaitForSignal (service_proxy, "notify::client-count");
  ASSERT_EQ (1, dbus_sync_service_get_client_count (service_proxy));

  // add a client to listen to menu changes from the service
  menu_client = dbusmenu_client_new (SYNC_SERVICE_DBUS_NAME,
                                     SYNC_SERVICE_DBUS_MENU_OBJECT);

  // add a menu to the SyncMenuApp
  root = dbusmenu_menuitem_new ();
  menu_server = dbusmenu_server_new ("/dbusmenu/ubuntuone");
  dbusmenu_server_set_root (menu_server, root);
  WaitForSignal (menu_client, "layout-updated");
  sync_menu_app_set_menu (app, menu_server);
  WaitForSignal (menu_client, "layout-updated");

  // cleanup
  g_clear_object (&menu_client);
  g_clear_object (&menu_server);
  g_clear_object (&app);
  TearDownServiceProxy ();
}

/***
****  Add a handful of SyncMenuApps & confirm that the service counts them
***/

TEST_F (ClientTest, TestClientCount)
{
  SyncMenuApp * app[3];

  ASSERT_TRUE (service_proxy == NULL);
  SetUpServiceProxy (true);
  ASSERT_TRUE (ServiceProxyIsOwned ());

  app[0] = sync_menu_app_new ("a.desktop");
  WaitForSignal (service_proxy, "notify::client-count");
  ASSERT_EQ (1, dbus_sync_service_get_client_count (service_proxy));

  app[1] = sync_menu_app_new ("b.desktop");
  WaitForSignal (service_proxy, "notify::client-count");
  ASSERT_EQ (2, dbus_sync_service_get_client_count (service_proxy));

  app[2] = sync_menu_app_new ("c.desktop");
  WaitForSignal (service_proxy, "notify::client-count");
  ASSERT_EQ (3, dbus_sync_service_get_client_count (service_proxy));

  g_clear_object (&app[2]);
  g_clear_object (&app[1]);
  g_clear_object (&app[0]);
  TearDownServiceProxy ();
}

/***
****  Confirm that the service's 'paused' property is true
****  iff any of the clients are paused
***/

TEST_F (ClientTest, TestPaused)
{
  /* start up the service */
  ASSERT_TRUE (service_proxy == NULL);
  SetUpServiceProxy (true);
  ASSERT_TRUE (ServiceProxyIsOwned ());

  /* add three SyncMenuApps */
  SyncMenuApp * app[3];
  app[0] = sync_menu_app_new ("a.desktop");
  app[1] = sync_menu_app_new ("b.desktop");
  app[2] = sync_menu_app_new ("c.desktop");
  while (dbus_sync_service_get_client_count (service_proxy) != 3)
    WaitForSignal (service_proxy, "notify::client-count");

  /* confirm that setting any one of the SyncMenuApps
     to paused will set the service's "Paused" property */
  ASSERT_FALSE (dbus_sync_service_get_paused (service_proxy));
  for (int i=0; i<3; i++)
    {
      sync_menu_app_set_paused (app[i], true);
      WaitForSignal (service_proxy, "notify::paused");
      ASSERT_TRUE (dbus_sync_service_get_paused (service_proxy));

      sync_menu_app_set_paused (app[i], false);
      WaitForSignal (service_proxy, "notify::paused");
      ASSERT_FALSE (dbus_sync_service_get_paused (service_proxy));
    }

  /* cleanup */
  g_clear_object (&app[2]);
  g_clear_object (&app[1]);
  g_clear_object (&app[0]);
  TearDownServiceProxy ();
}

/***
****  Test that SyncService correctly reports the right SyncMenuState
****  based on the SyncMenuApps that are connected to it
***/

TEST_F (ClientTest, TestState)
{
  /* start up the service */
  ASSERT_TRUE (service_proxy == NULL);
  SetUpServiceProxy (true);
  ASSERT_TRUE (ServiceProxyIsOwned ());

  /* add three SyncMenuApps */
  SyncMenuApp * app[3];
  app[0] = sync_menu_app_new ("a.desktop");
  app[1] = sync_menu_app_new ("b.desktop");
  app[2] = sync_menu_app_new ("c.desktop");
  while (dbus_sync_service_get_client_count (service_proxy) != 3)
    WaitForSignal (service_proxy, "notify::client-count");

  /* the default state is "idle" */
  ASSERT_TRUE (sync_menu_app_get_state (app[0]) == SYNC_MENU_STATE_IDLE);
  ASSERT_TRUE (sync_menu_app_get_state (app[1]) == SYNC_MENU_STATE_IDLE);
  ASSERT_TRUE (sync_menu_app_get_state (app[2]) == SYNC_MENU_STATE_IDLE);
  ASSERT_TRUE (dbus_sync_service_get_state (service_proxy) == SYNC_MENU_STATE_IDLE);

  /* test all the state combinations for three SyncMenuApps */
  const SyncMenuState test_states[27][4] =
  {
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING },

    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING },

    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_IDLE,    SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_ERROR },
    { SYNC_MENU_STATE_ERROR,   SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_SYNCING, SYNC_MENU_STATE_ERROR }
  };

  for (int i=0; i<27; i++)
    {
      /* set the three apps' states */
      for (int j=0; j<3; j++)
        {
          SyncMenuApp * a = app[j];
          const SyncMenuState state = test_states[i][j];
          sync_menu_app_set_state (a, state);
          ASSERT_EQ (state, sync_menu_app_get_state(a));
        }

      /* test to see the service's state reflects */
      const SyncMenuState expected = test_states[i][3];
      if (dbus_sync_service_get_state (service_proxy) != expected)
        WaitForSignal (service_proxy, "notify::state");
      ASSERT_EQ (expected, dbus_sync_service_get_state (service_proxy));
    }

  /* cleanup */
  g_clear_object (&app[2]);
  g_clear_object (&app[1]);
  g_clear_object (&app[0]);
  TearDownServiceProxy ();
}
