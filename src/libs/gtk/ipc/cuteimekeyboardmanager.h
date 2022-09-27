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

#ifndef CUTEIMEKEYBOARDMANAGER_H
#define CUTEIMEKEYBOARDMANAGER_H

#include "cuteimeabstractipcobject.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEKEYBOARDMANAGER_TYPE            (cuteime_keyboard_manager_get_type())
#define CUTEIMEKEYBOARDMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEKEYBOARDMANAGER_TYPE, CuteimeKeyboardManager))
#define CUTEIMEKEYBOARDMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEKEYBOARDMANAGER_TYPE, CuteimeKeyboardManagerClass))
#define IS_CUTEIMEKEYBOARDMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEKEYBOARDMANAGER_TYPE))
#define IS_CUTEIMEKEYBOARDMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEKEYBOARDMANAGER_TYPE))
#define CUTEIMEKEYBOARDMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEKEYBOARDMANAGER, CuteimeKeyboardManagerClass))

typedef struct _CuteimeKeyboardManager       CuteimeKeyboardManager;
typedef struct _CuteimeKeyboardManagerClass  CuteimeKeyboardManagerClass;

struct _CuteimeKeyboardManager
{
    CuteimeAbstractIpcObject object;
};

struct _CuteimeKeyboardManagerClass
{
    CuteimeAbstractIpcObjectClass parent_class;

    void (* cuteime_keyboard_manager) (CuteimeKeyboardManager *qkm);
};

CUTEIME_EXPORT GType cuteime_keyboard_manager_get_type();
CUTEIME_EXPORT CuteimeKeyboardManager *cuteime_keyboard_manager_new();

CUTEIME_EXPORT gboolean cuteime_application_manager_get_visible(CuteimeKeyboardManager *qam, gboolean *value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_visible(CuteimeKeyboardManager *qam, gboolean value);
// signal: "visible-changed"

CUTEIME_EXPORT gboolean cuteime_keyboard_manager_get_shared_data(CuteimeKeyboardManager *qkm, char **h);
CUTEIME_EXPORT gboolean cuteime_keyboard_manager_set_shared_data(CuteimeKeyboardManager *qkm, const char *h);
// signal: "shared-data-changed"

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEKEYBOARDMANAGER_H
