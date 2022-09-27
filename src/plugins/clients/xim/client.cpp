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

#include "client.h"
#include <QtPlugin>
#include <QWidget>

#include <cuteimedebug.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimekeymanager.h>
#include <cuteimepreeditmanager.h>

using namespace Xim;

class Client::Private : private QObject
{
    Q_OBJECT
public:
    Private(Client *parent);
    ~Private();

    void setActive(bool active);

private slots:
    void sendPreeditString(const CuteimePreeditItem &item);
    void sendCommitString(const QString &commitString, qulonglong target);

private:
    Client *q;
public:
    CuteimeApplicationManager applicationManager;
    CuteimeKeyManager keyManager;
    CuteimePreeditManager preeditManager;
};

Client::Private::Private(Client *parent)
    : QObject(parent)
    , q(parent)
{
    cuteimeDebugIn() << parent;
    applicationManager.init();
    keyManager.init();
    preeditManager.init();
    connect(&preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(sendPreeditString(CuteimePreeditItem)));
    connect(&preeditManager, SIGNAL(committed(QString, qulonglong)), this, SLOT(sendCommitString(QString, qulonglong)));
    cuteimeDebugOut();
}

Client::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Client::Private::sendPreeditString(const CuteimePreeditItem &item)
{
    if (!q->focusWindow() || (applicationManager.window() != q->focusWindow())) return;
    cuteimeDebugIn() << item;
    q->sendPreeditString(item.to.join(""), item.cursor, item.selection);
    cuteimeDebugOut();
}

void Client::Private::sendCommitString(const QString &commitString, qulonglong target)
{
    if (q->focusWindow() != target) return;
    cuteimeDebugIn() << commitString << target;
    q->sendCommitString(commitString);
    cuteimeDebugOut();
}

void Client::Private::setActive(bool active)
{
    cuteimeDebugIn() << active;
    if (active) {
        applicationManager.setWindow(q->clientWindow());
        applicationManager.setWidget(q->focusWindow());
        applicationManager.setFocus(true);
    } else {
        if (applicationManager.focus() == q->focusWindow()) {
            applicationManager.setWindow(0);
            applicationManager.setWidget(0);
            applicationManager.setFocus(false);
        }
    }
    cuteimeDebugOut();
}

Client::Client(QObject *parent)
    : QXimInputMethod(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Client::~Client()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool Client::filter(int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    cuteimeDebugIn() << unicode << keycode << modifiers << isPress << autoRepeat;
    if (isPress)
        d->keyManager.keyPress(QChar(unicode), keycode, modifiers, autoRepeat);
    else
        d->keyManager.keyRelease(QChar(unicode), keycode, modifiers, autoRepeat);

    bool ret = d->keyManager.isAccepted();
    cuteimeDebugOut() << ret;
    return ret;
}

qulonglong Client::winId()
{
    static QWidget *dummy = 0;
    if (!dummy)
        dummy = new QWidget;
    return dummy->winId();
}

const char *Client::name() const
{
    return "cuteime";
}

const char *Client::locale() const
{
    return "ja_JP";
}

void Client::updateHandler(UpdateType type)
{
    cuteimeDebugIn() << type;
    switch (type) {
    case Update:
        break;
    case FocusIn:
        d->setActive(true);
        break;
    case FocusOut:
        d->setActive(false);
        break;
    default:
        break;
    }
    cuteimeDebugOut();
}

#include "client.moc"
