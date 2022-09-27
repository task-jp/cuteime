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

#ifndef CUTEIMEAPPLICATIONMANAGER_H
#define CUTEIMEAPPLICATIONMANAGER_H

#include "cuteimeabstractipcobject.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define CUTEIMEAPPLICATIONMANAGER_TYPE            (cuteime_application_manager_get_type())
#define CUTEIMEAPPLICATIONMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUTEIMEAPPLICATIONMANAGER_TYPE, CuteimeApplicationManager))
#define CUTEIMEAPPLICATIONMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUTEIMEAPPLICATIONMANAGER_TYPE, CuteimeApplicationManagerClass))
#define IS_CUTEIMEAPPLICATIONMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIMEAPPLICATIONMANAGER_TYPE))
#define IS_CUTEIMEAPPLICATIONMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIMEAPPLICATIONMANAGER_TYPE))
#define CUTEIMEAPPLICATIONMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIMEAPPLICATIONMANAGER, CuteimeApplicationManagerClass))

typedef struct _CuteimeApplicationManager       CuteimeApplicationManager;
typedef struct _CuteimeApplicationManagerClass  CuteimeApplicationManagerClass;

enum CuteimeApplicationManager_ActionType {
    CuteimeApplicationManager_Reset,
    CuteimeApplicationManager_ShowDictionary,
    CuteimeApplicationManager_ShowSettings,
    CuteimeApplicationManager_ShowAboutCuteime,
    CuteimeApplicationManager_ShowController,
};

struct _CuteimeApplicationManager
{
    CuteimeAbstractIpcObject instance_class;
};

struct _CuteimeApplicationManagerClass
{
    CuteimeAbstractIpcObjectClass parent_class;

    void (* cuteime_application_manager) (CuteimeApplicationManager *qam);
};

CUTEIME_EXPORT GType cuteime_application_manager_get_type();
CUTEIME_EXPORT CuteimeApplicationManager *cuteime_application_manager_new();

CUTEIME_EXPORT gboolean cuteime_application_manager_get_display_language(CuteimeApplicationManager *qam, char **value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_display_language(CuteimeApplicationManager *qam, char *value);
// signal: "display-language-changed"

CUTEIME_EXPORT gboolean cuteime_application_manager_get_focus(CuteimeApplicationManager *qam, gboolean *value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_focus(CuteimeApplicationManager *qam, gboolean value);
// signal: "focus-changed"

CUTEIME_EXPORT gboolean cuteime_application_manager_get_window(CuteimeApplicationManager *qam, gulonglong *value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_window(CuteimeApplicationManager *qam, gulonglong value);
// signal: "window-changed"

CUTEIME_EXPORT gboolean cuteime_application_manager_get_widget(CuteimeApplicationManager *qam, gulonglong *value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_widget(CuteimeApplicationManager *qam, gulonglong value);
// signal: "widget-changed"

CUTEIME_EXPORT gboolean cuteime_application_manager_get_composing(CuteimeApplicationManager *qam, gboolean *value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_composing(CuteimeApplicationManager *qam, gboolean value);
// signal: "composing-changed"

CUTEIME_EXPORT gboolean cuteime_application_manager_get_current_icon(CuteimeApplicationManager *qam, GdkPixbuf **value);
CUTEIME_EXPORT gboolean cuteime_application_manager_set_current_icon(CuteimeApplicationManager *qam, GdkPixbuf *value);
// signal: "current-icon-changed"

gboolean cuteime_application_manager_exec(CuteimeApplicationManager *qam, int value);
// signal: triggered

gboolean cuteime_application_manager_settings_update(CuteimeApplicationManager *qam, char *value);
// settings_updated

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEAPPLICATIONMANAGER_H
