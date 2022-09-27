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

#ifndef CUTEIMEDEBUG_H
#define CUTEIMEDEBUG_H

#include <glib-object.h>
#include <syslog.h>
#include <stdio.h>

#include "cuteimeglobal.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int CuteimeLogLevelCounter;
extern int CuteimeLogEnabled;
extern int CuteimeLogLevel;

#define cuteime_debug_on() CuteimeLogEnabled++;
#define cuteime_debug_off() CuteimeLogEnabled--
#define cuteime_debug_in() if(CuteimeLogEnabled <= 0){} else g_print("+ %s\n", __PRETTY_FUNCTION__)
#define cuteime_debug_out() if(CuteimeLogEnabled <= 0){} else g_print("- %s\n", __PRETTY_FUNCTION__)
#define cuteime_debug(...) if(CuteimeLogEnabled <= 0){} else g_print(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // CUTEIMEDEBUG_H
