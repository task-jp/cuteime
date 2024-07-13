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

#include <QtCore/qglobal.h>

#ifndef CUTEIME_NO_DBUS
static const char *const CUTEIME_DBUS_SERVICE = "me.qtquick.cuteime";
#endif

#ifdef CUTEIME_LIBRARY
# define CUTEIME_EXPORT Q_DECL_EXPORT
#else
# define CUTEIME_EXPORT Q_DECL_IMPORT
#endif

//#define CUTEIME_DEBUG_MODE


#ifndef Q_FALLTHROUGH
# define Q_FALLTHROUGH() (void)0
#endif

#define CUTEIME_MACRO_TO_STRING(x) x
#define CUTEIME_MACRO_TO_STRING2(x) #x

#endif // CUTEIMEGLOBAL_H
