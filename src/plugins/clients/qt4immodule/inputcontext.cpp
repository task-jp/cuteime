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

#include "inputcontext.h"

#include <cuteimedebug.h>
#include <cuteimeapplicationmanager.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimekeymanager.h>
#include <cuteimekeyboardmanager.h>

#include <QApplication>
#include <QSettings>
#include <QTextCharFormat>
#include <QProcess>
#include <QTimer>

class InputContext::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputContext *parent);
    ~Private();

    void setFocusWidget(QWidget *w);
    void widgetDestroyed(QWidget *w);

public slots:
    void update();

private slots:
    void init();
    void itemChanged(const CuteimePreeditItem &item);
    void sendPreeditString();
    void sendCommitString(const QString &commitString, qulonglong target);

private:
    InputContext *q;
    QWidget *focusWidget;
    QTimer timer;

public:
    CuteimeApplicationManager *applicationManager;
    CuteimeInputMethodManager *inputMethodManager;
    CuteimePreeditManager *preeditManager;
    CuteimeKeyManager *keyManager;
    CuteimeKeyboardManager *keyboardManager;

    CuteimePreeditItem currentItem;
};

InputContext::Private::Private(InputContext *parent)
    : QObject(parent)
    , q(parent)
    , focusWidget(0)
    , applicationManager(0)
    , inputMethodManager(0)
    , preeditManager(0)
    , keyManager(0)
    , keyboardManager(0)
{
    cuteimeDebugIn() << parent;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    init();
    QApplication::restoreOverrideCursor();
    cuteimeDebugOut();
}

InputContext::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void InputContext::Private::init()
{
    cuteimeDebugIn();
    timer.setInterval(0);
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(sendPreeditString()));
    cuteimeDebugOut();
}

void InputContext::Private::setFocusWidget(QWidget *w)
{
    if (qApp->applicationFilePath() == QLatin1String("/usr/bin/qwassr")) {
        if (w && (w->inherits("QTextBrowser") || w->inherits("QListView"))) {
            w = 0;
        }
    }
    if (focusWidget == w) return;
    cuteimeDebugIn() << w;
    if (w) {
        focusWidget = w;
        if (!applicationManager) {
            applicationManager = new CuteimeApplicationManager(this);
            if (!applicationManager->init()) {
                applicationManager->deleteLater();
                applicationManager = 0;
            }
        }

        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            if (!inputMethodManager->init()) {
                inputMethodManager->deleteLater();
                inputMethodManager = 0;
            }
        }

        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            if (preeditManager->init()) {
                connect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
                connect(preeditManager, SIGNAL(committed(QString, qulonglong)), this, SLOT(sendCommitString(QString, qulonglong)));
            } else {
                preeditManager->deleteLater();
                preeditManager = 0;
            }
        }

        if (!keyManager) {
            keyManager = new CuteimeKeyManager(this);
            if (!keyManager->init()) {
                keyManager->deleteLater();
                keyManager = 0;
            }
        }

        if (!keyboardManager) {
            keyboardManager = new CuteimeKeyboardManager(this);
            if (!keyboardManager->init()) {
                keyboardManager->deleteLater();
                keyboardManager = 0;
            }
        }

        if (applicationManager) {
            applicationManager->setWindow(w->window()->winId());
            applicationManager->setWidget(w->winId());
            applicationManager->setFocus(true);
        }
        update();
    } else {
        if (!currentItem.to.isEmpty()) {
            sendCommitString(currentItem.to.join(""), focusWidget->winId());
            currentItem = CuteimePreeditItem();
            if (preeditManager)
                preeditManager->setItem(currentItem);
        }
        if (applicationManager) {
            if (applicationManager->widget() == focusWidget->winId()) {
                applicationManager->setWindow(0);
                applicationManager->setWidget(0);
                applicationManager->setFocus(false);
            }
            applicationManager->deleteLater();
            applicationManager = 0;
        }

        if (inputMethodManager) {
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }

        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(itemChanged(CuteimePreeditItem)), this, SLOT(itemChanged(CuteimePreeditItem)));
            disconnect(preeditManager, SIGNAL(committed(QString, qulonglong)), this, SLOT(sendCommitString(QString, qulonglong)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }

        if (keyManager) {
            keyManager->deleteLater();
            keyManager = 0;
        }

        if (keyboardManager) {
            keyboardManager->setVisible(false);
            keyboardManager->deleteLater();
            keyboardManager = 0;
        }

        focusWidget = w;
    }
    cuteimeDebugOut();
}

void InputContext::Private::widgetDestroyed(QWidget *w)
{
    cuteimeDebugIn() << w;
    cuteimeDebugOut();
}

void InputContext::Private::itemChanged(const CuteimePreeditItem &item)
{
    cuteimeDebugIn() << item;
    currentItem = item;
    if (!timer.isActive()) {
        timer.start();
    }
    cuteimeDebugOut();
}

