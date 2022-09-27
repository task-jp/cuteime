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

#include "cuteimetest.h"
#include "cuteimedebug.h"
#include "cuteimeserver.h"
#include "cuteimepluginmanager.h"

class CuteimeTest::Private
{
public:
    Private(CuteimeTest *parent);
    ~Private();

private:
    CuteimeTest *q;

public:
    QMap<CuteimeAbstractPluginObject*, bool> enabled;
};

CuteimeTest::Private::Private(CuteimeTest *parent)
    : q(parent)
{
    if (!qgetenv("CUTEIME_DEBUG").isEmpty()) cuteimeDebugOn();
}

CuteimeTest::Private::~Private()
{

}

CuteimeTest::CuteimeTest(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

CuteimeTest::~CuteimeTest()
{
    delete d;
}

void CuteimeTest::initTestCase()
{
    cuteimeDebugIn();
    new CuteimeServer(this);

#ifdef CUTEIME_APPLICATION_DIR_PATH
    QDir rootDir(CUTEIME_MACRO_TO_STRING2(CUTEIME_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    cuteimeDebug() << rootDir;

    // up to root dir
    QString appPath(CUTEIME_MACRO_TO_STRING(CUTEIME_APP_PATH));
    if (!appPath.isEmpty()) {
        for (int i = 0; i < appPath.count(QLatin1Char('/')) + 1; i++) {
            rootDir.cdUp();
        }
    }
    cuteimeDebug() << rootDir << QLatin1String(CUTEIME_MACRO_TO_STRING(CUTEIME_PLUGIN_PATH));

    rootDir.cd(QLatin1String(CUTEIME_MACRO_TO_STRING(CUTEIME_PLUGIN_PATH)));
    cuteimeDebug() << rootDir;

    cuteimeDebug() << rootDir.absolutePath();
    setCuteimePluginPath(rootDir.absolutePath());
#ifndef Q_OS_LINUX
    if (rootDir.exists(QLatin1String("inputmethods"))) {
        q->addLibraryPath(rootDir.absolutePath());
    }
#endif

    QStringList identifiers = activePluginList();
    if (!identifiers.isEmpty()) {
        foreach (CuteimeAbstractPluginObject* object, CuteimePluginManager::objects<CuteimeAbstractPluginObject>(this)) {
            d->enabled[object] = object->isEnabled();
            object->setEnabled(identifiers.contains(object->identifier()));
            object->setActive(identifiers.contains(object->identifier()));
        }
    }
    cuteimeDebugOut();
}

void CuteimeTest::init()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void CuteimeTest::cleanup()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void CuteimeTest::cleanupTestCase()
{
    cuteimeDebugIn();
    foreach (CuteimeAbstractPluginObject* object, CuteimePluginManager::objects<CuteimeAbstractPluginObject>(this)) {
        object->setEnabled(d->enabled[object]);
    }
    cuteimeDebugOut();
}

QStringList CuteimeTest::activePluginList() const
{
    return QStringList();
}
