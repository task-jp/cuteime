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

#include "engine.h"
#include "libanthy.h"
#include "dictionary.h"
#include "settings.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimecandidatemanager.h>
#include <cuteimedynamictranslator.h>

#include <QIcon>
#include <QSettings>
#include <QTextCodec>

namespace Japanese {
    namespace Anthy {

class Engine::Private : private QObject
{
    Q_OBJECT
public:
    Private(Engine *parent);
    ~Private();

#ifndef CUTEIME_NO_GUI
    CuteimeSettingsWidget *settings(const QString &hint, QWidget *parent);
#endif

private slots:
    void activeChanged(bool active);
    void stateChanged(uint state);
    void itemChanged(const CuteimePreeditItem &item);
    void currentIndexChanged(int currentIndex);

    void saved();

private:
    void predict();
    void convert();
    void resize();
    void setCandidates();
    void learn();

    void clearCandidates();

private:
    Engine *q;

    CuteimeInputMethodManager *inputMethodManager;
    CuteimePreeditManager *preeditManager;
    CuteimeCandidateManager *candidateManager;

    QTextCodec *eucjp;
    LibAnthy *libAnthy;
    LibAnthy::anthy_context_t context;

    State currentState;
    int preeditIndex;
    CuteimeConversionItemList predictionItems;
    CuteimeConversionItemList conversionItems;

    bool prediction;
    bool predictOnEmpty;
};

    }
}

using namespace Japanese::Anthy;

Engine::Private::Private(Engine *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , libAnthy(0)
    , preeditIndex(-1)
{
    cuteimeDebugIn() << parent;

    q->setIdentifier(QLatin1String("anthy"));
    q->setPriority(0x40);

    q->setLocale(QLatin1String("ja_JP"));
    q->setLanguage("Japanese(Standard)");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Conversion Engine"), q, "categoryName");

#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/icon/anthy.png"));
#endif
    trConnect(this, QT_TR_NOOP("Anthy"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
    eucjp = QTextCodec::codecForName("EUC-JP");
    Q_ASSERT(eucjp);

    saved();

    cuteimeDebugOut();
}

Engine::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Engine::Private::activeChanged(bool active)
{
    cuteimeDebugIn() << active;
    if (active) {
        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));

        }
        if (!libAnthy) {
            libAnthy = new LibAnthy;
            libAnthy->anthy_init();
            context = libAnthy->anthy_create_context();
        }
        stateChanged(inputMethodManager->state());
    } else {
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
        if (libAnthy) {
            libAnthy->anthy_release_context(context);
            libAnthy->anthy_quit();
            delete libAnthy;
            libAnthy = 0;
        }
        stateChanged(Direct);
    }
    cuteimeDebugOut();
}

void Engine::Private::stateChanged(uint state)
{
    cuteimeDebugIn() << state;
    currentState = (State)state;
    if (state == Direct) {
        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }
        if (candidateManager) {
            disconnect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            candidateManager->deleteLater();
            candidateManager = 0;
        }
    } else {
        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
        }
        if (!candidateManager) {
            candidateManager = new CuteimeCandidateManager(this);
            candidateManager->init();
            connect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        }
    }

    switch (state) {
    case Direct:
        preeditIndex = -1;
        predictionItems.clear();
        conversionItems.clear();
        clearCandidates();
        break;
    case Empty:
        learn();
        Q_FALLTHROUGH;
    case Input:
        clearCandidates();
        predict();
        preeditIndex = -1;
        conversionItems.clear();
        break;
    case Convert:
        predictionItems.clear();
        clearCandidates();
        convert();
        break;
    case Select:
        if (!conversionItems.isEmpty()) {
            clearCandidates();
            setCandidates();
        }
        break;
    }

    cuteimeDebugOut();
}

void Engine::Private::itemChanged(const CuteimePreeditItem &item)
{
    if (item.to.contains(QString())) return;

    cuteimeDebugIn() << item;
    switch (currentState) {
    case Empty:
        if (!predictOnEmpty) {
            break;
        }
        Q_FALLTHROUGH;
    case Input:
        if (!prediction) {
            break;
        }
//        cuteimeWarning() << "PREDICT:" << items;
        predict();
        break;
    case Convert: {
        int currentIndex = -1;
        int pos = 0;
        for (int i = 0; i < item.to.length(); i++) {
            if (pos == item.cursor) {
                currentIndex = i;
                break;
            }
            pos += item.to.at(i).length();
        }
        cuteimeDebug() << currentIndex << preeditIndex;
        if (currentIndex == preeditIndex) {
            resize();
        } else {
            preeditIndex = currentIndex;
//            move();
        }
        break;
    }
    case Select:
        // nothing to do ?
        break;
    default:
        break;
    }

    cuteimeDebugOut();
}

