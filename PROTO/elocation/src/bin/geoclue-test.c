#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <Ecore.h>
#include <EDBus.h>
#include <Elocation.h>
#include <elocation_private.h>

/* TODO:
   o Create a client object to operate on
   o Ask master for providers
   o Set requirements
   o Give out ecore events on signals
   o Start (how to stop?) service to get position updates
   o How to deal with AddReference and RemoveReference?
   o Implement MasterClient interface
   o Reply on address is inconsistent. Either all NULL or all empty
*/

static EDBus_Signal_Handler *cb_position_changed = NULL;
static EDBus_Signal_Handler *cb_address_changed = NULL;

static Eina_Bool
status_changed(void *data, int ev_type, void *event)
{
   unsigned int status = (unsigned int)&event;
   printf("Status changed to: %i\n", status);
   return ECORE_CALLBACK_DONE;
}

static void
provider_info_cb(void *data , const EDBus_Message *reply, EDBus_Pending *pending)
{
   char *name = NULL, *desc = NULL;
   const char *signature;

   signature = edbus_message_signature_get(reply);
   if (strcmp(signature, "ss")) return;

   if (!edbus_message_arguments_get(reply, "ss", &name, &desc)) return;

   printf("Provider name: %s\n description: %s\n", name, desc);
}

static void
address_cb(void *data , const EDBus_Message *reply, EDBus_Pending *pending)
{
   Elocation_Address address;
   int32_t level, timestamp;
   EDBus_Message_Iter *iter, *sub, *dict, *entry;
   double horizontal;
   double vertical;
   Elocation_Accuracy accur;
   const char *key, *signature;
   char *value;

   signature = edbus_message_signature_get(reply);
   if (strcmp(signature,"ia{ss}(idd)")) return;

   iter = edbus_message_iter_get(reply);

   edbus_message_iter_get_and_next(iter, 'i', &timestamp);

   edbus_message_iter_arguments_get(iter, "a{ss}", &dict);

   while (edbus_message_iter_get_and_next(dict, 'e', &entry))
    {
       edbus_message_iter_arguments_get(entry, "ss", &key, &value);

       if (!strcmp(key, "country"))
        {
            address.country = value;
            printf("Key: %s, value: %s\n", key, value);
         }
       else if (!strcmp(key, "countrycode"))
         {
            address.countrycode = value;
            printf("Key: %s, value: %s\n", key, value);
         }
       else if (!strcmp(key, "locality"))
         {
            address.locality = value;
            printf("Key: %s, value: %s\n", key, value);
         }
       else if (!strcmp(key, "postalcode"))
         {
            address.postalcode = value;
            printf("Key: %s, value: %s\n", key, value);
         }
       else if (!strcmp(key, "region"))
         {
            address.region = value;
            printf("Key: %s, value: %s\n", key, value);
         }
       else if (!strcmp(key, "timezone"))
         {
            address.timezone = value;
            printf("Key: %s, value: %s\n", key, value);
         }
    }

   edbus_message_iter_get_and_next(iter, 'r', &sub );
   edbus_message_iter_arguments_get(sub, "idd", &level, &horizontal, &vertical);
   accur.level = level;
   accur.horizontal = horizontal;
   accur.vertical = vertical;
}
static void
address_signal_cb(void *data , const EDBus_Message *reply)
{
   address_cb(data, reply, NULL);
}

static void
unmarshall_position(Elocation_Position *position, const EDBus_Message *reply)
{
   GeocluePositionFields fields;
   int32_t level, timestamp;
   double horizontal = 0.0;
   double vertical = 0.0;
   double latitude = 0.0;
   double longitude = 0.0;
   double altitude = 0.0;
   EDBus_Message_Iter *iter, *sub;
   const char *err, *errmsg;
   const char *signature;

   if (edbus_message_error_get(reply, &err, &errmsg))
     {
        fprintf(stderr, "Error: %s %s\n", err, errmsg);
        return;
     }

   signature = edbus_message_signature_get(reply);
   if (strcmp(signature, "iiddd(idd)"))
     {
        fprintf(stderr, "Error: position callback message did not match signature\n");
        return;
     }

   iter = edbus_message_iter_get(reply);

   // Possible to use a single edbus_message_iter_arguments_get(sub, "iiddd(idd)" ... here?

   edbus_message_iter_get_and_next(iter, 'i', &fields);
   position->fields = fields;

   edbus_message_iter_get_and_next(iter, 'i', &timestamp);
   position->timestamp = timestamp;

   edbus_message_iter_get_and_next(iter, 'd', &latitude);
   position->latitude = latitude;

   edbus_message_iter_get_and_next(iter, 'd', &longitude);
   position->longitude = longitude;

   edbus_message_iter_get_and_next(iter, 'd', &altitude);
   position->altitude = altitude;

   edbus_message_iter_get_and_next(iter, 'r', &sub );
   edbus_message_iter_arguments_get(sub, "idd", &level, &horizontal, &vertical);
   position->accur->level = level;
   position->accur->horizontal = horizontal;
   position->accur->vertical = vertical;
}

