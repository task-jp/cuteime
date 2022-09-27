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

#include "cuteimepreedititem.h"
#include "cuteimedebug.h"
#include "dbus.h"

static void cuteime_preedit_item_finalize(GObject *gobject);

G_DEFINE_TYPE(CuteimePreeditItem, cuteime_preedit_item, G_TYPE_OBJECT)

static void cuteime_preedit_item_class_init(CuteimePreeditItemClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = cuteime_preedit_item_finalize;
}

static void cuteime_preedit_item_init(CuteimePreeditItem *item)
{
    cuteime_debug_in();
    cuteime_debug_out();
}

CuteimePreeditItem *cuteime_preedit_item_new(GValueArray *data)
{
    char **strv_ptr;
    GValue *value;
    CuteimePreeditItem *ret;

    cuteime_debug_in();

    ret = CUTEIMEPREEDITITEM(g_object_new(CUTEIMEPREEDITITEM_TYPE, NULL));

    value = g_value_array_get_nth(data, 0);
    ret->to = g_value_dup_boxed(value);

    for (strv_ptr = ret->to; *strv_ptr; *strv_ptr++) {
        cuteime_debug("%s(%d) %s\n", __FUNCTION__, __LINE__, *strv_ptr);
    }

    value = g_value_array_get_nth(data, 1);
    ret->from = g_value_dup_boxed(value);

    value = g_value_array_get_nth(data, 2);
    ret->raw_string = g_value_dup_boxed(value);

    value = g_value_array_get_nth(data, 3);
    ret->cursor = g_value_get_int(value);

    value = g_value_array_get_nth(data, 4);
    ret->selection = g_value_get_int(value);

    value = g_value_array_get_nth(data, 5);
    ret->modified = g_value_get_int(value);

    cuteime_debug_out();
    return ret;
}

GValueArray *cuteime_preedit_item_get_value(CuteimePreeditItem *item)
{
    GValue value = G_VALUE_INIT;
    GValueArray *ret;

    cuteime_debug_in();

    ret = g_value_array_new(0);

    g_value_init(&value, G_TYPE_STRV);
    g_value_set_boxed(&value, item->to);
    g_value_array_append(ret, &value);
    g_value_unset(&value);

    g_value_set_boxed(&value, item->from);
    g_value_array_append(ret, &value);
    g_value_unset(&value);

    g_value_set_boxed(&value, item->raw_string);
    g_value_array_append(ret, &value);
    g_value_unset(&value);

    g_value_init(&value, G_TYPE_INT);
    g_value_set_int(&value, item->cursor);
    g_value_array_append(ret, &value);

    g_value_set_int(&value, item->selection);
    g_value_array_append(ret, &value);

    g_value_set_int(&value, item->modified);
    g_value_array_append(ret, &value);

    cuteime_debug_out();
    return ret;
}

static void cuteime_preedit_item_finalize(GObject *gobject)
{
    cuteime_debug_in();
    CuteimePreeditItem *item = CUTEIMEPREEDITITEM(gobject);
    g_strfreev(item->to);
    g_strfreev(item->from);
    g_strfreev(item->raw_string);
    cuteime_debug_out();
}
