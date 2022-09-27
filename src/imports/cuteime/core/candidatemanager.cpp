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

#include "candidatemanager.h"

#include <cuteimedebug.h>
#include <cuteimecandidatemanager.h>
#include <QtDeclarative/qdeclarative.h>

class CandidateManager::Private : public CuteimeCandidateManager
{
    Q_OBJECT
public:
    Private(CandidateManager *parent);

private slots:
    void slotItemsChanged(const CuteimeConversionItemList &items);

private:
    CandidateManager *q;

public:
    QList<QObject*> m_items;
};

CandidateManager::Private::Private(CandidateManager *parent)
    : CuteimeCandidateManager(parent)
    , q(parent)
{
    cuteimeDebugIn() << parent;
    init();
    connect(this, SIGNAL(itemsChanged(CuteimeConversionItemList)), this, SLOT(slotItemsChanged(CuteimeConversionItemList)));
    connect(this, SIGNAL(currentIndexChanged(int)), q, SIGNAL(currentIndexChanged(int)));
    cuteimeDebugOut();
}

void CandidateManager::Private::slotItemsChanged(const CuteimeConversionItemList &items)
{
    cuteimeDebugIn() << items;
    qDeleteAll(m_items);
    m_items.clear();
    foreach (const CuteimeConversionItem &item, items) {
        m_items.append(new ConversionItem(item, this));
    }
    cuteimeDebug() << m_items.length();
    emit q->itemsChanged(m_items);
    cuteimeDebug() << m_items.length();
    cuteimeDebugOut();
}

CandidateManager::CandidateManager(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

const QList<QObject*> &CandidateManager::items() const
{
    return d->m_items;
}

void CandidateManager::setItems(const QList<QObject*> &items)
{
    CuteimeConversionItemList list;
    foreach (const QObject *object, items) {
        list.append(qobject_cast<const ConversionItem*>(object)->item());
    }

    d->setItems(list);
}

int CandidateManager::currentIndex()
{
    return d->currentIndex();
}

void CandidateManager::setCurrentIndex(int currentIndex)
{
    d->setCurrentIndex(currentIndex);
}

QML_DECLARE_TYPE(CandidateManager)

#include "candidatemanager.moc"
