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

#include "inputcontextplugin.h"
#include "inputcontext.h"

#include <cuteimedebug.h>

InputContextPlugin::InputContextPlugin(QObject *parent)
    : QInputContextPlugin(parent)
{
    if (!qgetenv("CUTEIME_DEBUG").isEmpty()) {
        cuteimeDebugOn();
    }
    cuteimeDebugIn() << parent;
    cuteimeDebugOut();
}

InputContextPlugin::~InputContextPlugin()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

QInputContext *InputContextPlugin::create(const QString &key)
{
    if (!keys().contains(key)) return 0;
    cuteimeDebugIn() << key;
    InputContext *ret = new InputContext(this);
    cuteimeDebugOut() << ret;
    return ret;
}

QString InputContextPlugin::description(const QString &key)
{
    if (!keys().contains(key)) return QString();
    return tr("Qt Input Method System");
}

QString InputContextPlugin::displayName(const QString &key)
{
    if (!keys().contains(key)) return QString();
    return tr("Qt Input Method System");
}

QStringList InputContextPlugin::keys() const
{
    return QStringList() << QLatin1String("cuteime");
}

QStringList InputContextPlugin::languages(const QString &key)
{
    QStringList ret;
    if (!keys().contains(key)) return ret;
    ret << QLatin1String("ja");
    return ret;
}

Q_EXPORT_PLUGIN2(cuteimeInputContextPlugin, InputContextPlugin)
