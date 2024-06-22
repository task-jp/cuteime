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

#ifndef CUTEIMEABSTRACTIPCOBJECT_H
#define CUTEIMEABSTRACTIPCOBJECT_H

#include <QObject>
#include <QMetaType>
#include <QDataStream>
#ifndef CUTEIME_NO_DBUS
#include <QDBusAbstractInterface>
#include <QDBusArgument>
#include <QDBusError>
#include <QDBusMetaType>
#include <QDBusReply>
#endif // CUTEIME_NO_DBUS
#include <QVariant>

#include "cuteimeglobal.h"
#include "dbus.h"

class CUTEIME_EXPORT CuteimeAbstractIpcObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.CuteimeAbstractIpcObject")
    Q_DISABLE_COPY(CuteimeAbstractIpcObject)
public:
    enum Type {
        Server,
        Client,
    };

    explicit CuteimeAbstractIpcObject(QObject *parent = 0, Type type = Client);
    ~CuteimeAbstractIpcObject();

    Type type() const;

    virtual bool init();

    Q_INVOKABLE virtual bool ping() const { return true; }

    virtual bool hasError() const;

protected:
    virtual CuteimeAbstractIpcObject *server() const = 0;

    QObject *proxy() const;

private:
    class Private;
    Private *d;
};


#define Q_DECLARE_METAENUM(T) \
    Q_DECLARE_METATYPE(T) \
    inline QDataStream &operator<<(QDataStream &s, T value) \
    { \
        s << static_cast<qint32>(value); \
        return s; \
    } \
    \
    inline QDataStream &operator>>(QDataStream &s, T &value) \
    { \
        qint32 v = 0; \
        s >> v; \
        value = static_cast<T>(v); \
        return s; \
    }

#ifndef CUTEIME_NO_DBUS
#define Q_DECLARE_DBUS_METAENUM(T) \
    Q_DECLARE_METAENUM(T) \
    \
    DBUSARGUMENT_OUT(T value, \
                     arg.beginStructure(); \
                     arg << static_cast<int>(value); \
                     arg.endStructure(); \
                    ) \
    \
    DBUSARGUMENT_IN(T &value, \
                    arg.beginStructure(); \
                    value = static_cast<T>(qdbus_cast<int>(arg)); \
                    arg.endStructure(); \
                   )
#endif // CUTEIME_NO_DBUS

#define getProp(C, T, V, ...) \
    T C::V() const \
    {\
        T ret; \
        switch(type()) { \
        case Server: \
            ret = d->V; \
            break; \
        case Client: \
            if (proxy()) \
                ret = proxy()->property(#V).value<T>(); \
            else \
                cuteimeWarning() << this << "is not initialized."; \
            break; \
        } \
        __VA_ARGS__ \
        return ret; \
    }

#define setProp(C, T, v, V, ...) \
    void C::V(T v) \
    { \
        cuteimeDebugIn() << v; \
        __VA_ARGS__ \
        switch(type()) { \
        case Server: \
            if(d->v == v) break; \
            d->v = v; \
            emit v##Changed(d->v); \
            break; \
        case Client: \
            if (proxy()) \
                proxy()->setProperty(#v, QVariant::fromValue(v)); \
            else \
                cuteimeWarning() << this << "is not initialized."; \
            break; \
        } \
        cuteimeDebugOut(); \
    }

#endif // CUTEIMEABSTRACTIPCOBJECT_H
