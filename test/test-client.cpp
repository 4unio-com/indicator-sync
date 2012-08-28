
#include <glib-object.h>
#include <gio/gio.h>
#include <glib.h>

#include <gtest/gtest.h>

#include "dbus-shared.h"
#include "sync-service-dbus.h"
#include "sync-menu/sync-app.h"

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

    void ServiceProxyShutdown ()
    {
      GError * err;
      GVariant * ret;

      ASSERT_TRUE (session_bus != NULL);
      ASSERT_TRUE (service_proxy != NULL);

      err = NULL;
      ret = g_dbus_connection_call_sync (
              session_bus,
              SYNC_SERVICE_DBUS_NAME,
              "/org/ayatana/indicator/service",
              "org.ayatana.indicator.service",
              "Shutdown", NULL,
              NULL,
              G_DBUS_CALL_FLAGS_NONE,
              -1, NULL, &err);
      g_clear_pointer (&ret, g_variant_unref);
      if (err != NULL)
        g_error ("Unable to connect to %s: %s", SYNC_SERVICE_DBUS_NAME, err->message);

      ASSERT_TRUE (err == NULL);
    }

    // undo SetUpServiceProxy
    void TearDownServiceProxy ()
    {
      ASSERT_TRUE (service_proxy != NULL);

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


TEST_F(ClientTest, TestCanStartService)
{
  ASSERT_TRUE (service_proxy == NULL);
  SetUpServiceProxy (true);
  ASSERT_TRUE (ServiceProxyIsOwned ());
  TearDownServiceProxy ();
}


TEST_F(ClientTest, AppCanStartService)
{
  SyncMenuApp * app;
  SetUpServiceProxy (false);

  app = sync_menu_app_new ("transmission-gtk.desktop");
  WaitForSignal (service_proxy, "notify::g-name-owner");
  ASSERT_TRUE (ServiceProxyIsOwned ());

  WaitForSignal (service_proxy, "notify::client-count");
  ASSERT_EQ (1, dbus_sync_service_get_client_count (service_proxy));

  ServiceProxyShutdown ();
  WaitForSignal (service_proxy, "notify::g-name-owner");
  ASSERT_FALSE (ServiceProxyIsOwned ());
  ASSERT_EQ (0, dbus_sync_service_get_client_count (service_proxy));

  TearDownServiceProxy ();
  g_clear_object (&app);
}

/* wow this next test is not ready yet
   but I'm at a good stopping point for the night...
   I'll just disable it and commit */
#if 0
TEST_F(ClientTest, ServiceCountsClients)
{
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  // start up the service
  SetUpServiceProxy (true);
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  ASSERT_TRUE (ServiceProxyIsOwned ());
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  ASSERT_EQ (0, dbus_sync_service_get_client_count (service_proxy));
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);

  // add a client
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  SyncMenuApp * app_1 = sync_menu_app_new ("transmission-gtk.desktop");
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  WaitForSignal (service_proxy, "notify::client-count");
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  ASSERT_TRUE (ServiceProxyIsOwned ());
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
  ASSERT_EQ (1, dbus_sync_service_get_client_count (service_proxy));
      g_debug (G_STRLOC" the dbus connection %p refcount is %d", session_bus, G_OBJECT(session_bus)->ref_count);
 
  // remove the client
g_message (G_STRLOC" ... count is %d", dbus_sync_service_get_client_count(service_proxy));
g_clear_object (&app_1); 
  WaitForSignal (service_proxy, "notify::client-count");
//  ASSERT_TRUE (ServiceProxyIsOwned ());
  ASSERT_EQ (0, dbus_sync_service_get_client_count (service_proxy));

#if 0
  //client_2 = sync_client_new ("ubuntuone-installer.desktop");
  //WaitForSignal (service_proxy, "notify::client-count");
  //ASSERT_EQ (2, dbus_sync_service_get_client_count (service_proxy));

g_message (G_STRLOC" ... count is %d", dbus_sync_service_get_client_count(service_proxy));
//g_message (G_STRLOC" unreffing client_2");
 // g_clear_object (&client_2);
g_message (G_STRLOC" ... count is %d", dbus_sync_service_get_client_count(service_proxy));
  WaitForSignal (service_proxy, "notify::client-count");
g_message (G_STRLOC" ... count is %d", dbus_sync_service_get_client_count(service_proxy));
  ASSERT_EQ (1, dbus_sync_service_get_client_count (service_proxy));
g_message (G_STRLOC" ... count is %d", dbus_sync_service_get_client_count(service_proxy));

g_message (G_STRLOC" unreffing client_1");
  g_clear_object (&client_1);
g_message (G_STRLOC);
  WaitForSignal (service_proxy, "notify::client-count");
g_message (G_STRLOC);
  ASSERT_EQ (0, dbus_sync_service_get_client_count (service_proxy));
g_message (G_STRLOC);
#endif

g_message (G_STRLOC);
  ServiceProxyShutdown ();
g_message (G_STRLOC);
  TearDownServiceProxy ();
//g_message (G_STRLOC);
  //g_clear_object (&client_1);
}
#endif
