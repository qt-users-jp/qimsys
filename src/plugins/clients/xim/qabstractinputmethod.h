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

#ifndef QABSTRACTINPUTMETHOD_H
#define QABSTRACTINPUTMETHOD_H

#include <QObject>

class QAbstractInputMethod : public QObject
{
    Q_OBJECT
public:
    enum UpdateType {
        Update,
        FocusIn,
        FocusOut,
        Reset,
        Destroyed,
    };
    explicit QAbstractInputMethod(QObject *parent = 0);
    ~QAbstractInputMethod();

    virtual bool filter(int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat) = 0;
    virtual void reset() = 0;
    virtual void updateHandler(UpdateType type) = 0;

    virtual bool isComposing() const;

protected slots:
    virtual void setComposing(bool composing);

signals:
    void composingChanged(bool composing);

protected:
    virtual void sendCommitString(const QString &commitString) = 0;
    virtual void sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength = 0) = 0;

private:
    class Private;
    Private *d;
};

#endif // QABSTRACTINPUTMETHOD_H
