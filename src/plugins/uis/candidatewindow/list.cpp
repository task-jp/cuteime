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

#include "list.h"
#include "ui_list.h"

#include <cuteimedebug.h>

#include <cuteimeapplicationmanager.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimecandidatemanager.h>

#include <QDesktopWidget>
#include <QTimer>

using namespace CandidateWindow;

class List::Private : private QObject
{
    Q_OBJECT
public:

    Private(CuteimeAbstractPluginObject *object, List *parent);
    ~Private();

public slots:
    void move();

private slots:
    void init();

    void focusChanged(QWidget *previous, QWidget *current);

    void composingChanged(bool composing);

    void rectChanged(const QRect &rect);

    void itemsChanged(const CuteimeConversionItemList &candidates);
    void candidateIndexChanged(int index);

    void itemClicked(QListWidgetItem* item);

private:
    void setupUi();

    void setVisible(bool visible);

private:
    List *q;
    Ui::List ui;
    CuteimeAbstractPluginObject *plugin;

    CuteimeApplicationManager *applicationManager;
    CuteimeInputMethodManager *inputMethodManager;
    CuteimePreeditManager *preeditManager;
    CuteimeCandidateManager *candidateManager;

public:
    QWidget *lastFocusedWidget;
};

List::Private::Private(CuteimeAbstractPluginObject *object, List *parent)
    : QObject(parent)
    , q(parent)
    , plugin(object)
    , applicationManager(0)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , lastFocusedWidget(0)
{
    cuteimeDebugIn() << object << parent;
    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    cuteimeDebugOut();
}

List::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void List::Private::init()
{
    cuteimeDebugIn();

    setupUi();

    applicationManager = new CuteimeApplicationManager(this);
    applicationManager->init();
    connect(applicationManager, SIGNAL(composingChanged(bool)), this, SLOT(composingChanged(bool)));
    composingChanged(applicationManager->composing());

    cuteimeDebugOut();
}

void List::Private::setupUi()
{
    cuteimeDebugIn();

    q->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    q->setFocusPolicy(Qt::NoFocus);
    q->setLineWidth(1);
    q->setFrameShape(QFrame::Box);
    q->setFrameShadow(QFrame::Plain);

    // ui part
    ui.setupUi(q);

    connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));

    cuteimeDebugOut();
}

void List::Private::focusChanged(QWidget *previous, QWidget *current)
{
    cuteimeDebugIn() << previous << current;
    if (current != 0 && !q->isAncestorOf(current)) {
        cuteimeDebug() << previous << current;
        lastFocusedWidget = current;
    }
    cuteimeDebugOut();
}

void List::Private::composingChanged(bool composing)
{
    cuteimeDebugIn() << composing;
    if (composing) {
        if (!candidateManager) {
            candidateManager = new CuteimeCandidateManager(this);
            candidateManager->init();
            connect(candidateManager, SIGNAL(itemsChanged(CuteimeConversionItemList)), this, SLOT(itemsChanged(CuteimeConversionItemList)));
            connect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(candidateIndexChanged(int)));
        }
        itemsChanged(candidateManager->items());
        candidateIndexChanged(candidateManager->currentIndex());
    } else {
        itemsChanged(CuteimeConversionItemList());
        candidateIndexChanged(-1);
        if (candidateManager) {
            disconnect(candidateManager, SIGNAL(itemsChanged(CuteimeConversionItemList)), this, SLOT(itemsChanged(CuteimeConversionItemList)));
            disconnect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(candidateIndexChanged(int)));
            candidateManager->deleteLater();
            candidateManager = 0;
        }
    }
    cuteimeDebugOut();
}

void List::Private::setVisible(bool visible)
{
    cuteimeDebugIn() << visible;
    q->setVisible(visible);
    if (visible) {
        q->raise();
        connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(move()));
        connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(move()));

        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
        }

        if (!preeditManager) {
            preeditManager = new CuteimePreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(rectChanged(QRect)), this, SLOT(rectChanged(QRect)));
        }
        rectChanged(preeditManager->rect());
    } else {
        disconnect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(move()));
        disconnect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(move()));

        if (inputMethodManager) {
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }

        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(rectChanged(QRect)), this, SLOT(rectChanged(QRect)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }
    }
    cuteimeDebugOut();
}

