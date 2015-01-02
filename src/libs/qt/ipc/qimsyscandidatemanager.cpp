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

#include "qimsyscandidatemanager.h"

#include "qimsysdebug.h"

class QimsysCandidateManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysCandidateManager *parent);
    ~Private();

    bool init();

private:
    QimsysCandidateManager *q;

public:
    static QimsysCandidateManager *server;
    QimsysConversionItemList items;
    int currentIndex;
};

QimsysCandidateManager *QimsysCandidateManager::Private::server = 0;

QimsysCandidateManager::Private::Private(QimsysCandidateManager *parent)
    : QObject(parent)
    , q(parent)
    , items()
    , currentIndex(-1)
{
    qimsysDebugIn() << parent;
    qRegisterMetaType<QimsysConversionItem>("QimsysConversionItem");
    qRegisterMetaTypeStreamOperators<QimsysConversionItem>("QimsysConversionItem");
    qRegisterMetaType< QimsysConversionItemList >("QimsysConversionItemList");
    qRegisterMetaTypeStreamOperators< QimsysConversionItemList >("QimsysConversionItemList");
#ifndef QIMSYS_NO_DBUS
    qDBusRegisterMetaType<QimsysConversionItem>();
    qDBusRegisterMetaType< QimsysConversionItemList >();
#endif
    switch (q->type()) {
    case Server:
        server = q;
        break;
    case Client:
        break;
    }
    qimsysDebugOut();
}

QimsysCandidateManager::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

bool QimsysCandidateManager::Private::init()
{
    qimsysDebugIn();
    bool ret = q->QimsysAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(itemsChanged(QimsysConversionItemList)), q, SIGNAL(itemsChanged(QimsysConversionItemList)));
            connect(q->proxy(), SIGNAL(currentIndexChanged(int)), q, SIGNAL(currentIndexChanged(int)));
            break;
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysCandidateManager::QimsysCandidateManager(QObject *parent, Type type)
    : QimsysAbstractIpcObject(parent, type)
{
    qimsysDebugIn() << parent << type;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysCandidateManager::~QimsysCandidateManager()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool QimsysCandidateManager::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject *QimsysCandidateManager::server() const
{
    return d->server;
}

getProp(QimsysCandidateManager, QimsysConversionItemList, items)
setProp(QimsysCandidateManager, const QimsysConversionItemList&, items, setItems)

getProp(QimsysCandidateManager, int, currentIndex)
setProp(QimsysCandidateManager, int, currentIndex, setCurrentIndex)

void QimsysCandidateManager::clear()
{
    qimsysDebugIn();
    switch (type()) {
    case Server:
        d->currentIndex = -1;
        d->items.clear();
        emit itemsChanged(d->items);
        emit currentIndexChanged(-1);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "clear");
        break;
    }
    qimsysDebugOut();
}

#include "qimsyscandidatemanager.moc"
