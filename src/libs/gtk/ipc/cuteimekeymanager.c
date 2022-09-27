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

#include "cuteimekeymanager.h"
#include "dbus.h"

G_DEFINE_TYPE(CuteimeKeyManager, cuteime_key_manager, CUTEIMEABSTRACTIPCOBJECT_TYPE)

static void cuteime_key_manager_class_init(CuteimeKeyManagerClass *klass)
{
}

static void cuteime_key_manager_init(CuteimeKeyManager *qkm)
{
    CuteimeAbstractIpcObject *qaio;

    qaio = CUTEIMEABSTRACTIPCOBJECT(qkm);

    /* Create a proxy object */
    cuteime_abstract_ipc_object_connect(qaio, CUTEIME_DBUS_SERVICE, "/CuteimeKeyManager", "local.KeyManager");
}

CuteimeKeyManager *cuteime_key_manager_new()
{
    CuteimeKeyManager *ret = CUTEIMEKEYMANAGER(g_object_new(CUTEIMEKEYMANAGER_TYPE, NULL));
    return ret;
}

gboolean cuteime_key_manager_process(CuteimeKeyManager *qkm, char *value, int keycode, int modifiers, gboolean isPress, gboolean autoRepeat)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "process", &error, G_TYPE_STRING, value, G_TYPE_INT, keycode, G_TYPE_INT, modifiers, G_TYPE_BOOLEAN, isPress, G_TYPE_BOOLEAN, autoRepeat, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_key_manager_key_press(CuteimeKeyManager *qkm, char *value, int keycode, int modifiers, gboolean autoRepeat, gboolean *is_accepted)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "keyPress", &error, G_TYPE_STRING, value, G_TYPE_INT, keycode, G_TYPE_INT, modifiers, G_TYPE_BOOLEAN, autoRepeat, G_TYPE_INVALID, G_TYPE_BOOLEAN, is_accepted, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_key_manager_key_release(CuteimeKeyManager *qkm, char *value, int keycode, int modifiers, gboolean autoRepeat, gboolean *is_accepted)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "keyRelease", &error, G_TYPE_STRING, value, G_TYPE_INT, keycode, G_TYPE_INT, modifiers, G_TYPE_BOOLEAN, autoRepeat, G_TYPE_INVALID, G_TYPE_BOOLEAN, is_accepted, G_TYPE_INVALID))
    return ret;
}

CUTEIME_EXPORT gboolean cuteime_key_manager_get_accepted(CuteimeKeyManager *qkm, gboolean *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "isAccepted", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, value, G_TYPE_INVALID))
    return ret;
}

CUTEIME_EXPORT gboolean cuteime_key_manager_accept(CuteimeKeyManager *qkm)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qkm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qkm)->proxy, "accept", &error, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}
