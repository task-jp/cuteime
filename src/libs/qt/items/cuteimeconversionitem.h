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

#ifndef CUTEIMECONVERSIONITEM_H
#define CUTEIMECONVERSIONITEM_H

#include "cuteimeglobal.h"
#include "dbus.h"
#include <QMetaType>

struct CUTEIME_EXPORT CuteimeConversionItem {
    int index;
    QString from;
    QString to;
    QString hint;
    QString source;

    inline bool operator==(const CuteimeConversionItem &other) const {
        return (from == other.from && to == other.to && hint == other.hint && source == other.source);
    }
};

typedef QList<CuteimeConversionItem> CuteimeConversionItemList;

CUTEIME_EXPORT QDebug &operator<<(QDebug &debug, const CuteimeConversionItem &elem);

Q_DECLARE_METATYPE(CuteimeConversionItem)
Q_DECLARE_METATYPE(CuteimeConversionItemList)

DATASTREAM_IN(CuteimeConversionItem &e,
    s >> e.index >> e.from >> e.to >> e.hint >> e.source;
)

DATASTREAM_OUT(const CuteimeConversionItem &e,
    s << e.index << e.from << e.to << e.hint << e.source;
)

#ifndef CUTEIME_NO_DBUS
DBUSARGUMENT_IN(CuteimeConversionItem &e,
    arg.beginStructure();
    e.index = qdbus_cast<int>(arg);
    e.from = qdbus_cast<QString>(arg);
    e.to = qdbus_cast<QString>(arg);
    e.hint = qdbus_cast<QString>(arg);
    e.source = qdbus_cast<QString>(arg);
    arg.endStructure();
)

DBUSARGUMENT_OUT(const CuteimeConversionItem &e,
    arg.beginStructure();
    arg << e.index;
    arg << e.from;
    arg << e.to;
    arg << e.hint;
    arg << e.source;
    arg.endStructure();
)

DBUSARGUMENT_IN(CuteimeConversionItemList &c,
    arg.beginArray();
    while (!arg.atEnd())
    {
        CuteimeConversionItem e = qdbus_cast<CuteimeConversionItem>(arg);
        c.append(e);
    }
    arg.endArray();
)

DBUSARGUMENT_OUT(CuteimeConversionItemList &c,
    arg.beginArray();
    foreach(CuteimeConversionItem e, c)
    {
        arg << e;
    }
    arg.endArray();
)
#endif // CUTEIME_NO_DBUS

#endif//CUTEIMECONVERSIONITEM_H
