/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   cuteime                                                                  *
 *   Copyright (C) 2010 by Tasuku Suzuki <stasuku@gmail.com>                 *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Lesser Public License as          *
 *   published by the Free Software Foundation; either version 2 of the      *
 *   License, or (at your option) any later version.                         *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU Lesser General Public License for more details.                     *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this program; if not, write to the                   *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "cuteimeabstractipcobject.h"
#include "cuteimedebug.h"

static void cuteime_abstract_ipc_object_dispose(GObject *gobject);

G_DEFINE_TYPE(CuteimeAbstractIpcObject, cuteime_abstract_ipc_object, G_TYPE_OBJECT)

static void cuteime_abstract_ipc_object_class_init(CuteimeAbstractIpcObjectClass *klass)
{
    cuteime_debug_in();
    G_OBJECT_CLASS(klass)->dispose = cuteime_abstract_ipc_object_dispose;
    cuteime_debug_out();
}

static void cuteime_abstract_ipc_object_init(CuteimeAbstractIpcObject *qaio)
{
    cuteime_debug_in();
    qaio->proxy = 0;
    qaio->is_connected = FALSE;
    cuteime_debug_out();
}

static void cuteime_abstract_ipc_object_dispose(GObject *gobject)
{
    CuteimeAbstractIpcObject *qaio;
    cuteime_debug_in();
    qaio = CUTEIMEABSTRACTIPCOBJECT(gobject);
    if (qaio->proxy) {
        g_object_unref(qaio->proxy);
    }
    G_OBJECT_CLASS(cuteime_abstract_ipc_object_parent_class)->dispose(gobject);
    cuteime_debug_out();
}

void cuteime_abstract_ipc_object_connect(CuteimeAbstractIpcObject *qaio, const char *name, const char *path, const char *interface)
{
    DBusGConnection *connection;
    GError *error;
    cuteime_debug_in();

    error = NULL;
    connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
        g_printerr("Failed to open connection to bus: %s\n", error->message);
        g_error_free(error);
        return;
    }

    if (qaio->proxy) {
        g_object_unref(qaio->proxy);
    }

    /* Create a proxy object */
    qaio->proxy = dbus_g_proxy_new_for_name(connection, name, path, interface);

    cuteime_debug_out();
}

gboolean cuteime_abstract_ipc_object_is_connected(CuteimeAbstractIpcObject *qaio)
{
    GError *error;
    gboolean pong;
    cuteime_debug_in();
    cuteime_debug("%s(%d) qaio->is_connected = %s\n", __PRETTY_FUNCTION__, __LINE__, qaio->is_connected ? "True" : "False");
    if (!qaio->is_connected) {
        error = NULL;
        qaio->is_connected = dbus_g_proxy_call(qaio->proxy, "ping", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, &pong, G_TYPE_INVALID);
    }
    cuteime_debug("%s(%d) ret = %s\n", __PRETTY_FUNCTION__, __LINE__, qaio->is_connected ? "True" : "False");
    cuteime_debug_out();
    return qaio->is_connected;
}
