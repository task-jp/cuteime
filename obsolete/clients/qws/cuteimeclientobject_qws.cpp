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

#include "cuteimeclientobject_qws.h"
#include "cuteimeclient_qws.h"

class CuteimeClientObjectQWS::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeClientObjectQWS *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);

private:
    CuteimeClientObjectQWS *q;
    CuteimeClientQWS *client;
};

CuteimeClientObjectQWS::Private::Private(CuteimeClientObjectQWS *parent)
    : QObject(parent)
    , q(parent)
    , client(0)
{
    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Platform"));
    q->setIcon(QIcon(":/cuteimeclient_qws.png"));
    trConnect(this, QT_TR_NOOP("Input Method Backend for QWS"));
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"));
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
}

CuteimeClientObjectQWS::Private::~Private()
{
}

void CuteimeClientObjectQWS::Private::enabledChanged(bool enabled)
{
    if (enabled && !client) {
        client = new CuteimeClientQWS(this);
    } else if (!enabled && client) {
        delete client;
        client = 0;
    }
}

CuteimeClientObjectQWS::CuteimeClientObjectQWS(QObject *parent)
    : CuteimeAbstractPluginObject(parent)
{
    d = new Private(this);
}

CuteimeClientObjectQWS::~CuteimeClientObjectQWS()
{
    delete d;
}

#include "cuteimeclientobject_qws.moc"
