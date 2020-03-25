#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <dbus/dbus.h>

int main() {
    DBusConnection *connection = NULL;
    DBusError error;
    char buffer[1024];

    dbus_error_init(&error);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "%s", error.message);
        abort();
    }

    puts("This is my unique name");
    puts(dbus_bus_get_unique_name(connection));
    fgets(buffer, sizeof(buffer), stdin);

    return 0;
}