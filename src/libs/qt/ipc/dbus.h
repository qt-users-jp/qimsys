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

#ifndef DBUS_H
#define DBUS_H

#include "qimsysglobal.h"

#ifdef QIMSYS_PLATFORM_SYMBIAN
#define DATASTREAM_IN(T, ARGS...) \
    QIMSYS_EXPORT inline QDataStream &operator>>(QDataStream &s, T) \
    { \
        ARGS \
        return s; \
    }

#define DATASTREAM_OUT(T, ARGS...) \
    QIMSYS_EXPORT inline QDataStream &operator<<(QDataStream &s, T) \
    { \
        ARGS \
        return s; \
    }
#else // QIMSYS_PLATFORM_SYMBIAN
#define DATASTREAM_IN(T, ...) \
    QIMSYS_EXPORT inline QDataStream &operator>>(QDataStream &s, T) \
    { \
        __VA_ARGS__ \
        return s; \
    }

#define DATASTREAM_OUT(T, ...) \
    QIMSYS_EXPORT inline QDataStream &operator<<(QDataStream &s, T) \
    { \
        __VA_ARGS__ \
        return s; \
    }
#endif // QIMSYS_PLATFORM_SYMBIAN

#ifndef QIMSYS_NO_DBUS
#include <QDBusArgument>

#define DBUSARGUMENT_IN(T, ...) \
    QIMSYS_EXPORT inline const QDBusArgument &operator>>(const QDBusArgument &arg, T) \
    { \
        __VA_ARGS__ \
        return arg; \
    }

#define DBUSARGUMENT_OUT(T, ...) \
    QIMSYS_EXPORT inline QDBusArgument &operator<<(QDBusArgument &arg, T) \
    { \
        __VA_ARGS__ \
        return arg; \
    }
#endif // QIMSYS_NO_DBUS

#endif//DBUS_H
