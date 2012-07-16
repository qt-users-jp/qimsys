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

#ifndef QXIMINPUTMETHOD_H
#define QXIMINPUTMETHOD_H

#include "qabstractinputmethod.h"

class QXimInputMethod : public QAbstractInputMethod
{
    Q_OBJECT
public:
    explicit QXimInputMethod(QObject *parent = 0);
    ~QXimInputMethod();

    virtual void reset();
    virtual void updateHandler(UpdateType type);

protected:
    virtual qulonglong winId() = 0;
    virtual const char *name() const = 0;
    virtual const char *locale() const = 0;
    void sendCommitString(const QString &commitString);
    void sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength = 0);

    qulonglong clientWindow() const;
    qulonglong focusWindow() const;

private:
    class Private;
    Private *d;
};

#endif // QXIMINPUTMETHOD_H
