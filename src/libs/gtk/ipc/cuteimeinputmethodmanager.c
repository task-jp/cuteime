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

#include "cuteimeinputmethodmanager.h"
#include "dbus.h"
#include "cuteimedebug.h"
#include "cuteimemarshalers.h"

enum {
    CUTEIMEINPUTMETHODMANAGER_LOCALE_CHANGED,
    CUTEIMEINPUTMETHODMANAGER_IDENTIFIER_CHANGED,
    CUTEIMEINPUTMETHODMANAGER_CONVERTER_CHANGED,
    CUTEIMEINPUTMETHODMANAGER_INTERPRETER_CHANGED,
    CUTEIMEINPUTMETHODMANAGER_ENGINE_CHANGED,
    CUTEIMEINPUTMETHODMANAGER_STATE_CHANGED,
    CUTEIMEINPUTMETHODMANAGER_EXECUTED,
    CUTEIMEINPUTMETHODMANAGER_LAST_SIGNAL
};

static guint cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_LAST_SIGNAL] = { 0, 0 };

G_DEFINE_TYPE(CuteimeInputMethodManager, cuteime_inputmethod_manager, CUTEIMEABSTRACTIPCOBJECT_TYPE)

static void cuteime_inputmethod_manager_locale_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void cuteime_inputmethod_manager_identifier_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void cuteime_inputmethod_manager_converter_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void cuteime_inputmethod_manager_interpreter_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void cuteime_inputmethod_manager_engine_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void cuteime_inputmethod_manager_state_changed(DBusGProxy *proxy, guint value, gpointer user_data);
static void cuteime_inputmethod_manager_executed(DBusGProxy *proxy, char *value, gpointer user_data);

static void cuteime_inputmethod_manager_class_init(CuteimeInputMethodManagerClass *klass)
{
    cuteime_debug_in();
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_LOCALE_CHANGED]
            = g_signal_new("locale-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_IDENTIFIER_CHANGED]
            = g_signal_new("identifier-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_CONVERTER_CHANGED]
            = g_signal_new("converter-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_INTERPRETER_CHANGED]
            = g_signal_new("interpreter-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_ENGINE_CHANGED]
            = g_signal_new("engine-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_STATE_CHANGED]
            = g_signal_new("state-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__UINT,
                           G_TYPE_NONE, 1, G_TYPE_UINT);
    cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_EXECUTED]
            = g_signal_new("executed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeInputMethodManagerClass, cuteime_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_debug_out();
}

static void cuteime_inputmethod_manager_init(CuteimeInputMethodManager *qimm)
{
    CuteimeAbstractIpcObject *qaio;
    cuteime_debug_in();

    qaio = CUTEIMEABSTRACTIPCOBJECT(qimm);

    /* Create a proxy object */
    cuteime_abstract_ipc_object_connect(qaio, CUTEIME_DBUS_SERVICE, "/CuteimeInputMethodManager", "local.InputMethodManager");

    dbus_g_proxy_add_signal(qaio->proxy, "localeChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "localeChanged", G_CALLBACK(cuteime_inputmethod_manager_locale_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "identifierChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "identifierChanged", G_CALLBACK(cuteime_inputmethod_manager_identifier_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "converterChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "converterChanged", G_CALLBACK(cuteime_inputmethod_manager_converter_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "interpreterChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "interpreterChanged", G_CALLBACK(cuteime_inputmethod_manager_interpreter_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "engineChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "engineChanged", G_CALLBACK(cuteime_inputmethod_manager_engine_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "stateChanged", G_TYPE_UINT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "stateChanged", G_CALLBACK(cuteime_inputmethod_manager_state_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "executed", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "executed", G_CALLBACK(cuteime_inputmethod_manager_executed), qimm, NULL);

    cuteime_debug_out();
}

CuteimeInputMethodManager *cuteime_inputmethod_manager_new()
{
    CuteimeInputMethodManager *ret = CUTEIMEINPUTMETHODMANAGER(g_object_new(CUTEIMEINPUTMETHODMANAGER_TYPE, NULL));
    return ret;
}

gboolean cuteime_inputmethod_manager_get_locale(CuteimeInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "locale", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_inputmethod_manager_set_locale(CuteimeInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "setLocale", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_locale_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_LOCALE_CHANGED], 0, value);
}

gboolean cuteime_inputmethod_manager_get_identifier(CuteimeInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "identifier", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_inputmethod_manager_set_identifier(CuteimeInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "setIdentifier", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_identifier_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_IDENTIFIER_CHANGED], 0, value);
}

gboolean cuteime_inputmethod_manager_get_converter(CuteimeInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "converter", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_inputmethod_manager_set_converter(CuteimeInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "setConverter", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_converter_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_CONVERTER_CHANGED], 0, value);
}

gboolean cuteime_inputmethod_manager_get_interpreter(CuteimeInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "interpreter", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_inputmethod_manager_set_interpreter(CuteimeInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "setInterpreter", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_interpreter_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_INTERPRETER_CHANGED], 0, value);
}

gboolean cuteime_inputmethod_manager_get_engine(CuteimeInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "engine", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_inputmethod_manager_set_engine(CuteimeInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "setEngine", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_engine_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_ENGINE_CHANGED], 0, value);
}

gboolean cuteime_inputmethod_manager_get_state(CuteimeInputMethodManager *qimm, guint *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "state", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean cuteime_inputmethod_manager_set_state(CuteimeInputMethodManager *qimm, guint value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "setState", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_state_changed(DBusGProxy *proxy, guint value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_STATE_CHANGED], 0, value);
}

gboolean cuteime_inputmethod_manager_execte(CuteimeInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qimm)->proxy, "execute", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void cuteime_inputmethod_manager_executed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(CUTEIMEINPUTMETHODMANAGER(user_data), cuteime_inputmethod_manager_signals[CUTEIMEINPUTMETHODMANAGER_EXECUTED], 0, value);
}
