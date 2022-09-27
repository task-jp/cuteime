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

#include "cuteimeclient_qws.h"
#include <QWidget>
#include <QWSServer>

#include "cuteimedebug.h"
#include "cuteimeapplicationmanager.h"
#include "cuteimekeymanager.h"
#include "cuteimepreeditmanager.h"

class CuteimeClientQWS::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeClientQWS *parent);
    ~Private();

    void queryResponse(int property, const QVariant &result);
    void updateHandler(int type);

private slots:
    void sendPreeditString();
    void sendCommitString(const QString &commitString);

public slots:
    void update();

private:
    CuteimeClientQWS *q;
public:
    CuteimeKeyManager filter;
    CuteimePreeditManager preedit;
    CuteimeApplicationManager manager;
};

CuteimeClientQWS::Private::Private(CuteimeClientQWS *parent)
    : QObject(parent)
    , q(parent)
{
    cuteimeDebugIn() << parent;
    filter.init();
    preedit.init();
    manager.init();
    QWSServer::setCurrentInputMethod(q);
    cuteimeDebugOut();
}

CuteimeClientQWS::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void CuteimeClientQWS::Private::sendPreeditString()
{
    cuteimeDebugIn();
    q->sendPreeditString(preedit.items(), preedit.cursorPosition(), preedit.selectionLength());
    update();
    cuteimeDebugOut();
}

void CuteimeClientQWS::Private::sendCommitString(const QString &commitString)
{
    cuteimeDebugIn() << commitString;
    q->sendCommitString(commitString);
    cuteimeDebugOut();
}

void CuteimeClientQWS::Private::update()
{
    cuteimeDebugIn();
    qwsServer->sendIMQuery(Qt::ImMicroFocus);
    qwsServer->sendIMQuery(Qt::ImFont);
    cuteimeDebugOut();
}

CuteimeClientQWS::CuteimeClientQWS(QObject *parent)
    : QWSInputMethod()
{
    cuteimeDebugIn() << parent;
    setParent(parent);
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeClientQWS::~CuteimeClientQWS()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool CuteimeClientQWS::filter(int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    cuteimeDebugIn() << unicode << keycode << modifiers << isPress << autoRepeat;
    bool ret = d->filter.filter(QChar(unicode), keycode, modifiers, isPress, autoRepeat);
    cuteimeDebugOut() << ret;
    return ret;
}

void CuteimeClientQWS::reset()
{
    cuteimeDebugIn();
    d->manager.exec(CuteimeApplicationManager::Reset);
// d->preedit.commit();
// d->preedit.reset();
    cuteimeDebugOut();
}

void CuteimeClientQWS::Private::queryResponse(int property, const QVariant &result)
{
    cuteimeDebugIn() << property << result;
    switch (property) {
    case Qt::ImMicroFocus:
        preedit.setRect(result.toRect());
        break;
    case Qt::ImFont:
        preedit.setFont(result.value<QFont>());
        break;
    }
    cuteimeDebugOut();
}

void CuteimeClientQWS::queryResponse(int property, const QVariant &result)
{
    cuteimeDebugIn() << property << result;
    d->queryResponse(property, result);
    QWSInputMethod::queryResponse(property, result);
    cuteimeDebugOut();
}

void CuteimeClientQWS::Private::updateHandler(int type)
{
    cuteimeDebugIn() << type;
    switch (type) {
    case Update:
        update();
        break;
    case FocusIn:
        connect(&preedit, SIGNAL(preeditStringChanged(QString)), this, SLOT(sendPreeditString()), Qt::QueuedConnection);
        connect(&preedit, SIGNAL(cursorPositionChanged(int)), this, SLOT(sendPreeditString()), Qt::QueuedConnection);
        connect(&preedit, SIGNAL(selectionLengthChanged(int)), this, SLOT(sendPreeditString()), Qt::QueuedConnection);
        connect(&preedit, SIGNAL(committed(QString, uint)), this, SLOT(sendCommitString(QString)));
        manager.setFocus(true);
        break;
    case FocusOut:
        manager.setFocus(false);
        disconnect(&preedit, SIGNAL(preeditStringChanged(QString)), this, SLOT(sendPreeditString()));
        disconnect(&preedit, SIGNAL(cursorPositionChanged(int)), this, SLOT(sendPreeditString()));
        disconnect(&preedit, SIGNAL(selectionLengthChanged(int)), this, SLOT(sendPreeditString()));
        disconnect(&preedit, SIGNAL(committed(QString, uint)), this, SLOT(sendCommitString(QString)));
        break;
    case Reset:
        q->reset();
        break;
    case Destroyed:
        break;
    }
    cuteimeDebugOut();
}

void CuteimeClientQWS::updateHandler(int type)
{
    cuteimeDebugIn() << type;
    d->updateHandler(type);
    QWSInputMethod::updateHandler(type);
    cuteimeDebugOut();
}

#include "cuteimeclient_qws.moc"
