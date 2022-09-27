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

#include "cuteimeabstractipcobject.h"

#include "cuteimedebug.h"

#include <QMetaClassInfo>

#ifndef CUTEIME_NO_DBUS
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#endif // CUTEIME_NO_DBUS

class CuteimeAbstractIpcObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeAbstractIpcObject *parent, Type t);
    bool init();

private:
    CuteimeAbstractIpcObject *q;

public:
    Type type;
    QObject *proxy;
};

CuteimeAbstractIpcObject::Private::Private(CuteimeAbstractIpcObject *parent, Type t)
    : QObject(parent)
    , q(parent)
    , type(t)
    , proxy(0)
{
    cuteimeDebugIn() << parent << t;
    switch (type) {
    case Server:
        break;
    case Client:
        break;
    }
    cuteimeDebugOut();
}

bool CuteimeAbstractIpcObject::Private::init()
{
    bool ret = true;
    cuteimeDebugIn();
    const QMetaObject *mobject = q->metaObject();
    while (mobject->superClass()->className() != q->staticMetaObject.className()) {
        mobject = mobject->superClass();
    }
#ifndef CUTEIME_NO_DBUS
    QDBusConnection connection = QDBusConnection::sessionBus();
#endif
    switch (type) {
    case Server: {
#ifndef CUTEIME_NO_DBUS
        if (!connection.isConnected()) {
            cuteimeWarning() << "Cannot connect to the D-Bus session bus.";
            ret = false;
            break;
        }

        bool ret = connection.registerObject(QString("/%1").arg(mobject->className()), q, QDBusConnection::ExportAllContents);
        if (!ret) {
            cuteimeWarning() << "registerObject failure" << CUTEIME_DBUS_SERVICE;
            ret = false;
            break;
        }
#endif // CUTEIME_NO_DBUS
        break;
    }
    case Client:
        if (!proxy) {
            if (q->server()) {
                proxy = q->server();
            } else {
                QString interface;
                for (int i = 0; i < mobject->classInfoCount(); i++) {
                    if (QString(mobject->classInfo(i).name()) == QLatin1String("D-Bus Interface")) {
                        interface = QString(mobject->classInfo(i).value());
                    }
                }
                if (interface.isNull()) {
                    cuteimeWarning() << "interface not found" << q;
                    ret = false;
#ifndef CUTEIME_NO_DBUS
                } else {
                    QDBusInterface *iface = new QDBusInterface(CUTEIME_DBUS_SERVICE, QString("/%1").arg(mobject->className()), interface, connection);
                    if (!iface->isValid()) {
                        cuteimeWarning() << iface->lastError() << q;
                        delete iface;
                        ret = false;
                    } else {
                        proxy = iface;
                    }
#endif // CUTEIME_NO_DBUS
                }
            }
        }
        break;
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject::CuteimeAbstractIpcObject(QObject *parent, Type type)
    : QObject(parent)
{
    cuteimeDebugIn() << parent << type;
    d = new Private(this, type);
    cuteimeDebugOut();
}

CuteimeAbstractIpcObject::~CuteimeAbstractIpcObject()
{
    cuteimeDebugIn();
    delete d;
    d = 0;
    cuteimeDebugOut();
}

bool CuteimeAbstractIpcObject::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject::Type CuteimeAbstractIpcObject::type() const
{
    return d ? d->type : CuteimeAbstractIpcObject::Client;
}

QObject *CuteimeAbstractIpcObject::proxy() const
{
    return d ? d->proxy : 0;
}

bool CuteimeAbstractIpcObject::hasError() const
{
    cuteimeDebugIn();
    bool ret = false;
    if (!d->proxy) {
        ret = true;
#ifndef CUTEIME_NO_DBUS
    } else if (!server()) {
        QDBusAbstractInterface *iface = qobject_cast<QDBusAbstractInterface*>(proxy());
        if (!iface->isValid()) {
            ret = true;
        }
#endif // CUTEIME_NO_DBUS
    }
    cuteimeDebugOut() << ret;
    return ret;
}

#include "cuteimeabstractipcobject.moc"
