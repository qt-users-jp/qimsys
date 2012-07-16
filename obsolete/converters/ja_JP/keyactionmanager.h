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

#ifndef KEYACTIONMANAGER_H
#define KEYACTIONMANAGER_H

#include <QObject>
#include "keyaction.h"
#include "qimsyskeysequence.h"
#include "namespace.h"

namespace ja_JP
{

struct KeyActionData {
    State state;
    QimsysKeySequence key;
    KeyAction *action;
    bool user;
};

class KeyActionDataList : public QList<KeyActionData>
{
public:
    KeyActionDataList();
    KeyActionDataList(const QString &fileName);
    ~KeyActionDataList();
    KeyActionDataList &operator=(const KeyActionDataList &other);

    void merge(const KeyActionDataList &other);

    const QString &fileName() const;

    void read(const QString &fileName);
    void save() const;

private:
    class Private;
    Private *d;
};

class KeyActionManager : public QObject
{
    Q_OBJECT
public:
    static KeyActionManager *instance();
private:
    KeyActionManager(QObject *parent = 0);
public:
    ~KeyActionManager();

    void addAction(KeyAction *action);
    const KeyActionList &actions() const;
    const KeyActionDataList &list() const;
    void setList(const KeyActionDataList &list);
    KeyAction *action(const QString &name) const;

signals:
    void listChanged(const KeyActionDataList &list);

private:
    class Private;
    Private *d;
};

}
#endif // KEYACTIONMANAGER_H
