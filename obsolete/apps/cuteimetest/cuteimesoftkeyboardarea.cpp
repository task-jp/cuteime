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

#include "cuteimesoftkeyboardarea.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QScrollArea>

namespace Cuteime{
    enum {
        ShowSoftwareInputPanel = QEvent::User + 1234,
        HideSoftwareInputPanel,
        SoftwareInputPanelShown,
        SoftwareInputPanelHidden
    };
}

CuteimeSoftKeyboardArea::CuteimeSoftKeyboardArea(QWidget *parent)
    : QWidget(parent)
    , _visible(false)
    , _buddy(0)
{
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(resetGeometry()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(resetGeometry()));
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
    qApp->installEventFilter(this);
    focusChanged(0, qApp->focusWidget());
}

void CuteimeSoftKeyboardArea::setBuddy(QScrollArea *buddy)
{
    if (_buddy == buddy) return;
    _buddy = buddy;
 }

bool CuteimeSoftKeyboardArea::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case Cuteime::SoftwareInputPanelShown:
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "Cuteime::SoftwareInputPanelShown";
        _visible = true;
        show();
        QMetaObject::invokeMethod(this, "ensureWidgetVisible", Qt::QueuedConnection);
        return true;
    case Cuteime::SoftwareInputPanelHidden:
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "Cuteime::SoftwareInputPanelHidden";
        _visible = false;
        hide();
        return true;
    default:
        break;
    }
    return QWidget::eventFilter(o, e);
}

void CuteimeSoftKeyboardArea::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
//    QEvent event((QEvent::Type)Cuteime::ShowSoftwareInputPanel);
//    QApplication::sendEvent(QApplication::instance(), &event);
}

void CuteimeSoftKeyboardArea::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
//    QEvent event((QEvent::Type)Cuteime::HideSoftwareInputPanel);
//    QApplication::sendEvent(QApplication::instance(), &event);
}

void CuteimeSoftKeyboardArea::resetGeometry()
{
    QRect desktop = QApplication::desktop()->screenGeometry();
    desktop.setTop(desktop.top() + desktop.height() / 2);
    setFixedSize(desktop.size());
}

void CuteimeSoftKeyboardArea::focusChanged(QWidget *old, QWidget *now)
{
    if (old) {
//        old->removeEventFilter(this);
    }

    if (now) {
//        now->installEventFilter(this);
        if (_visible) show();
    } else {
        hide();
    }
}

void CuteimeSoftKeyboardArea::ensureWidgetVisible()
{
    if (_buddy) {
        _buddy->ensureWidgetVisible(QApplication::focusWidget());
    }
}
