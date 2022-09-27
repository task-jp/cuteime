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
#include "settings.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimecandidatemanager.h>
#include <cuteimedynamictranslator.h>

#include <QCoreApplication>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QUrl>
#include <QUrlQuery>
#include <QSettings>

namespace Japanese {
    namespace SocialIME {

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
    void enabledChanged(bool enabled);
    void activeChanged(bool active);
    void stateChanged(uint state);
    void itemChanged(const CuteimePreeditItem &item);

    void clearCandidates();

    void error(QNetworkReply::NetworkError err);
    void sslErrors(QList<QSslError> errs);

    void saved();

    void predict();
    void readPrediction();

    void convert();
    void readConversion();

    void resize();
    void setCandidates();
    void learn();

    void cancel();

private:
    Engine *q;
    CuteimeInputMethodManager *inputMethodManager;
    CuteimePreeditManager *preeditManager;
    CuteimeCandidateManager *candidateManager;
    QNetworkAccessManager *networkManager;
    QNetworkReply *conversionReply;

    State currentState;
    int preeditIndex;
    CuteimeConversionItemList predictionItems;
    QList<CuteimeConversionItemList> candidateList;

    bool prediction;
    bool predictOnEmpty;

    QList<int> initialSizes;
    QString user;
};

    }
}

using namespace Japanese::SocialIME;

Engine::Private::Private(Engine *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , networkManager(0)
    , conversionReply(0)
    , prediction(false)
    , predictOnEmpty(false)
{
    cuteimeDebugIn() << parent;

    q->setIdentifier(QLatin1String("Social IME"));
    q->setPriority(0x80);

    q->setLocale(QLatin1String("ja_JP"));
    q->setLanguage("Japanese(Standard)");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/icons/socialime.png"));
#endif
    trConnect(this, QT_TR_NOOP("Social IME Engine"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("See <a href=\"http://www.social-ime.com/\">http://www.social-ime.com/</a> for detail."), q, "description");

    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Conversion Engine"), q, "categoryName");

    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    user = settings.value("User").toString();

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)));
    enabledChanged(q->isEnabled());

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());

    cuteimeDebugOut();
}

Engine::Private::~Private()
{
}

void Engine::Private::enabledChanged(bool enabled)
{
    cuteimeDebugIn() << enabled;
    if (enabled) {

    } else {
    }
    cuteimeDebugOut();
}

void Engine::Private::activeChanged(bool active)
{
    cuteimeDebugIn() << active;
    if (active) {
        if (!networkManager) {
            networkManager = new QNetworkAccessManager(this);
        }
        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));

        }
        stateChanged(inputMethodManager->state());
    } else {
        stateChanged(Direct);
        if (networkManager) {
            networkManager->deleteLater();
            networkManager = 0;
        }
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
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
        }
    }

    switch (state) {
    case Direct:
        preeditIndex = -1;
        predictionItems.clear();
        candidateList.clear();
        clearCandidates();
        break;
    case Empty:
        learn();
        fallthrough;
    case Input:
        clearCandidates();
        candidateList.clear();
        predict();
        preeditIndex = -1;
        break;
    case Convert:
        predictionItems.clear();
        clearCandidates();
        convert();
        break;
    case Select:
        if (!candidateList.isEmpty()) {
            clearCandidates();
            setCandidates();
        }
        break;
    }

    cuteimeDebugOut();
}

void Engine::Private::itemChanged(const CuteimePreeditItem &item)
{
    if (item.rawString.contains(QString())) return;

    cuteimeDebugIn() << item;
    switch (currentState) {
    case Empty:
        if (!predictOnEmpty) {
            break;
        }
        fallthrough;
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
            cancel();
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

void Engine::Private::predict()
{
    if (!prediction) return;
    CuteimePreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    if (!predictOnEmpty && preeditString.isEmpty()) return;
    cuteimeDebugIn();

    QString url = QString("http://www.social-ime.com/api2/predict.php?string=%1").arg(preeditString);
    url.append(QLatin1String("&charset=UTF8"));
    url.append(QString("&user=%1").arg(user));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "CUTEIME");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readPrediction()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>))
            , this, SLOT(sslErrors(QList<QSslError>)));

    cuteimeDebugOut();
}

