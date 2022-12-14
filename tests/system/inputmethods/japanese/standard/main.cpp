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
#include <QtGui>
#include <QtWidgets>

#include <cuteimedebug.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimekeymanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimecandidatemanager.h>

class JapaneseStandardTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void init();
    void cleanup();

private slots:
    void functionality_data();
    void functionality();

private:
    void key(const QString& text, int keycode, int modifiers = Qt::NoModifier) {
        keyManager.keyPress(text, keycode, modifiers, false);
        QTest::qWait(10);
        keyManager.keyRelease(text, keycode, modifiers, false);
        QTest::qWait(10);
    }

    void key(const QString& data) {
        QKeySequence seq(data);
        int keycode = seq[0];
        int modifiers = 0;
        if (keycode & Qt::CTRL) {
            modifiers |= Qt::CTRL;
            keycode &= ~Qt::CTRL;
        }
        if (keycode & Qt::ALT) {
            modifiers |= Qt::ALT;
            keycode &= ~Qt::ALT;
        }
        if (keycode & Qt::SHIFT) {
            modifiers |= Qt::SHIFT;
            keycode &= ~Qt::SHIFT;
        }
//  DEBUG() << data << keycode << modifiers;
        key(QString::null, keycode, modifiers);
    }

    void wait() {
        QTime timer;
        timer.start();
        while (waiting) {
            QTest::qWait(50);
            if (timer.elapsed() > 250) break;
        }
        QVERIFY(!waiting);
    }
private:
    QPlainTextEdit* widget;
    CuteimeApplicationManager applicationManager;
    CuteimeInputMethodManager inputMethodManager;
    CuteimeKeyManager keyManager;
    CuteimePreeditManager preeditManager;
    CuteimeCandidateManager candidateManager;
    bool waiting;
    QVariant expected;
};

void JapaneseStandardTest::initTestCase() {
    keyManager.init();
    if (keyManager.hasError()) {
        QFAIL("Run cuteime by yourself first.");
    }
    applicationManager.init();
    inputMethodManager.init();
    preeditManager.init();
    candidateManager.init();

    widget = new QPlainTextEdit;
    widget->show();
    QTest::qWaitForWindowExposed(widget);
    widget->setFocus();
    QTest::qWait(200);
}

void JapaneseStandardTest::init()
{
    CuteimePreeditItem item;
    item.cursor = 0;
    item.selection = 0;
    item.modified = 0;
    preeditManager.setItem(item);
    widget->clear();
    applicationManager.setComposing(true);
    inputMethodManager.setConverter(QLatin1String("Hiragana"));
    inputMethodManager.setInterpreter(QLatin1String("Japanese(Romaji)"));
}

void JapaneseStandardTest::cleanup()
{
    if (QTest::currentTestFailed()) {
        QTest::qWait(50000);
    }
    applicationManager.setComposing(false);
    widget->clear();
}

void JapaneseStandardTest::functionality_data()
{
    QTest::addColumn<QStringList>("sequence");

    QFile file(":/data/functionality.txt");
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QTextStream stream(&file);

    QString name;
    QStringList sequence;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.isEmpty()) {
            if (name.isNull()) continue;
            QTest::newRow(name.toLatin1().data()) << sequence;
            name.clear();
            sequence.clear();
        } else if (line.startsWith("//")) {
        } else if (name.isNull()) {
            name = line;
        } else {
            sequence.append(line);
        }
    }

    if (!name.isNull()) {
        QTest::newRow(name.toLatin1().data()) << sequence;
    }
    file.close();
}

