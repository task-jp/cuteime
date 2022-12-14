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

#include "object.h"

#include <cuteimedeclarativeview.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimedynamictranslator.h>

#include <QPointer>

using namespace Debug::StatusViewer;

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);
    void setWindowIcon(const QIcon &icon);

private:
    Object *q;

    QPointer<CuteimeDeclarativeView> statusViewer;
};

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
{
    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Debug tools"), q, "categoryName");

    trConnect(this, QT_TR_NOOP("Status viewer for cuteime"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)));
    enabledChanged(q->isEnabled());
}

Object::Private::~Private()
{
    delete statusViewer;
}

void Object::Private::enabledChanged(bool enabled)
{
    if (enabled) {
        statusViewer = new CuteimeDeclarativeView;
        statusViewer->setWindowTitle(tr("Status viewer for cuteime"));
        statusViewer->setSource(statusViewer->resolveUrl("Debug/StatusViewer/main.qml"));
        statusViewer->setAttribute(Qt::WA_DeleteOnClose);

        CuteimeApplicationManager *applicationManager = new CuteimeApplicationManager(statusViewer);
        applicationManager->init();
        connect(applicationManager, SIGNAL(currentIconChanged(QIcon)), this, SLOT(setWindowIcon(QIcon)));
        setWindowIcon(applicationManager->currentIcon());

        statusViewer->show();
    } else {
        statusViewer->deleteLater();
    }
}

void Object::Private::setWindowIcon(const QIcon &icon)
{
    statusViewer->setWindowIcon(icon);
}

Object::Object(QObject *parent)
    : CuteimeAbstractPluginObject(parent)
{
    d = new Private(this);
}

Object::~Object()
{
    delete d;
}

#include "object.moc"
