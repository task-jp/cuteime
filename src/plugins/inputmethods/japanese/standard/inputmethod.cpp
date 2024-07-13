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

#include "inputmethod.h"
#include "global.h"
#include "keyactions.h"

#include <cuteimedebug.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimekeymanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimecandidatemanager.h>
#include <cuteimepluginmanager.h>
#include <cuteimeconverter.h>
#include <cuteimeinterpreter.h>
#include <cuteimeengine.h>
#include <cuteimedynamictranslator.h>
#include <cuteimekeysequence.h>

#ifndef CUTEIME_NO_GUI
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#endif

namespace Japanese {
    namespace Standard {

class InputMethod::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputMethod *parent);
    ~Private();

private slots:
    void init();
    void activeChanged(bool active);

    void stateChanged(uint state);
    void converterChanged(const QString &identifier);
    void interpreterChanged(const QString &identifier);
    void engineChanged(const QString &identifier);

    void keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat);
    void keyReleased(const QString &text, int keycode, int modifiers, bool autoRepeat);

    void currentIndexChanged(int currentIndex);

#ifndef CUTEIME_NO_GUI
    void resetIcon();
private:
    void updateIcon(const QIcon &overlay);
#endif

private:
    InputMethod *q;

    CuteimeApplicationManager *applicationManager;
    CuteimeInputMethodManager *inputMethodManager;
    CuteimeKeyManager *keyManager;
    CuteimePreeditManager *preeditManager;
    CuteimeCandidateManager *candidateManager;
    KeyActions *keyActions;

#ifndef CUTEIME_NO_GUI
    QTimer resetIconTimer;
#endif

    int acceptedKeyCount;
};

    }
}

using namespace Japanese::Standard;

InputMethod::Private::Private(InputMethod *parent)
    : QObject(parent)
    , q(parent)
    , applicationManager(0)
    , inputMethodManager(0)
    , keyManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , keyActions(0)
    , acceptedKeyCount(0)
{
    cuteimeDebugIn() << parent;
    init();
    cuteimeDebugOut();
}

InputMethod::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void InputMethod::Private::init()
{
    q->setIdentifier(QLatin1String("Japanese(Standard)"));
    q->setPriority(0x10);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/japanese/standard/resources/japanese.png"));
#endif
    trConnect(this, QT_TR_NOOP("Japanese(Standard)"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese language"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/InputMethod"), q, "categoryName");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));

#ifndef CUTEIME_NO_GUI
    resetIconTimer.setInterval(2000);
    resetIconTimer.setSingleShot(true);
    connect(&resetIconTimer, SIGNAL(timeout()), this, SLOT(resetIcon()));
#endif
}

void InputMethod::Private::activeChanged(bool active)
{
    cuteimeDebugIn();
    if (active) {
        if (!applicationManager) {
            applicationManager = new CuteimeApplicationManager(this);
            applicationManager->init();
        }

        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
            inputMethodManager->setState(Direct);
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)), Qt::QueuedConnection);
            connect(inputMethodManager, SIGNAL(converterChanged(QString)), this, SLOT(converterChanged(QString)), Qt::QueuedConnection);
            connect(inputMethodManager, SIGNAL(interpreterChanged(QString)), this, SLOT(interpreterChanged(QString)), Qt::QueuedConnection);
            connect(inputMethodManager, SIGNAL(engineChanged(QString)), this, SLOT(engineChanged(QString)), Qt::QueuedConnection);
        }
#ifndef CUTEIME_NO_GUI
        QMetaObject::invokeMethod(this, "resetIcon", Qt::QueuedConnection);
