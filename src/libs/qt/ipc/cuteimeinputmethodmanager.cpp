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

#include "cuteimeinputmethodmanager.h"
#include "cuteimedebug.h"

class CuteimeInputMethodManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeInputMethodManager *parent);
    ~Private();

    bool init();

private:
    CuteimeInputMethodManager *q;

public:
    static CuteimeInputMethodManager *server;

    QString locale;
    QString identifier;
    QString converter;
    QString interpreter;
    QString engine;
    uint state;
};

CuteimeInputMethodManager *CuteimeInputMethodManager::Private::server = 0;

CuteimeInputMethodManager::Private::Private(CuteimeInputMethodManager *parent)
    : QObject(parent)
    , q(parent)
    , state(0)
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

CuteimeInputMethodManager::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

bool CuteimeInputMethodManager::Private::init()
{
    cuteimeDebugIn() << q->type();
    bool ret = q->CuteimeAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(localeChanged(QString)), q, SIGNAL(localeChanged(QString)));
            connect(q->proxy(), SIGNAL(identifierChanged(QString)), q, SIGNAL(identifierChanged(QString)));
            connect(q->proxy(), SIGNAL(converterChanged(QString)), q, SIGNAL(converterChanged(QString)));
            connect(q->proxy(), SIGNAL(interpreterChanged(QString)), q, SIGNAL(interpreterChanged(QString)));
            connect(q->proxy(), SIGNAL(engineChanged(QString)), q, SIGNAL(engineChanged(QString)));
            connect(q->proxy(), SIGNAL(stateChanged(uint)), q, SIGNAL(stateChanged(uint)));
            connect(q->proxy(), SIGNAL(executed(QString)), q, SIGNAL(executed(QString)));
            break;
        }
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeInputMethodManager::CuteimeInputMethodManager(QObject *parent, Type type)
    : CuteimeAbstractIpcObject(parent, type)
{
    cuteimeDebugIn() << parent << type;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeInputMethodManager::~CuteimeInputMethodManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool CuteimeInputMethodManager::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject *CuteimeInputMethodManager::server() const
{
    return d->server;
}

getProp(CuteimeInputMethodManager, QString, locale)
setProp(CuteimeInputMethodManager, const QString &, locale, setLocale)

getProp(CuteimeInputMethodManager, QString, identifier)
setProp(CuteimeInputMethodManager, const QString &, identifier, setIdentifier)

getProp(CuteimeInputMethodManager, QString, converter)
setProp(CuteimeInputMethodManager, const QString &, converter, setConverter)

getProp(CuteimeInputMethodManager, QString, interpreter)
setProp(CuteimeInputMethodManager, const QString &, interpreter, setInterpreter)

getProp(CuteimeInputMethodManager, QString, engine)
setProp(CuteimeInputMethodManager, const QString &, engine, setEngine)

getProp(CuteimeInputMethodManager, uint, state)
setProp(CuteimeInputMethodManager, uint, state, setState)

void CuteimeInputMethodManager::execute(const QString &command)
{
    cuteimeDebugIn() << command;
    switch (type()) {
    case Server:
        emit executed(command);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "execute", Q_ARG(QString, command));
        break;
    }
    cuteimeDebugOut();
}

#include "cuteimeinputmethodmanager.moc"
