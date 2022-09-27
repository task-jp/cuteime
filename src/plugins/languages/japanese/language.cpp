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

#include "language.h"

#include <cuteimedebug.h>
#include <cuteimedynamictranslator.h>

namespace Japanese {

class Language::Private : private QObject
{
    Q_OBJECT
public:
    Private(Language *parent);
    ~Private();

private:
    Language *q;
};

}

using namespace Japanese;

Language::Private::Private(Language *parent)
    : QObject(parent)
    , q(parent)
{
    cuteimeDebugIn() << parent;

    q->setLocale("ja_JP");
    q->setLanguage("Japanese");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/icons/japan.png"));
#endif
    trConnect(this, QT_TR_NOOP("Japanese"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese language"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Language"), q, "categoryName");

    q->setActive(true);
    cuteimeDebugOut();
}

Language::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

Language::Language(QObject *parent)
    : CuteimeLanguage(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Language::~Language()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "language.moc"
