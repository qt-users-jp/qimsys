/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#include "qboolsignal.h"

QBoolSignal::QBoolSignal(QObject *sender, const char *signal, bool value, QObject *parent)
    : QObject(parent)
    , _value(value)
{
    connect(sender, signal, this, SLOT(changed(bool)));
}

bool QBoolSignal::value() const
{
    return _value;
}

void QBoolSignal::changed(bool o)
{
    if (_value == o) return;
    _value = o;
    if (_value)
        emit on();
    else
        emit off();
}

QBoolsSignal::QBoolsSignal(QBoolSignal *b1, QBoolSignal *b2, Operator op, QObject *parent)
    : QObject(parent)
    , _b1(b1)
    , _b2(b2)
    , _op(op)
{
    switch (_op) {
    case And:
        _value = _b1->value() && _b2->value();
        break;
    case Or:
        _value = _b1->value() || _b2->value();
        break;
    }
    connect(_b1, SIGNAL(on()), this, SLOT(changed()));
    connect(_b1, SIGNAL(off()), this, SLOT(changed()));
    connect(_b2, SIGNAL(on()), this, SLOT(changed()));
    connect(_b2, SIGNAL(off()), this, SLOT(changed()));
}

bool QBoolsSignal::value() const
{
    return _value;
}

void QBoolsSignal::changed()
{
    bool v;
    switch (_op) {
    case And:
        v = _b1->value() && _b2->value();
        break;
    case Or:
        v = _b1->value() || _b2->value();
        break;
    }
    if (v == _value) return;
    _value = v;
    if (_value)
        emit on();
    else
        emit off();
}