void InputContext::Private::sendPreeditString()
{
    cuteimeDebugIn();
    QList<QInputMethodEvent::Attribute> attrs;

    bool textFormatAdded = false;
    if (currentItem.cursor > -1) {
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, currentItem.cursor, !currentItem.selection, QVariant()));
        if (currentItem.selection != 0) {
            QTextCharFormat format;
            format.setForeground(QPalette().brush(QPalette::HighlightedText));
            format.setBackground(QPalette().brush(QPalette::Highlight));
            int start = currentItem.cursor;
            start = qMin(start, start + currentItem.selection);
            int len = qAbs(currentItem.selection);
            attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, start, len, format));
            textFormatAdded = true;
        }
    }

    if (!textFormatAdded)
    {
        QTextCharFormat format;
        format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0, currentItem.to.join("").length(), format));
    }

    QInputMethodEvent e(currentItem.to.join(""), attrs);
    q->sendEvent(e);
    update();
    cuteimeDebugOut();
}

void InputContext::Private::sendCommitString(const QString &commitString, qulonglong target)
{
    if (!focusWidget || focusWidget->winId() != target) return;
    cuteimeDebugIn() << commitString << target;

    {
        QList<QInputMethodEvent::Attribute> attrs;
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, 0, 1, QVariant()));
        QInputMethodEvent e(QString::null, attrs);
        e.setCommitString(commitString);
        QApplication::sendEvent(focusWidget, &e);
//  q->sendEvent( e );
    }
    metaObject()->invokeMethod(this, "update", Qt::QueuedConnection);
    cuteimeDebugOut();
}

void InputContext::Private::update()
{
    cuteimeDebugIn();
    QWidget *widget = q->focusWidget();
    if (widget && q->isComposing()) {
        QRect r = widget->inputMethodQuery(Qt::ImMicroFocus).toRect();
        QPoint topleft = widget->mapToGlobal(QPoint(0, 0));
        r.translate(topleft);
        if (preeditManager) {
            preeditManager->setRect(r);
            preeditManager->setFont(widget->inputMethodQuery(Qt::ImFont).value<QFont>());
            preeditManager->setCursorPosition(widget->inputMethodQuery(Qt::ImCursorPosition).toInt());
            preeditManager->setSurroundingText(widget->inputMethodQuery(Qt::ImSurroundingText).toString());
            preeditManager->setCurrentSelection(widget->inputMethodQuery(Qt::ImCurrentSelection).toString());
            preeditManager->setMaximumTextLength(widget->inputMethodQuery(Qt::ImMaximumTextLength).toInt());
        }
    }
    cuteimeDebugOut();
}

InputContext::InputContext(QObject *parent)
    : QInputContext(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

InputContext::~InputContext()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool InputContext::filterEvent(const QEvent *event)
{
    cuteimeDebugIn();
    bool ret = false;
    switch (event->type()) {
    case QEvent::KeyPress:
        {
            cuteimeDebugIn() << event;
            const QKeyEvent *e = static_cast<const QKeyEvent*>(event);
            if (d->keyManager) {
                ret = d->keyManager->keyPress(e->text(), e->key(), e->modifiers(), e->isAutoRepeat());
                cuteimeDebug() << e->text() << e->key() << e->modifiers() << e->isAutoRepeat() << ret;
            }
            cuteimeDebugOut() << ret;
            break;
        }
    case QEvent::KeyRelease:
        {
            cuteimeDebugIn() << event;
            const QKeyEvent *e = static_cast<const QKeyEvent*>(event);
            if (d->keyManager) {
                ret = d->keyManager->keyRelease(e->text(), e->key(), e->modifiers(), e->isAutoRepeat());
                cuteimeDebug() << e->text() << e->key() << e->modifiers() << e->isAutoRepeat() << ret;
            }
            cuteimeDebugOut() << ret;
            break;
        }
    case QEvent::RequestSoftwareInputPanel:
        {
            cuteimeDebugIn() << event;
            if (d->keyboardManager)
                d->keyboardManager->setVisible(true);
            ret = true;
            cuteimeDebugOut() << ret;
            break;
        }
    case QEvent::CloseSoftwareInputPanel:
        {
            cuteimeDebugIn() << event;
            if (d->keyboardManager)
                d->keyboardManager->setVisible(false);
            ret = true;
            cuteimeDebugOut() << ret;
            break;
        }
    default:
        break;
    }
    update();
    cuteimeDebugOut();
    return ret;
}

QFont InputContext::font() const
{
    QFont ret;
// if( focusWidget() )
// {
//  ret = focusWidget()->inputMethodQuery( Qt::ImFont ).value<QFont>();
// }
    return ret;
}

QString InputContext::identifierName()
{
    return "cuteime";
}

bool InputContext::isComposing() const
{
    if (d && d->applicationManager) {
        return d->applicationManager->composing();
    }
    return false;
}

QString InputContext::language()
{
    return d->inputMethodManager->locale();
}

void InputContext::update()
{
    d->update();
}

void InputContext::reset()
{
    cuteimeDebugIn();
    if (d->applicationManager) {
        d->applicationManager->exec(CuteimeApplicationManager::Reset);
    }
    update();
    cuteimeDebugOut();
}

void InputContext::setFocusWidget(QWidget *w)
{
    cuteimeDebugIn() << w;
    d->setFocusWidget(w);
    QInputContext::setFocusWidget(w);
    update();
    cuteimeDebugOut();
}

void InputContext::widgetDestroyed(QWidget *w)
{
    d->widgetDestroyed(w);
}

#include "inputcontext.moc"
