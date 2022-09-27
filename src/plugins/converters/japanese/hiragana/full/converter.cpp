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

#include "converter.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <cuteimedebug.h>
#include <cuteimeinputmethodmanager.h>
#include <cuteimepreeditmanager.h>
#include <cuteimedynamictranslator.h>

namespace Japanese {
    namespace Hiragana {
        namespace Full {

class Converter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Converter *parent);
    ~Private();

private slots:
    void init();

    void activeChanged(bool isActive);
    void stateChanged(uint state);

private:
    Converter *q;
    CuteimeInputMethodManager *inputMethodManager;
};

        }
    }
}

using namespace Japanese::Hiragana::Full;

Converter::Private::Private(Converter *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
{
    cuteimeDebugIn() << parent;
    init();
    cuteimeDebugOut();
}

Converter::Private::~Private()
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

void Converter::Private::init()
{
    q->setIdentifier(QLatin1String("Hiragana"));
    q->setPriority(0x10);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese(Standard)");
#ifndef CUTEIME_NO_GUI
    q->setIcon(QIcon(":/japanese/hiragana/full/resources/hiragana-full.png"));
#endif
    trConnect(this, QT_TR_NOOP("Hiragana"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese converter for Hiragana"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Converter"), q, "categoryName");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
}

void Converter::Private::activeChanged(bool isActive)
{
    cuteimeDebugIn() << isActive;
    if (isActive) {
        if (!inputMethodManager) {
            inputMethodManager = new CuteimeInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
        }
        stateChanged(inputMethodManager->state());
    } else {
        stateChanged(Reset);
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
    }
    cuteimeDebugOut();
}

void Converter::Private::stateChanged(uint state)
{
    cuteimeDebugIn() << state;
    switch (state) {
    case ConvertTo: {
        CuteimePreeditManager preeditManager;
        preeditManager.init();

        CuteimePreeditItem item = preeditManager.item();
        item.to = item.from;
        preeditManager.setItem(item);
        break;
    }
    default:
        break;
    }
    cuteimeDebugOut();
}

Converter::Converter(QObject *parent)
    : CuteimeConverter(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private(this);
    cuteimeDebugOut();
}

Converter::~Converter()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

#include "converter.moc"
