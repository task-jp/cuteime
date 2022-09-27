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

#ifndef CUTEIMEPREEDITMANAGER_H
#define CUTEIMEPREEDITMANAGER_H

#include "cuteimeabstractipcobject.h"
#include "cuteimepreedititem.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEPREEDITMANAGER_TYPE            (cuteime_preedit_manager_get_type())
#define CUTEIMEPREEDITMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEPREEDITMANAGER_TYPE, CuteimePreeditManager))
#define CUTEIMEPREEDITMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEPREEDITMANAGER_TYPE, CuteimePreeditManagerClass))
#define IS_CUTEIMEPREEDITMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEPREEDITMANAGER_TYPE))
#define IS_CUTEIMEPREEDITMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEPREEDITMANAGER_TYPE))
#define CUTEIMEPREEDITMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEPREEDITMANAGER, CuteimePreeditManagerClass))

typedef struct _CuteimePreeditManager       CuteimePreeditManager;
typedef struct _CuteimePreeditManagerClass  CuteimePreeditManagerClass;

struct _CuteimePreeditManager
{
    CuteimeAbstractIpcObject object;
};

struct _CuteimePreeditManagerClass
{
    CuteimeAbstractIpcObjectClass parent_class;

    void (* cuteime_preedit_manager) (CuteimePreeditManager *qam);
};

CUTEIME_EXPORT GType cuteime_preedit_manager_get_type();
CUTEIME_EXPORT CuteimePreeditManager *cuteime_preedit_manager_new();

CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_item(CuteimePreeditManager *qam, CuteimePreeditItem **value);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_item(CuteimePreeditManager *qam, CuteimePreeditItem *value);
// signal: "item-changed"

CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_rect(CuteimePreeditManager *qam, int *x, int *y, int *w, int *h);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_rect(CuteimePreeditManager *qam, int x, int y, int w, int h);
// signal: "rect-changed"

#ifdef CUTEIMEPREEDITMANAGER_FONT_SUPPORT
CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_font(CuteimePreeditManager *qam, char **value);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_font(CuteimePreeditManager *qam, char *value);
// signal: "font-changed"
#endif

CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_cursor_position(CuteimePreeditManager *qam, int *cursor_position);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_cursor_position(CuteimePreeditManager *qam, int cursor_position);
// signal: "cursor-position-changed"

CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_surrounding_text(CuteimePreeditManager *qam, char **surrounding_text);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_surrounding_text(CuteimePreeditManager *qam, const char *surrounding_text);
// signal: "surrounding-text-changed"

CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_current_selection(CuteimePreeditManager *qam, char **current_selection);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_current_selection(CuteimePreeditManager *qam, const char *current_selection);
// signal: "current-selection-changed"

CUTEIME_EXPORT gboolean cuteime_preedit_manager_get_maximum_text_length(CuteimePreeditManager *qam, int *maximum_text_length);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_set_maximum_text_length(CuteimePreeditManager *qam, int maximum_text_length);
// signal: "maximum-text-length-changed"

CUTEIME_EXPORT gboolean cuteime_preedit_manager_commit(CuteimePreeditManager *qam, gulonglong target);
CUTEIME_EXPORT gboolean cuteime_preedit_manager_commit_string(CuteimePreeditManager *qam, char *string, gulonglong target);
// signal: "committed"
G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEPREEDITMANAGER_H
