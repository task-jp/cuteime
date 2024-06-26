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
#ifndef CUTEIME_NO_GUI
#include <QIcon>
#include <QPainter>
#endif

#include <cuteimeapplicationmanager.h>

class CuteimeApplicationManagerTest : public QObject
{
    Q_OBJECT
public:
    CuteimeApplicationManagerTest(QObject *parent = 0)
        : QObject(parent)
    {
        bool ret = QDBusConnection::sessionBus().registerService(CUTEIME_DBUS_SERVICE);
        if (ret) {
            applicationManager = new CuteimeApplicationManager(this, CuteimeApplicationManager::Server);
			applicationManager->init();
			QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            applicationManager = new CuteimeApplicationManager(this, CuteimeApplicationManager::Client);
			applicationManager->init();
		}
    }

    ~CuteimeApplicationManagerTest() {
        if (client.isStarted()) {
            client.waitForFinished();
        }
    }

public slots:
    void initTestCase() {
        qDebug() << applicationManager->type();
    }

    void init() {
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            break;
        case CuteimeApplicationManager::Client:
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
    void displayLanguageChanged(const QString &displayLanguage) {
        QCOMPARE(displayLanguage, expected.toString());
        signalReceived = true;
    }

    void focusChanged(bool focus) {
        QCOMPARE(focus, expected.toBool());
        signalReceived = true;
    }

    void windowChanged(qulonglong window) {
        QCOMPARE(window, expected.toULongLong());
        signalReceived = true;
    }

    void widgetChanged(qulonglong widget) {
        QCOMPARE(widget, expected.toULongLong());
        signalReceived = true;
    }

    void composingChanged(bool composing) {
        QCOMPARE(composing, expected.toBool());
        signalReceived = true;
    }

#ifndef CUTEIME_NO_GUI
    void currentIconChanged(const QIcon &currentIcon) {
        QCOMPARE(currentIcon, expected.value<QIcon>());
        signalReceived = true;
    }
#endif

    void keyboardChanged(bool keyboard) {
        QCOMPARE(keyboard, expected.toBool());
        signalReceived = true;
    }

    void triggered(int type) {
        qDebug() << type << expected.value<int>();
        QCOMPARE(type, expected.value<int>());
        signalReceived = true;
    }

private slots:
    void setDisplayLanguage_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("language_%1").arg(i);
        }
    }

    void setDisplayLanguage() {
        QFETCH(QString, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(displayLanguageChanged(QString)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(applicationManager->displayLanguage(), data);
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->setDisplayLanguage(data);
            QCOMPARE(applicationManager->displayLanguage(), data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(displayLanguageChanged(QString)));
    }

    void setFocus_data() {
        QTest::addColumn<bool>("data");

        QTest::newRow("true") << true;
        QTest::newRow("false") << false;
    }

    void setFocus() {
        QFETCH(bool, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(focusChanged(bool)), this, SLOT(focusChanged(bool)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(applicationManager->focus(), data);
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->setFocus(data);
            QCOMPARE(applicationManager->focus(), data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(focusChanged(bool)), this, SLOT(focusChanged(bool)));
    }

    void setWindow_data() {
        QTest::addColumn<qulonglong>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << (qulonglong)i;
        }
    }

    void setWindow() {
        QFETCH(qulonglong, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(windowChanged(qulonglong)), this, SLOT(windowChanged(qulonglong)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(applicationManager->window(), data);
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->setWindow(data);
            QCOMPARE(applicationManager->window(), data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(windowChanged(qulonglong)), this, SLOT(windowChanged(qulonglong)));
    }

    void setWidget_data() {
        QTest::addColumn<qulonglong>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << (qulonglong)i;
        }
    }

    void setWidget() {
        QFETCH(qulonglong, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(widgetChanged(qulonglong)), this, SLOT(widgetChanged(qulonglong)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(applicationManager->widget(), data);
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->setWidget(data);
            QCOMPARE(applicationManager->widget(), data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(widgetChanged(qulonglong)), this, SLOT(widgetChanged(qulonglong)));
    }

    void setComposing_data() {
        QTest::addColumn<bool>("data");

        QTest::newRow("true") << true;
        QTest::newRow("false") << false;
    }

    void setComposing() {
        QFETCH(bool, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(composingChanged(bool)), this, SLOT(composingChanged(bool)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(applicationManager->composing(), data);
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->setComposing(data);
            QCOMPARE(applicationManager->composing(), data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(composingChanged(bool)), this, SLOT(composingChanged(bool)));
    }

#ifndef CUTEIME_NO_GUI
    void setCurrentIcon_data() {
        QTest::addColumn<QIcon>("data");

        for (int i = 0; i < 10; i++) {
            QPixmap pix(32, 32);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            p.drawText(0, 0, 31, 31, Qt::AlignCenter, QString::number(i));
            p.end();
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QIcon(pix);
        }
    }

    void setCurrentIcon() {
        QFETCH(QIcon, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(currentIconChanged(QIcon)), this, SLOT(currentIconChanged(QIcon)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(applicationManager->currentIcon(), data);
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->setCurrentIcon(data);
            QCOMPARE(applicationManager->currentIcon(), data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(currentIconChanged(QIcon)), this, SLOT(currentIconChanged(QIcon)));
    }
#endif

    void exec_data() {
        QTest::addColumn<int>("data");
        QTest::newRow("ShowAboutCuteime") << (int)CuteimeApplicationManager::ShowAboutCuteime;
        QTest::newRow("ShowSettings") << (int)CuteimeApplicationManager::ShowSettings;
    }

    void exec() {
        QFETCH(int, data);
        expected = QVariant::fromValue(data);
        connect(applicationManager, SIGNAL(triggered(int)), this, SLOT(triggered(int)));
        switch (applicationManager->type()) {
        case CuteimeApplicationManager::Server:
            signalReceived = false;
            wait();
            break;
        case CuteimeApplicationManager::Client:
            wait(200);
            signalReceived = false;
            applicationManager->exec(data);
            wait();
            break;
        }
        disconnect(applicationManager, SIGNAL(triggered(int)), this, SLOT(triggered(int)));
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
    CuteimeApplicationManager *applicationManager;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

QTEST_MAIN(CuteimeApplicationManagerTest)
#include "tst_cuteimeapplicationmanager.moc"
