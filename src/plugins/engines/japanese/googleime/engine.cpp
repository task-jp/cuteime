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

#include <plugins/inputmethods/japanese/standard/global.h>

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimecandidatemanager.h>
#include <cuteimedynamictranslator.h>

#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTextCodec>

namespace Japanese {
    namespace GoogleIME {

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

    void convert();
    void readConversion();

    void resize();
    void setCandidates();

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
    QList<CuteimeConversionItemList> candidateList;
};

    }
}

using namespace Japanese::GoogleIME;

Engine::Private::Private(Engine *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , networkManager(0)
    , conversionReply(0)
{
    cuteimeDebugIn() << parent;

    q->setIdentifier(QLatin1String("Google IME"));
    q->setPriority(0x100);

    q->setLocale(QLatin1String("ja_JP"));
    q->setLanguage("Japanese(Standard)");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/icons/googleime.png"));
#endif
    trConnect(this, QT_TR_NOOP("Google IME Engine"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("See <a href=\"http://www.google.com/intl/ja/ime/cgiapi.html\">http://www.google.com/intl/ja/ime/cgiapi.html</a> for detail."), q, "description");

    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Conversion Engine"), q, "categoryName");

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
        candidateList.clear();
        clearCandidates();
        break;
    case Empty:
        Q_FALLTHROUGH;
    case Input:
        clearCandidates();
        candidateList.clear();
        preeditIndex = -1;
        break;
    case Convert:
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
    cuteimeDebugIn() << item;
    switch (currentState) {
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
    default:
        break;
    }

    cuteimeDebugOut();
}

void Engine::Private::convert()
{
    if (!candidateList.isEmpty()) return;
    cuteimeDebugIn();

    CuteimePreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    cuteimeDebug() << preeditString;

    QString url = QString("http://www.google.com/transliterate?langpair=ja-Hira|ja&text=%1").arg(preeditString);
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "CUTEIME");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));

    cancel();
    conversionReply = reply;
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

    cuteimeDebugIn();
    cuteimeDebug() << "preeditIndex =" << preeditIndex;

    candidateList.clear();
    QStringList cache = item.to;
    while (cache.length() > preeditIndex) cache.takeLast();
    cuteimeDebug() << cache;

    item.to.clear();
    item.from.clear();
    item.rawString.clear();

    QTextCodec *textCodec = 0;
    foreach (const QByteArray &headerName, reply->rawHeaderList()) {
        if (headerName == "Content-Type") {
            QRegExp charset("charset=([A-Za-z0-9\\-_]+)");
            if (charset.indexIn(reply->rawHeader(headerName))) {
                cuteimeDebug() << headerName << reply->rawHeader(headerName) << charset.cap(1);
                textCodec = QTextCodec::codecForName(charset.cap(1).toUtf8());
            }
        }
    }

    QString data = textCodec ? textCodec->toUnicode(reply->readAll()) : QString::fromUtf8(reply->readAll());
    cuteimeDebug() << data;

    int level = 0;
    foreach (QString line, QString(data).split(QLatin1String("\n"))) {
        if (line.isEmpty()) continue;
        cuteimeDebug() << line;
        switch (line.at(0).unicode()) {
        case '[':
            level++;
            break;
        case ']':
            level--;
            break;
        default:
            switch (level) {
            case 2:
                item.from.append(line.mid(1, line.length() - 3));
                item.rawString.append(QString());
                break;
            case 3: {
                QStringList tos = line.mid(1, line.length() - 2).split(QLatin1String("\",\""));
                cuteimeDebug() << tos;
                QString to = tos.first();
                if (preeditIndex == candidateList.length()) {
                    item.cursor = item.to.join("").length();
                    item.selection = to.length();
                }
                cuteimeDebug() << item.to << cache;
                if (item.to.length() < cache.length()) {
                    item.to.append(cache.at(item.to.length()));
                } else {
                    item.to.append(to);
                }
                cuteimeDebug() << item.to << cache;

                CuteimeConversionItemList candidates;
                for (int i = 0; i < tos.length(); i++) {
                    CuteimeConversionItem candidate;
                    candidate.from = item.from.last();
                    candidate.to = tos.at(i);
                    candidate.index = i;
                    candidate.source = q->identifier();
                    candidates.append(candidate);
                }
                candidateList.append(candidates);
                break; }
            default:
                cuteimeWarning() << level << line;
                break;
            }
            break;
        }
    }
    cuteimeDebug() << item;

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);
    cuteimeDebugOut();
}

void Engine::Private::resize()
{
    CuteimePreeditItem item = preeditManager->item();
    if (item.to.isEmpty()) return;

    cuteimeDebugIn();

    QString preeditString;
    cuteimeDebug() << item.from << preeditIndex;
    while (item.from.length() > preeditIndex + 2) {
        cuteimeDebug() << item.from.last();
        preeditString.prepend(item.from.takeLast());
    }
    preeditString.prepend(item.from.join(","));

    qDebug() << preeditString;

    QString url = QString("http://www.google.com/transliterate?langpair=ja-Hira|ja&text=%1").arg(preeditString);
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "CUTEIME");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));

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

Engine::Engine(QObject *parent)
    : CuteimeEngine(parent)
{
    d = new Private(this);
}

Engine::~Engine()
{
    delete d;
}

#include "engine.moc"
