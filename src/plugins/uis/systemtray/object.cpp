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

#include "object.h"
#include "icon.h"

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsysdynamictranslator.h>

namespace SystemTray {

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);
    void languageChanged();

private:
    Object *q;
    Icon *icon;
    QimsysApplicationManager *applicationManager;
};

}

using namespace SystemTray;

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , icon(0)
    , applicationManager(0)
{
    q->setIdentifier(QLatin1String("System Tray"));
    q->setPriority(0x00);

    q->setIcon(QIcon(":/systemtrayicon.png"));
    trConnect(this, QT_TR_NOOP("System tray"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("User Interface/System Controler"), q, "categoryName");

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
}

Object::Private::~Private()
{
}

void Object::Private::enabledChanged(bool enabled)
{
    if (enabled) {
        if (!applicationManager) {
            applicationManager = new QimsysApplicationManager(this);
            applicationManager->init();
            connect(applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(languageChanged()), Qt::QueuedConnection);
        }
        languageChanged();
    } else {
        if (applicationManager) {
            disconnect(applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(languageChanged()));
            applicationManager->deleteLater();
            applicationManager = 0;
        }
        if (icon) {
            delete icon;
            icon = 0;
        }
    }
}

void Object::Private::languageChanged()
{
    qimsysDebugIn();
    if (icon) {
        delete icon;
    }
    icon = new Icon(q);
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
