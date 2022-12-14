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

#include "application.h"

#include <cuteimecommandlineargs.h>

int main(int argc, char **argv)
{
    int ret = 0;
#ifdef CUTEIME_PLATFORM_LINUX
    if (qgetenv("XMODIFIERS") == QByteArray("@im=cuteime")) {
        qputenv("XMODIFIERS", QByteArray());
    }
    if (qgetenv("GTK_IM_MODULE") == QByteArray("cuteime")) {
        qputenv("GTK_IM_MODULE", QByteArray());
        qputenv("GTK_IM_MODULE_FILE", QByteArray());
    }
    if (qgetenv("QT_IM_MODULE") == QByteArray("cuteime")) {
        qputenv("QT_IM_MODULE", QByteArray());
    }
#endif
    if (CuteimeCommandlineArgs::instance()->parse(argc, argv)) {
        Application app(argc, argv);
#ifdef CUTEIME_PLATFORM_LINUX
        qputenv("XMODIFIERS", QByteArray("@im=cuteime"));
        qputenv("GTK_IM_MODULE", QByteArray("cuteime"));
        qputenv("QT_IM_MODULE", QByteArray("cuteime"));
#endif
        ret = app.exec();
    }
    return ret;
}
