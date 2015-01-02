/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
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

#include "keyaction.h"

#include <QSpinBox>
#include <QComboBox>

using namespace ja_JP;

class KeyAction::Private
{
public:
    int states;
    const char *name;
};

KeyAction::KeyAction(int states, const char *name, QObject *parent)
    : QObject(parent)
{
    d = new Private;
    d->states = states;
    d->name = name;
}

KeyAction::~KeyAction()
{
    delete d;
}

KeyAction &KeyAction::operator=(const KeyAction & other)
{
    d->states = other.d->states;
    d->name = other.d->name;
    return *this;
}

int KeyAction::states() const
{
    return d->states;
}

const char *KeyAction::name() const
{
    return d->name;
}

void KeyAction::execute()
{
    emit triggered();
}
