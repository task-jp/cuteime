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

#include <QtTest>
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>
#include <QtConcurrentRun>

#include <cuteimecandidatemanager.h>

class CuteimeCandidateManagerTest : public QObject
{
    Q_OBJECT
public:
    CuteimeCandidateManagerTest(QObject *parent = 0)
        : QObject(parent)
    {
        bool ret = QDBusConnection::sessionBus().registerService(CUTEIME_DBUS_SERVICE);
        if (ret) {
            candidateManager = new CuteimeCandidateManager(this, CuteimeCandidateManager::Server);
            candidateManager->init();
            QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            candidateManager = new CuteimeCandidateManager(this, CuteimeCandidateManager::Client);
            candidateManager->init();
        }
    }

    ~CuteimeCandidateManagerTest() {
        if (client.isStarted()) {
            client.waitForFinished();
        }
    }

public slots:
    void initTestCase() {
        qDebug() << candidateManager->type();
    }

    void init() {
        switch (candidateManager->type()) {
        case CuteimeCandidateManager::Server:
            break;
        case CuteimeCandidateManager::Client:
            break;
        }
        expected.clear();
        signalReceived = false;
    }

    void cleanup() {
        signalReceived = false;
    }

    void cleanupTestCase() {
        wait();
    }

protected slots:
    void itemsChanged(const CuteimeConversionItemList &candidates) {
        QCOMPARE(candidates, expected.value<CuteimeConversionItemList>());
        signalReceived = true;
    }

    void currentIndexChanged(int currentIndex) {
        QCOMPARE(currentIndex, expected.toInt());
        signalReceived = true;
    }

private slots:
    void setItems_data() {
        QTest::addColumn<CuteimeConversionItemList>("data");

        CuteimeConversionItemList candidates;
        QString alphabet = "abcde";
        for (int i = 0; i < alphabet.length() + 1; i++) {
            QTest::newRow(QString("candidates(%1)").arg(i).toLocal8Bit().data()) << candidates;
            CuteimeConversionItem elem;
            elem.from = QLatin1String("from:") + alphabet.left(i + 1);
            elem.to = QLatin1String("to:") + alphabet.left(i + 1);
            candidates.append(elem);
        }
    }

    void setItems() {
        QFETCH(CuteimeConversionItemList, data);
        expected.setValue(data);
        connect(candidateManager, SIGNAL(itemsChanged(CuteimeConversionItemList)), this, SLOT(itemsChanged(CuteimeConversionItemList)));
        switch (candidateManager->type()) {
        case CuteimeCandidateManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(candidateManager->items(), data);
            break;
        case CuteimeCandidateManager::Client:
            wait(200);
            signalReceived = false;
            candidateManager->setItems(data);
            QCOMPARE(candidateManager->items(), data);
            wait();
            break;
        }
        disconnect(candidateManager, SIGNAL(itemsChanged(CuteimeConversionItemList)), this, SLOT(itemsChanged(CuteimeConversionItemList)));
    }

    void setCurrentIndex_data() {
        QTest::addColumn<int>("data");

        for (int i = 0; i < 2; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << i;
        }
    }

    void setCurrentIndex() {
        QFETCH(int, data);
        expected = data;
        connect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        switch (candidateManager->type()) {
        case CuteimeCandidateManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(candidateManager->currentIndex(), data);
            break;
        case CuteimeCandidateManager::Client:
            wait(200);
            signalReceived = false;
            candidateManager->setCurrentIndex(data);
            QCOMPARE(candidateManager->currentIndex(), data);
            wait();
            break;
        }
        disconnect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    }

private:
    bool wait(int timeout = 1000) {
        bool ret = false;
        QTime timer;
        timer.start();
        while (!signalReceived) {
            QTest::qWait(50);
            if (timer.elapsed() > timeout) {
                ret = true;
                break;
            }
        }
        return ret;
    }

private:
    CuteimeCandidateManager *candidateManager;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

QTEST_MAIN(CuteimeCandidateManagerTest)

#include "tst_cuteimecandidatemanager.moc"
