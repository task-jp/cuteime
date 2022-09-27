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

#include "inputmanager.h"
#include <cuteimedebug.h>
#include <QSettings>

using namespace ja_JP;

class InputManager::Private
{
public:
    Private();

public:
    State state;
    int character;
    int characterTemp;
    Typing typing;
};

InputManager::Private::Private()
    : state(Direct)
    , character(0)
    , characterTemp(0)
{
    cuteimeDebugIn();
    cuteimeDebugOut();
}

InputManager *InputManager::instance()
{
    cuteimeDebugIn();
    static InputManager *ret = 0;
    if (!ret)
        ret = new InputManager;
    cuteimeDebugOut();
    return ret;
}

InputManager::InputManager(QObject *parent)
    : QObject(parent)
{
    cuteimeDebugIn() << parent;
    d = new Private;
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    d->character = settings.value("character", Hiragana | FullWidth).toInt();
    d->typing = static_cast<Typing>(settings.value("typing", Roma).toInt());
    cuteimeDebugOut();
}

InputManager::~InputManager()
{
    cuteimeDebugIn();
    delete d;
    cuteimeDebugOut();
}

State InputManager::state() const
{
    return d->state;
}

void InputManager::setState(State state)
{
    if (d->state == state) return;
    cuteimeDebugIn() << state;
    d->state = state;
    switch (d->state) {
    case Direct:
    case EmptyString:
        setCharacter(d->character);
        break;
    default:
        break;
    }
    emit stateChanged(d->state);
    cuteimeDebugOut();
}

int InputManager::character() const
{
    return (d->characterTemp > 0 ? d->characterTemp : d->character);
}

bool InputManager::isTemp() const
{
    return (d->characterTemp > 0);
}

void InputManager::setCharacter(int character, bool temp)
{
    if (temp) {
        if (d->characterTemp == character) return;
        d->characterTemp = character;
        emit characterChanged(d->characterTemp);
    } else {
        if (d->characterTemp > 0) {
            d->characterTemp = 0;
        } else if (d->character == character) {
            return;
        }
        QSettings settings;
        settings.beginGroup(metaObject()->className());
        settings.setValue("character", character);

        d->character = character;
        emit characterChanged(d->character);
    }
}

Typing InputManager::typing() const
{
    return d->typing;
}

void InputManager::setTyping(Typing typing)
{
    if (d->typing == typing) return;
    cuteimeDebugIn() << typing;

    QSettings settings;
    settings.beginGroup(metaObject()->className());
    settings.setValue("typing", typing);

    d->typing = typing;
    emit typingChanged(d->typing);
    cuteimeDebugOut();
}
