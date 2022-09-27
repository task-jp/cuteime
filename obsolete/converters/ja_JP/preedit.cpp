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

#include "preedit.h"

#include <cuteimeapplicationmanager.h>
#include <cuteimeconverter.h>
#include <cuteimecandidates.h>
#include <cuteimedebug.h>

using namespace ja_JP;

class Preedit::Private : private QObject
{
    Q_OBJECT
public:
    Private(Preedit *parent);
    ~Private();
    bool init();

    void setPreeditString();
    void cancel();
    void setCurrentText(const QString &text);

private slots:
    void currentCandidateChanged(int currentIndex);

private:
    Preedit *q;
public:
    CuteimeApplicationManager manager;
private:
    CuteimeCandidates candidates;

public:
    int conversionIndex;
    QList< CuteimeConversionItemList > conversionsMap;
};

Preedit::Private::Private(Preedit *parent)
    : QObject(parent)
    , q(parent)
    , conversionIndex(-1)
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

Preedit::Private::~Private()
{
}

bool Preedit::Private::init()
{
    cuteimeDebugIn();
    bool ret = q->CuteimePreedit::init();
    if (ret) {
        manager.init();
        candidates.init();
        connect(&candidates, SIGNAL(currentIndexChanged(int)), this, SLOT(currentCandidateChanged(int)));
    }
    cuteimeDebugOut() << ret;
    return ret;
}

void Preedit::Private::cancel()
{
    cuteimeDebugIn();
    CuteimePreeditItem item;
    item.cursor = -1;
    int i = 0;
    foreach(const CuteimeConversionItemList &conversion, conversionsMap) {
        item.to.append(conversion.first().from);
        if (i++ == conversionIndex) {
            item.cursor = item.to.length();
        }
    }
    if (item.cursor < 0) {
        item.cursor = item.to.length();
    }
    item.selection = 0;
#ifdef CUTEIME_PLATFORM_MAEMO
    item.underline = QTextCharFormat::SingleUnderline;
#else
    item.underline = QTextCharFormat::WaveUnderline;
#endif
    q->setItems(CuteimePreeditItemList() << item);
    q->setConversions(CuteimeConversionItemList());
    cuteimeDebugOut();
}

Preedit *Preedit::instance()
{
    static Preedit *ret = 0;
    if (!ret)
        ret = new Preedit;
    return ret;
}

