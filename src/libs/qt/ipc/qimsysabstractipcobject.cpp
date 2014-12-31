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

#include "qimsysabstractipcobject.h"

#include "qimsysdebug.h"

#include <QMetaClassInfo>

#ifndef QIMSYS_NO_DBUS
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#endif // QIMSYS_NO_DBUS

class QimsysAbstractIpcObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysAbstractIpcObject *parent, Type t);
    bool init();

private:
    QimsysAbstractIpcObject *q;

public:
    Type type;
    QObject *proxy;
};

QimsysAbstractIpcObject::Private::Private(QimsysAbstractIpcObject *parent, Type t)
    : QObject(parent)
    , q(parent)
    , type(t)
    , proxy(0)
{
    qimsysDebugIn() << parent << t;
    switch (type) {
    case Server:
        break;
    case Client:
        break;
    }
    qimsysDebugOut();
}

bool QimsysAbstractIpcObject::Private::init()
{
    bool ret = true;
    qimsysDebugIn();
    const QMetaObject *mobject = q->metaObject();
    while (mobject->superClass()->className() != q->staticMetaObject.className()) {
        mobject = mobject->superClass();
    }
#ifndef QIMSYS_NO_DBUS
    QDBusConnection connection = QDBusConnection::sessionBus();
#endif
    switch (type) {
    case Server: {
#ifndef QIMSYS_NO_DBUS
        if (!connection.isConnected()) {
            qimsysWarning() << "Cannot connect to the D-Bus session bus.";
            ret = false;
            break;
        }

        bool ret = connection.registerObject(QString("/%1").arg(mobject->className()), q, QDBusConnection::ExportAllContents);
        if (!ret) {
            qimsysWarning() << "registerObject failure" << QIMSYS_DBUS_SERVICE;
            ret = false;
            break;
        }
#endif // QIMSYS_NO_DBUS
        break;
    }
    case Client:
        if (!proxy) {
            if (q->server()) {
                proxy = q->server();
            } else {
                QString interface;
                for (int i = 0; i < mobject->classInfoCount(); i++) {
                    if (QString(mobject->classInfo(i).name()) == QLatin1String("D-Bus Interface")) {
                        interface = QString(mobject->classInfo(i).value());
                    }
                }
                if (interface.isNull()) {
                    qimsysWarning() << "interface not found" << q;
                    ret = false;
#ifndef QIMSYS_NO_DBUS
                } else {
                    QDBusInterface *iface = new QDBusInterface(QIMSYS_DBUS_SERVICE, QString("/%1").arg(mobject->className()), interface, connection);
                    if (!iface->isValid()) {
                        qimsysWarning() << iface->lastError() << q;
                        delete iface;
                        ret = false;
                    } else {
                        proxy = iface;
                    }
#endif // QIMSYS_NO_DBUS
                }
            }
        }
        break;
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject::QimsysAbstractIpcObject(QObject *parent, Type type)
    : QObject(parent)
{
    qimsysDebugIn() << parent << type;
    d = new Private(this, type);
    qimsysDebugOut();
}

QimsysAbstractIpcObject::~QimsysAbstractIpcObject()
{
    qimsysDebugIn();
    delete d;
    d = 0;
    qimsysDebugOut();
}

bool QimsysAbstractIpcObject::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject::Type QimsysAbstractIpcObject::type() const
{
    return d ? d->type : QimsysAbstractIpcObject::Client;
}

QObject *QimsysAbstractIpcObject::proxy() const
{
    return d ? d->proxy : 0;
}

bool QimsysAbstractIpcObject::hasError() const
{
    qimsysDebugIn();
    bool ret = false;
    if (!d->proxy) {
        ret = true;
#ifndef QIMSYS_NO_DBUS
    } else if (!server()) {
        QDBusAbstractInterface *iface = qobject_cast<QDBusAbstractInterface*>(proxy());
        if (!iface->isValid()) {
            ret = true;
        }
#endif // QIMSYS_NO_DBUS
    }
    qimsysDebugOut() << ret;
    return ret;
}

#include "qimsysabstractipcobject.moc"
