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

#include "cuteimeapplicationmanager.h"

#include "cuteimedebug.h"

#ifndef CUTEIME_NO_GUI
inline bool operator==(const QIcon &icon1, const QIcon &icon2)
{
    return icon1.cacheKey() == icon2.cacheKey();
}
#endif

class CuteimeApplicationManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeApplicationManager *parent);
    ~Private();

    bool init();

private:
    CuteimeApplicationManager *q;

public:
    static CuteimeApplicationManager *server;

    bool focus;
    qulonglong window;
    qulonglong widget;
    bool composing;
    QString displayLanguage;
#ifndef CUTEIME_NO_GUI
    QIcon currentIcon;
#endif
};

CuteimeApplicationManager *CuteimeApplicationManager::Private::server = 0;

CuteimeApplicationManager::Private::Private(CuteimeApplicationManager *parent)
    : QObject(parent)
    , q(parent)
    , focus(false)
    , window(0)
    , widget(0)
    , composing(false)
    , displayLanguage()
#ifndef CUTEIME_NO_GUI
    , currentIcon("qrc:/icons/cuteime_32x32.png")
#endif
{
    cuteimeDebugIn() << parent;
#ifndef CUTEIME_NO_DBUS
# ifndef CUTEIME_NO_GUI
    qDBusRegisterMetaType<QIcon>();
# endif
#endif
    switch (q->type()) {
    case Server:
        server = q;
        break;
    case Client:
        break;
    }
    cuteimeDebugOut();
}

CuteimeApplicationManager::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

bool CuteimeApplicationManager::Private::init()
{
    cuteimeDebugIn() << q->type();
    bool ret = q->CuteimeAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(focusChanged(bool)), q, SIGNAL(focusChanged(bool)));
            connect(q->proxy(), SIGNAL(windowChanged(qulonglong)), q, SIGNAL(windowChanged(qulonglong)));
            connect(q->proxy(), SIGNAL(widgetChanged(qulonglong)), q, SIGNAL(widgetChanged(qulonglong)));
            connect(q->proxy(), SIGNAL(composingChanged(bool)), q, SIGNAL(composingChanged(bool)));
            connect(q->proxy(), SIGNAL(displayLanguageChanged(QString)), q, SIGNAL(displayLanguageChanged(QString)));
#ifndef CUTEIME_NO_GUI
            connect(q->proxy(), SIGNAL(currentIconChanged(QIcon)), q, SIGNAL(currentIconChanged(QIcon)));
#endif
            connect(q->proxy(), SIGNAL(triggered(int)), q, SIGNAL(triggered(int)));
            connect(q->proxy(), SIGNAL(settingsUpdated(QString)), q, SIGNAL(settingsUpdated(QString)));
            break;
        }
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeApplicationManager::CuteimeApplicationManager(QObject *parent, Type type)
    : CuteimeAbstractIpcObject(parent, type)
{
    cuteimeDebugIn() << parent << type;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeApplicationManager::~CuteimeApplicationManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool CuteimeApplicationManager::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject *CuteimeApplicationManager::server() const
{
    return d->server;
}

getProp(CuteimeApplicationManager, QString, displayLanguage)
setProp(CuteimeApplicationManager, const QString&, displayLanguage, setDisplayLanguage)

getProp(CuteimeApplicationManager, bool, focus)
setProp(CuteimeApplicationManager, bool, focus, setFocus)

getProp(CuteimeApplicationManager, qulonglong, window)
setProp(CuteimeApplicationManager, qulonglong, window, setWindow)

getProp(CuteimeApplicationManager, qulonglong, widget)
setProp(CuteimeApplicationManager, qulonglong, widget, setWidget)

getProp(CuteimeApplicationManager, bool, composing)
setProp(CuteimeApplicationManager, bool, composing, setComposing)

#ifndef CUTEIME_NO_GUI
getProp(CuteimeApplicationManager, QIcon, currentIcon)
setProp(CuteimeApplicationManager, const QIcon&, currentIcon, setCurrentIcon)
#endif

void CuteimeApplicationManager::exec(int action)
{
    cuteimeDebugIn() << action;
    switch (type()) {
    case Server:
        emit triggered(action);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "exec", Q_ARG(int, action));
        break;
    }
    cuteimeDebugOut();
}

void CuteimeApplicationManager::settingsUpdate(const QString &name)
{
    cuteimeDebugIn() << name;
    switch (type()) {
    case Server:
        emit settingsUpdated(name);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "settingsUpdated", Q_ARG(QString, name));
        break;
    }
    cuteimeDebugOut();
}

#include "cuteimeapplicationmanager.moc"
