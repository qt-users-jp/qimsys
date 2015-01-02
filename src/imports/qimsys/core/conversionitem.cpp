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

#include "conversionitem.h"

#include <qimsysdebug.h>

#include <QtDeclarative/qdeclarative.h>

ConversionItem::ConversionItem(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d.index = 0;
    qimsysDebugOut();
}

ConversionItem::ConversionItem(const QimsysConversionItem &item, QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << item << parent;
    d.index = item.index;
    d.from = item.from;
    d.to = item.to;
    d.hint = item.hint;
    d.source = item.source;
    qimsysDebugOut();
}

ConversionItem::~ConversionItem()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

int ConversionItem::index() const
{
    return d.index;
}

void ConversionItem::setIndex(int index)
{
    if (d.index == index) return;
    d.index = index;
    emit indexChanged(index);
}

const QString &ConversionItem::from() const
{
    return d.from;
}

void ConversionItem::setFrom(const QString &from)
{
    if (d.from == from) return;
    d.from = from;
    emit fromChanged(d.from);
}

const QString &ConversionItem::to() const
{
    return d.to;
}

void ConversionItem::setTo(const QString &to)
{
    if (d.to == to) return;
    qimsysDebugIn() << to;
    d.to = to;
    emit toChanged(d.to);
    qimsysDebugOut();
}

const QString &ConversionItem::hint() const
{
    return d.hint;
}

void ConversionItem::setHint(const QString &hint)
{
    if (d.hint == hint) return;
    d.hint = hint;
    emit hintChanged(d.hint);
}

const QString &ConversionItem::source() const
{
    return d.source;
}

void ConversionItem::setSource(const QString &source)
{
    if (d.source == source) return;
    d.source = source;
    emit sourceChanged(source);
}

QML_DECLARE_TYPE(ConversionItem);
