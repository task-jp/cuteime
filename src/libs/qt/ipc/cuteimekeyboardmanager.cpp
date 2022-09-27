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

#include "cuteimekeyboardmanager.h"

#include "cuteimedebug.h"

class CuteimeKeyboardManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeKeyboardManager *parent);
    ~Private();

    bool init();

private:
    CuteimeKeyboardManager *q;

public:
    static CuteimeKeyboardManager *server;
    bool visible;
    QRect geometry;
    QRect focusArea;
    QString sharedData;
};

CuteimeKeyboardManager *CuteimeKeyboardManager::Private::server = 0;

CuteimeKeyboardManager::Private::Private(CuteimeKeyboardManager *parent)
    : QObject(parent)
    , q(parent)
    , visible(false)
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

CuteimeKeyboardManager::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

bool CuteimeKeyboardManager::Private::init()
{
    cuteimeDebugIn();
    bool ret = q->CuteimeAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(visibleChanged(bool)), q, SIGNAL(visibleChanged(bool)));
            connect(q->proxy(), SIGNAL(geometryChanged(QRect)), q, SIGNAL(geometryChanged(QRect)));
            connect(q->proxy(), SIGNAL(focusAreaChanged(QRect)), q, SIGNAL(focusAreaChanged(QRect)));
            connect(q->proxy(), SIGNAL(sharedDataChanged(QString)), q, SIGNAL(sharedDataChanged(QString)));
            break;
        }
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeKeyboardManager::CuteimeKeyboardManager(QObject *parent, Type type)
    : CuteimeAbstractIpcObject(parent, type)
{
    cuteimeDebugIn() << parent << type;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimeKeyboardManager::~CuteimeKeyboardManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool CuteimeKeyboardManager::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject *CuteimeKeyboardManager::server() const
{
    return d->server;
}

getProp(CuteimeKeyboardManager, bool, visible)
setProp(CuteimeKeyboardManager, bool, visible, setVisible)

getProp(CuteimeKeyboardManager, QRect, geometry)
setProp(CuteimeKeyboardManager, const QRect &, geometry, setGeometry)

getProp(CuteimeKeyboardManager, QRect, focusArea)
setProp(CuteimeKeyboardManager, const QRect &, focusArea, setFocusArea)

getProp(CuteimeKeyboardManager, QString, sharedData)
setProp(CuteimeKeyboardManager, const QString &, sharedData, setSharedData)

#include "cuteimekeyboardmanager.moc"