#endif

        if (!keyManager) {
            keyManager = new CuteimeKeyManager(this);
            keyManager->init();
            connect(keyManager, SIGNAL(keyPressed(QString,int,int,bool)), this, SLOT(keyPressed(QString,int,int,bool)));
            connect(keyManager, SIGNAL(keyReleased(QString,int,int,bool)), this, SLOT(keyReleased(QString,int,int,bool)));
        }

        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            preeditManager->init();
        }

        if (!candidateManager) {
            candidateManager = new CuteimeCandidateManager(this);
            candidateManager->init();
            connect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        }

        if (!keyActions) {
            keyActions = new KeyActions(this);
        }
    } else {
        if (applicationManager) {
            applicationManager->deleteLater();
            applicationManager = 0;
        }

        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            disconnect(inputMethodManager, SIGNAL(converterChanged(QString)), this, SLOT(converterChanged(QString)));
            disconnect(inputMethodManager, SIGNAL(interpreterChanged(QString)), this, SLOT(interpreterChanged(QString)));
            disconnect(inputMethodManager, SIGNAL(engineChanged(QString)), this, SLOT(engineChanged(QString)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }

        if (keyManager) {
            disconnect(keyManager, SIGNAL(keyPressed(QString,int,int,bool)), this, SLOT(keyPressed(QString,int,int,bool)));
            disconnect(keyManager, SIGNAL(keyReleased(QString,int,int,bool)), this, SLOT(keyReleased(QString,int,int,bool)));
            keyManager->deleteLater();
            keyManager = 0;
        }

        if (preeditManager) {
            preeditManager->deleteLater();
            preeditManager = 0;
        }

        if (candidateManager) {
            disconnect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            candidateManager->deleteLater();
            candidateManager = 0;
        }

        if (keyActions) {
            keyActions->deleteLater();
            keyActions = 0;
        }
    }
    cuteimeDebugOut();
}

void InputMethod::Private::stateChanged(uint state)
{
    cuteimeDebugIn() << state;
#ifndef CUTEIME_NO_GUI
    resetIcon();
#endif
    switch (state) {
    case Convert:
        candidateManager->setCurrentIndex(-1);
        break;
    default:
        break;
    }
    cuteimeDebugOut();
}

void InputMethod::Private::converterChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    QList<CuteimeConverter *> converters = CuteimePluginManager::objects<CuteimeConverter>();
    foreach(CuteimeConverter *converter, converters) {
        if (converter->identifier() == identifier) {
#ifndef CUTEIME_NO_GUI
            updateIcon(converter->icon());
#endif
            break;
        }
    }
#ifndef CUTEIME_NO_GUI
    if (resetIconTimer.isActive()) {
        resetIconTimer.stop();
    }
    resetIconTimer.start();
#endif
    cuteimeDebugOut();
}

void InputMethod::Private::interpreterChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    QList<CuteimeInterpreter *> interpreters = CuteimePluginManager::objects<CuteimeInterpreter>();
    foreach(CuteimeInterpreter *interpreter, interpreters) {
        if (interpreter->identifier() == identifier) {
#ifndef CUTEIME_NO_GUI
            updateIcon(interpreter->icon());
#endif
            break;
        }
    }
#ifndef CUTEIME_NO_GUI
    if (resetIconTimer.isActive()) {
        resetIconTimer.stop();
    }
    resetIconTimer.start();
#endif
    cuteimeDebugOut();
}

void InputMethod::Private::engineChanged(const QString &identifier)
{
    cuteimeDebugIn() << identifier;
    QList<CuteimeEngine *> engines = CuteimePluginManager::objects<CuteimeEngine>();
    foreach(CuteimeEngine *engine, engines) {
        if (engine->identifier() == identifier) {
#ifndef CUTEIME_NO_GUI
            updateIcon(engine->icon());
#endif
            break;
        }
    }
#ifndef CUTEIME_NO_GUI
    if (resetIconTimer.isActive()) {
        resetIconTimer.stop();
    }
    resetIconTimer.start();
#endif
    cuteimeDebugOut();
}

#ifndef CUTEIME_NO_GUI
void InputMethod::Private::updateIcon(const QIcon &overlay)
{
    static qint64 cache = 0;
    if (cache == overlay.cacheKey()) return;
    cuteimeDebugIn();
    cache = overlay.cacheKey();

    QIcon::Mode mode = applicationManager->composing() ? QIcon::Active : QIcon::Disabled;
    QIcon icon;
    foreach (const QSize &size, q->icon().availableSizes()) {
        QRect r;
        r.setSize(size);
        QPixmap pix = q->icon().pixmap(size, mode);
        QPainter p(&pix);
        p.setOpacity(0.50);
        p.drawPixmap(r, overlay.pixmap(size, mode));
        p.end();
        icon.addPixmap(pix);
    }

    applicationManager->setCurrentIcon(icon);
    cuteimeDebugOut();
}

