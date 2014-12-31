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

#include "qimsyskeyboardmanager.h"

#include "qimsysdebug.h"

class QimsysKeyboardManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysKeyboardManager *parent);
    ~Private();

    bool init();

private:
    QimsysKeyboardManager *q;

public:
    static QimsysKeyboardManager *server;
    bool visible;
    QRect geometry;
    QRect focusArea;
    QString sharedData;
};

QimsysKeyboardManager *QimsysKeyboardManager::Private::server = 0;

QimsysKeyboardManager::Private::Private(QimsysKeyboardManager *parent)
    : QObject(parent)
    , q(parent)
    , visible(false)
{
    qimsysDebugIn() << parent;
    switch (q->type()) {
    case Server:
        server = q;
        break;
    case Client:
        break;
    }
    qimsysDebugOut();
}

QimsysKeyboardManager::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

bool QimsysKeyboardManager::Private::init()
{
    qimsysDebugIn();
    bool ret = q->QimsysAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(visibleChanged(bool)), q, SIGNAL(visibleChanged(bool)));
            connect(q->proxy(), SIGNAL(geometryChanged(QRect)), q, SIGNAL(geometryChanged(QRect)));
            connect(q->proxy(), SIGNAL(focusAreaChanged(QRect)), q, SIGNAL(focusAreaChanged(QRect)));
            connect(q->proxy(), SIGNAL(sharedDataChanged(QString)), q, SIGNAL(sharedDataChanged(QString)));
            break;
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysKeyboardManager::QimsysKeyboardManager(QObject *parent, Type type)
    : QimsysAbstractIpcObject(parent, type)
{
    qimsysDebugIn() << parent << type;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysKeyboardManager::~QimsysKeyboardManager()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool QimsysKeyboardManager::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject *QimsysKeyboardManager::server() const
{
    return d->server;
}

getProp(QimsysKeyboardManager, bool, visible)
setProp(QimsysKeyboardManager, bool, visible, setVisible)

getProp(QimsysKeyboardManager, QRect, geometry)
setProp(QimsysKeyboardManager, const QRect &, geometry, setGeometry)

getProp(QimsysKeyboardManager, QRect, focusArea)
setProp(QimsysKeyboardManager, const QRect &, focusArea, setFocusArea)

getProp(QimsysKeyboardManager, QString, sharedData)
setProp(QimsysKeyboardManager, const QString &, sharedData, setSharedData)

#include "qimsyskeyboardmanager.moc"
