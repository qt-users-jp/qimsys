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

#include "language.h"

#include <qimsysdebug.h>
#include <qimsysdynamictranslator.h>

namespace Japanese {

class Language::Private : private QObject
{
    Q_OBJECT
public:
    Private(Language *parent);
    ~Private();

private:
    Language *q;
};

}

using namespace Japanese;

Language::Private::Private(Language *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;

    q->setLocale("ja_JP");
    q->setLanguage("Japanese");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/icons/japan.png"));
#endif
    trConnect(this, QT_TR_NOOP("Japanese"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese language"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Language"), q, "categoryName");

    q->setActive(true);
    qimsysDebugOut();
}

Language::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

Language::Language(QObject *parent)
    : QimsysLanguage(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Language::~Language()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "language.moc"
