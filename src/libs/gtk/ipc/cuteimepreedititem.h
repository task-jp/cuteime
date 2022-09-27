/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   cuteime                                                                  *
 *   Copyright (C) 2009-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#ifndef CUTEIMEPREEDITITEM_H
#define CUTEIMEPREEDITITEM_H

#include <glib-object.h>
#include "cuteimeglobal.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEPREEDITITEM_TYPE            (cuteime_preedit_item_get_type())
#define CUTEIMEPREEDITITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEPREEDITITEM_TYPE, CuteimePreeditItem))
#define CUTEIMEPREEDITITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEPREEDITITEM_TYPE, CuteimePreeditItemClass))
#define IS_CUTEIMEPREEDITITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEPREEDITITEM_TYPE))
#define IS_CUTEIMEPREEDITITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEPREEDITITEM_TYPE))
#define CUTEIMEPREEDITITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEPREEDITITEM, CuteimePreeditItemClass))

#define CUTEIME_STRUCT_PREEDITITEM (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRV, G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))

typedef struct _CuteimePreeditItem       CuteimePreeditItem;
typedef struct _CuteimePreeditItemClass  CuteimePreeditItemClass;

struct _CuteimePreeditItem
{
    GObject instance_parent;

    GStrv to;
    GStrv from;
    GStrv raw_string;
    int cursor;
    int selection;
    int modified;
};

struct _CuteimePreeditItemClass
{
    GObjectClass class_parent;

    void (* cuteime_preedit_item) (CuteimePreeditItem *item);
};

CUTEIME_EXPORT GType cuteime_preedit_item_get_type();
CUTEIME_EXPORT CuteimePreeditItem *cuteime_preedit_item_new(GValueArray *data);
CUTEIME_EXPORT GValueArray *cuteime_preedit_item_get_value(CuteimePreeditItem *item);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEPREEDITITEM_H
