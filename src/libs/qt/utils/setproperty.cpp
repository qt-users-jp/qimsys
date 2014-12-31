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

#include "setproperty.h"
#include <QVariant>

class SetProperty::Private : private QObject
{
    Q_OBJECT
public:
    Private(SetProperty *parent, QObject *sender, const char *signal, QObject *receiver, const char *name);
    Private(SetProperty *parent, const char *property, QObject *sender, const char *signal, QObject *receiver, const char *name);
    ~Private();

private slots:
    void setProperty(const QString &value);
    void setProperty();

private:
    SetProperty *q;
    const char *property;
    QObject *receiver;
    const char *name;
};

SetProperty::Private::Private(SetProperty *parent, QObject *sender, const char *signal, QObject *r, const char *n)
    : QObject(parent)
    , q(parent)
    , property(0)
    , receiver(r)
    , name(n)
{
    connect(receiver, SIGNAL(destroyed()), q, SLOT(deleteLater()));
    connect(sender, signal, this, SLOT(setProperty(QString)));
}

SetProperty::Private::Private(SetProperty *parent, const char *p, QObject *sender, const char *signal, QObject *r, const char *n)
    : QObject(parent)
    , q(parent)
    , property(p)
    , receiver(r)
    , name(n)
{
    connect(receiver, SIGNAL(destroyed()), q, SLOT(deleteLater()));
    connect(sender, signal, this, SLOT(setProperty()));
}

SetProperty::Private::~Private()
{
}

void SetProperty::Private::setProperty(const QString &value)
{
    receiver->setProperty(name, qVariantFromValue(value));
}

void SetProperty::Private::setProperty()
{
    receiver->setProperty(name, sender()->property(property));
}

SetProperty::SetProperty(QObject *sender, const char *signal, QObject *receiver, const char *name)
    : QObject(sender)
{
    d = new Private(this, sender, signal, receiver, name);
}

SetProperty::SetProperty(QObject *sender, const char *property, const char *signal, QObject *receiver, const char *name)
    : QObject(sender)
{
    d = new Private(this, property, sender, signal, receiver, name);
}

SetProperty::~SetProperty()
{
    delete d;
}

#include "setproperty.moc"
