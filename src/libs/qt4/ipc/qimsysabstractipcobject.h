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

#ifndef QIMSYSABSTRACTIPCOBJECT_H
#define QIMSYSABSTRACTIPCOBJECT_H

#include <QObject>
#include <QMetaType>
#include <QDataStream>
#ifndef QIMSYS_NO_DBUS
#include <QDBusAbstractInterface>
#include <QDBusArgument>
#include <QDBusError>
#include <QDBusMetaType>
#include <QDBusReply>
#else
#include <QVariant>
#endif // QIMSYS_NO_DBUS

#include "qimsysglobal.h"
#include "dbus.h"

class QIMSYS_EXPORT QimsysAbstractIpcObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.QimsysAbstractIpcObject")
    Q_DISABLE_COPY(QimsysAbstractIpcObject)
public:
    enum Type {
        Server,
        Client,
    };

    explicit QimsysAbstractIpcObject(QObject *parent = 0, Type type = Client);
    ~QimsysAbstractIpcObject();

    Type type() const;

    virtual bool init();

    Q_INVOKABLE virtual bool ping() const { return true; }

    virtual bool hasError() const;

protected:
    virtual QimsysAbstractIpcObject *server() const = 0;

    QObject *proxy() const;

private:
    class Private;
    Private *d;
};


#define Q_DECLARE_METAENUM(T) \
    Q_DECLARE_METATYPE(T) \
    inline QDataStream &operator<<(QDataStream &s, T value) \
    { \
        s << static_cast<qint32>(value); \
        return s; \
    } \
    \
    inline QDataStream &operator>>(QDataStream &s, T &value) \
    { \
        qint32 v = 0; \
        s >> v; \
        value = static_cast<T>(v); \
        return s; \
    }

#ifndef QIMSYS_NO_DBUS
#define Q_DECLARE_DBUS_METAENUM(T) \
    Q_DECLARE_METAENUM(T) \
    \
    DBUSARGUMENT_OUT(T value, \
                     arg.beginStructure(); \
                     arg << static_cast<int>(value); \
                     arg.endStructure(); \
                    ) \
    \
    DBUSARGUMENT_IN(T &value, \
                    arg.beginStructure(); \
                    value = static_cast<T>(qdbus_cast<int>(arg)); \
                    arg.endStructure(); \
                   )
#endif // QIMSYS_NO_DBUS

#define getProp(C, T, V, ...) \
    T C::V() const \
    {\
        T ret; \
        switch(type()) { \
        case Server: \
            ret = d->V; \
            break; \
        case Client: \
            if (proxy()) \
                ret = proxy()->property(#V).value<T>(); \
            else \
                qimsysWarning() << this << "is not initialized."; \
            break; \
        } \
        __VA_ARGS__ \
        return ret; \
    }

#define setProp(C, T, v, V, ...) \
    void C::V(T v) \
    { \
        qimsysDebugIn() << v; \
        __VA_ARGS__ \
        switch(type()) { \
        case Server: \
            if(d->v == v) break; \
            d->v = v; \
            emit v##Changed(d->v); \
            break; \
        case Client: \
            if (proxy()) \
                proxy()->setProperty(#v, qVariantFromValue(v)); \
            else \
                qimsysWarning() << this << "is not initialized."; \
            break; \
        } \
        qimsysDebugOut(); \
    }

#endif // QIMSYSABSTRACTIPCOBJECT_H
