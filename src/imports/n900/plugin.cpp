/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010 by Tasuku Suzuki <stasuku@gmail.com>                 *
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

#include "plugin.h"

#include <qimsysdebug.h>

#include "deviceinfo.h"

using namespace N900::Device;

void Plugin::registerTypes(const char *uri)
{
//    qimsysDebugOn();
    qimsysDebugIn() << uri;
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Qimsys.N900"));
    qmlRegisterType<DeviceInfo>(uri, 1, 0, "DeviceInfo");
    qimsysDebugOut();
}

Q_EXPORT_PLUGIN2(qimsysCorePlugin, QT_PREPEND_NAMESPACE(Plugin));