void Engine::Private::readPrediction()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    CuteimePreeditItem item = preeditManager->item();
    QUrlQuery query(reply->url());
    QString preeditString = query.queryItemValue("string");
    if (preeditString != item.from.join("")) {
        return;
    }
    cuteimeDebugIn();

    QStringList result = QString::fromUtf8(reply->readAll()).split("\t");
    result.removeAll(QLatin1String("\n"));

    predictionItems = candidateManager->items();
    foreach (const CuteimeConversionItem &item, predictionItems) {
        if (item.source == q->identifier()) {
            predictionItems.removeOne(item);
        }
    }

    foreach(const QString &p, result) {
        CuteimeConversionItem convertionItem;
        convertionItem.index = -1;
        convertionItem.to = p;
        convertionItem.from = preeditString;
        convertionItem.source = q->identifier();
        predictionItems.append(convertionItem);
    }

    cuteimeDebug() << predictionItems;
    candidateManager->setItems(predictionItems);
    reply->deleteLater();
    cuteimeDebugOut();
}


void Engine::Private::convert()
{
    if (!candidateList.isEmpty()) return;
    cuteimeDebugIn();

    CuteimePreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    cuteimeDebug() << preeditString;

    QString url = QString("http://www.social-ime.com/api/?string=%1").arg(preeditString);
    url.append(QLatin1String("&charset=UTF8"));
    url.append(QString("&user=%1").arg(user));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "CUTEIME");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>))
            , this, SLOT(sslErrors(QList<QSslError>)));

    cancel();
    conversionReply = reply;
    initialSizes.clear();
    preeditIndex = 0;

    cuteimeDebugOut();
}

void Engine::Private::readConversion()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (conversionReply != reply) return;
    conversionReply = 0;
    if (reply->error() != QNetworkReply::NoError) return;
    CuteimePreeditItem item = preeditManager->item();
    QUrlQuery query(reply->url());
    if (query.queryItemValue("string") != item.from.join("")) {
        cuteimeDebug() << query.queryItemValue("string") << item.from;
        return;
    }

    cuteimeDebugIn();

    QStringList all = QString::fromUtf8(reply->readAll()).split("\n");
    cuteimeDebug() << all;
    cuteimeDebug() << item;
    cuteimeDebug() << preeditIndex;

    QStringList from = item.from;
    QStringList rawString = item.rawString;

    while (item.to.length() > preeditIndex) {
        cuteimeDebug() << item;
        item.to.removeLast();
        item.from.removeLast();
        item.rawString.removeLast();
    }

    cuteimeDebug() << from;
    foreach (QString f, item.from) {
        while (f.startsWith(from.first())) {
            f = f.mid(from.length());
            from.removeFirst();
            rawString.removeFirst();
        }
    }
    cuteimeDebug() << from;

    cuteimeDebug() << candidateList;
    while (candidateList.length() > preeditIndex) {
        candidateList.removeLast();
    }
    cuteimeDebug() << candidateList;

    for (int i = 0; i < preeditIndex; i++) {
        all.removeFirst();
    }
    cuteimeDebug() << all;
    cuteimeDebug() << item;
    item.cursor = item.to.join("").length();

    foreach(const QString &line, all) {
        QStringList data = line.split("\t");
        cuteimeDebug() << data;
        Q_ASSERT(!data.isEmpty());
        if (data.first().isEmpty()) continue;

        item.to.append(data.first());

        QString f;
        CuteimeConversionItemList cand;
        for (int i = 0; i < data.length(); i++) {
            if (data.at(i).isEmpty()) continue;
            CuteimeConversionItem c;
            c.to = data.at(i);
            c.index = i;
            c.source = q->identifier();
            cand.append(c);

            if (f.isNull() && !from.isEmpty() && from.join("").startsWith(data.at(i))) {
                f = data.at(i);
                cuteimeDebug() << f << from;
                while (!f.isEmpty() && !from.isEmpty()) {
                    cuteimeDebug() << f << from.first() << item;
                    if (f.startsWith(from.first())) {
                        f = f.mid(from.first().length());
                        int l = item.to.length();
                        if (item.from.length() < l) {
                            item.from.append(from.takeFirst());
                            item.rawString.append(rawString.takeFirst());
                        } else {
                            item.from.replace(l - 1, item.from.at(l - 1) + from.takeFirst());
                            item.rawString.replace(l - 1, item.rawString.at(l - 1) + rawString.takeFirst());
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

        }
        cuteimeDebug() << item;
        for (int i = 0; i < cand.count(); i++) {
            cand[i].from = item.from.last();
        }
        candidateList.append(cand);
    }
    cuteimeDebug() << item;

    item.selection = item.to.at(preeditIndex).length();
    cuteimeDebug() << item;

    for (int i = initialSizes.length(); i < item.from.size(); i++) {
        initialSizes.append(item.from.at(i).length());
    }

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);

    cuteimeDebugOut();
}

void Engine::Private::resize()
{
    CuteimePreeditItem item = preeditManager->item();
    if (item.to.isEmpty()) return;
    QStringList from = item.from;

    int delta = item.from.at(preeditIndex).length() - initialSizes.at(preeditIndex);

    if (initialSizes.at(preeditIndex) + delta < 1) return;
    if (delta > 0 && preeditIndex == initialSizes.count() - 1) return;

    cuteimeDebugIn() << delta;

    cuteimeDebug() << item;
    cuteimeDebug() << initialSizes;

    QString url = QString("http://www.social-ime.com/api/?string=%1").arg(from.join(""));
    while (initialSizes.count() - 1 > preeditIndex) {
        initialSizes.takeLast();
    }
    for (int i = 0; i < preeditIndex; i++) {
        if (item.from.at(i).length() != initialSizes.at(i))
            url.append(QString("&resize[%1]=%2").arg(i).arg(item.from.at(i).length() - initialSizes.at(i)));
    }
    url.append(QString("&resize[%1]=%2").arg(preeditIndex).arg(item.from.at(preeditIndex).length() - initialSizes.at(preeditIndex)));
    url.append(QLatin1String("&charset=UTF8"));
    url.append(QString("&user=%1").arg(user));
    cuteimeDebug() << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", QString("%1(%2)").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()).toUtf8());

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>))
            , this, SLOT(sslErrors(QList<QSslError>)));
    cancel();
    conversionReply = reply;
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

    cuteimeDebug() << item << currentIndex;
    cuteimeDebug() << candidateList;
    candidateManager->setItems(candidateList.at(currentIndex));
    cuteimeDebugOut();
}

