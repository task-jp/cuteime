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

#include "cuteimekeyboardmanager.h"
#include "cuteimedebug.h"
#include "dbus.h"
#include "cuteimemarshalers.h"

enum {
    CUTEIMEKEYBOARDMANAGER_VISIBLE_CHANGED,
    CUTEIMEKEYBOARDMANAGER_SHARED_DATA_CHANGED,
    CUTEIMEKEYBOARDMANAGER_LAST_SIGNAL
};

static guint cuteime_keyboard_manager_signals[CUTEIMEKEYBOARDMANAGER_LAST_SIGNAL] = { 0, 0 };

G_DEFINE_TYPE(CuteimeKeyboardManager, cuteime_keyboard_manager, CUTEIMEABSTRACTIPCOBJECT_TYPE)

static void cuteime_keyboard_manager_visible_changed(DBusGProxy *proxy, gboolean value, gpointer user_data);

static void cuteime_keyboard_manager_shared_data_changed(DBusGProxy *proxy, const char *value, gpointer user_data);

static void cuteime_keyboard_manager_class_init(CuteimeKeyboardManagerClass *klass)
{
    cuteime_keyboard_manager_signals[CUTEIMEKEYBOARDMANAGER_VISIBLE_CHANGED]
            = g_signal_new("visible-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeKeyboardManagerClass, cuteime_keyboard_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__BOOLEAN,
                           G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

    cuteime_keyboard_manager_signals[CUTEIMEKEYBOARDMANAGER_SHARED_DATA_CHANGED]
            = g_signal_new("shared-data-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeKeyboardManagerClass, cuteime_keyboard_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void cuteime_keyboard_manager_init(CuteimeKeyboardManager *qkm)
{
    CuteimeAbstractIpcObject *qaio;
    cuteime_debug_in();

    qaio = CUTEIMEABSTRACTIPCOBJECT(qkm);

    /* Create a proxy object */
    cuteime_abstract_ipc_object_connect(qaio, CUTEIME_DBUS_SERVICE, "/CuteimeKeyboardManager", "local.KeyboardManager");

    dbus_g_proxy_add_signal(qaio->proxy, "visibleChanged", G_TYPE_BOOLEAN, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "visibleChanged", G_CALLBACK(cuteime_keyboard_manager_visible_changed), qkm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "sharedDataChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "sharedDataChanged", G_CALLBACK(cuteime_keyboard_manager_shared_data_changed), qkm, NULL);

    cuteime_debug_out();
}

CuteimeKeyboardManager *cuteime_keyboard_manager_new()
{
    CuteimeKeyboardManager *ret = CUTEIMEKEYBOARDMANAGER(g_object_new(CUTEIMEKEYBOARDMANAGER_TYPE, NULL));
    return ret;
}

gboolean cuteime_keyboard_manager_get_visible(CuteimeKeyboardManager *qkm, gboolean *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "visible", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_keyboard_manager_set_visible(CuteimeKeyboardManager *qkm, gboolean value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "setVisible", &error, G_TYPE_BOOLEAN, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_keyboard_manager_visible_changed(DBusGProxy *proxy, gboolean value, gpointer user_data)
{
    g_signal_emit(CUTEIMEKEYBOARDMANAGER(user_data), cuteime_keyboard_manager_signals[CUTEIMEKEYBOARDMANAGER_VISIBLE_CHANGED], 0, value);
}

gboolean cuteime_keyboard_manager_get_shared_data(CuteimeKeyboardManager *qkm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "sharedData", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_keyboard_manager_set_shared_data(CuteimeKeyboardManager *qkm, const char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "setSharedData", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_keyboard_manager_shared_data_changed(DBusGProxy *proxy, const char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEKEYBOARDMANAGER(user_data), cuteime_keyboard_manager_signals[CUTEIMEKEYBOARDMANAGER_SHARED_DATA_CHANGED], 0, value);
}
