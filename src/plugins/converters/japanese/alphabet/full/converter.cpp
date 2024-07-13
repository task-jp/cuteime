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

#include "converter.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimedynamictranslator.h>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>

namespace Japanese {
    namespace Alphabet {
        namespace Full {

class Converter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Converter *parent);
    ~Private();

private slots:
    void init();

    void activeChanged(bool isActive);
    void stateChanged(uint state);

    void itemChanged(const CuteimePreeditItem &item);

private:
    void readMap(const QString &fileName);

private:
    Converter *q;
    CuteimeInputMethodManager *inputMethodManager;
    CuteimePreeditManager *preeditManager;

    QStringList previous;

    QMap<QString, QString> convertMap;
};

        }
    }
}

using namespace Japanese::Alphabet::Full;

Converter::Private::Private(Converter *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
{
    cuteimeDebugIn() << parent;
    init();
    cuteimeDebugOut();
}

Converter::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Converter::Private::init()
{
    cuteimeDebugIn();
    q->setIdentifier(QLatin1String("Fullwidth Alphabet"));
    q->setPriority(0x30);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese(Standard)");

#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/japanese/alphabet/full/resources/alphabet-full.png"));
#endif
    trConnect(this, QT_TR_NOOP("Alphabet(Full)"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese converter for Alphabet"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Converter"), q, "categoryName");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
    cuteimeDebugOut();
}

void Converter::Private::readMap(const QString &fileName)
{
    cuteimeDebugIn() << fileName;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        convertMap.clear();
        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains('\t')) {
                QStringList fields = line.split('\t');
                if (fields.isEmpty() || fields.first().isEmpty() || fields.first().startsWith("#")) continue;
                cuteimeDebug() << fields.at(0) << fields.at(1);
                convertMap[fields.at(0)] = fields.at(1);
            }
        }
        file.close();
    } else {
        cuteimeWarning() << file.error() << file.errorString() << fileName;
    }
    cuteimeDebugOut();
}

void Converter::Private::activeChanged(bool isActive)
{
    cuteimeDebugIn() << isActive;
    if (isActive) {
        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
        }
        if (convertMap.isEmpty()) {
            readMap(QLatin1String(":/japanese/alphabet/full/resources/alphabet-full.txt"));
        }
        stateChanged(inputMethodManager->state());
    } else {
        convertMap.clear();
        stateChanged(Reset);
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
    }
    cuteimeDebugOut();
}

void Converter::Private::stateChanged(uint state)
{
    cuteimeDebugIn() << state;
    switch (state) {
    case Input:
        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
        }
        itemChanged(preeditManager->item());
        break;
    case ConvertTo: {
        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
        }

        CuteimePreeditItem item = preeditManager->item();
        item.to.clear();
        foreach (const QString &rawString, item.rawString) {
            QString to;
            foreach (const QChar &ch, rawString) {
                if (convertMap.contains(ch)) {
                    to.append(convertMap[ch]);
                } else {
                    to.append(ch);
                }
            }
            item.to.append(to);
        }
        preeditManager->setItem(item);
        Q_FALLTHROUGH;
    }
    default:
        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }
        break;
    }
    cuteimeDebugOut();
}

void Converter::Private::itemChanged(const CuteimePreeditItem &item)
{
    if (item.selection != 0) return;
    if (item.from == previous) return;

    cuteimeDebugIn() << item;

    QStringList to = item.to;
    int cursor = item.cursor;

    int l = 0;
    int i = 0;
    for (i = 0; i < to.length(); i++) {
        l += to.at(i).length();
        if (l > cursor - 1) {
            break;
        }
    }

    cuteimeDebug() << i;
    int modified = item.modified;
    int k = i;
    for (int j = 0; j < modified; j++) {
        QString str = item.rawString.at(k);
        int index = str.length() - j - 1 - k + i;
        str = str.mid(index, 1);
        cuteimeDebug() << str << index;
        if (convertMap.contains(str)) {
            to.replace(k, to[k].replace(index, 1, convertMap[str]));
        }
        if (index == 0) k--;
    }

    CuteimePreeditItem newItem = item;
    newItem.to = to;
    newItem.cursor = cursor;
    preeditManager->blockSignals(true);
    preeditManager->setItem(newItem);
    preeditManager->blockSignals(false);
    previous = item.from;
    cuteimeDebugOut();
}

Converter::Converter(QObject *parent)
    : CuteimeConverter(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Converter::~Converter()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "converter.moc"
