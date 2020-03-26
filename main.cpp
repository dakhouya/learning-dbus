#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

static void check_and_abort(DBusError *error);

// Dbus callback
static DBusHandlerResult dbus_messages_handler(DBusConnection *connection, DBusMessage *message, void *user_data);
static void respond_to_introspect(DBusConnection *connection, DBusMessage *request);
static void respond_to_get_status(DBusConnection *connection, DBusMessage *request);

#define DBUS_INTERFACE_NAME "com.dkhouya.learning1"
#define DBUS_INTERFACE_PATH "/com/dkhouya/learning1"

int main() {
    DBusConnection *connection = NULL;
    DBusError error;
    DBusObjectPathVTable vtable;


    dbus_error_init(&error);
    // Get connection
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    check_and_abort(&error);

    // Get name
    dbus_bus_request_name(connection, DBUS_INTERFACE_NAME, 0, &error);
    check_and_abort(&error);

    vtable.message_function = dbus_messages_handler;
    vtable.unregister_function = NULL;

    dbus_connection_try_register_object_path(connection,
                                             DBUS_INTERFACE_PATH,
                                             &vtable,
                                             NULL,
                                             &error);
    check_and_abort(&error);

    while(1) {
        dbus_connection_read_write_dispatch(connection, 1000);
    }

    return 0;
}

static void check_and_abort(DBusError *error) {
    if (!dbus_error_is_set(error)) return;
    puts(error->message);
    abort();
}

// Message handler
static DBusHandlerResult dbus_messages_handler(DBusConnection *connection, DBusMessage *message, void *user_data) {
    const char *interface_name = dbus_message_get_interface(message);
    const char *member_name = dbus_message_get_member(message);

    if (0==strcmp("org.freedesktop.DBus.Introspectable", interface_name) &&
        0==strcmp("Introspect", member_name)) {

        respond_to_introspect(connection, message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (0==strcmp(DBUS_INTERFACE_NAME, interface_name) &&
               0==strcmp("Status", member_name)) {

        respond_to_get_status(connection, message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
}

static void respond_to_introspect(DBusConnection *connection, DBusMessage *request) {
    DBusMessage *reply;

    const char *introspection_data =
            " <!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
            "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">"
            " <!-- dbus-sharp 0.8.1 -->"
            " <node>"
            "   <interface name=\"org.freedesktop.DBus.Introspectable\">"
            "     <method name=\"Introspect\">"
            "       <arg name=\"data\" direction=\"out\" type=\"s\" />"
            "     </method>"
            "   </interface>"
            "   <interface name=\"com.dkhouya.learning1\">"
            "     <method name=\"Status\">"
            "       <arg name=\"selection\" direction=\"in\" type=\"i\" />"
            "       <arg name=\"status\" direction=\"out\" type=\"s\" />"
            "     </method>"
            "   </interface>"
            " </node>";

    reply = dbus_message_new_method_return(request);
    dbus_message_append_args(reply,
                             DBUS_TYPE_STRING, &introspection_data,
                             DBUS_TYPE_INVALID);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
}

static void respond_to_get_status(DBusConnection *connection, DBusMessage *request) {
    DBusMessage *reply;
    DBusError error;
    int selection;
    const char* status0="Awesome status";
    const char* status1="Awesomer status";
    const char* status2="No status";
    const char* status;
    dbus_error_init(&error);

    dbus_message_get_args(request, &error,
                          DBUS_TYPE_INT32, &selection,
                          DBUS_TYPE_INVALID);
    if (dbus_error_is_set(&error)) {
        reply = dbus_message_new_error(request, "wrong_arguments", "Illegal arguments to Status");
        dbus_connection_send(connection, reply, NULL);
        dbus_message_unref(reply);
        return;
    }

    if (selection == 0) {
        status = status0;
    } else if (selection == 1) {
        status = status1;
    } else {
        status = status2;
    }

    reply = dbus_message_new_method_return(request);
    dbus_message_append_args(reply,
                             DBUS_TYPE_STRING, &status,
                             DBUS_TYPE_INVALID);
    dbus_connection_send(connection, reply, NULL);
    dbus_message_unref(reply);
}