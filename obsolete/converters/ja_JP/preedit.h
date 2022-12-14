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

#ifndef PREEDIT_H
#define PREEDIT_H

#include "cuteimepreedit.h"
#include "cuteimeconversionitem.h"
#include "japanese.h"

namespace ja_JP
{

class Preedit : public CuteimePreedit
{
    Q_OBJECT
public:
    static Preedit *instance();
private:
    Preedit(QObject *parent = 0);
public:
    ~Preedit();
    bool init();

    void commit();

    void move(int offset, bool selection);

    int conversionIndex() const;
    int convercionCount() const;

public slots:
    void cancel();
    void setConversions(const CuteimeConversionItemList &conversions);
    void setCurrentText(const QString &text);

signals:
    void committed(const CuteimeConversionItemList &conversions);

private:
    class Private;
    Private *d;
};

}
#endif//PREEDIT_H