void Engine::Private::currentIndexChanged(int currentIndex)
{
    if (currentIndex == -1) return;
    if (conversionItems.isEmpty()) return;
    cuteimeDebugIn() << currentIndex;

    CuteimePreeditItem item = preeditManager->item();
    int index = -1;
    int pos = 0;
    for (int i = 0; i < item.to.length(); i++) {
        if (pos == item.cursor) {
            index = i;
            break;
        }
        pos += item.to.at(i).length();
    }

    QString to = candidateManager->items().at(currentIndex).to;

    CuteimeConversionItem convertionItem = conversionItems.at(index);
    convertionItem.to = to;
    if (candidateManager->items().at(currentIndex).source == q->identifier()) {
        convertionItem.index = candidateManager->items().at(currentIndex).index;
    } else {
        convertionItem.index = -1;
    }
    conversionItems.replace(index, convertionItem);

    cuteimeDebugOut();
}

void Engine::Private::predict()
{
    if (!prediction) return;
    CuteimePreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    if (!predictOnEmpty && preeditString.isEmpty()) return;
    cuteimeDebugIn();

    predictionItems = candidateManager->items();
    foreach (const CuteimeConversionItem &item, predictionItems) {
        if (item.source == q->identifier()) {
            predictionItems.removeOne(item);
        }
    }

    int ret = libAnthy->anthy_set_prediction_string(context, eucjp->fromUnicode(preeditString).data());
    Q_ASSERT(ret >= 0);
    LibAnthy::anthy_prediction_stat stat;
    ret = libAnthy->anthy_get_prediction_stat(context, &stat);
    Q_ASSERT(ret >= 0);
    for (int i = 0; i < stat.nr_prediction; i++) {
        char buf[8192];
        int len = libAnthy->anthy_get_prediction(context, i, buf, sizeof(buf));
        Q_ASSERT(len > 0);

        CuteimeConversionItem convertionItem;
        convertionItem.to = eucjp->toUnicode(buf, len);
        convertionItem.from = preeditString;
        convertionItem.index = i;
        convertionItem.source = q->identifier();
        predictionItems.append(convertionItem);
    }

    cuteimeDebug() << predictionItems;
    candidateManager->setItems(predictionItems);
    cuteimeDebugOut();
}

