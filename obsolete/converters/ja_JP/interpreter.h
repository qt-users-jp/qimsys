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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QObject>

namespace ja_JP
{
class Interpreter : public QObject
{
    Q_OBJECT
public:
    Interpreter(QObject *parent = 0);
    ~Interpreter();

    QString rawString() const;
    QString inputString(int character = -1) const;

public slots:
    void input(const QString &string);
    void remove(int length);
    void termiateInput();
    void clear();

signals:
    void rawStringChanged(const QString &inputString);
    void inputStringChanged(const QString &inputString);
    void cursorPositionChanged(int cursorPosition);
    void selectionLengthChanged(int selectionLength);

private:
    class Private;
    Private *d;
};

}

#endif // INTERPRETER_H
