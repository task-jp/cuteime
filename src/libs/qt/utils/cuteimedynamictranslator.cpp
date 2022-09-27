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

#include "cuteimedynamictranslator.h"
#include "cuteimeapplicationmanager.h"
#include "cuteimedebug.h"

#include <QHash>
#include <QPair>
#include <QCoreApplication>

class CuteimeDynamicTranslator::Private : private QObject
{
    Q_OBJECT
public:
    Private(CuteimeDynamicTranslator *parent);
    void setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName);

private slots:
    void reset();
    void objectDestroyed();

private:
    CuteimeDynamicTranslator *q;
    CuteimeApplicationManager applicationManager;

    struct CuteimeDynamicTranslatorData {
        QObject *owner;
        const char *source;
        QObject *target;
        const char *propertyName;
    };

    QList<CuteimeDynamicTranslatorData> data;
};

CuteimeDynamicTranslator::Private::Private(CuteimeDynamicTranslator *parent)
    : QObject(parent)
    , q(parent)
{
    applicationManager.init();
    connect(&applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(reset()), Qt::QueuedConnection);
}

void CuteimeDynamicTranslator::Private::setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName)
{
    CuteimeDynamicTranslatorData d;
    d.owner = owner;
    connect(owner, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
    d.source = source;
    d.target = target;
    connect(target, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
    d.propertyName = propertyName;
    data.append(d);
    target->setProperty(propertyName, QCoreApplication::translate(owner->metaObject()->className(), source));
}

void CuteimeDynamicTranslator::Private::reset()
{
    cuteimeDebugIn();

    foreach (const CuteimeDynamicTranslatorData &d, data) {
        d.target->setProperty(d.propertyName, QCoreApplication::translate(d.owner->metaObject()->className(), d.source));
    }

    cuteimeDebugOut();
}

void CuteimeDynamicTranslator::Private::objectDestroyed()
{

}

CuteimeDynamicTranslator::CuteimeDynamicTranslator(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

CuteimeDynamicTranslator &CuteimeDynamicTranslator::instance()
{
    static CuteimeDynamicTranslator ret;
    return ret;
}

CuteimeDynamicTranslator::~CuteimeDynamicTranslator()
{
    delete d;
}

void CuteimeDynamicTranslator::setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName)
{
    instance().d->setTranslation(owner, source, target, propertyName);
}

#include "cuteimedynamictranslator.moc"
