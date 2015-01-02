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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QObject>
#include "namespace.h"

namespace ja_JP
{

class InputManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state WRITE setState)
    Q_PROPERTY(int character READ character WRITE setCharacter)
    Q_PROPERTY(Typing typing READ typing WRITE setTyping)
public:
    static InputManager *instance();
private:
    InputManager(QObject *parent = 0);
public:
    ~InputManager();

    State state() const;
    int character() const;
    bool isTemp() const;
    Typing typing() const;

public slots:
    void setState(State state);
    void setCharacter(int character, bool temp = false);
    void setTyping(Typing typing);

signals:
    void stateChanged(State state);
    void characterChanged(int character);
    void typingChanged(Typing typing);

private:
    class Private;
    Private *d;
};

}

#endif // INPUTMANAGER_H