void Engine::Private::learn()
{
    CuteimePreeditItem item = preeditManager->item();
    if (candidateList.isEmpty()) return;
    if (item.to.length() != initialSizes.length()) return;
    cuteimeDebugIn();
    QString preeditString = item.from.join("");
    QString url = QString("http://www.social-ime.com/api/?string=%1").arg(preeditString);
    for (int i = 0; i < initialSizes.length(); i++) {
        url.append(QString("&resize[%1]=%2").arg(i).arg(item.from.at(i).length() - initialSizes.at(i)));
    }
    initialSizes.clear();
    for (int i = 0; i < item.to.length(); i++) {
        int k = 0;
        for (int j = 0; j < candidateList.at(i).length(); j++) {
            if (candidateList.at(i).at(j).source == q->identifier()) {
                if (item.to.at(i) == candidateList.at(i).at(j).to) {
                    url.append(QString("&commit[%1]=%2").arg(i).arg(k));
                    break;
                }
                k++;
            }
        }
    }
    url.append(QString("&user=%1").arg(user));
    cuteimeDebug() << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "CUTEIME");

    networkManager->get(request);
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

void Engine::Private::cancel()
{
    if (conversionReply) {
        cuteimeWarning() << conversionReply;
        conversionReply->abort();
        conversionReply = 0;
    }
}

void Engine::Private::error(QNetworkReply::NetworkError err)
{
    cuteimeDebugIn() << err;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    cuteimeDebug() << reply;
    reply->deleteLater();
    cuteimeWarning() << err;
    cuteimeDebugOut();
}

void Engine::Private::sslErrors(QList<QSslError> errs)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    cuteimeWarning() << errs.count();
}

#ifndef CUTEIME_NO_GUI
CuteimeSettingsWidget *Engine::Private::settings(const QString &hint, QWidget *parent)
{
    Q_UNUSED(hint);
    Settings *settings = new Settings(q, parent);
    connect(settings, SIGNAL(saved()), this, SLOT(saved()));
    return settings;
}
#endif

void Engine::Private::saved()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    user = settings.value("User").toString();
}

Engine::Engine(QObject *parent)
    : CuteimeEngine(parent)
{
    d = new Private(this);
}

Engine::~Engine()
{
    delete d;
}

#ifndef CUTEIME_NO_GUI
CuteimeSettingsWidget *Engine::settings(const QString &hint, QWidget *parent)
{
    return d->settings(hint, parent);
}
#endif

#include "engine.moc"
