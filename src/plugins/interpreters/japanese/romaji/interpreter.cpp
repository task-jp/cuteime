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

#include "interpreter.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimekeymanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimedynamictranslator.h>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>

namespace Japanese {
    namespace Romaji {

class Interpreter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Interpreter *parent);
    ~Private();

private slots:
    void init();
    void activeChanged(bool active);
    void stateChanged(uint state);

    void itemChanged(const CuteimePreeditItem &item);

private:
    void readMap(const QString &fileName);

private:
    Interpreter *q;

    CuteimeInputMethodManager *inputMethodManager;
    CuteimePreeditManager *preeditManager;

    QMap<QString, QString> convertMap;
    QMap<QString, QString> convertMapReversed;
    int convertMaxLength;

    QStringList previous;
};

    }
}

using namespace Japanese::Romaji;

Interpreter::Private::Private(Interpreter *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , convertMaxLength(0)
{
    cuteimeDebugIn() << parent;
    init();
    cuteimeDebugOut();
}

Interpreter::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Interpreter::Private::init()
{
    q->setIdentifier(QLatin1String("Japanese(Romaji)"));
    q->setPriority(0x01);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese(Standard)");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/japanese/romaji/resources/romaji.png"));
#endif
    trConnect(this, QT_TR_NOOP("Romaji"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese interpreter for Romaji input"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Interpreter"), q, "categoryName");

    inputMethodManager = new CuteimeInputMethodManager(this);
    inputMethodManager->init();
    connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    q->setActive(inputMethodManager->converter() == q->identifier());
    activeChanged(q->isActive());
}

void Interpreter::Private::readMap(const QString &fileName)
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
                if (fields.length() > 2) {
                    convertMap[fields.at(0)] += QString("\t%1").arg(fields.at(2));
                }
                convertMapReversed[fields.at(1)] = fields.at(0);
                convertMaxLength = qMax(convertMaxLength, fields.at(0).length());
            }
        }
        file.close();
    } else {
        cuteimeWarning() << file.error() << file.errorString() << fileName;
    }
    cuteimeDebugOut();
}

void Interpreter::Private::activeChanged(bool active)
{
    cuteimeDebugIn() << active;
    if (active) {
        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
        }
        if (convertMap.isEmpty()) {
            readMap(QLatin1String(":/japanese/romaji/resources/table.txt"));
        }
        stateChanged(inputMethodManager->state());

    } else if(inputMethodManager) {
        stateChanged(Reset);
        convertMap.clear();
        convertMapReversed.clear();
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
    }
    cuteimeDebugOut();
}

void Interpreter::Private::stateChanged(uint state)
{
    cuteimeDebugIn() << state;
    switch (state) {
    case Empty:
    case Input:
    case Convert:
        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
        }
        previous.clear();
        itemChanged(preeditManager->item());
        break;
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

void Interpreter::Private::itemChanged(const CuteimePreeditItem &item)
{
    if (item.to.isEmpty()) return;
    if (item.rawString == previous) return;
    if (item.selection != 0) return;

    cuteimeDebugIn() << item;

    QStringList to = item.to;
    QStringList from = item.from;
    QStringList rawString = item.rawString;
    int cursor = item.cursor;
    int modified = item.modified;

    // determine string to check
    QString t;
    QString f;
    QString rS;
    int currentIndex = -1;
    int pos = 0;
    for (int i = 0; i < to.join("").length(); i++) {
        pos += to.at(i).length();
        if (pos == cursor) {
            if (to.at(i) == from.at(i) && from.at(i) == rawString.at(i)) {
                currentIndex = i;
                t += to.at(i);
                f += from.at(i);
                rS += rawString.at(i);
            }
            break;
        } else if (pos > cursor) {
            if (to.at(i) == from.at(i) && from.at(i) == rawString.at(i)) {
                currentIndex = i;
                t += to.at(i).left(cursor - pos);
                f += from.at(i).left(cursor - pos);
                rS += rawString.at(i).left(cursor - pos);
            }
            break;
        } else {
            if (to.at(i) == from.at(i) && from.at(i) == rawString.at(i)) {
                t += to.at(i);
                f += from.at(i);
                rS += rawString.at(i);
            } else {
                t.clear();
                f.clear();
                rS.clear();
            }
        }
    }

    cuteimeDebug() << currentIndex << cursor << t << f << rS;

    if (currentIndex == -1) {
        // nothing to do
    } else {
        bool terminate = to.contains(QString());
        cuteimeDebug() << terminate;

        // check from the longest string
        for (int i = rS.length(); i > 0; i--) {
            cuteimeDebug() << i << rS.right(i);
            QString target = rS.right(i);
            if (terminate) {
                target.append(QLatin1String("\\0"));
            }
            if (convertMap.contains(target.toLower())) {

                // remove the part replaced
                int k = 0;
                for (int j = 0; j < i;) {
                    cuteimeDebug() << currentIndex << currentIndex - k;
                    cuteimeDebug() << item.to.at(currentIndex - k);
                    int l = to.at(currentIndex - k).length();
                    to.removeAt(currentIndex - k);
                    from.removeAt(currentIndex - k);
                    rawString.removeAt(currentIndex - k);
                    j += l;
                    cursor--;
                    k++;
                }
                cuteimeDebug() << rS << to << from << rawString;
                rS = rS.right(i);

                // insert the replacement
                QString conv = convertMap[target.toLower()];
                modified = conv.length();
                cuteimeDebug() << conv << cursor;

                if (conv.length() == 2 && (conv.at(0) == rS.at(0) || conv.at(1) == rS.at(1) || conv.at(0) == QChar(0x3093))) {
                    cuteimeDebug() << conv << rS;
                    QString first = conv.left(1);
                    to.insert(cursor, first);
                    from.insert(cursor, first);
                    rawString.insert(cursor, rS.left(1));
                    conv = conv.mid(1);
                    rS = rS.mid(1);
                    cursor++;
                }

                bool found = false;
                k = 0;
                for (int j = 0; j < to.length(); j++) {
                    cuteimeDebug() << k << to.at(j) << j << cursor;
                    if (k == cursor) {
                        to.insert(j, conv);
                        from.insert(j, conv);
                        rawString.insert(j, rS);
                        found = true;
                        break;
                    }
                    k += to.at(j).length();
                }
                if (!found) {
                    to.append(conv);
                    from.append(conv);
                    rawString.append(rS);
                }
                cursor = cursor + conv.length();
                cuteimeDebug() << to << from << rawString << cursor;
                break;
            }
        }
    }
    // determine rawString missing
    for (int i = 0; i < from.length(); i++) {
        if (rawString.at(i).isNull()) {
//            cuteimeWarning() << item.from.at(i);
            if (convertMapReversed.contains(from.at(i))) {
                rawString.replace(i, convertMapReversed[from.at(i)]);
            }
        }
    }

    CuteimePreeditItem newItem;
    newItem.to = to;
    newItem.from = from;
    newItem.rawString = rawString;
    newItem.cursor = cursor;
    newItem.selection = 0;
    newItem.modified = modified;
    preeditManager->blockSignals(true);
    preeditManager->setItem(newItem);
    preeditManager->blockSignals(false);

    previous = rawString;
    cuteimeDebugOut();
}

Interpreter::Interpreter(QObject *parent)
    : CuteimeInterpreter(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Interpreter::~Interpreter()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "interpreter.moc"
