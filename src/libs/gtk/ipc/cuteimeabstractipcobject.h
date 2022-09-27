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

#ifndef CUTEIMEABSTRACTIPCOBJECT_H
#define CUTEIMEABSTRACTIPCOBJECT_H

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib.h>

#include "cuteimeglobal.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEABSTRACTIPCOBJECT_TYPE            (cuteime_abstract_ipc_object_get_type())
#define CUTEIMEABSTRACTIPCOBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEABSTRACTIPCOBJECT_TYPE, CuteimeAbstractIpcObject))
#define CUTEIMEABSTRACTIPCOBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEABSTRACTIPCOBJECT_TYPE, CuteimeAbstractIpcObjectClass))
#define IS_CUTEIMEABSTRACTIPCOBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEABSTRACTIPCOBJECT_TYPE))
#define IS_CUTEIMEABSTRACTIPCOBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEABSTRACTIPCOBJECT_TYPE))
#define CUTEIMEABSTRACTIPCOBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEABSTRACTIPCOBJECT, CuteimeAbstractIpcObjectClass))

typedef struct _CuteimeAbstractIpcObject       CuteimeAbstractIpcObject;
typedef struct _CuteimeAbstractIpcObjectClass  CuteimeAbstractIpcObjectClass;

struct _CuteimeAbstractIpcObject
{
    GObject parent_instance;

    DBusGProxy *proxy;
    gboolean is_connected;
};

struct _CuteimeAbstractIpcObjectClass
{
    GObjectClass parent_class;

    void (* cuteime_abstract_ipc_object) (CuteimeAbstractIpcObject *qaio);
};

CUTEIME_EXPORT GType cuteime_abstract_ipc_object_get_type();

void cuteime_abstract_ipc_object_connect(CuteimeAbstractIpcObject *qaio, const char *name, const char *path, const char *interface);
gboolean cuteime_abstract_ipc_object_is_connected(CuteimeAbstractIpcObject *qaio);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif//CUTEIMEABSTRACTIPCOBJECT_H
