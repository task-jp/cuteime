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

#include "cuteimecandidatemanager.h"

#include "cuteimedebug.h"

class CuteimeCandidateManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeCandidateManager *parent);
    ~Private();

    bool init();

private:
    CuteimeCandidateManager *q;

public:
    static CuteimeCandidateManager *server;
    CuteimeConversionItemList items;
    int currentIndex;
};

CuteimeCandidateManager *CuteimeCandidateManager::Private::server = 0;

CuteimeCandidateManager::Private::Private(CuteimeCandidateManager *parent)
    : QObject(parent)
    , q(parent)
    , items()
    , currentIndex(-1)
{
    cuteimeDebugIn() << parent;
    qRegisterMetaType<CuteimeConversionItem>("CuteimeConversionItem");
    qRegisterMetaTypeStreamOperators<CuteimeConversionItem>("CuteimeConversionItem");
    qRegisterMetaType< CuteimeConversionItemList >("CuteimeConversionItemList");
    qRegisterMetaTypeStreamOperators< CuteimeConversionItemList >("CuteimeConversionItemList");
#ifndef CUTEIME_NO_DBUS
    qDBusRegisterMetaType<CuteimeConversionItem>();
    qDBusRegisterMetaType< CuteimeConversionItemList >();
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

CuteimeCandidateManager::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

bool CuteimeCandidateManager::Private::init()
{
    cuteimeDebugIn();
    bool ret = q->CuteimeAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(itemsChanged(CuteimeConversionItemList)), q, SIGNAL(itemsChanged(CuteimeConversionItemList)));
            connect(q->proxy(), SIGNAL(currentIndexChanged(int)), q, SIGNAL(currentIndexChanged(int)));
            break;
        }
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeCandidateManager::CuteimeCandidateManager(QObject *parent, Type type)
    : CuteimeAbstractIpcObject(parent, type)
{
    cuteimeDebugIn() << parent << type;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeCandidateManager::~CuteimeCandidateManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool CuteimeCandidateManager::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject *CuteimeCandidateManager::server() const
{
    return d->server;
}

getProp(CuteimeCandidateManager, CuteimeConversionItemList, items)
setProp(CuteimeCandidateManager, const CuteimeConversionItemList&, items, setItems)

getProp(CuteimeCandidateManager, int, currentIndex)
setProp(CuteimeCandidateManager, int, currentIndex, setCurrentIndex)

void CuteimeCandidateManager::clear()
{
    cuteimeDebugIn();
    switch (type()) {
    case Server:
        d->currentIndex = -1;
        d->items.clear();
        emit itemsChanged(d->items);
        emit currentIndexChanged(-1);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "clear");
        break;
    }
    cuteimeDebugOut();
}

#include "cuteimecandidatemanager.moc"
