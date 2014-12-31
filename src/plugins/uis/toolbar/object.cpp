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
#include "widget.h"
#include "settings.h"

#include <qimsysdebug.h>
#include <qimsysdynamictranslator.h>

namespace ToolBar {

class Object::Private : private QObject
{
    Q_OBJECT
public:
    Private(Object *parent);
    ~Private();

    QimsysSettingsWidget *settings(const QString &hint, QWidget *parent = 0);

private slots:
    void enabledChanged(bool enabled);
    void languageChanged();
    void saved();

private:
    Object *q;
    Widget *widget;
};

}

using namespace ToolBar;

Object::Private::Private(Object *parent)
    : QObject(parent)
    , q(parent)
    , widget(0)
{
    q->setIdentifier(QLatin1String("Tool Bar"));
    q->setPriority(0x00);

    trConnect(this, QT_TR_NOOP("Tool Bar"), q, "name");
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
    delete widget;
}

void Object::Private::enabledChanged(bool enabled)
{
    if (enabled && !widget) {
        languageChanged();
    } else if (!enabled && widget) {
        delete widget;
        widget = 0;
    }
}

void Object::Private::languageChanged()
{
    qimsysDebugIn();
    if (widget)
        delete widget;
    widget = new Widget(q);
    connect(widget, SIGNAL(languageChanged()), this, SLOT(languageChanged()));
    qimsysDebugOut();
}

QimsysSettingsWidget *Object::Private::settings(const QString &hint, QWidget *parent)
{
    Settings *settings = new Settings(q, parent);
    connect(settings, SIGNAL(saved()), this, SLOT(saved()));
    return settings;
}

void Object::Private::saved()
{
    if (!widget) return;
    widget->settingsUpdated();
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

QimsysSettingsWidget *Object::settings(const QString &hint, QWidget *parent)
{
    return d->settings(hint, parent);
}

#include "object.moc"
