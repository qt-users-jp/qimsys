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

#include "qimsysobject.h"

#include "qimsysdebug.h"

class AbstractIpcObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(AbstractIpcObject *parent);
    ~Private();

    bool init();

private:
    AbstractIpcObject *q;

public:
    static AbstractIpcObject *server;

#define VARIABLE( T, get ) \
    T get##Value;

    VARIABLE(bool, bool)
    VARIABLE(int, int)
    VARIABLE(uint, uint)
    VARIABLE(qulonglong, qulonglong)

    VARIABLE(QString, string)
    VARIABLE(QPoint, point)
    VARIABLE(QSize, size)
    VARIABLE(QRect, rect)
#ifdef QT_GUI_LIB
    VARIABLE(QFont, font)
#endif

    VARIABLE(AbstractIpcObject::Enum, enum)
#undef VARIABLE
};

AbstractIpcObject *AbstractIpcObject::Private::server = 0;

AbstractIpcObject::Private::Private(AbstractIpcObject *parent)
        : QObject(parent)
        , q(parent)
#define INIT( get, value ) \
    , get##Value( value )

        INIT(bool, false)
        INIT(int, 0)
        INIT(uint, 0)
        INIT(qulonglong, 0)

        INIT(string, QString::null)
        INIT(point,)
        INIT(size,)
        INIT(rect,)
#ifdef QT_GUI_LIB
        INIT(font,)
#endif

        INIT(enum,)
#undef INIT
{
#define REGISTERTYPE( T ) \
    qRegisterMetaType<T>( #T ); \
    qRegisterMetaTypeStreamOperators<T>( #T ); \
    qDBusRegisterMetaType<T>();
#ifdef QT_GUI_LIB
    REGISTERTYPE(QFont)
#endif
    REGISTERTYPE(AbstractIpcObject::Enum)
#undef REGISTERTYPE

    switch (q->type()) {
    case Server:
        server = q;
        break;
    case Client:
        break;
    }
}

AbstractIpcObject::Private::~Private()
{
}

bool AbstractIpcObject::Private::init()
{
    bool ret = q->QimsysAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
#define CONNECT(T, get) \
            get##Value = q->proxy()->property(#get).value<T>(); \
            connect(q->proxy(), SIGNAL(get##ValueChanged(T)), q, SIGNAL(get##ValueChanged(T)));

            CONNECT(bool, bool)
            CONNECT(int, int)
            CONNECT(uint, uint)
            CONNECT(qulonglong, qulonglong)

            CONNECT(QString, string)
            CONNECT(QPoint, point)
            CONNECT(QSize, size)
            CONNECT(QRect, rect)
#ifdef QT_GUI_LIB
            CONNECT(QFont, font)
#endif

            CONNECT(AbstractIpcObject::Enum, enum)
#undef CONNECT
//   q->setClientMode( Sync );
            break;
        }
    }
    return ret;
}

AbstractIpcObject::AbstractIpcObject(QObject *parent, Type type)
    : QimsysAbstractIpcObject(parent, type)
{
    d = new Private(this);
}

AbstractIpcObject::~AbstractIpcObject()
{
    delete d;
}

bool AbstractIpcObject::init()
{
    bool ret = d->init();
    return ret;
}

QimsysAbstractIpcObject *AbstractIpcObject::server() const
{
    return d->server;
}

#define PROP( T, func ) \
    getProp( AbstractIpcObject, T, func##Value ) \
    setProp( AbstractIpcObject, T, func##Value, func##Value )
PROP(bool, bool)
PROP(int, int)
PROP(uint, uint)
PROP(qulonglong, qulonglong)

PROP(QString, string)
PROP(QPoint, point)
PROP(QSize, size)
PROP(QRect, rect)
#ifdef QT_GUI_LIB
PROP(QFont, font)
#endif

PROP(AbstractIpcObject::Enum, enum)
#undef PROP

#include "qimsysobject.moc"
