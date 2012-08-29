
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
      GError * err;
      GVariant * ret;

      ASSERT_TRUE (session_bus != NULL);
      ASSERT_TRUE (service_proxy != NULL);
      ASSERT_TRUE (ServiceProxyIsOwned ());

      err = NULL;
      ret = g_dbus_connection_call_sync (
              session_bus,
              SYNC_SERVICE_DBUS_NAME,
              INDICATOR_SERVICE_OBJECT_PATH,
              INDICATOR_SERVICE_INTERFACE_NAME,
              method, NULL,
              NULL,
              G_DBUS_CALL_FLAGS_NONE,
              -1, NULL, &err);
      g_clear_pointer (&ret, g_variant_unref);
      if (err != NULL)
          g_warning ("Error calling \"%s\": %s", method, err->message);

      EXPECT_TRUE (err == NULL);
      g_clear_error (&err);
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

    void WaitForSignal (gpointer instance, const gchar * detailed_signal)
    {
      ASSERT_TRUE (instance != NULL);
      ASSERT_TRUE (main_loop != NULL);

      const gulong handler_id = g_signal_connect_swapped (instance, detailed_signal, G_CALLBACK(g_main_loop_quit), main_loop);
      g_main_loop_run (main_loop);
      g_signal_handler_disconnect (instance, handler_id);
    }
};

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
