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

#ifndef CUTEIMEDYNAMICTRANSLATOR_H
#define CUTEIMEDYNAMICTRANSLATOR_H

#include <QObject>
#include "cuteimeglobal.h"

#define trConnect CuteimeDynamicTranslator::setTranslation

class CUTEIME_EXPORT CuteimeDynamicTranslator : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CuteimeDynamicTranslator)
private:
    CuteimeDynamicTranslator(QObject *parent = 0);
    static CuteimeDynamicTranslator &instance();
public:
    ~CuteimeDynamicTranslator();

    static void setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName);

private:
    class Private;
    Private *d;
};

#endif//CUTEIMEDYNAMICTRANSLATOR_H
