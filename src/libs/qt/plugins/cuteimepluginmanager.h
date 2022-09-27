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

#ifndef CUTEIMEPLUGINMANAGER_H
#define CUTEIMEPLUGINMANAGER_H

#include <QCoreApplication>
#include <QList>
#include <QDir>
#include <QPluginLoader>
#include <QPointer>
#include <QLibraryInfo>
#include "cuteimedebug.h"
#include "cuteimeglobal.h"
#include "cuteimeabstractpluginobject.h"
#include "cuteimeplugin.h"

const CUTEIME_EXPORT QString &cuteimePluginPath();
void CUTEIME_EXPORT setCuteimePluginPath(const QString &);

bool priorityLessThan(const CuteimeAbstractPluginObject *o1, const CuteimeAbstractPluginObject *o2);

class CUTEIME_EXPORT CuteimePluginManager
{
public:
    template<class T>
    static QList<T*> objects(QObject *parent = 0) {
//        cuteimeDebugIn();
        QList<T*> ret;
//        cuteimeDebug() << instance();
//        cuteimeDebug() << instance()->m_plugins.count();
//        cuteimeDebug() << instance()->m_plugins;
        foreach(CuteimePlugin *plugin, instance()->m_plugins) {
//            cuteimeDebug() << plugin;
            CuteimeAbstractPluginObject *o = plugin->object(parent);
            T *t = qobject_cast<T*>(o);
//            cuteimeDebug() << o << t;
            if (t) ret.append(t);
        }

        qSort(ret.begin(), ret.end(), priorityLessThan);
//        cuteimeDebugOut() << ret;
        return ret;
    }

private:
    static CuteimePluginManager *instance() {
        static CuteimePluginManager *manager = 0;
        if (!manager)
            manager = new CuteimePluginManager;
        return manager;
    }

    CuteimePluginManager() {
        loadPlugins();
    }

public:
    ~CuteimePluginManager() {
//        cuteimeDebugIn();
        qDeleteAll(m_plugins);
//        cuteimeDebugOut();
    }

private:
    void loadPlugins() {
//        cuteimeDebugOn();
        cuteimeDebugIn();
        QDir dir(cuteimePluginPath() + QLatin1String("/"));
        cuteimeDebug() << dir;
        if (!dir.exists()) {
            qWarning() << QString::fromLatin1("No such file or directory: %1").arg(dir.absolutePath());
        }
        QStringList pluginFilter;
#if defined(CUTEIME_PLATFORM_WINDOWS)
        pluginFilter << QLatin1String("*.dll");
#elif defined(CUTEIME_PLATFORM_MAC)
        pluginFilter << QLatin1String("*.dylib");
#elif defined(CUTEIME_PLATFORM_LINUX)
        pluginFilter << QLatin1String("*.so");
#else
        pluginFilter << QLatin1String("*");
#endif

        foreach(const QString &lib, dir.entryList(pluginFilter, QDir::Files)) {
            cuteimeDebug() << "file =" << lib;
            QPluginLoader loader(dir.absoluteFilePath(lib));
            QObject *object = loader.instance();
            cuteimeDebug() << "object =" << object;
            if (object) {
                CuteimePlugin *plugin = qobject_cast<CuteimePlugin*>(object);
                cuteimeDebug() << "plugin =" << plugin;
                if (plugin) {
                    m_plugins.append(plugin);
                    plugin->setObjectName(lib);
                } else {
                    delete object;
                }
            } else {
                cuteimeWarning() << dir.absoluteFilePath(lib) << loader.errorString();
            }
        }
        cuteimeDebugOut();
//        cuteimeDebugOff();
    }

    QList< QPointer<CuteimePlugin> > m_plugins;
};

#endif // CUTEIMEPLUGINMANAGER_H
