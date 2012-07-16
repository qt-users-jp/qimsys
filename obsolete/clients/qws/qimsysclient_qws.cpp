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

#include "qimsysclient_qws.h"
#include <QWidget>
#include <QWSServer>

#include "qimsysdebug.h"
#include "qimsysapplicationmanager.h"
#include "qimsyskeymanager.h"
#include "qimsyspreeditmanager.h"

class QimsysClientQWS::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysClientQWS *parent);
    ~Private();

    void queryResponse(int property, const QVariant &result);
    void updateHandler(int type);

private slots:
    void sendPreeditString();
    void sendCommitString(const QString &commitString);

public slots:
    void update();

private:
    QimsysClientQWS *q;
public:
    QimsysKeyManager filter;
    QimsysPreeditManager preedit;
    QimsysApplicationManager manager;
};

QimsysClientQWS::Private::Private(QimsysClientQWS *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    filter.init();
    preedit.init();
    manager.init();
    QWSServer::setCurrentInputMethod(q);
    qimsysDebugOut();
}

QimsysClientQWS::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void QimsysClientQWS::Private::sendPreeditString()
{
    qimsysDebugIn();
    q->sendPreeditString(preedit.items(), preedit.cursorPosition(), preedit.selectionLength());
    update();
    qimsysDebugOut();
}

void QimsysClientQWS::Private::sendCommitString(const QString &commitString)
{
    qimsysDebugIn() << commitString;
    q->sendCommitString(commitString);
    qimsysDebugOut();
}

void QimsysClientQWS::Private::update()
{
    qimsysDebugIn();
    qwsServer->sendIMQuery(Qt::ImMicroFocus);
    qwsServer->sendIMQuery(Qt::ImFont);
    qimsysDebugOut();
}

QimsysClientQWS::QimsysClientQWS(QObject *parent)
    : QWSInputMethod()
{
    qimsysDebugIn() << parent;
    setParent(parent);
    d = new Private(this);
    qimsysDebugOut();
}

QimsysClientQWS::~QimsysClientQWS()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool QimsysClientQWS::filter(int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    qimsysDebugIn() << unicode << keycode << modifiers << isPress << autoRepeat;
    bool ret = d->filter.filter(QChar(unicode), keycode, modifiers, isPress, autoRepeat);
    qimsysDebugOut() << ret;
    return ret;
}

void QimsysClientQWS::reset()
{
    qimsysDebugIn();
    d->manager.exec(QimsysApplicationManager::Reset);
// d->preedit.commit();
// d->preedit.reset();
    qimsysDebugOut();
}

void QimsysClientQWS::Private::queryResponse(int property, const QVariant &result)
{
    qimsysDebugIn() << property << result;
    switch (property) {
    case Qt::ImMicroFocus:
        preedit.setRect(result.toRect());
        break;
    case Qt::ImFont:
        preedit.setFont(result.value<QFont>());
        break;
    }
    qimsysDebugOut();
}

void QimsysClientQWS::queryResponse(int property, const QVariant &result)
{
    qimsysDebugIn() << property << result;
    d->queryResponse(property, result);
    QWSInputMethod::queryResponse(property, result);
    qimsysDebugOut();
}

void QimsysClientQWS::Private::updateHandler(int type)
{
    qimsysDebugIn() << type;
    switch (type) {
    case Update:
        update();
        break;
    case FocusIn:
        connect(&preedit, SIGNAL(preeditStringChanged(QString)), this, SLOT(sendPreeditString()), Qt::QueuedConnection);
        connect(&preedit, SIGNAL(cursorPositionChanged(int)), this, SLOT(sendPreeditString()), Qt::QueuedConnection);
        connect(&preedit, SIGNAL(selectionLengthChanged(int)), this, SLOT(sendPreeditString()), Qt::QueuedConnection);
        connect(&preedit, SIGNAL(committed(QString, uint)), this, SLOT(sendCommitString(QString)));
        manager.setFocus(true);
        break;
    case FocusOut:
        manager.setFocus(false);
        disconnect(&preedit, SIGNAL(preeditStringChanged(QString)), this, SLOT(sendPreeditString()));
        disconnect(&preedit, SIGNAL(cursorPositionChanged(int)), this, SLOT(sendPreeditString()));
        disconnect(&preedit, SIGNAL(selectionLengthChanged(int)), this, SLOT(sendPreeditString()));
        disconnect(&preedit, SIGNAL(committed(QString, uint)), this, SLOT(sendCommitString(QString)));
        break;
    case Reset:
        q->reset();
        break;
    case Destroyed:
        break;
    }
    qimsysDebugOut();
}

void QimsysClientQWS::updateHandler(int type)
{
    qimsysDebugIn() << type;
    d->updateHandler(type);
    QWSInputMethod::updateHandler(type);
    qimsysDebugOut();
}

#include "qimsysclient_qws.moc"
