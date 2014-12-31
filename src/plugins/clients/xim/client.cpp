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

#include "client.h"
#include <QtPlugin>
#include <QWidget>

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsyskeymanager.h>
#include <qimsyspreeditmanager.h>

using namespace Xim;

class Client::Private : private QObject
{
    Q_OBJECT
public:
    Private(Client *parent);
    ~Private();

    void setActive(bool active);

private slots:
    void sendPreeditString(const QimsysPreeditItem &item);
    void sendCommitString(const QString &commitString, qulonglong target);

private:
    Client *q;
public:
    QimsysApplicationManager applicationManager;
    QimsysKeyManager keyManager;
    QimsysPreeditManager preeditManager;
};

Client::Private::Private(Client *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    applicationManager.init();
    keyManager.init();
    preeditManager.init();
    connect(&preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(sendPreeditString(QimsysPreeditItem)));
    connect(&preeditManager, SIGNAL(committed(QString, qulonglong)), this, SLOT(sendCommitString(QString, qulonglong)));
    qimsysDebugOut();
}

Client::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Client::Private::sendPreeditString(const QimsysPreeditItem &item)
{
    if (!q->focusWindow() || (applicationManager.window() != q->focusWindow())) return;
    qimsysDebugIn() << item;
    q->sendPreeditString(item.to.join(""), item.cursor, item.selection);
    qimsysDebugOut();
}

void Client::Private::sendCommitString(const QString &commitString, qulonglong target)
{
    if (q->focusWindow() != target) return;
    qimsysDebugIn() << commitString << target;
    q->sendCommitString(commitString);
    qimsysDebugOut();
}

void Client::Private::setActive(bool active)
{
    qimsysDebugIn() << active;
    if (active) {
        applicationManager.setWindow(q->clientWindow());
        applicationManager.setWidget(q->focusWindow());
        applicationManager.setFocus(true);
    } else {
        if (applicationManager.focus() == q->focusWindow()) {
            applicationManager.setWindow(0);
            applicationManager.setWidget(0);
            applicationManager.setFocus(false);
        }
    }
    qimsysDebugOut();
}

Client::Client(QObject *parent)
    : QXimInputMethod(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Client::~Client()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool Client::filter(int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    qimsysDebugIn() << unicode << keycode << modifiers << isPress << autoRepeat;
    if (isPress)
        d->keyManager.keyPress(QChar(unicode), keycode, modifiers, autoRepeat);
    else
        d->keyManager.keyRelease(QChar(unicode), keycode, modifiers, autoRepeat);

    bool ret = d->keyManager.isAccepted();
    qimsysDebugOut() << ret;
    return ret;
}

qulonglong Client::winId()
{
    static QWidget *dummy = 0;
    if (!dummy)
        dummy = new QWidget;
    return dummy->winId();
}

const char *Client::name() const
{
    return "qimsys";
}

const char *Client::locale() const
{
    return "ja_JP";
}

void Client::updateHandler(UpdateType type)
{
    qimsysDebugIn() << type;
    switch (type) {
    case Update:
        break;
    case FocusIn:
        d->setActive(true);
        break;
    case FocusOut:
        d->setActive(false);
        break;
    default:
        break;
    }
    qimsysDebugOut();
}

#include "client.moc"
