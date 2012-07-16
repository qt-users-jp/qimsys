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

#include "keyboard.h"

#include <qimsysdebug.h>

#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include <QTimer>

using namespace N900::Keyboard;

class Keyboard::Private : private QObject
{
    Q_OBJECT
public:

    Private(QimsysAbstractPluginObject *object, Keyboard *parent);
    ~Private();

private slots:
    void init();

private:
    Keyboard *q;
    QimsysAbstractPluginObject *plugin;
};

Keyboard::Private::Private(QimsysAbstractPluginObject *object, Keyboard *parent)
    : QObject(parent)
    , q(parent)
    , plugin(object)
{
    qimsysDebugIn() << object << parent;
    init();
    qimsysDebugOut();
}

Keyboard::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Keyboard::Private::init()
{
    qimsysDebugIn();
    q->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    q->setFocusPolicy(Qt::NoFocus);

    qimsysDebugOut();
}

Keyboard::Keyboard(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QimsysDeclarativeView(parent)
{
    qimsysDebugIn();
    d = new Private(plugin, this);
    qimsysDebugOut();
}

Keyboard::~Keyboard()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "keyboard.moc"
