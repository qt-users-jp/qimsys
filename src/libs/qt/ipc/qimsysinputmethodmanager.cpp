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

#include "qimsysinputmethodmanager.h"
#include "qimsysdebug.h"

class QimsysInputMethodManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysInputMethodManager *parent);
    ~Private();

    bool init();

private:
    QimsysInputMethodManager *q;

public:
    static QimsysInputMethodManager *server;

    QString locale;
    QString identifier;
    QString converter;
    QString interpreter;
    QString engine;
    uint state;
};

QimsysInputMethodManager *QimsysInputMethodManager::Private::server = 0;

QimsysInputMethodManager::Private::Private(QimsysInputMethodManager *parent)
    : QObject(parent)
    , q(parent)
    , state(0)
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

QimsysInputMethodManager::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

bool QimsysInputMethodManager::Private::init()
{
    qimsysDebugIn() << q->type();
    bool ret = q->QimsysAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(localeChanged(QString)), q, SIGNAL(localeChanged(QString)));
            connect(q->proxy(), SIGNAL(identifierChanged(QString)), q, SIGNAL(identifierChanged(QString)));
            connect(q->proxy(), SIGNAL(converterChanged(QString)), q, SIGNAL(converterChanged(QString)));
            connect(q->proxy(), SIGNAL(interpreterChanged(QString)), q, SIGNAL(interpreterChanged(QString)));
            connect(q->proxy(), SIGNAL(engineChanged(QString)), q, SIGNAL(engineChanged(QString)));
            connect(q->proxy(), SIGNAL(stateChanged(uint)), q, SIGNAL(stateChanged(uint)));
            connect(q->proxy(), SIGNAL(executed(QString)), q, SIGNAL(executed(QString)));
            break;
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysInputMethodManager::QimsysInputMethodManager(QObject *parent, Type type)
    : QimsysAbstractIpcObject(parent, type)
{
    qimsysDebugIn() << parent << type;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysInputMethodManager::~QimsysInputMethodManager()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool QimsysInputMethodManager::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject *QimsysInputMethodManager::server() const
{
    return d->server;
}

getProp(QimsysInputMethodManager, QString, locale)
setProp(QimsysInputMethodManager, const QString &, locale, setLocale)

getProp(QimsysInputMethodManager, QString, identifier)
setProp(QimsysInputMethodManager, const QString &, identifier, setIdentifier)

getProp(QimsysInputMethodManager, QString, converter)
setProp(QimsysInputMethodManager, const QString &, converter, setConverter)

getProp(QimsysInputMethodManager, QString, interpreter)
setProp(QimsysInputMethodManager, const QString &, interpreter, setInterpreter)

getProp(QimsysInputMethodManager, QString, engine)
setProp(QimsysInputMethodManager, const QString &, engine, setEngine)

getProp(QimsysInputMethodManager, uint, state)
setProp(QimsysInputMethodManager, uint, state, setState)

void QimsysInputMethodManager::execute(const QString &command)
{
    qimsysDebugIn() << command;
    switch (type()) {
    case Server:
        emit executed(command);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "execute", Q_ARG(QString, command));
        break;
    }
    qimsysDebugOut();
}

#include "qimsysinputmethodmanager.moc"