void Engine::Private::convert()
{
    if (!conversionItems.isEmpty()) return;
    cuteimeDebugIn();
    CuteimePreeditItem item = preeditManager->item();
    QStringList from = item.from;
    QString preeditString = from.join("");

    cuteimeDebug() << from << preeditString;

    int ret = libAnthy->anthy_set_string(context, eucjp->fromUnicode(preeditString).data());
    Q_ASSERT(ret >= 0);
    LibAnthy::anthy_conv_stat stat;
    ret = libAnthy->anthy_get_stat(context, &stat);
    Q_ASSERT(ret >= 0);
    conversionItems.clear();
    for (int i = 0; i < stat.nr_segment; i++) {
        CuteimeConversionItem conversionItem;
        conversionItem.index = 0;
        char buf[8192];
        int len = libAnthy->anthy_get_segment(context, i, 0, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        conversionItem.to = eucjp->toUnicode(buf, len);
        len = libAnthy->anthy_get_segment(context, i, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        conversionItem.from = eucjp->toUnicode(buf, len);
        cuteimeDebug() << conversionItem;
        conversionItems.append(conversionItem);
    }

    item.to.clear();
    item.from.clear();
    item.rawString.clear();
    item.cursor = 0;
    foreach (const CuteimeConversionItem &conversionItem, conversionItems) {
        cuteimeDebug() << conversionItem;
        if (item.to.isEmpty()) {
            item.selection = conversionItem.to.length();
        }
        item.to.append(conversionItem.to);
        QString f = conversionItem.from;
        while (!f.isEmpty()) {
            cuteimeDebug() << f << from.first() << item;
            if (f.startsWith(from.first())) {
                f = f.mid(from.first().length());
                int l = item.to.length();
                if (item.from.length() < l) {
                    item.from.append(from.takeFirst());
                    item.rawString.append(QString());
                } else {
                    item.from.replace(l - 1, item.from.at(l - 1) + from.takeFirst());
                }
            } else {
                QString f = from.first();
                QString ch = f.left(1);
                cuteimeDebug() << f << ch;
                if (f.startsWith(ch)) {
                    f = f.mid(1);
                    item.from.append(ch);
                    item.rawString.append(QString());
                }
                from.replace(0, f.mid(1));
            }
        }
    }
    cuteimeDebug() << item;

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);
    preeditIndex = 0;

    cuteimeDebugOut();
}

void Engine::Private::resize()
{
    CuteimePreeditItem item = preeditManager->item();
    if (item.to.isEmpty()) return;
    cuteimeDebugIn();
    QStringList from = item.from;

    cuteimeDebug() << item << preeditIndex;
    int delta = item.from.at(preeditIndex).length() - conversionItems.at(preeditIndex).from.length();
    cuteimeDebug() << delta;
    if (delta == 0) {
        cuteimeDebugOut();
        return;
    }
    libAnthy->anthy_resize_segment(context, preeditIndex, delta);
    LibAnthy::anthy_conv_stat stat;
    int ret = libAnthy->anthy_get_stat(context, &stat);
    Q_ASSERT(ret >= 0);

    while (conversionItems.length() > preeditIndex) {
        conversionItems.removeLast();
    }

    for (int i = preeditIndex; i < stat.nr_segment; i++) {
        CuteimeConversionItem convertionItem;
        convertionItem.index = 0;
        char buf[8192];
        int len = libAnthy->anthy_get_segment(context, i, 0, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        convertionItem.to = eucjp->toUnicode(buf, len);
        len = libAnthy->anthy_get_segment(context, i, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        convertionItem.from = eucjp->toUnicode(buf, len);
        conversionItems.append(convertionItem);
    }

    item.to.clear();
    item.from.clear();
    item.rawString.clear();
    foreach (const CuteimeConversionItem &conversionItem, conversionItems) {
        if (item.to.length() == preeditIndex) {
            item.cursor = item.to.join("").length();
            item.selection = conversionItem.to.length();
        }
        item.to.append(conversionItem.to);
        item.from.append(QString());
        item.rawString.append(QString());
        QString f = conversionItem.from;
        cuteimeDebug() << f << from;
        while (!f.isEmpty()) {
            if (f.startsWith(from.first())) {
                f= f.mid(from.first().length());
                item.from[item.from.length() - 1] = item.from.last() + from.takeFirst();
            } else {
                break;
            }
        }
    }

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);

    cuteimeDebugOut();
}

void Engine::Private::setCandidates()
{
    cuteimeDebugIn();

    CuteimePreeditItem item = preeditManager->item();
    int currentIndex = -1;
    int pos = 0;
    for (int i = 0; i < item.to.length(); i++) {
        if (pos == item.cursor) {
            currentIndex = i;
            break;
        }
        pos += item.to.at(i).length();
    }

    CuteimeConversionItemList candidateItemList = candidateManager->items();

    LibAnthy::anthy_segment_stat stat;
    int ret = libAnthy->anthy_get_segment_stat(context, currentIndex, &stat);
    Q_ASSERT(ret >= 0);

    for (int i = 0; i < stat.nr_candidate; i++) {
        CuteimeConversionItem convertionItem;
        convertionItem.index = i;
        char buf[8192];
        int len = libAnthy->anthy_get_segment(context, currentIndex, i, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        convertionItem.to = eucjp->toUnicode(buf, len);
        convertionItem.from = conversionItems[currentIndex].from;
        convertionItem.source = q->identifier();
        candidateItemList.append(convertionItem);
    }

    candidateManager->setItems(candidateItemList);
    cuteimeDebugOut();
}

void Engine::Private::learn()
{
    cuteimeDebugIn() << conversionItems << predictionItems;
    if (!conversionItems.isEmpty()) {
        for (int i = 0; i < conversionItems.count(); i++) {
            if (conversionItems[i].index < 0) continue;
            libAnthy->anthy_commit_segment(context, i, conversionItems[i].index);
        }
        conversionItems.clear();
    } else if (!predictionItems.isEmpty()) {
        for (int i = 0; i < conversionItems.count(); i++) {
            if (conversionItems[i].index < 0) continue;
            libAnthy->anthy_commit_prediction(context, predictionItems[i].index);
        }
        predictionItems.clear();
    }
    cuteimeDebugOut();
}

void Engine::Private::clearCandidates()
{
    if (!candidateManager) return;
    cuteimeDebugIn();

    CuteimeConversionItemList candidateItemList = candidateManager->items();
    foreach (CuteimeConversionItem candidateItem, candidateItemList) {
        if (candidateItem.source == q->identifier()) {
            candidateItemList.removeOne(candidateItem);
        }
    }
    candidateManager->setItems(candidateItemList);

    cuteimeDebugOut();
}

#ifndef CUTEIME_NO_GUI
CuteimeSettingsWidget *Engine::Private::settings(const QString &hint, QWidget *parent)
{
    cuteimeDebugIn() << hint << parent;
    Q_UNUSED(hint);
    Settings *settings = new Settings(q, parent);
    connect(settings, SIGNAL(saved()), this, SLOT(saved()));
    cuteimeDebugOut() << settings;
    return settings;
}
#endif

void Engine::Private::saved()
{
    cuteimeDebugIn();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    prediction = settings.value("Prediction", false).toBool();
    predictOnEmpty = settings.value("Empty", false).toBool();
    cuteimeDebugOut();
}

Engine::Engine(QObject *parent)
    : CuteimeEngine(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Engine::~Engine()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}


CuteimeEngineDictionary *Engine::dictionary(QObject *parent)
{
    return new Dictionary(parent);
}

#ifndef CUTEIME_NO_GUI
CuteimeSettingsWidget *Engine::settings(const QString &hint, QWidget *parent)
{
    return d->settings(hint, parent);
}
#endif

#include "engine.moc"
