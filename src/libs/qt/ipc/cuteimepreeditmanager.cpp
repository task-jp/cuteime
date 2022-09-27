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

#include "cuteimepreeditmanager.h"

#include "cuteimedebug.h"

class CuteimePreeditManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimePreeditManager *parent);
    ~Private();

    bool init();
    void insert(const QString &str);
    void remove(int length);
    QString text(int length) const;

private:
    CuteimePreeditManager *q;

public:
    static CuteimePreeditManager *server;
    CuteimePreeditItem item;
    QRect rect;
#ifndef CUTEIME_NO_GUI
    QFont font;
#endif
    int cursorPosition;
    QString surroundingText;
    QString currentSelection;
    int maximumTextLength;
};

CuteimePreeditManager *CuteimePreeditManager::Private::server = 0;

CuteimePreeditManager::Private::Private(CuteimePreeditManager *parent)
    : QObject(parent)
    , q(parent)
    , item()
    , rect()
#ifndef CUTEIME_NO_GUI
    , font()
#endif
    , cursorPosition(0)
    , maximumTextLength(0)
{
    cuteimeDebugIn() << parent;
    qRegisterMetaType<CuteimePreeditItem>("CuteimePreeditItem");
    qRegisterMetaTypeStreamOperators<CuteimePreeditItem>("CuteimePreeditItem");
#ifndef CUTEIME_NO_DBUS
    qDBusRegisterMetaType<CuteimePreeditItem>();
#ifndef CUTEIME_NO_GUI
    qDBusRegisterMetaType<QFont>();
#endif
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

CuteimePreeditManager::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

bool CuteimePreeditManager::Private::init()
{
    cuteimeDebugIn();
    bool ret = q->CuteimeAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(itemChanged(CuteimePreeditItem)), q, SIGNAL(itemChanged(CuteimePreeditItem)));
            connect(q->proxy(), SIGNAL(rectChanged(QRect)), q, SIGNAL(rectChanged(QRect)));
#ifndef CUTEIME_NO_GUI
            connect(q->proxy(), SIGNAL(fontChanged(QFont)), q, SIGNAL(fontChanged(QFont)));
#endif
            connect(q->proxy(), SIGNAL(cursorPositionChanged(int)), q, SIGNAL(cursorPositionChanged(int)));
            connect(q->proxy(), SIGNAL(surroundingTextChanged(QString)), q, SIGNAL(surroundingTextChanged(QString)));
            connect(q->proxy(), SIGNAL(currentSelectionChanged(QString)), q, SIGNAL(currentSelectionChanged(QString)));
            connect(q->proxy(), SIGNAL(maximumTextLengthChanged(int)), q, SIGNAL(maximumTextLengthChanged(int)));
            connect(q->proxy(), SIGNAL(committed(QString, qulonglong)), q, SIGNAL(committed(QString, qulonglong)));
            break;
        }
    }
    cuteimeDebugOut() << ret;
    return ret;
}

void CuteimePreeditManager::Private::insert(const QString &str)
{
//    cuteimeDebugOn();
    cuteimeDebugIn() << str;
    cuteimeDebug() << item;

    if (item.to.isEmpty()) {
        item.to.prepend(str);
        item.from.prepend(str);
        item.rawString.prepend(str);
        item.selection = 0;
    } else {
        int pos = 0;
        for (int i = 0; i < item.to.length() + 1; i++) {
            cuteimeDebug() << pos << item.cursor;
            if (pos == item.cursor) {
                item.to.insert(i, str);
                item.from.insert(i, str);
                item.rawString.insert(i, str);
                break;
            }
            if (i == item.to.length()) break;
            pos += item.to.at(i).length();
            cuteimeDebug() << pos << item.cursor;
            if (pos > item.cursor) {
                QString t = item.to.at(i);
                item.to.replace(i, t.mid(pos - item.cursor));
                item.to.insert(i, str);
                item.to.insert(i, t.left(pos - item.cursor));

                QString f = item.from.at(i);
                item.from.replace(i, f.mid(pos - item.cursor));
                item.from.insert(i, str);
                item.from.insert(i, f.left(pos - item.cursor));

                item.rawString.replace(i, QString());
                item.rawString.insert(i, str);
                item.rawString.insert(i, QString());
                break;
            }
        }
    }
    item.cursor += str.length();
    item.modified = str.length();

    cuteimeDebug() << item;
    cuteimeDebugOut();
//    cuteimeDebugOff();
}

void CuteimePreeditManager::Private::remove(int length)
{
//    cuteimeDebugOn();
    cuteimeDebugIn() << length;
    cuteimeDebug() << item;

    QStringList to = item.to;
    QStringList from = item.from;
    QStringList rawString = item.rawString;
    int cursor = item.cursor;
    int selection = item.selection;

    if (selection != 0) {
        length = selection;
        selection = 0;
    }

    if (length < 0) {
        cursor += length;
        length = -length;
    }

    if (cursor < 0) {
        length += cursor;
        cursor = 0;
    }

    cuteimeDebug() << cursor << length;
    int pos = 0;
    int i = 0;
    while (length > 0) {
        int l = to.at(i).length();
        cuteimeDebug() << pos << cursor << l << i;
        if (pos == cursor) {
            cuteimeDebug() << l << length;
            if (l <= length) {
                length -= l;
                to.removeAt(i);
                from.removeAt(i);
                rawString.removeAt(i);
                continue;
            } else {
                to.replace(i, to[i].remove(cursor, length));
                from.replace(i, from[i].remove(cursor, length));
                rawString.replace(i, QString());
                break;
            }
        } else if (pos + l > cursor) {
            cuteimeDebug() << pos << cursor << length << l;
            to.replace(i, to[i].remove(cursor - pos, length));
            from.replace(i, from[i].remove(cursor - pos, length));
            rawString.replace(i, QString());
            break;
        }
        pos += l;
        i++;
    }

    item.to = to;
    item.from = from;
    item.rawString = rawString;
    item.cursor = cursor;
    item.selection = selection;
    item.modified = 0;

    cuteimeDebug() << item;
    cuteimeDebug() << item;
    cuteimeDebugOut();
//    cuteimeDebugOff();
}

