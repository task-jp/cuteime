/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   cuteime                                                                  *
 *   Copyright (C) 2009-2016 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#ifndef CUTEIMEIMCONTEXT_H
#define CUTEIMEIMCONTEXT_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CUTEIME_IM_CONTEXT_TYPE            (cuteime_im_context_get_type())
#define CUTEIME_IM_CONTEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CUTEIME_IM_CONTEXT_TYPE, CuteimeIMContext))
#define CUTEIME_IM_CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), CUTEIME_TYPE_IM_CONTEXT, CuteimeIMContextClass))
#define IS_CUTEIME_IM_CONTEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUTEIME_IM_CONTEXT_TYPE))
#define IS_CUTEIME_IM_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUTEIME_IM_CONTEXT_TYPE))
#define CUTEIME_IM_CONTEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CUTEIME_IM_CONTEXT, CuteimeIMContext))

typedef struct _CuteimeIMContext CuteimeIMContext;
typedef struct _CuteimeIMContextClass CuteimeIMContextClass;
typedef struct _CuteimeIMContextPrivate CuteimeIMContextPrivate;

struct _CuteimeIMContext {
    GtkIMContext parent_instance;

    CuteimeIMContextPrivate *d;
};

struct _CuteimeIMContextClass {
    GtkIMContextClass parent_class;

    GtkIMContext *current_context;
};

void cuteime_im_context_register_type(GTypeModule *module);
GType cuteime_im_context_get_type();

CuteimeIMContext *cuteime_im_context_new();

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // CUTEIMEIMCONTEXT_H
