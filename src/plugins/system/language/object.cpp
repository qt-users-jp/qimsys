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

#include "object.h"
#include "settings.h"

#include <qimsysdebug.h>
#include <qimsysdynamictranslator.h>

namespace System {
    namespace Language {

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private:
    Object *q;
};

    }
}

using namespace System::Language;

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
{
    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(All);
    trConnect(this, QT_TR_NOOP("Settings/Display Language"), q, "categoryName");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/language.png"));
#endif
    trConnect(this, QT_TR_NOOP("Display Language"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP("This plugin is used to change display language."), q, "description");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
}

Object::Private::~Private()
{
}

Object::Object(QObject *parent)
    : QimsysAbstractPluginObject(parent)
{
    d = new Private(this);
}

Object::~Object()
{
    delete d;
}

#ifndef QIMSYS_NO_GUI
QimsysSettingsWidget *Object::settings(const QString &hint, QWidget *parent)
{
    return new Settings(this, parent);
}
#endif

#include "object.moc"
