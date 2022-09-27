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

#ifndef CUTEIMEINPUTMETHODMANAGER_H
#define CUTEIMEINPUTMETHODMANAGER_H

#include "cuteimeabstractipcobject.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEINPUTMETHODMANAGER_TYPE            (cuteime_inputmethod_manager_get_type())
#define CUTEIMEINPUTMETHODMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEINPUTMETHODMANAGER_TYPE, CuteimeInputMethodManager))
#define CUTEIMEINPUTMETHODMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEINPUTMETHODMANAGER_TYPE, CuteimeInputMethodManagerClass))
#define IS_CUTEIMEINPUTMETHODMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEINPUTMETHODMANAGER_TYPE))
#define IS_CUTEIMEINPUTMETHODMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEINPUTMETHODMANAGER_TYPE))
#define CUTEIMEINPUTMETHODMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEINPUTMETHODMANAGER, CuteimeInputMethodManagerClass))

typedef struct _CuteimeInputMethodManager       CuteimeInputMethodManager;
typedef struct _CuteimeInputMethodManagerClass  CuteimeInputMethodManagerClass;

struct _CuteimeInputMethodManager
{
    CuteimeAbstractIpcObject instance_class;
};

struct _CuteimeInputMethodManagerClass
{
    CuteimeAbstractIpcObjectClass parent_class;

    void (* cuteime_inputmethod_manager) (CuteimeInputMethodManager *qimm);
};

CUTEIME_EXPORT GType cuteime_inputmethod_manager_get_type();
CUTEIME_EXPORT CuteimeInputMethodManager *cuteime_inputmethod_manager_new();

CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_get_locale(CuteimeInputMethodManager *qimm, char **value);
CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_set_locale(CuteimeInputMethodManager *qimm, char *value);
// signal: "locale-changed"

CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_get_identifier(CuteimeInputMethodManager *qimm, char **value);
CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_set_identifier(CuteimeInputMethodManager *qimm, char *value);
// signal: "identifier-changed"

CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_get_converter(CuteimeInputMethodManager *qimm, char **value);
CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_set_converter(CuteimeInputMethodManager *qimm, char *value);
// signal: "converter-changed"

CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_get_interpreter(CuteimeInputMethodManager *qimm, char **value);
CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_set_interpreter(CuteimeInputMethodManager *qimm, char *value);
// signal: "interpreter-changed"

CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_get_engine(CuteimeInputMethodManager *qimm, char **value);
CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_set_engine(CuteimeInputMethodManager *qimm, char *value);
// signal: "engine-changed"

CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_get_state(CuteimeInputMethodManager *qimm, guint *value);
CUTEIME_EXPORT gboolean cuteime_inputmethod_manager_set_state(CuteimeInputMethodManager *qimm, guint value);
// signal: "state-changed"

gboolean cuteime_inputmethod_manager_execute(CuteimeInputMethodManager *qimm, char *value);
// signal: executed

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEINPUTMETHODMANAGER_H
