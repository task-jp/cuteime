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

#ifndef CUTEIMEKEYBOARDMANAGER_H
#define CUTEIMEKEYBOARDMANAGER_H

#include "cuteimeglobal.h"
#include "cuteimeabstractipcobject.h"

#include <QRect>

class CUTEIME_EXPORT CuteimeKeyboardManager : public CuteimeAbstractIpcObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(QRect focusArea READ focusArea WRITE setFocusArea NOTIFY focusAreaChanged)
    Q_PROPERTY(QString sharedData READ sharedData WRITE setSharedData NOTIFY sharedDataChanged)
    Q_CLASSINFO("D-Bus Interface", "local.KeyboardManager")
    Q_DISABLE_COPY(CuteimeKeyboardManager)
public:
    explicit CuteimeKeyboardManager(QObject *parent = 0, Type type = Client);
    ~CuteimeKeyboardManager();

    bool init();

    bool visible() const;
    QRect geometry() const;
    QRect focusArea() const;
    QString sharedData() const;

public slots:
    void setVisible(bool visible);
    void setGeometry(const QRect &geometry);
    void setFocusArea(const QRect &focusArea);
    void setSharedData(const QString &sharedData);

signals:
    void visibleChanged(bool visible);
    void geometryChanged(const QRect &geometry);
    void focusAreaChanged(const QRect &focusArea);
    void sharedDataChanged(const QString &sharedData);

protected:
    CuteimeAbstractIpcObject *server() const;

private:
    class Private;
    Private *d;
};

#endif // CUTEIMEKEYBOARDMANAGER_H
