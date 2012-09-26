#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <Ecore.h>
#include <E_DBus.h>
#include <Elocation.h>
#include <elocation_private.h>

static char *unique_name = NULL;

static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;

static void
_system_name_owner_changed(void *data , DBusMessage *msg)
{
   DBusError err;
   const char *name, *from, *to;

   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err,
                              DBUS_TYPE_STRING, &name,
                              DBUS_TYPE_STRING, &from,
                              DBUS_TYPE_STRING, &to,
                              DBUS_TYPE_INVALID))
     {
        dbus_error_free(&err);
        return;
     }

   if (strcmp(name, GEOCLUE_DBUS_NAME) != 0)
      return;

   if (from[0] == '\0' && to[0] != '\0')
     {
        ecore_event_add(E_LOCATION_EVENT_IN, NULL, NULL, NULL);
        unique_name = strdup(to);
     }
   else if (from[0] != '\0' && to[0] == '\0')
     {
        if (strcmp(unique_name, from) != 0)
           printf("%s was not the known name %s, ignored.\n", from, unique_name);
        else
           ecore_event_add(E_LOCATION_EVENT_OUT, NULL, NULL, NULL);
     }
   else
     {
        printf("unknow change from %s to %s\n", from, to);
     }
}

static void
_get_name_owner(void *data , DBusMessage *msg, DBusError *err)
{
   DBusMessageIter itr;
   const char *uid;

   pending_get_name_owner = NULL;

   if (!dbus_message_iter_init(msg, &itr))
      return;

   dbus_message_iter_get_basic(&itr, &uid);
   if (!uid)
        return;

   printf("enter GeoClue at %s (old was %s)\n", uid, unique_name);
   if (unique_name && strcmp(unique_name, uid) == 0)
     {
        return;
     }

   if (unique_name)
      ecore_event_add(E_LOCATION_EVENT_IN, NULL, NULL, NULL);

   unique_name = strdup(uid);
   return;
}

EAPI int
elocation_init(E_DBus_Connection *conn)
{
   if (E_LOCATION_EVENT_IN == 0)
      E_LOCATION_EVENT_IN = ecore_event_type_new();

   if (E_LOCATION_EVENT_OUT == 0)
      E_LOCATION_EVENT_OUT = ecore_event_type_new();

   cb_name_owner_changed = e_dbus_signal_handler_add
         (conn, E_DBUS_FDO_BUS, E_DBUS_FDO_PATH, E_DBUS_FDO_INTERFACE, "NameOwnerChanged", _system_name_owner_changed, NULL);

   if (pending_get_name_owner)
      dbus_pending_call_cancel(pending_get_name_owner);

   pending_get_name_owner = e_dbus_get_name_owner(conn, GEOCLUE_DBUS_NAME, _get_name_owner, NULL);
}

EAPI int
elocation_shutdown(E_DBus_Connection *conn)
{
   if (pending_get_name_owner)
     {
        dbus_pending_call_cancel(pending_get_name_owner);
        pending_get_name_owner = NULL;
     }

   if (cb_name_owner_changed)
     {
        e_dbus_signal_handler_del(conn, cb_name_owner_changed);
        cb_name_owner_changed = NULL;
     }
}