QString CuteimePreeditManager::Private::text(int length) const
{
    cuteimeDebugIn() << length;
    QString ret;
    if (length < 0) {
        ret = item.to.join("").mid(item.cursor + length, -length);
    } else {
        ret = item.to.join("").mid(item.cursor, length);
    }
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimePreeditManager::CuteimePreeditManager(QObject *parent, Type type)
    : CuteimeAbstractIpcObject(parent, type)
{
    cuteimeDebugIn() << parent << type;
    d = new Private(this);
    cuteimeDebugOut();
}

CuteimePreeditManager::~CuteimePreeditManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool CuteimePreeditManager::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

CuteimeAbstractIpcObject *CuteimePreeditManager::server() const
{
    return d->server;
}

getProp(CuteimePreeditManager, CuteimePreeditItem, item)
setProp(CuteimePreeditManager, const CuteimePreeditItem&, item, setItem)

getProp(CuteimePreeditManager, QRect, rect)
setProp(CuteimePreeditManager, const QRect&, rect, setRect)

#ifndef CUTEIME_NO_GUI
getProp(CuteimePreeditManager, QFont, font)
setProp(CuteimePreeditManager, const QFont&, font, setFont)
#endif

getProp(CuteimePreeditManager, int, cursorPosition)
setProp(CuteimePreeditManager, int, cursorPosition, setCursorPosition)

getProp(CuteimePreeditManager, QString, surroundingText)
setProp(CuteimePreeditManager, const QString &, surroundingText, setSurroundingText)

getProp(CuteimePreeditManager, QString, currentSelection)
setProp(CuteimePreeditManager, const QString &, currentSelection, setCurrentSelection)

getProp(CuteimePreeditManager, int, maximumTextLength)
setProp(CuteimePreeditManager, int, maximumTextLength, setMaximumTextLength)

void CuteimePreeditManager::commit(const QString &string, qulonglong target)
{
    if (string.isEmpty()) return;
    cuteimeDebugIn() << string << target;
    switch (type()) {
    case Server:
        emit committed(string, target);
        break;
    case Client:
        if (d->server) {
            d->server->commit(string, target);
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "commit", qVariantFromValue(string), qVariantFromValue(target));
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            }
#endif // CUTEIME_NO_DBUS
        }
//   proxy()->metaObject()->invokeMethod( proxy(), "commit", Q_ARG( QString, preeditString() ) );
        break;
    }
//    setItems(CuteimePreeditItemList());
    cuteimeDebugOut();
}

bool CuteimePreeditManager::isEmpty() const
{
    cuteimeDebugIn();
    bool ret = true;
    switch (type()) {
    case Server:
        ret = d->item.to.isEmpty();
        break;
    case Client:
        if (d->server) {
            ret = d->server->isEmpty();
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusReply<bool> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "isEmpty");
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            } else {
                ret = reply.value();
            }
#endif // CUTEIME_NO_DBUS
        }
        break;
    }
    cuteimeDebugOut() << ret;
    return ret;
}

void CuteimePreeditManager::insert(const QString &str)
{
    cuteimeDebugIn() << str;
    switch (type()) {
    case Server:
        d->insert(str);
        emit itemChanged(d->item);
        break;
    case Client:
        if (d->server) {
            d->server->insert(str);
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "insert", qVariantFromValue(str));
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            }
#endif // CUTEIME_NO_DBUS
        }
        break;
    }
    cuteimeDebugOut();
}

void CuteimePreeditManager::replace(const QString &str)
{
    cuteimeDebugIn() << str;
    switch (type()) {
    case Server:
        d->remove(-1);
        d->insert(str);
        emit itemChanged(d->item);
        break;
    case Client:
        if (d->server) {
            d->server->replace(str);
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "replace", qVariantFromValue(str));
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            }
#endif // CUTEIME_NO_DBUS
        }
        break;
    }
    cuteimeDebugOut();
}

void CuteimePreeditManager::remove(int length)
{
    cuteimeDebugIn() << length;
    switch (type()) {
    case Server:
        d->remove(length);
        emit itemChanged(d->item);
        break;
    case Client:
        if (d->server) {
            d->server->remove(length);
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "remove", qVariantFromValue(length));
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            }
#endif // CUTEIME_NO_DBUS
        }
        break;
    }
    cuteimeDebugOut();
}

QString CuteimePreeditManager::text(int length) const
{
    cuteimeDebugIn() << length;
    QString ret;
    switch (type()) {
    case Server:
        ret = d->text(length);
        break;
    case Client:
        if (d->server) {
            ret = d->server->text(length);
#ifndef CUTEIME_NO_DBUS
        } else {
            QDBusReply<QString> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "text", qVariantFromValue(length));
            if (!reply.isValid()) {
                cuteimeWarning() << reply.error();
            } else {
                ret = reply.value();
            }
#endif // CUTEIME_NO_DBUS
        }
        break;
    }
    cuteimeDebugOut() << ret;
    return ret;
}

#include "cuteimepreeditmanager.moc"
