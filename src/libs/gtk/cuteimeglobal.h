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

#ifndef CUTEIMEGLOBAL_H
#define CUTEIMEGLOBAL_H

#include <gmodule.h>

#ifndef CUTEIME_NO_DBUS
static const char *const CUTEIME_DBUS_SERVICE = "me.qtquick.cuteime";

typedef unsigned long long gulonglong;
#define cuteime_marshal_VOID__ULONGLONG cuteime_marshal_VOID__UINT64
#define G_TYPE_ULONGLONG G_TYPE_UINT64

#define cuteime_marshal_VOID__STRING_ULONGLONG cuteime_marshal_VOID__STRING_UINT64
#endif

#ifdef CUTEIME_LIBRARY
# define CUTEIME_EXPORT G_MODULE_EXPORT
#else
# define CUTEIME_EXPORT G_MODULE_IMPORT
#endif

#endif // CUTEIMEGLOBAL_H
