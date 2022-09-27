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

#include "cuteimeapplicationmanager.h"
#include "dbus.h"
#include "cuteimedebug.h"
#include "cuteimemarshalers.h"

enum {
    CUTEIMEAPPLICATIONMANAGER_DISPLAY_LANGUAGE_CHANGED,
    CUTEIMEAPPLICATIONMANAGER_FOCUS_CHANGED,
    CUTEIMEAPPLICATIONMANAGER_WINDOW_CHANGED,
    CUTEIMEAPPLICATIONMANAGER_WIDGET_CHANGED,
    CUTEIMEAPPLICATIONMANAGER_COMPOSING_CHANGED,
    CUTEIMEAPPLICATIONMANAGER_CURRENT_ICON_CHANGED,
    CUTEIMEAPPLICATIONMANAGER_TRIGGERED,
    CUTEIMEAPPLICATIONMANAGER_SETTINGS_UPDATED,
    CUTEIMEAPPLICATIONMANAGER_LAST_SIGNAL
};

static guint cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_LAST_SIGNAL] = { 0, 0 };

G_DEFINE_TYPE(CuteimeApplicationManager, cuteime_application_manager, CUTEIMEABSTRACTIPCOBJECT_TYPE)

static void cuteime_application_manager_display_language_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void cuteime_application_manager_focus_changed(DBusGProxy *proxy, gboolean value, gpointer user_data);
static void cuteime_application_manager_window_changed(DBusGProxy *proxy, gulonglong value, gpointer user_data);
static void cuteime_application_manager_widget_changed(DBusGProxy *proxy, gulonglong value, gpointer user_data);
static void cuteime_application_manager_composing_changed(DBusGProxy *proxy, gboolean value, gpointer user_data);
static void cuteime_application_manager_current_icon_changed(DBusGProxy *proxy, GValueArray *value, gpointer user_data);
static void cuteime_application_manager_triggered(DBusGProxy *proxy, int value, gpointer user_data);
static void cuteime_application_manager_settings_updated(DBusGProxy *proxy, char *value, gpointer user_data);

//#define CUTEIME_STRUCT_PREEDITITEM (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))
//#define CUTEIME_STRUCT_PREEDITITEMLIST (dbus_g_type_get_collection ("GPtrArray", CUTEIME_STRUCT_PREEDITITEM))
//#define CUTEIME_STRUCT_RECT (dbus_g_type_get_struct ("GValueArray", G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))

//#define CUTEIME_STRUCT_BYTEARRAY (dbus_g_type_get_collection ("GArray", G_TYPE_UCHAR))
#define CUTEIME_STRUCT_ICON (dbus_g_type_get_struct ("GValueArray", DBUS_TYPE_G_UCHAR_ARRAY, G_TYPE_INVALID))