static void
position_cb(void *data , const EDBus_Message *reply, EDBus_Pending *pending)
{
   Elocation_Position *position;

   position = malloc(sizeof(Elocation_Position));
   position->accur = malloc(sizeof(Elocation_Accuracy));

   unmarshall_position(position, reply);

   printf("DBus GeoClue reply with data from timestamp %i\n", position->timestamp);

   if (position->fields & GEOCLUE_POSITION_FIELDS_LATITUDE)
      printf("Latitude:\t %f (valid)\n", position->latitude);
   else
      printf("Latitude:\tinvalid.\n");

   if (position->fields & GEOCLUE_POSITION_FIELDS_LONGITUDE)
      printf("Longitude:\t %f (valid)\n", position->longitude);
   else
      printf("Longitude:\tinvalid.\n");

   if (position->fields & GEOCLUE_POSITION_FIELDS_ALTITUDE)
      printf("Altitude:\t %f (valid)\n", position->altitude);
   else
      printf("Altitude:\tinvalid.\n");

   printf("Accuracy: level %i, horizontal %f and vertical %f\n", position->accur->level, position->accur->horizontal, position->accur->vertical);

   free(position->accur);
   free(position);
}

static void
position_signal_cb(void *data , const EDBus_Message *reply)
{
   Elocation_Position *position;

   position = malloc(sizeof(position));

   unmarshall_position(position, reply);
   ecore_event_add(ELOCATION_EVENT_POSITION, position, NULL, NULL);
}

int
main()
{
   EDBus_Connection *conn;
   EDBus_Message *msg;
   EDBus_Object *obj;
   EDBus_Proxy *manager, *manager_address, *manager_position;
   EDBus_Pending *pending, *pending2, *pending3;
   Elocation_Accuracy *accur;

   ecore_init();
   edbus_init();

   conn = edbus_connection_get(EDBUS_CONNECTION_TYPE_SESSION);
   if (!conn)
     {
      printf("Error: could not connect to session bus.\n");
      return EXIT_FAILURE;
     }

   // FIXME conn should no longer be needed here when all dbus calls are moved into the lib
   elocation_init(conn);

   ecore_event_handler_add(ELOCATION_EVENT_STATUS, status_changed, NULL);

   obj = edbus_object_get(conn, UBUNTU_DBUS_NAME, UBUNTU_OBJECT_PATH);
   if (!obj)
     {
        fprintf(stderr, "Error: could not get object\n");
        return EXIT_FAILURE;
     }

   manager = edbus_proxy_get(obj, GEOCLUE_IFACE);
   if (!manager)
     {
        fprintf(stderr, "Error: could not get proxy\n");
        return EXIT_FAILURE;
     }

   manager_address = edbus_proxy_get(obj, GEOCLUE_ADDRESS_IFACE);
   if (!manager_address)
     {
        fprintf(stderr, "Error: could not get proxy\n");
        return EXIT_FAILURE;
     }

   manager_position = edbus_proxy_get(obj, GEOCLUE_POSITION_IFACE);
   if (!manager_position)
     {
        fprintf(stderr, "Error: could not get proxy\n");
        return EXIT_FAILURE;
     }

   pending = edbus_proxy_call(manager, "GetProviderInfo", provider_info_cb, NULL, -1, "");
   if (!pending)
     {
        fprintf(stderr, "Error: could not call\n");
        return EXIT_FAILURE;
     }

   // SetOptions

   pending2 = edbus_proxy_call(manager_address, "GetAddress", address_cb, NULL, -1, "");
   if (!pending2)
     {
        fprintf(stderr, "Error: could not call\n");
        return EXIT_FAILURE;
     }

   cb_address_changed = edbus_proxy_signal_handler_add(manager_address, "GetAddress", address_signal_cb, NULL);

   pending3 = edbus_proxy_call(manager_position, "GetPosition", position_cb, NULL, -1, "");
   if (!pending3)
     {
        fprintf(stderr, "Error: could not call\n");
        return EXIT_FAILURE;
     }

   cb_position_changed = edbus_proxy_signal_handler_add(manager_position, "GetPosition", position_signal_cb, NULL);

   ecore_main_loop_begin();

   edbus_signal_handler_unref(cb_address_changed);
   edbus_signal_handler_unref(cb_position_changed);
   edbus_connection_unref(conn);

   elocation_shutdown(conn);
   edbus_shutdown();
   ecore_shutdown();
   return 0;
}