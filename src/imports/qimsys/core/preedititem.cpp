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

#include "preedititem.h"

#include <qimsysdebug.h>

#include <QtDeclarative/qdeclarative.h>

PreeditItem::PreeditItem(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d.cursor = 0;
    d.selection = 0;
    d.modified = 0;
    qimsysDebugOut();
}

PreeditItem::PreeditItem(const QimsysPreeditItem &item, QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << item << parent;
    d.to = item.to;
    d.from = item.from;
    d.rawString = item.rawString;
    d.cursor = item.cursor;
    d.selection = item.selection;
    d.modified = 0;
    qimsysDebugOut();
}

PreeditItem::~PreeditItem()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

const QStringList &PreeditItem::to() const
{
    return d.to;
}

void PreeditItem::setTo(const QStringList &to)
{
    if (d.to == to) return;
    qimsysDebugIn() << to;
    d.to = to;
    emit toChanged(d.to);
    qimsysDebugOut();
}

const QStringList &PreeditItem::from() const
{
    return d.from;
}

void PreeditItem::setFrom(const QStringList &from)
{
    if (d.from == from) return;
    d.from = from;
    emit fromChanged(d.from);
}

const QStringList &PreeditItem::rawString() const
{
    return d.rawString;
}

void PreeditItem::setRawString(const QStringList &rawString)
{
    if (d.rawString == rawString) return;
    d.rawString = rawString;
    emit rawStringChanged(d.rawString);
}

int PreeditItem::cursor() const
{
    return d.cursor;
}

void PreeditItem::setCursor(int cursor)
{
    if (d.cursor == cursor) return;
    d.cursor = cursor;
    emit cursorChanged(cursor);
}

int PreeditItem::selection() const
{
    return d.selection;
}

void PreeditItem::setSelection(int selection)
{
    if (d.selection == selection) return;
    d.selection = selection;
    emit selectionChanged(selection);
}

int PreeditItem::modified() const
{
    return d.modified;
}

void PreeditItem::setModified(int modified)
{
    if (d.modified == modified) return;
    d.modified = modified;
    emit modifiedChanged(modified);
}

QML_DECLARE_TYPE(PreeditItem);
