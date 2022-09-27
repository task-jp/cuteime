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

#include "zinniawidget.h"

#include <cuteimedebug.h>

#include <QMouseEvent>
#include <QPainter>
#include <QSettings>

namespace zinnia {

class ZinniaWidget::Private
{
public:
    Private(ZinniaWidget *parent);
    ~Private();

    void start();
    void append(const QPoint &pos);
    void end();
    void clear();
private:
    ZinniaWidget *q;
    bool started;
public:
    QList<QPolygon> data;
    QPoint oldPos;
};

}

using namespace zinnia;

ZinniaWidget::Private::Private(ZinniaWidget *parent)
    : q(parent)
    , started(false)
{
    cuteimeDebugIn() << parent;
    q->setFocusPolicy(Qt::NoFocus);
    QPalette palette = q->palette();
    palette.setColor(QPalette::Window, palette.color(QPalette::Base));
    q->setPalette(palette);
    cuteimeDebugOut();
}

ZinniaWidget::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void ZinniaWidget::Private::start()
{
    cuteimeDebugIn();
    started = true;
    data.append(QPolygon());
    cuteimeDebugOut();
}

void ZinniaWidget::Private::append(const QPoint &pos)
{
    if (!started) return;
    cuteimeDebugIn() << pos;
    data[data.size() - 1] << pos;
    q->update();
    cuteimeDebugOut();
}

void ZinniaWidget::Private::end()
{
    if (!started) return;
    cuteimeDebugIn();
    emit q->dataChanged(data);
    started = false;
    cuteimeDebugOut();
}

void ZinniaWidget::Private::clear()
{
    started = false;
    if (data.isEmpty()) return;
    cuteimeDebugIn();
    data.clear();
    emit q->dataChanged(data);
    q->update();
    cuteimeDebug();
}

ZinniaWidget::ZinniaWidget(QWidget *parent)
    : QWidget(parent)
{
    d = new Private(this);
}

ZinniaWidget::~ZinniaWidget()
{
    delete d;
}

QSize ZinniaWidget::sizeHint() const
{
    return QSize(200, 200);
}

void ZinniaWidget::clear()
{
    d->clear();
}

void ZinniaWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    int x = 0;
    int y = 0;
    int w = width() - 1;
    int h = height() - 1;
    int s = 0;

    int m = 0;
    if (w > h) {
        s = h;
        m = s / 30;
        s -= m;
        x = (w - h) / 2 + m / 2;
        y = m / 2;
    } else {
        s = w;
        m = s / 30;
        s -= m;
        x = m / 2;
        y = (h - w) / 2 + m / 2;
    }

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
//    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(palette().color(QPalette::Dark));
    pen.setWidth(m);
    p.setPen(pen);
    p.drawRoundedRect(x, y, s, s, s / 10, s / 10);
    p.drawLine(x, y + s / 2, x + s, y + s / 2);
    p.drawLine(x + s / 2, y, x + s / 2, y + s);

    pen.setColor(palette().color(QPalette::Text));
    p.setPen(pen);
    foreach (const QPolygon &polygon, d->data) {
        p.drawPolyline(polygon);
    }
}

void ZinniaWidget::mousePressEvent(QMouseEvent *e)
{
    int w = width();
    int h = height();
    if (QRect(0, 0, w / 10, h / 10).contains(e->pos())
        || QRect(w - w / 10, 0, w / 10, h / 10).contains(e->pos())
        || QRect(0, h - h / 10, w / 10, h / 10).contains(e->pos())
        || QRect(w - w / 10, h - h / 10, w / 10, h / 10).contains(e->pos())) {
        d->oldPos = e->pos();
    } else {
        d->start();
        d->append(e->pos());
    }
    e->accept();
}

void ZinniaWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (d->oldPos.isNull()) {
        d->append(e->pos());
    } else {
        move(pos() + e->pos() - d->oldPos);
    }
    e->accept();
}

void ZinniaWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->oldPos.isNull()) {
        d->append(e->pos());
        d->end();
    } else {
        move(pos() + e->pos() - d->oldPos);
        d->oldPos = QPoint();
    }
    e->accept();
}

void ZinniaWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    d->clear();
    e->accept();
}

void ZinniaWidget::showEvent(QShowEvent *e)
{
    cuteimeDebugIn();
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    restoreGeometry(settings.value(QLatin1String("Geometry")).toByteArray());
    QWidget::showEvent(e);
    cuteimeDebugOut();
}

void ZinniaWidget::hideEvent(QHideEvent *e)
{
    cuteimeDebugIn();
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    settings.setValue(QLatin1String("Geometry"), saveGeometry());
    QWidget::hideEvent(e);
    cuteimeDebugOut();
}
