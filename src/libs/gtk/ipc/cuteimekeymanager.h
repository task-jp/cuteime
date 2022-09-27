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

#ifndef CUTEIMEKEYMANAGER_H
#define CUTEIMEKEYMANAGER_H

#include "cuteimeabstractipcobject.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEKEYMANAGER_TYPE            (cuteime_key_manager_get_type())
#define CUTEIMEKEYMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEKEYMANAGER_TYPE, CuteimeKeyManager))
#define CUTEIMEKEYMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEKEYMANAGER_TYPE, CuteimeKeyManagerClass))
#define IS_CUTEIMEKEYMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEKEYMANAGER_TYPE))
#define IS_CUTEIMEKEYMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEKEYMANAGER_TYPE))
#define CUTEIMEKEYMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEKEYMANAGER, CuteimeKeyManagerClass))

typedef struct _CuteimeKeyManager       CuteimeKeyManager;
typedef struct _CuteimeKeyManagerClass  CuteimeKeyManagerClass;

struct _CuteimeKeyManager
{
    CuteimeAbstractIpcObject object;
};

struct _CuteimeKeyManagerClass
{
    CuteimeAbstractIpcObjectClass parent_class;

    void (* cuteime_key_manager) (CuteimeKeyManager *qam);
};

CUTEIME_EXPORT GType cuteime_key_manager_get_type();
CUTEIME_EXPORT CuteimeKeyManager *cuteime_key_manager_new();

CUTEIME_EXPORT gboolean cuteime_key_manager_process(CuteimeKeyManager *qam, char *text, int keycode, int modifiers, gboolean isPress, gboolean autoRepeat);

CUTEIME_EXPORT gboolean cuteime_key_manager_key_press(CuteimeKeyManager *qam, char *text, int keycode, int modifiers, gboolean autoRepeat, gboolean *is_accepted);
CUTEIME_EXPORT gboolean cuteime_key_manager_key_release(CuteimeKeyManager *qam, char *text, int keycode, int modifiers, gboolean autoRepeat, gboolean *is_accepted);

CUTEIME_EXPORT gboolean cuteime_key_manager_get_accepted(CuteimeKeyManager *qam, gboolean *value);
CUTEIME_EXPORT gboolean cuteime_key_manager_accept(CuteimeKeyManager *qam);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEKEYMANAGER_H
