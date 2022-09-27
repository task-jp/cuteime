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

#include <QCursor>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QPointer>
#include <QSettings>
#include <QTextCharFormat>
#include <QProcess>
#include <QTimer>

#include <QWindow>
#include <QWidget>
#include <QQuickItem>
#include <QQuickWindow>

class InputContext::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputContext *parent);
    ~Private();

    void setFocusObject(QObject *o);

public slots:
    void update(Qt::InputMethodQueries queries = Qt::ImQueryInput);
    void commit();

private slots:
    void init();
    void itemChanged(const CuteimePreeditItem &item);
    void sendPreeditString();
    void sendCommitString(const QString &commitString, qulonglong target);

private:
    InputContext *q;
    QPointer<QObject> focusObject;
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
    , applicationManager(0)
    , inputMethodManager(0)
    , preeditManager(0)
    , keyManager(0)
    , keyboardManager(0)
{
    cuteimeDebugIn() << parent;
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    init();
    QGuiApplication::restoreOverrideCursor();
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

static void widgetAndWindowFor(const QObject *object, qulonglong *widgetId, qulonglong *windowId)
{
    cuteimeDebugIn() << object;

    *widgetId = (qulonglong)object;

    cuteimeDebug();
    if (windowId) {
        QWindow *window = 0;
        const QWidget *widget = qobject_cast<const QWidget *>(object);
        const QQuickItem *item = qobject_cast<const QQuickItem *>(object);
        if (widget) {
            window = widget->windowHandle();
        } else if (item) {
            window = item->window();
        }

        if (window) {
            *windowId = window->winId();
            cuteimeDebug() << *windowId;
        }
    }

    cuteimeDebugOut() << *widgetId;
}

void InputContext::Private::setFocusObject(QObject *o)
{
    if (focusObject == o) return;
    cuteimeDebugIn() << o;

    qulonglong nextWindow = 0;
    qulonglong nextWidget = 0;
    widgetAndWindowFor(o, &nextWidget, &nextWindow);

    if (o) {
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
            applicationManager->setWindow(nextWindow);
            applicationManager->setWidget(nextWidget);
            applicationManager->setFocus(true);
        }
        //cuteimeDebugOn();
        //update();
        //cuteimeDebugOff();
    } else {
        if (applicationManager) {
            if (applicationManager->widget() == (qulonglong)focusObject.data()) {
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
    }
    focusObject = o;
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
    if (!focusObject) return;
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
    QCoreApplication::sendEvent(focusObject, &e);
    //update();
    cuteimeDebugOut();
}

void InputContext::Private::sendCommitString(const QString &commitString, qulonglong target)
{
    if (!focusObject || (qulonglong)focusObject.data() != target) return;
    cuteimeDebugIn() << commitString << target;

    {
        QList<QInputMethodEvent::Attribute> attrs;
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, 0, 1, QVariant()));
        QInputMethodEvent e(QString::null, attrs);
        e.setCommitString(commitString);
        QCoreApplication::sendEvent(focusObject, &e);
    }
    //metaObject()->invokeMethod(this, "update", Qt::QueuedConnection);
    cuteimeDebugOut();
}

void InputContext::Private::update(Qt::InputMethodQueries queries)
{
    if (!focusObject) return;
    cuteimeDebugIn();
    QInputMethodQueryEvent query(queries);
    QCoreApplication::sendEvent(focusObject, &query);
    if (queries & Qt::ImEnabled) {
        if (!query.value(Qt::ImEnabled).toBool()) {
            setFocusObject(0);
        }
    }

    if (preeditManager && queries & Qt::ImCursorRectangle) {
        QRect r = query.value(Qt::ImCursorRectangle).toRect();
        cuteimeDebug() << r;
        QPoint topLeft;
        if (QWidget *widget = qobject_cast<QWidget *>(focusObject)) {
            topLeft = widget->mapToGlobal(QPoint(0, 0));
        } else if (QQuickItem *item = qobject_cast<QQuickItem *>(focusObject)) {
            topLeft = item->mapToScene(QPointF(0, 0)).toPoint() + item->window()->geometry().topLeft();
        }
        r.translate(topLeft);
        preeditManager->setRect(r);
    }
    if (preeditManager && queries & Qt::ImFont) {
        preeditManager->setFont(query.value(Qt::ImFont).value<QFont>());
    }
    if (preeditManager && queries & Qt::ImCursorPosition) {
        preeditManager->setCursorPosition(query.value(Qt::ImCursorPosition).toInt());
    }
    if (preeditManager && queries & Qt::ImSurroundingText) {
        preeditManager->setSurroundingText(query.value(Qt::ImSurroundingText).toString());
    }
    if (preeditManager && queries & Qt::ImCurrentSelection) {
        preeditManager->setCurrentSelection(query.value(Qt::ImCurrentSelection).toString());
    }
    if (preeditManager && queries & Qt::ImMaximumTextLength) {
        preeditManager->setMaximumTextLength(query.value(Qt::ImMaximumTextLength).toInt());
    }
    cuteimeDebugOut();
}

void InputContext::Private::commit()
{
    if (currentItem.to.isEmpty()) return;
    cuteimeDebugIn();
    sendCommitString(currentItem.to.join(""), (qulonglong)focusObject.data());
    currentItem = CuteimePreeditItem();
    if (preeditManager)
        preeditManager->setItem(currentItem);
    cuteimeDebugOut();
}

InputContext::InputContext()
    : QPlatformInputContext()
{
    cuteimeDebugIn();
    d = new Private(this);
    cuteimeDebugOut();
}

InputContext::~InputContext()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool InputContext::isValid() const
{
    cuteimeDebugIn();
    cuteimeDebugOut();
    return true;
}

void InputContext::reset()
{
    cuteimeDebugIn();
    if (d->applicationManager) {
        d->applicationManager->exec(CuteimeApplicationManager::Reset);
    }
    update(Qt::ImQueryAll);
    cuteimeDebugOut();
}

void InputContext::commit()
{
    cuteimeDebugIn();
    d->commit();
    cuteimeDebugOut();
}

void InputContext::update(Qt::InputMethodQueries queries)
{
    cuteimeDebugIn();
    d->update(queries);
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
    d->update();
    cuteimeDebugOut();
    return ret;
}

void InputContext::setFocusObject(QObject *object)
{
    cuteimeDebugIn() << object;
    d->setFocusObject(object);
    QPlatformInputContext::setFocusObject(object);
    d->update();
    cuteimeDebugOut() << object;
}

#include "inputcontext.moc"