static void cuteime_application_manager_class_init(CuteimeApplicationManagerClass *klass)
{
    cuteime_debug_in();
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_DISPLAY_LANGUAGE_CHANGED]
            = g_signal_new("display-language-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_FOCUS_CHANGED]
            = g_signal_new("focus-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__BOOLEAN,
                           G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_WINDOW_CHANGED]
            = g_signal_new("window-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           cuteime_marshal_VOID__ULONGLONG,
                           G_TYPE_NONE, 1, G_TYPE_ULONGLONG);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_WIDGET_CHANGED]
            = g_signal_new("widget-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           cuteime_marshal_VOID__ULONGLONG,
                           G_TYPE_NONE, 1, G_TYPE_ULONGLONG);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_COMPOSING_CHANGED]
            = g_signal_new("composing-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__BOOLEAN,
                           G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_CURRENT_ICON_CHANGED]
            = g_signal_new("current-icon-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__POINTER,
                           G_TYPE_NONE, 1, G_TYPE_POINTER);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_TRIGGERED]
            = g_signal_new("triggered",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__INT,
                           G_TYPE_NONE, 1, G_TYPE_INT);
    cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_SETTINGS_UPDATED]
            = g_signal_new("settings-updated",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(CuteimeApplicationManagerClass, cuteime_application_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    cuteime_debug_out();
}

static void cuteime_application_manager_init(CuteimeApplicationManager *qam)
{
    CuteimeAbstractIpcObject *qaio;
    cuteime_debug_in();

    qaio = CUTEIMEABSTRACTIPCOBJECT(qam);

    /* Create a proxy object */
    cuteime_abstract_ipc_object_connect(qaio, CUTEIME_DBUS_SERVICE, "/CuteimeApplicationManager", "local.ApplicationManager");

    dbus_g_proxy_add_signal(qaio->proxy, "displayLanguageChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "displayLanguageChanged", G_CALLBACK(cuteime_application_manager_display_language_changed), qam, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "focusChanged", G_TYPE_BOOLEAN, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "focusChanged", G_CALLBACK(cuteime_application_manager_focus_changed), qam, NULL);

    dbus_g_object_register_marshaller(cuteime_marshal_VOID__ULONGLONG, G_TYPE_NONE, G_TYPE_ULONGLONG, G_TYPE_INVALID);
    dbus_g_proxy_add_signal(qaio->proxy, "windowChanged", G_TYPE_ULONGLONG, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "windowChanged", G_CALLBACK(cuteime_application_manager_window_changed), qam, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "widgetChanged", G_TYPE_ULONGLONG, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "widgetChanged", G_CALLBACK(cuteime_application_manager_widget_changed), qam, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "composingChanged", G_TYPE_BOOLEAN, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "composingChanged", G_CALLBACK(cuteime_application_manager_composing_changed), qam, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "currentIconChanged", CUTEIME_STRUCT_ICON, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "currentIconChanged", G_CALLBACK(cuteime_application_manager_current_icon_changed), qam, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "triggered", G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "triggered", G_CALLBACK(cuteime_application_manager_triggered), qam, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "settingsUpdated", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "settingsUpdated", G_CALLBACK(cuteime_application_manager_settings_updated), qam, NULL);

    cuteime_debug_out();
}

CuteimeApplicationManager *cuteime_application_manager_new()
{
    cuteime_debug_in();
    CuteimeApplicationManager *ret = CUTEIMEAPPLICATIONMANAGER(g_object_new(CUTEIMEAPPLICATIONMANAGER_TYPE, NULL));
    cuteime_debug("%s(%d) ret = %p\n", __PRETTY_FUNCTION__, __LINE__, ret);
    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_get_display_language(CuteimeApplicationManager *qam, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "displayLanguage", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_set_display_language(CuteimeApplicationManager *qam, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "setDisplayLanguage", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_display_language_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_DISPLAY_LANGUAGE_CHANGED], 0, value);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_get_focus(CuteimeApplicationManager *qam, gboolean *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "focus", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, value, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_set_focus(CuteimeApplicationManager *qam, gboolean value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "setFocus", &error, G_TYPE_BOOLEAN, value, G_TYPE_INVALID, G_TYPE_INVALID));


    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_focus_changed(DBusGProxy *proxy, gboolean value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_FOCUS_CHANGED], 0, value);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_get_window(CuteimeApplicationManager *qam, gulonglong *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "window", &error, G_TYPE_INVALID, G_TYPE_ULONGLONG, value, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_set_window(CuteimeApplicationManager *qam, gulonglong value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "setWindow", &error, G_TYPE_ULONGLONG, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_window_changed(DBusGProxy *proxy, gulonglong value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_WINDOW_CHANGED], 0, value);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_get_widget(CuteimeApplicationManager *qam, gulonglong *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "widget", &error, G_TYPE_INVALID, G_TYPE_ULONGLONG, value, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_set_widget(CuteimeApplicationManager *qam, gulonglong value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "setWidget", &error, G_TYPE_ULONGLONG, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_widget_changed(DBusGProxy *proxy, gulonglong value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_WIDGET_CHANGED], 0, value);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_get_composing(CuteimeApplicationManager *qam, gboolean *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "composing", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, value, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_set_composing(CuteimeApplicationManager *qam, gboolean value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "setComposing", &error, G_TYPE_BOOLEAN, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_composing_changed(DBusGProxy *proxy, gboolean value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_COMPOSING_CHANGED], 0, value);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_get_current_icon(CuteimeApplicationManager *qam, GdkPixbuf **value)
{
    guint i;
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *va = 0;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam))) {
        va = g_value_array_new(0);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "currentIcon", &error, G_TYPE_INVALID, CUTEIME_STRUCT_ICON, &va, G_TYPE_INVALID))

        for (i = 0; i < va->n_values; i++) {
            GValue *v;
            GByteArray *ba;
            v = g_value_array_get_nth(va, i);
            if (G_VALUE_HOLDS (v, DBUS_TYPE_G_UCHAR_ARRAY)) {
                GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
                ba = (GByteArray *)g_value_get_boxed(v);
                if (gdk_pixbuf_loader_write(loader, ba->data, ba->len, &error)) {
                    *value = gdk_pixbuf_loader_get_pixbuf(loader);
                }
                gdk_pixbuf_loader_close(loader, NULL);
//                g_object_unref(loader);
            } else {
                g_print("Error\n");
            }
        }
        g_value_array_free(va);
    }

    cuteime_debug_out();
    return ret;
}

gboolean cuteime_application_manager_set_current_icon(CuteimeApplicationManager *qam, GdkPixbuf *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "setCurrentIcon", &error, CUTEIME_STRUCT_ICON, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_current_icon_changed(DBusGProxy *proxy, GValueArray *value, gpointer user_data)
{
    guint i;
    GError *error = NULL;
    GdkPixbuf *pixbuf = 0;

    cuteime_debug_in();

    for (i = 0; i < value->n_values; i++) {
        GValue *v;
        v = g_value_array_get_nth(value, i);
        if (G_VALUE_HOLDS (v, DBUS_TYPE_G_UCHAR_ARRAY)) {
            GByteArray *ba;
            GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
            ba = (GByteArray *)g_value_get_boxed(v);
            if (gdk_pixbuf_loader_write(loader, ba->data, ba->len, &error)) {
                pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
            }
            gdk_pixbuf_loader_close(loader, NULL);
        }
    }

    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_CURRENT_ICON_CHANGED], 0, pixbuf);
    g_object_unref(pixbuf);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_exec(CuteimeApplicationManager *qam, int value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "exec", &error, G_TYPE_INT, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_triggered(DBusGProxy *proxy, int value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_TRIGGERED], 0, value);
    cuteime_debug_out();
}

gboolean cuteime_application_manager_settings_update(CuteimeApplicationManager *qam, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    cuteime_debug_in();

    if (cuteime_abstract_ipc_object_is_connected(CUTEIMEABSTRACTIPCOBJECT(qam)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(CUTEIMEABSTRACTIPCOBJECT(qam)->proxy, "settingsUpdate", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID));

    cuteime_debug_out();
    return ret;
}

void cuteime_application_manager_settings_updated(DBusGProxy *proxy, char *value, gpointer user_data)
{
    cuteime_debug_in();
    g_signal_emit(CUTEIMEAPPLICATIONMANAGER(user_data), cuteime_application_manager_signals[CUTEIMEAPPLICATIONMANAGER_SETTINGS_UPDATED], 0, value);
    cuteime_debug_out();
}
