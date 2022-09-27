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

#ifndef CUTEIMEKEYWIDGET_H
#define CUTEIMEKEYWIDGET_H

#include <QToolButton>
#include "cuteimeglobal.h"
#include "cuteimekeysequence.h"

class CUTEIME_EXPORT CuteimeKeyWidget : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(CuteimeKeySequence sequence READ sequence WRITE setSequence NOTIFY sequenceChanged)
    Q_DISABLE_COPY(CuteimeKeyWidget)
public:
    explicit CuteimeKeyWidget(QWidget *parent = 0);
    ~CuteimeKeyWidget();

    virtual QSize sizeHint() const;

    CuteimeKeySequence sequence() const;
    void setSequence(const CuteimeKeySequence &sequence);

public slots:
    void reset();
    void clear();

signals:
    void sequenceChanged(CuteimeKeySequence sequence);

private:
    class Private;
    Private *d;
};
#endif//CUTEIMEKEYWIDGET_H
