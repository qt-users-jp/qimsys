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
#include "keyboard.h"

#include <qimsyscommandlineargs.h>
#include <qimsyskeyboardmanager.h>
#include <qimsysdynamictranslator.h>

#include <QPointer>

using namespace N900::Keyboard;

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);

private:
    Object *q;
    QList< QPointer<Keyboard> > keyboards;
    QimsysKeyboardManager *keyboardManager;
};

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , keyboardManager(0)
{
    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("User Interface/Input Window"), q, "categoryName");
    trConnect(this, QT_TR_NOOP("N900: Virtual Keyboard"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
}

Object::Private::~Private()
{
    qDeleteAll(keyboards);
}

void Object::Private::enabledChanged(bool enabled)
{
    keyboards.removeAll(QPointer<Keyboard>());
    if (enabled) {
        if (!keyboardManager) {
            keyboardManager = new QimsysKeyboardManager(this);
            keyboardManager->init();
        }
        QVariant url = QimsysCommandlineArgs::instance()->value("Virtual Keyboard");
        QPointer<Keyboard> keyboard = new Keyboard(q);
        if (url.isNull()) {
            keyboard->setSource(keyboard->resolveUrl("N900/Keyboard/main.qml"));
        } else {
            keyboard->setSource(url.toUrl());
        }
        keyboards.append(keyboard);

        connect(keyboard, SIGNAL(geometryChanged(QRect)), keyboardManager, SLOT(setGeometry(QRect)));
        keyboardManager->setGeometry(keyboard->geometry());

        url = QimsysCommandlineArgs::instance()->value("Virtual Keyboard(Sub)");
        if (!url.isNull()) {
            QPointer<Keyboard> keyboard = new Keyboard(q);
            keyboard->setSource(url.toUrl());
            keyboards.append(keyboard);
        }
    } else if (!enabled) {
        qDeleteAll(keyboards);
        keyboards.clear();
        if (keyboardManager) {
            keyboardManager->deleteLater();
            keyboardManager = 0;
        }
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
