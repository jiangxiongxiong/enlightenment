/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_response.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/10/08 16:58:46 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EXALT_DBUS_RESPONSE_INC
#define  EXALT_DBUS_RESPONSE_INC


typedef enum _exalt_dbus_response_type Exalt_DBus_Response_Type;
typedef struct _exalt_dbus_response Exalt_DBus_Response;

#include "libexalt_dbus.h"

enum _exalt_dbus_response_type
{
    EXALT_DBUS_RESPONSE_DNS_ADD,
    EXALT_DBUS_RESPONSE_DNS_DEL,
    EXALT_DBUS_RESPONSE_DNS_REPLACE,
    EXALT_DBUS_RESPONSE_DNS_LIST_GET,
    EXALT_DBUS_RESPONSE_IFACE_IP_GET
};

int exalt_dbus_response_valid_is(Exalt_DBus_Response *response);

Exalt_DBus_Response_Type exalt_dbus_response_type_get(Exalt_DBus_Response* response);

int exalt_dbus_response_error_id_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_error_msg_get(Exalt_DBus_Response* response);

Ecore_List* exalt_dbus_response_list_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_address_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_iface_get(Exalt_DBus_Response* response);


#endif   /* ----- #ifndef EXALT_DBUS_RESPONSE_INC  ----- */

