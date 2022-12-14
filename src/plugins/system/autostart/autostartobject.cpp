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

#include "autostartobject.h"

#include <cuteimedynamictranslator.h>
#include <cuteimedebug.h>

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

class AutoStartObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(AutoStartObject *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);

private:
    AutoStartObject *q;
};

AutoStartObject::Private::Private(AutoStartObject *parent)
    : QObject(parent)
    , q(parent)
{
    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Settings/Auto Start"), q, "categoryName");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/autostarticon.png"));
#endif
    trConnect(this, QT_TR_NOOP("Auto Start"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP("Turn this on/off to switch auto start.\nThis plugin creates ~/.config/autostart/cuteime.desktop to start cuteime at login."), q, "description");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
}

AutoStartObject::Private::~Private()
{
}

void AutoStartObject::Private::enabledChanged(bool enabled)
{
    QString desktop = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, QLatin1String("cuteime.desktop"));
    if (desktop.isNull()) {
        cuteimeWarning() << "cuteime.desktop not found";
    }
    QString autostart = QString("%1/%2/%3").arg(QDir::homePath()).arg(".config/autostart").arg("cuteime.desktop");
    if (enabled) {
        QFile::link(desktop, autostart);
    } else {
        QFile::remove(autostart);
    }
}

AutoStartObject::AutoStartObject(QObject *parent)
    : CuteimeAbstractPluginObject(parent)
{
    d = new Private(this);
}

AutoStartObject::~AutoStartObject()
{
    delete d;
}

#include "autostartobject.moc"