Preedit::Preedit(QObject *parent)
    : CuteimePreedit(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Preedit::~Preedit()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool Preedit::init()
{
    cuteimeDebugIn();
    bool ret = d->init();
    cuteimeDebugOut() << ret;
    return ret;
}

void Preedit::move(int offset, bool selection)
{
    cuteimeDebugIn() << offset << selection;
    if (d->conversionIndex < 0) {
        CuteimePreeditItem item = items().first();
        int pos = (item.cursor + offset + item.to.length() + 1) % (item.to.length() + 1);
        if (pos == item.cursor) {
            if (!selection) {
                item.selection = 0;
            }
        } else {
            item.cursor = pos;
            if (selection) {
                item.selection -= offset;
            } else {
                item.selection = 0;
            }
        }
        setItems(CuteimePreeditItemList() << item);
    } else {
        int conversionIndex = d->conversionIndex + offset;
        conversionIndex = (conversionIndex + d->conversionsMap.count()) % (d->conversionsMap.count());
        if (conversionIndex !=  d->conversionIndex) {
            d->conversionIndex = conversionIndex;
            d->setPreeditString();
        }
    }
    cuteimeDebugOut();
}

int Preedit::conversionIndex() const
{
    return d->conversionIndex;
}

int Preedit::convercionCount() const
{
    return d->conversionsMap.count();
}

void Preedit::setConversions(const CuteimeConversionItemList &conversions)
{
    CuteimeConverter *converter = qobject_cast<CuteimeConverter*>(parent());
    if (!converter || !converter->isActive()) return;
    cuteimeDebugIn() << conversions;
    if (conversions.isEmpty()) {
        d->conversionIndex = -1;
        d->conversionsMap.clear();
    } else if (d->conversionIndex < 0) {
        d->conversionIndex = 0;
        for (int i = 0; i < conversions.count(); i++) {
            CuteimeConversionItemList qce;
            qce.append(conversions[i]);
            d->conversionsMap.append(qce);
        }
        d->setPreeditString();
    } else {
        for (int i = 0; i < conversions.count(); i++) {
            if (i > d->conversionsMap.count() - 1) {
                CuteimeConversionItemList qce;
                qce.append(conversions[i]);
                d->conversionsMap.append(qce);
            } else {
                Q_ASSERT(!d->conversionsMap[i].isEmpty());
                if (d->conversionsMap[i].first() == conversions[i]) {
                } else {
                    CuteimeConversionItemList qce;
                    qce.append(conversions[i]);
                    d->conversionsMap[i] = qce;
                }
            }
        }
        while (conversions.count() < d->conversionsMap.count()) {
            d->conversionsMap.takeLast();
        }
        d->setPreeditString();
    }
    cuteimeDebugOut();
}

void Preedit::Private::setPreeditString()
{
    cuteimeDebugIn();
    cuteimeDebug() << conversionIndex;
    CuteimeConversionItemList c = candidates.candidates();
    CuteimePreeditItemList items;
    for (int i = 0; i < conversionsMap.count(); i++) {
        CuteimePreeditItem item;
        item.to = conversionsMap[i].last().to;
        item.cursor = -1;
        item.selection = 0;
        item.underline = QTextCharFormat::SingleUnderline;
        if (conversionIndex < 0 && !c.isEmpty()) {
            item.cursor = 0;
            item.selection = item.to.length();
            item.underline = QTextCharFormat::NoUnderline;
        } else if (i == conversionIndex) {
            item.cursor = 0;
            item.selection = item.to.length();
            item.underline = QTextCharFormat::NoUnderline;
        }
        items.append(item);
    }
    cuteimeDebug() << items;
    q->setItems(items);
    cuteimeDebugOut();
}

void Preedit::Private::currentCandidateChanged(int currentIndex)
{
    CuteimeConverter *converter = qobject_cast<CuteimeConverter*>(q->parent());
    if (!converter || !converter->isActive()) return;
    if (currentIndex < 0) return;
    cuteimeDebugIn() << currentIndex;
    if (conversionIndex < 0) {
        CuteimeConversionItemList list;
        list.append(candidates.candidates()[currentIndex]);
        switch (conversionsMap.count()) {
        case 0:
            conversionsMap.append(list);
            break;
        case 1:
            conversionsMap[0] = list;
            break;
        default:
            cuteimeWarning() << currentIndex << "is not handled here.";
            break;
        }
        setPreeditString();
    } else {
        switch (conversionsMap[conversionIndex].count()) {
        case 1:
            conversionsMap[conversionIndex].append(candidates.candidates().at(currentIndex));
            break;
        case 2:
            conversionsMap[conversionIndex][1] = candidates.candidates().at(currentIndex);
            break;
        default:
            cuteimeWarning();
        }
        setPreeditString();
    }
    cuteimeDebugOut();
}

void Preedit::setCurrentText(const QString &text)
{
    d->setCurrentText(text);
}

void Preedit::Private::setCurrentText(const QString &text)
{
    cuteimeDebugIn() << text;
    CuteimeConversionItem qce;
    Q_ASSERT(!conversionsMap[conversionIndex].isEmpty());
    qce.from = conversionsMap[conversionIndex].first().from;
    qce.to = text;
    switch (conversionsMap[conversionIndex].count()) {
    case 1:
        conversionsMap[conversionIndex].append(qce);
        break;
    case 2:
        conversionsMap[conversionIndex][1] = qce;
        break;
    default:
        cuteimeWarning();
    }
    setPreeditString();
    cuteimeDebugOut();
}

void Preedit::commit()
{
    cuteimeDebugIn();
    QString commitString;
    foreach (const CuteimePreeditItem &item, items())
        commitString += item.to;
    CuteimePreedit::commit(commitString, d->manager.widget());
    CuteimeConversionItemList qce;
    for (int i = 0; i < d->conversionsMap.count(); i++) {
        qce.append(d->conversionsMap.last().last());
    }
    emit committed(qce);
    d->conversionsMap.clear();
    d->conversionIndex = -1;
    cuteimeDebugOut();
}

void Preedit::cancel()
{
    d->cancel();
}

#include "preedit.moc"
