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

#include "object.h"
#include "client.h"

#include <qimsysdebug.h>
#include <qimsysdynamictranslator.h>

using namespace Xim;

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);

private:
    Object *q;
    Client *client;
};

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , client(0)
{
    qimsysDebugIn() << parent;
    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(Hidden);
    trConnect(this, QT_TR_NOOP("Input/Platform"), q, "categoryName");
    q->setIcon(QIcon(":/icons/xim.png"));
    trConnect(this, QT_TR_NOOP("XIM client"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)));
    enabledChanged(q->isEnabled());
    qimsysDebugOut();
}

Object::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Object::Private::enabledChanged(bool enabled)
{
    qimsysDebugIn() << enabled;
    if (enabled && !client) {
        client = new Client(this);
    } else if (!enabled && client) {
        delete client;
        client = 0;
    }
    qimsysDebugOut();
}

Object::Object(QObject *parent)
    : QimsysAbstractPluginObject(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Object::~Object()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "object.moc"
