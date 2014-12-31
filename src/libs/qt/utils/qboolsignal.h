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

#ifndef QBOOLSIGNAL_H
#define QBOOLSIGNAL_H

#include <QObject>
#include "qimsysglobal.h"

class QIMSYS_EXPORT QBoolSignal : public QObject
{
    Q_OBJECT
public:
    QBoolSignal(QObject *sender, const char *signal, bool value, QObject *parent = 0);

    bool value() const;
signals:
    void on();
    void off();

private slots:
    void changed(bool on);

private:
    bool _value;
};

class QIMSYS_EXPORT QBoolsSignal : public QObject
{
    Q_OBJECT
public:
    enum Operator {
        And,
        Or,
    };
    QBoolsSignal(QBoolSignal *b1, QBoolSignal *b2, Operator op = And, QObject *parent = 0);

    bool value() const;
signals:
    void on();
    void off();

private slots:
    void changed();

private:
    QBoolSignal *_b1;
    QBoolSignal *_b2;
    Operator _op;
    bool _value;
};


#endif // QBOOLSIGNAL_H