void List::Private::move()
{
    if (!preeditManager) return;
    cuteimeDebugIn() << sender();
    QRect desktop = QApplication::desktop()->screenGeometry();
    QRect rect = preeditManager->rect();

    QPoint pos = rect.topLeft();
    if (pos.x() + q->width() > desktop.right()) {
        pos.setX(desktop.right() - q->width());
    }
    if (rect.bottom() + q->height() < desktop.height()) {
        pos.setY(rect.bottom());
    } else if (rect.top() - q->height() > desktop.top()) {
        pos.setY(rect.top() - q->height());
    } else {
        pos.setY(rect.bottom() - q->height());
    }

    q->move(pos);

    cuteimeDebugOut();
}

void List::Private::rectChanged(const QRect &rect)
{
    cuteimeDebugIn() << rect;
    move();
    cuteimeDebugOut();
}

void List::Private::candidateIndexChanged(int index)
{
    cuteimeDebugIn() << index;
//    for (int i = 0; i < ui.listWidget->count(); i++) {
//        cuteimeDebug() << i << ui.listWidget->item(i)->text();
//    }
    ui.listWidget->blockSignals(true);
    ui.listWidget->setCurrentRow(index);
    ui.listWidget->blockSignals(false);
    move();
    cuteimeDebugOut();
}

void List::Private::itemsChanged(const CuteimeConversionItemList &candidates)
{
    cuteimeDebugIn() << candidates;

    ui.listWidget->clear();

    QFontMetrics met(q->font());
    int width = 0;
    int height = 0;

    foreach (const CuteimeConversionItem &candidate, candidates) {
        int w = met.width(candidate.to);
        QListWidgetItem *item = new QListWidgetItem(candidate.to);
        ui.listWidget->addItem(item);
        width = qMax(width, w);
    }

    if (!candidates.isEmpty()) {
        int h = qMax(met.height(), ui.listWidget->visualItemRect(ui.listWidget->item(0)).height());
        height = qMin(candidates.length(), 10) * h + q->lineWidth() * 2 + ui.listWidget->lineWidth() * 2;
        q->setFixedSize(width + 100, height);
        q->update();
    }

    ui.listWidget->scrollToTop();

    setVisible(!candidates.isEmpty());
    cuteimeDebugOut();
}

void List::Private::itemClicked(QListWidgetItem* item)
{
    cuteimeDebugIn();
    if (candidateManager) {
        candidateManager->setCurrentIndex(ui.listWidget->currentRow());
    }
    if (inputMethodManager && preeditManager) {
        CuteimePreeditItem preeditItem = preeditManager->item();
        if (preeditItem.to.join("").length() == preeditItem.cursor + preeditItem.selection) {
            inputMethodManager->execute(QLatin1String("Commit all"));
        } else {
            inputMethodManager->execute(QLatin1String("Move cursor next"));
        }
    }
    cuteimeDebugOut();
}

List::List(CuteimeAbstractPluginObject *plugin, QWidget *parent)
    : QFrame(parent)
{
    cuteimeDebugIn();
    d = new Private(plugin, this);
    cuteimeDebugOut();
}

List::~List()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

bool List::event(QEvent *e)
{
    switch(e->type()) {
    case QEvent::WindowActivate:
        cuteimeDebugIn();
        if (d->lastFocusedWidget)
            d->lastFocusedWidget->activateWindow();
        cuteimeDebugOut();
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

void List::resizeEvent(QResizeEvent *e)
{
    if (isVisible()) {
        hide();
        QMetaObject::invokeMethod(this, "show", Qt::QueuedConnection);
    }
    QFrame::resizeEvent(e);
}

void List::moveEvent(QMoveEvent *e)
{
    if (isVisible()) {
        hide();
        QMetaObject::invokeMethod(this, "show", Qt::QueuedConnection);
    }
    QFrame::moveEvent(e);
}

#include "list.moc"
