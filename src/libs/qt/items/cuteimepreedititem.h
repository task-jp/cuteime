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

#ifndef CUTEIMEPREEDITITEM_H
#define CUTEIMEPREEDITITEM_H

#include "cuteimeglobal.h"
#include "dbus.h"
#include <QMetaType>
#include <QStringList>

struct CUTEIME_EXPORT CuteimePreeditItem {
    CuteimePreeditItem()
        : cursor(0), selection(0), modified(0)
    {}

    QStringList to;
    QStringList from;
    QStringList rawString;
    int cursor;
    int selection;
    int modified;

    inline bool operator==(const CuteimePreeditItem &other) {
        return (to == other.to && from == other.from && rawString == other.rawString && cursor == other.cursor && selection == other.selection && modified == other.modified);
    }
};

CUTEIME_EXPORT QDebug &operator<<(QDebug &debug, const CuteimePreeditItem &elem);

Q_DECLARE_METATYPE(CuteimePreeditItem)

DATASTREAM_IN(CuteimePreeditItem &e,
    s >> e.to >> e.from >> e.rawString >> e.cursor >> e.selection >> e.modified;
)

DATASTREAM_OUT(const CuteimePreeditItem &e,
    s << e.to << e.from << e.rawString << e.cursor << e.selection << e.modified;
)

#ifndef CUTEIME_NO_DBUS
DBUSARGUMENT_IN(CuteimePreeditItem &e,
    arg.beginStructure();
    e.to = qdbus_cast<QStringList>(arg);
    e.from = qdbus_cast<QStringList>(arg);
    e.rawString = qdbus_cast<QStringList>(arg);
    e.cursor = qdbus_cast<int>(arg);
    e.selection = qdbus_cast<int>(arg);
    e.modified = qdbus_cast<int>(arg);
    arg.endStructure();
)

DBUSARGUMENT_OUT(const CuteimePreeditItem &e,
    arg.beginStructure();
    arg << e.to;
    arg << e.from;
    arg << e.rawString;
    arg << e.cursor;
    arg << e.selection;
    arg << e.modified;
    arg.endStructure();
)
#endif // CUTEIME_NO_DBUS

#endif//CUTEIMEPREEDITITEM_H
