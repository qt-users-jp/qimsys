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
#include <qimsysdebug.h>
#include <qimsysdynamictranslator.h>

namespace AboutQimsys {

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);
    void languageChanged();
    void exec(int action);

private:
    Object *q;
    QimsysApplicationManager *applicationManager;
public:
    QPointer<Widget> widget;
};

}

using namespace AboutQimsys;

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , applicationManager(0)
    , widget(0)
{
    q->setIdentifier(QLatin1String("About qimsys"));
    q->setPriority(0x00);

    q->setIcon(qApp->windowIcon());
    trConnect(this, QT_TR_NOOP("About qimsys"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    q->setCategoryType(AlwaysOne);
    trConnect(this, QT_TR_NOOP("User Interface/About"), q, "categoryName");

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
        if (widget) {
            delete widget;
            widget = 0;
        }
    }
}

void Object::Private::exec(int action)
{
    switch (action) {
    case QimsysApplicationManager::ShowAboutQimsys:
        if (!widget) {
            languageChanged();
        } else {
            widget->activateWindow();
        }
        break;
    default:
        break;
    }
}

void Object::Private::languageChanged()
{
    qimsysDebugIn();
    if (widget) {
        delete widget;
    }
    widget = new Widget(q);
    connect(widget, SIGNAL(languageChanged()), this, SLOT(languageChanged()));
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->show();
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
