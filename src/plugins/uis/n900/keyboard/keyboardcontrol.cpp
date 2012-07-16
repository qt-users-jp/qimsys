/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010 by Tasuku Suzuki <stasuku@gmail.com>                 *
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

#include "keyboardcontrol.h"

#include <qimsysdebug.h>

class KeyboardControl::Private
{
public:
    bool isVisible;
    int x;
    int y;
    int width;
    int height;
};

KeyboardControl::KeyboardControl(QObject *parent)
    : QObject(parent)
{
    d = new Private;
    d->isVisible = false;
    d->x = 0;
    d->y = 0;
    d->width = 0;
    d->height = 0;
}

KeyboardControl::~KeyboardControl()
{
    delete d;
}

bool KeyboardControl::isVisible() const
{
    return d->isVisible;
}

void KeyboardControl::setVisible(bool isVisible)
{
    if (d->isVisible == isVisible) return;
    d->isVisible = isVisible;
    emit visibleChanged(isVisible);
}

int KeyboardControl::x() const
{
    return d->x;
}

void KeyboardControl::setX(int x)
{
    if (d->x == x) return;
    qimsysDebugIn() << x;
    d->x = x;
    emit xChanged(x);
    qimsysDebugOut();
}

int KeyboardControl::y() const
{
    return d->y;
}

void KeyboardControl::setY(int y)
{
    if (d->y == y) return;
    qimsysDebugIn() << y;
    d->y = y;
    emit yChanged(y);
    qimsysDebugOut();
}

int KeyboardControl::width() const
{
    return d->width;
}

void KeyboardControl::setWidth(int width)
{
    if (d->width == width) return;
    qimsysDebugIn() << width;
    d->width = width;
    emit widthChanged(width);
    qimsysDebugOut();
}

int KeyboardControl::height() const
{
    return d->height;
}

void KeyboardControl::setHeight(int height)
{
    if (d->height == height) return;
    qimsysDebugIn() << height;
    d->height = height;
    emit heightChanged(height);
    qimsysDebugOut();
}