void InputMethod::Private::resetIcon()
{
    cuteimeDebugIn();
    static QHash<QString, CuteimeConverter *> converters;
    if (inputMethodManager) {
        switch (inputMethodManager->state()) {
        case Direct:
            updateIcon(QIcon());
            break;
        default:
            if (converters.isEmpty()) {
                foreach(CuteimeConverter *converter, CuteimePluginManager::objects<CuteimeConverter>()) {
                    converters[converter->identifier()] = converter;
                }
            }
            if (converters.contains(inputMethodManager->converter())) {
                updateIcon(converters[inputMethodManager->converter()]->icon());
                break;
            }
            break;
        }
    }
    cuteimeDebugOut();
}
#endif // CUTEIME_NO_GUI

void InputMethod::Private::keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat)
{
    if (keyManager->isAccepted()) return;

    cuteimeDebugIn() << text << keycode << modifiers << autoRepeat;

    int key = keycode;
    acceptedKeyCount = 1;
    if (modifiers & Qt::ControlModifier) {
        key |= Qt::CTRL;
        acceptedKeyCount++;
    }
    if (modifiers & Qt::AltModifier) {
        key |= Qt::ALT;
        acceptedKeyCount++;
    }
    if (modifiers & Qt::ShiftModifier) {
        key |= Qt::SHIFT;
        acceptedKeyCount++;
    }

    CuteimeKeySequence seq(key);
    if (keyActions->contains(seq)) {
        keyActions->trigger(seq);
        keyManager->accept();
        // reset
    } else {
        QChar ch(0);
        if (text.isEmpty()) {
            switch (keycode) {
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Meta:
            case Qt::Key_Alt:
                break;
            default:
                ch = QChar(keycode);
                break;
            }
        } else {
            ch = text.at(0);
        }
        cuteimeDebug() << ch << text << key << Qt::SHIFT;
        if (!ch.isNull()) {
            uint state = inputMethodManager->state();
            if (ch.isPrint()) {
                switch (state) {
                case Direct:
                        break;
                case Convert:
                case Select:
                        inputMethodManager->execute(QLatin1String("Commit all"));
                        Q_FALLTHROUGH;
                case Empty:
                    if (modifiers & Qt::ControlModifier || modifiers & Qt::AltModifier) break;
                case Input:
                        // clear selection first
                        inputMethodManager->execute(QLatin1String("Clear Selection"));
                        inputMethodManager->setState(Input);

                        preeditManager->insert(ch);
                        keyManager->accept();
                }
            } else {
                switch (state) {
                case Direct:
                case Empty:
                    break;
                default:
                    keyManager->accept();
                    break;
                }
            }
        }
    }

    if (!keyManager->isAccepted()) {
        acceptedKeyCount = 0;
    }
    cuteimeDebugOut() << keyManager->isAccepted();
}

void InputMethod::Private::keyReleased(const QString &text, int keycode, int modifiers, bool autoRepeat)
{
    if (keyManager->isAccepted()) return;

//    cuteimeDebugOn();
    cuteimeDebugIn() << text << keycode << modifiers << autoRepeat;

    if (acceptedKeyCount > 0) {
        keyManager->accept();
        acceptedKeyCount--;
    }
    cuteimeDebugOut();
}

void InputMethod::Private::currentIndexChanged(int currentIndex)
{
    if (currentIndex == -1) return;
    cuteimeDebugIn() << currentIndex;

    CuteimePreeditItem item = preeditManager->item();
    int cursor = item.cursor;
    int index = 0;
    int pos = 0;
    for (int i = 0; i < item.to.length(); i++) {
        if (pos == cursor) {
            index = i;
            break;
        }
        pos += item.to.at(i).length();
    }
    cuteimeDebug() << index;

    cuteimeDebug() << candidateManager;
    cuteimeDebug() << candidateManager->items();

    QString t = candidateManager->items().at(currentIndex).to;
    QString f = candidateManager->items().at(currentIndex).from;

    cuteimeDebug() << item.to;

    if (item.to.isEmpty()) {
        item.to.append(t);
        item.from.append(f);
        item.rawString.append(f);
    } else {
        item.to.replace(index, t);
        item.from.replace(index, f);
    }
    item.selection = t.length();
    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);

    cuteimeDebugOut();
}

InputMethod::InputMethod(QObject *parent)
    : CuteimeInputMethod(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

InputMethod::~InputMethod()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "inputmethod.moc"
