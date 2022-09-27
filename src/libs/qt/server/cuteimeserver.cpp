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

#include "cuteimeserver.h"
#include "cuteimedebug.h"

#include "cuteimeapplicationmanager.h"
#include "cuteimeinputmethodmanager.h"
#include "cuteimepreeditmanager.h"
#include "cuteimecandidatemanager.h"
#include "cuteimepluginmanager.h"
#include "cuteimeconverter.h"
#include "cuteimekeymanager.h"
#include "cuteimekeyboardmanager.h"

#include <QSettings>
#ifndef CUTEIME_NO_DBUS
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#endif

class CuteimeServer::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeServer *parent);
    ~Private();

private:
    CuteimeServer *q;
};


CuteimeServer::Private::Private(CuteimeServer *parent)
    : QObject(parent)
    , q(parent)
{
    cuteimeDebugIn() << parent;
#ifndef CUTEIME_NO_DBUS
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        cuteimeWarning() << "Cannot connect to the D-Bus session bus.";
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }

    QDBusReply<QStringList> registeredServiceNames = connection.interface()->registeredServiceNames();
    if (!registeredServiceNames.isValid()) {
        cuteimeWarning() << registeredServiceNames.error();
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    if (registeredServiceNames.value().contains(CUTEIME_DBUS_SERVICE)) {
        cuteimeWarning() << "another process is running already.";
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    bool ret = connection.registerService(CUTEIME_DBUS_SERVICE);
    if (!ret) {
        cuteimeWarning() << "registerService failure" << CUTEIME_DBUS_SERVICE;
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
#endif // CUTEIME_NO_DBUS

    QSettings settings;
    settings.beginGroup("Application");
    QString lang = settings.value("Language", tr("en")).toString();

    CuteimeApplicationManager *manager = new CuteimeApplicationManager(this, CuteimeAbstractIpcObject::Server);
    manager->init();
    manager->setDisplayLanguage(lang);
    (new CuteimeInputMethodManager(this, CuteimeAbstractIpcObject::Server))->init();
    (new CuteimePreeditManager(this, CuteimeAbstractIpcObject::Server))->init();
    (new CuteimeCandidateManager(this, CuteimeAbstractIpcObject::Server))->init();
    (new CuteimeKeyManager(this, CuteimeAbstractIpcObject::Server))->init();
    (new CuteimeKeyboardManager(this, CuteimeAbstractIpcObject::Server))->init();
    cuteimeDebugOut();
}

CuteimeServer::Private::~Private()
{

}

CuteimeServer::CuteimeServer(QObject *parent)
    : QObject(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeServer::~CuteimeServer()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "cuteimeserver.moc"
