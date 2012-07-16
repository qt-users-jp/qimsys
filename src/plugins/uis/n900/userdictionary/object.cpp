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

#include "object.h"
#include "widget.h"

#include <QApplication>
#include <QPointer>

#include <qimsysapplicationmanager.h>
#include <qimsysdynamictranslator.h>

using namespace N900::UserDictionary;

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);
    void exec(int action);

private:
    Object *q;
    QimsysApplicationManager *applicationManager;
public:
    QPointer<Widget> widget;
};

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , applicationManager(0)
    , widget(0)
{
    q->setIdentifier(QLatin1String("User Dictionary Editor for N900"));
    q->setPriority(0x00);

    q->setIcon(qApp->windowIcon());
    trConnect(this, QT_TR_NOOP("User Dictionary Editor"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    q->setCategoryType(AlwaysOne);
    trConnect(this, QT_TR_NOOP("User Interface/User Dictionary Editor"), q, "categoryName");

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)));
    enabledChanged(q->isEnabled());
}

Object::Private::~Private()
{
    delete widget;
}

void Object::Private::enabledChanged(bool enabled)
{
    if (enabled) {
        if (!applicationManager) {
            applicationManager = new QimsysApplicationManager(this);
            applicationManager->init();
            connect(applicationManager, SIGNAL(triggered(int)), this, SLOT(exec(int)));
        }
    } else {
        if (applicationManager) {
            disconnect(applicationManager, SIGNAL(triggered(int)), this, SLOT(exec(int)));
            applicationManager->deleteLater();
            applicationManager = 0;
        }
    }
}

void Object::Private::exec(int action)
{
    switch (action) {
    case QimsysApplicationManager::ShowDictionary:
        if (!widget) {
            widget = new Widget;
            widget->setAttribute(Qt::WA_DeleteOnClose);
        }
        if (widget->isVisible()) {
            widget->activateWindow();
        } else {
            widget->show();
        }
        break;
    default:
        break;
    }
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

#include "object.moc"
