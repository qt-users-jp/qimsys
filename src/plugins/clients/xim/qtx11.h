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

#ifndef Q2X_H
#define Q2X_H

#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <IMdkit.h>
#include <Xi18n.h>

static const unsigned int KeyTbl[] = {
#include "keysym2qtkey.tbl"
    0,                          0
};

namespace Qt2X11
{
inline XRectangle *convert(const QRect &rect)
{
    XRectangle *ret = new XRectangle;
    ret->x = rect.x();
    ret->y = rect.y();
    ret->width = rect.width();
    ret->height = rect.height();
    return ret;
}

inline XPoint *convert(const QPoint &point)
{
    XPoint *ret = new XPoint;
    ret->x = point.x();
    ret->y = point.y();
    return ret;
}

inline XID *convert(qulonglong value)
{
    XID *ret = new XID;
    *ret = value;
    return ret;
}

inline char *convert(const QString &value)
{
    return value.toLatin1().data();
}
}

namespace X112Qt
{
# ifdef LONG64
inline quint32 convert(CARD32 *id)
{
    if (!id) return 0;
    return (quint32)*id;
}
#endif

inline qint32 convert(INT32 *id)
{
    if (!id) return 0;
    return (qint32)*id;
}

inline QString convert(char *value)
{
    return QString(value);
}

inline unsigned long convert(XID *xid)
{
    if (!xid) return 0;
    return (unsigned long)*xid;
}

inline QPoint convert(const XPoint *point)
{
    if (!point) return QPoint();
    return QPoint(point->x, point->y);
}

inline QRect convert(const XRectangle *rect)
{
    if (!rect) return QRect();
    return QRect(rect->x, rect->y, rect->width, rect->height);
}

inline QChar convert(XKeyEvent *xev, int &key)
{
    QChar ret;
    QByteArray chars;
    chars.resize(513);
    KeySym keysym;
    int count = XLookupString(xev, chars.data(), chars.size(), &keysym, 0);
    for (int i = 0; KeyTbl[i]; i += 2) {
        if (KeyTbl[i+1] == keysym) {
            key = KeyTbl[i];
            break;
        }
    }
    if (key == 0) {
        qimsysWarning() << xev->keycode << keysym;
    }
    switch (count) {
    case 0:
        break;
    case 1:
        ret = chars.at(0);
        break;
    default:
        qimsysWarning() << chars;
    }
    return ret;
}
}
#endif // Q2X_H
