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

#ifndef WIDGET_H
#define WIDGET_H

#include <QFrame>

class CuteimeAbstractPluginObject;

namespace ToolBar {

class Widget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QSize normalSize READ normalSize WRITE setNormalSize NOTIFY normalSizeChanged)
public:
    Widget(CuteimeAbstractPluginObject *plugin, QWidget *parent = 0);
    ~Widget();

    const QSize &normalSize() const;

public slots:
    void settingsUpdated();

signals:
    void normalSizeChanged(const QSize &normalSize);
    void languageChanged();

protected slots:
    void setNormalSize(const QSize &normalSize);

protected:
    bool event(QEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    class Private;
    Private *d;
};

}
#endif//WIDGET_H
