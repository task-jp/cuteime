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

#include <cuteimekeyboardmanager.h>

class CuteimeKeyboardManagerTest : public QObject
{
    Q_OBJECT
public:
    CuteimeKeyboardManagerTest(QObject *parent = 0)
        : QObject(parent)
    {
        bool ret = QDBusConnection::sessionBus().registerService(CUTEIME_DBUS_SERVICE);
        if (ret) {
            keyboardManager = new CuteimeKeyboardManager(this, CuteimeKeyboardManager::Server);
            keyboardManager->init();
            QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            keyboardManager = new CuteimeKeyboardManager(this, CuteimeKeyboardManager::Client);
            keyboardManager->init();
        }
    }

    ~CuteimeKeyboardManagerTest() {
        if (client.isStarted()) {
            client.waitForFinished();
        }
    }

public slots:
    void initTestCase() {
        qDebug() << keyboardManager->type();
    }

    void init() {
        switch (keyboardManager->type()) {
        case CuteimeKeyboardManager::Server:
            break;
        case CuteimeKeyboardManager::Client:
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
    void visibleChanged(bool visible) {
        QCOMPARE(visible, expected.toBool());
        signalReceived = true;
    }

private slots:

    void setVisible_data() {
        QTest::addColumn<bool>("data");

        QTest::newRow("true") << true;
        QTest::newRow("false") << true;
    }

    void setVisible() {
        QFETCH(bool, data);
        expected = QVariant::fromValue(data);
        connect(keyboardManager, SIGNAL(visibleChanged(bool)), this, SLOT(visibleChanged(bool)));
        switch (keyboardManager->type()) {
        case CuteimeKeyboardManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(keyboardManager->visible(), data);
            break;
        case CuteimeKeyboardManager::Client:
            wait(200);
            signalReceived = false;
            keyboardManager->setVisible(data);
            QCOMPARE(keyboardManager->visible(), data);
            wait();
            break;
        }
        disconnect(keyboardManager, SIGNAL(visibleChanged(bool)), this, SLOT(visibleChanged(bool)));
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
    CuteimeKeyboardManager *keyboardManager;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

QTEST_MAIN(CuteimeKeyboardManagerTest)

#include "tst_cuteimekeyboardmanager.moc"