void JapaneseStandardTest::functionality()
{
    QFETCH(QStringList, sequence);

    foreach(const QString& line, sequence) {
        QChar ch = line.at(0);
        QString data = line.mid(2);
        switch (ch.unicode()) {
        case '+':   // input data
            inputMethodManager.execute(QLatin1String("Clear Selection"));
            foreach(const QChar& ch, data) {
#if 0
                key(ch, 0);
#else
                if (preeditManager.item().to.isEmpty()) {
                    inputMethodManager.setState(inputMethodManager.state() << 1);
                }
                preeditManager.insert(ch);
                QTest::qWait(20);
#endif
            }
            break;
        case '=': { // compare preedit string(to)
            QCOMPARE(preeditManager.item().to.join(""), data);
            break;
        }
        case '~': { // compare preedit string(from)
            QCOMPARE(preeditManager.item().from.join(""), data);
            break;
        }
        case '_': { // compare preedit string(rawString)
            QCOMPARE(preeditManager.item().rawString.join(""), data);
            break;
        }
        case '|': { // check cursor position
            QCOMPARE(preeditManager.item().cursor, data.toInt());
            break;
        }
        case '#': { // check selection length
            QCOMPARE(preeditManager.item().selection, data.toInt());
            break;
        }
        case '<': { // move cursor previous
            for (int i = 0; i < data.toInt(); i++) {
                inputMethodManager.execute(QLatin1String("Move cursor previous"));
            }
            break;
        }
        case '>': { // move cursor next
            for (int i = 0; i < data.toInt(); i++) {
                inputMethodManager.execute(QLatin1String("Move cursor next"));
            }
            break;
        }
        case '[': { // backspace
                inputMethodManager.execute(QLatin1String("Remove previous"));
            break;
        }
        case ']': { // delete
                inputMethodManager.execute(QLatin1String("Remove next"));
            break;
        }
        case '{': { // change selection previous
            for (int i = 0; i < data.toInt(); i++) {
                inputMethodManager.execute(QLatin1String("Change selection length previous"));
            }
            break;
        }
        case '}': { // change selection next
            for (int i = 0; i < data.toInt(); i++) {
                inputMethodManager.execute(QLatin1String("Change selection length next"));
            }
            break;
        }
        case 'c': { // convert
            inputMethodManager.execute(QLatin1String("Convert"));
            break;
        }
        case 'x': { // cancel
            inputMethodManager.execute(QLatin1String("Cancel"));
            break;
        }
        case 'C': { // commit
            inputMethodManager.execute(QLatin1String("Commit all"));
            break;
        }
        case 'b': { // cancel
            inputMethodManager.execute(QLatin1String("Cancel"));
            break;
        }
        case 'i': { // converter
            inputMethodManager.setConverter(data);
            break;
        }
        case '-': { // quit
            QTest::qWait(50000);
            qFatal(data.toLatin1().data());
            break;
        }
        case 'r': { // resize segment
            CuteimePreeditItem item = preeditManager.item();
            int pos = 0;
            for (int i = 0; i < item.to.length(); i++) {
                if (pos == item.cursor) {
                    int size = data.toInt();
                    int length = item.from.at(i).length();
                    while (length < size) {
                        inputMethodManager.execute(QLatin1String("Resize segment longer"));
                        length++;
                    }
                    while (length > size) {
                        inputMethodManager.execute(QLatin1String("Resize segment shorter"));
                        length--;
                    }
                    break;
                }
                pos += item.to.at(i).length();
            }
            break;
        }
        case 's': { // select candidate
            CuteimePreeditItem item = preeditManager.item();
            int pos = 0;
            for (int i = 0; i < item.to.length(); i++) {
                if (pos == item.cursor) {
                    if (item.to.at(i) != data) {
                        inputMethodManager.execute(QLatin1String("Select next candidate"));
                        CuteimeConversionItemList candidateItems = candidateManager.items();
                        for (int j = 0; j < candidateItems.length(); j++) {
                            if (candidateItems.at(j).to == data) {
                                candidateManager.setCurrentIndex(j);
                                QTest::qWait(100);
                                break;
                            }
                        }
                        QCOMPARE(preeditManager.item().to.at(i), data);
                    }
                    break;
                }
                pos += item.to.at(i).length();
            }
            break;
        }
        default:
            cuteimeWarning() << ch << data;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    bool found = false;
    QDir dir(QCoreApplication::applicationDirPath());
    while (!found && dir.cdUp()) {
        foreach(const QString &subdir, dir.entryList(QDir::Dirs)) {
            if (subdir == QLatin1String("bin")) {
                dir.cd("lib");
                dir.cd("plugins");
                found = true;
                break;
            }
        }
    }
    if (!found) {
        qFatal("!");
    }

    QByteArray qtPluginPath = qgetenv("QT_PLUGIN_PATH");
    qtPluginPath.prepend(QStringLiteral("%1:").arg(dir.absolutePath()).toUtf8());
    qputenv("QT_PLUGIN_PATH", qtPluginPath);
    qputenv("QT_IM_MODULE", QByteArrayLiteral("cuteime"));

    QApplication app(argc, argv);
    JapaneseStandardTest tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "main.moc"
