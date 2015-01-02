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

#include "qimsysserver.h"
#include "qimsysdebug.h"

#include "qimsysapplicationmanager.h"
#include "qimsysinputmethodmanager.h"
#include "qimsyspreeditmanager.h"
#include "qimsyscandidatemanager.h"
#include "qimsyspluginmanager.h"
#include "qimsysconverter.h"
#include "qimsyskeymanager.h"
#include "qimsyskeyboardmanager.h"

#include <QSettings>
#ifndef QIMSYS_NO_DBUS
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#endif

class QimsysServer::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysServer *parent);
    ~Private();

private:
    QimsysServer *q;
};


QimsysServer::Private::Private(QimsysServer *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
#ifndef QIMSYS_NO_DBUS
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qimsysWarning() << "Cannot connect to the D-Bus session bus.";
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }

    QDBusReply<QStringList> registeredServiceNames = connection.interface()->registeredServiceNames();
    if (!registeredServiceNames.isValid()) {
        qimsysWarning() << registeredServiceNames.error();
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    if (registeredServiceNames.value().contains(QIMSYS_DBUS_SERVICE)) {
        qimsysWarning() << "another process is running already.";
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    bool ret = connection.registerService(QIMSYS_DBUS_SERVICE);
    if (!ret) {
        qimsysWarning() << "registerService failure" << QIMSYS_DBUS_SERVICE;
        metaObject()->invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
#endif // QIMSYS_NO_DBUS

    QSettings settings;
    settings.beginGroup("Application");
    QString lang = settings.value("Language", tr("en")).toString();

    QimsysApplicationManager *manager = new QimsysApplicationManager(this, QimsysAbstractIpcObject::Server);
    manager->init();
    manager->setDisplayLanguage(lang);
    (new QimsysInputMethodManager(this, QimsysAbstractIpcObject::Server))->init();
    (new QimsysPreeditManager(this, QimsysAbstractIpcObject::Server))->init();
    (new QimsysCandidateManager(this, QimsysAbstractIpcObject::Server))->init();
    (new QimsysKeyManager(this, QimsysAbstractIpcObject::Server))->init();
    (new QimsysKeyboardManager(this, QimsysAbstractIpcObject::Server))->init();
    qimsysDebugOut();
}

QimsysServer::Private::~Private()
{

}

QimsysServer::QimsysServer(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysServer::~QimsysServer()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "qimsysserver.moc"
