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

#include <QPointer>

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsysdynamictranslator.h>

using namespace N900::Dialog;

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
    QPointer<Widget> widget;
};

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , applicationManager(0)
    , widget(0)
{
    qimsysDebugIn() << parent;
    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("User Interface/System Controler"), q, "categoryName");
    trConnect(this, QT_TR_NOOP("N900: Settings Dialog"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
    qimsysDebugOut();
}

Object::Private::~Private()
{
    delete widget;
}

void Object::Private::enabledChanged(bool enabled)
{
    qimsysDebugIn() << enabled;
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
        if (widget) {
            widget->close();
        }
    }
    qimsysDebugOut();
}

void Object::Private::exec(int action)
{
    qimsysDebugIn() << action;
    switch (action) {
    case QimsysApplicationManager::ShowController:
        if (!widget) {
            widget = new Widget(q);
            widget->setAttribute(Qt::WA_DeleteOnClose);
            widget->open();
        } else {
            widget->activateWindow();
        }
        break;
    default:
        break;
    }
    qimsysDebugOut();
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
