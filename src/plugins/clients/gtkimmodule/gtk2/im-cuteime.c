/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   cuteime                                                                  *
 *   Copyright (C) 2010-2016 by Tasuku Suzuki <stasuku@gmail.com>            *
 *   Copyright (C) 2016 by Takahiro Hashimoto <kenya888@gmail.com>           *
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

#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

#include <cuteimedebug.h>
#include "../cuteimeimcontext.h"

// http://library.gnome.org/devel/gtk/unstable/GtkIMContext.html

// http://library.gnome.org/devel/gtk/unstable/GtkIMContext.html#GtkIMContextInfo
static const GtkIMContextInfo cuteime_im_context_info = {
    /* const gchar *context_id */
    "cuteime"
    /* const gchar *context_name */
    , "Qt Input Method System"
    /* const gchar *domain */
    , "cuteime"
    /* const gchar *domain_dirname */
    , ""
    /* const gchar *default_locales */
    , "ja"
};

static const GtkIMContextInfo *cuteime_im_context_info_list[] = {
    &cuteime_im_context_info
};

void im_module_init(GTypeModule *module)
{
    if (g_getenv("CUTEIME_DEBUG")) {
        cuteime_debug_on();
    }
    cuteime_debug_in();
    g_type_module_use(module);
    cuteime_im_context_register_type(module);
    cuteime_debug_out();
}

void im_module_exit()
{
    cuteime_debug_in();
    cuteime_debug_out();
}

void im_module_list(const GtkIMContextInfo ***contexts, int *n_contexts)
{
//    cuteime_debug_in();
    *contexts = cuteime_im_context_info_list;
    *n_contexts = G_N_ELEMENTS(cuteime_im_context_info_list);
//    cuteime_debug_out();
}

GtkIMContext *im_module_create(const gchar *context_id)
{
    GtkIMContext *ret = NULL;
    cuteime_debug_in();
    if (g_strcmp0(context_id, "cuteime") == 0) {
        ret = GTK_IM_CONTEXT(cuteime_im_context_new());
    }
    cuteime_debug_out();
    return ret;
}
