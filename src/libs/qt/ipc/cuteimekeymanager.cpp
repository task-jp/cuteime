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

#include "cuteimekeymanager.h"
#include "cuteimedebug.h"

class CuteimeKeyManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeKeyManager *parent);
    ~Private();

    bool init();

private:
    CuteimeKeyManager *q;

public:
    static CuteimeKeyManager *server;
    bool isAccepted;
};

CuteimeKeyManager *CuteimeKeyManager::Private::server = 0;

CuteimeKeyManager::Private::Private(CuteimeKeyManager *parent)
    : QObject(parent)
    , q(parent)
    , isAccepted(false)
{
    cuteimeDebugIn() << parent;
    switch (q->type()) {
    case Server:
        server = q;
        break;
    case Client:
        break;
    }
    cuteimeDebugOut();
}

CuteimeKeyManager::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

bool CuteimeKeyManager::Private::init()
{
    cuteimeDebugIn() << q->type();
    bool ret = q->CuteimeAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(keyPressed(QString, int, int, bool)), q, SIGNAL(keyPressed(QString, int, int, bool)));
            connect(q->proxy(), SIGNAL(keyReleased(QString, int, int, bool)), q, SIGNAL(keyReleased(QString, int, int, bool)));
            break;
        }
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeKeyManager::CuteimeKeyManager(QObject *parent, Type type)
    : CuteimeAbstractIpcObject(parent, type)
{
    cuteimeDebugIn() <<  parent << type;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeKeyManager::~CuteimeKeyManager()
{
    cuteimeDebugIn();
    delete d;
    d = 0;
    cuteimeDebugOut();
}

bool CuteimeKeyManager::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

void CuteimeKeyManager::process(QString text, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    cuteimeDebugIn() << text << QString("%1").arg(keycode, 0, 16) << modifiers << isPress << autoRepeat;
    switch (type()) {
    case Server:
        processKey(text, keycode, modifiers, isPress, autoRepeat);
        break;
    case Client:
#if 0
        QMetaObject::invokeMethod(proxy(), "process", Q_ARG(QString, text), Q_ARG(int, keycode), Q_ARG(int, modifiers), Q_ARG(bool, isPress), Q_ARG(bool, autoRepeat));
#else
        if (d->server) {
            d->server->process(text, keycode, modifiers, isPress, autoRepeat);
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusAbstractInterface *iface = qobject_cast<QDBusAbstractInterface*>(proxy());
            QDBusReply<void> reply = iface->call(QDBus::BlockWithGui, "process", text, keycode, modifiers, isPress, autoRepeat);
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            }
#endif // CUTEIME_NO_DBUS
        }
#endif
        break;
    }
    cuteimeDebugOut();
}

bool CuteimeKeyManager::keyPress(const QString &text, int keycode, int modifiers, bool autoRepeat)
{
    cuteimeDebugIn() << text << QString("0x%1").arg(keycode, 0, 16) << modifiers << autoRepeat;
    switch (type()) {
    case Server:
        d->isAccepted = false;
        emit keyPressed(text, keycode, modifiers, autoRepeat);
        break;
    case Client:
        if (d->server) {
            d->server->keyPress(text, keycode, modifiers, autoRepeat);
#ifndef CUTEIME_NO_DBUS
        } else {
            qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::Block, "keyPress", text, keycode, modifiers, autoRepeat);
#endif // CUTEIME_NO_DBUS
        }
//        QMetaObject::invokeMethod(proxy(), "keyPress", Q_ARG(QString, text), Q_ARG(int, keycode), Q_ARG(int, modifiers), Q_ARG(bool, autoRepeat));
        break;
    }
    bool ret = isAccepted();
    cuteimeDebugOut() << ret;
    return ret;
}

bool CuteimeKeyManager::keyRelease(const QString &text, int keycode, int modifiers, bool autoRepeat)
{
    cuteimeDebugIn() << text << QString("0x%1").arg(keycode, 0, 16) << modifiers << autoRepeat;
    switch (type()) {
    case Server:
        d->isAccepted = false;
        emit keyReleased(text, keycode, modifiers, autoRepeat);
        break;
    case Client:
        if (d->server) {
            d->server->keyRelease(text, keycode, modifiers, autoRepeat);
#ifndef CUTEIME_NO_DBUS
        } else {
            qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::Block, "keyRelease", text, keycode, modifiers, autoRepeat);
#endif // CUTEIME_NO_DBUS
        }
//        QMetaObject::invokeMethod(proxy(), "keyRelease", Q_ARG(QString, text), Q_ARG(int, keycode), Q_ARG(int, modifiers), Q_ARG(bool, autoRepeat));
        break;
    }
    bool ret = isAccepted();
    cuteimeDebugOut() << ret;
    return ret;
}

bool CuteimeKeyManager::isAccepted() const
{
    bool ret = false;
    cuteimeDebugIn();
    switch (type()) {
    case Server:
        if (d)
            ret = d->isAccepted;
        break;
    case Client:
        if (proxy())
            ret = proxy()->property("accepted").toBool();
        break;
    }
    cuteimeDebugOut() << ret;
    return ret;
}

void CuteimeKeyManager::accept()
{
    cuteimeDebugIn();
    switch (type()) {
    case Server:
        d->isAccepted = true;
        break;
    case Client:
        if (d->server) {
            d->server->accept();
#ifndef CUTEIME_NO_DBUS
        } else {
            qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "accept");
#endif // CUTEIME_NO_DBUS
        }
        break;
    }
    cuteimeDebugOut();
}

CuteimeAbstractIpcObject *CuteimeKeyManager::server() const
{
    return d->server;
}

#ifdef CUTEIME_NO_GUI
void CuteimeKeyManager::processKey(QString text, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    if (isPress)
        keyPress(text, keycode, modifiers, autoRepeat);
    else
        keyRelease(text, keycode, modifiers, autoRepeat);
}
#endif

#include "cuteimekeymanager.moc"
