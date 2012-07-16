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

#ifndef KEYACTION_H
#define KEYACTION_H

#include <QObject>
#include <QList>
#include "japanese.h"

namespace ja_JP
{
class KeyAction : public QObject
{
    Q_OBJECT
public:
    explicit KeyAction(int states, const char *name, QObject *parent = 0);
    ~KeyAction();

    int states() const;
    const char *name() const;

    void execute();

    KeyAction &operator=(const KeyAction &other);

signals:
    void triggered();

private:
    class Private;
    Private *d;
};

typedef QList<KeyAction*> KeyActionList;
}

#endif // KEYACTION_H
