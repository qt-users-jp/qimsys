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

#include "settings.h"
#include "ui_settings.h"

#include <QSettings>
#include <qimsysabstractpluginobject.h>

using namespace Japanese::SocialIME;

class Settings::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysAbstractPluginObject *plugin, Settings *parent);
    ~Private();

    void save();
private:
    Settings *q;
    Ui::Settings ui;
    QimsysAbstractPluginObject *plugin;
};

Settings::Private::Private(QimsysAbstractPluginObject *p, Settings *parent)
    : QObject(parent)
    , q(parent)
    , plugin(p)
{
    ui.setupUi(q);
    ui.tabWidget->addTab(q->about(plugin, ""), "");

    QSettings settings;
    settings.beginGroup(plugin->metaObject()->className());
    ui.user->setText(settings.value("User").toString());
}

Settings::Private::~Private()
{
}

void Settings::Private::save()
{
    QSettings settings;
    settings.beginGroup(plugin->metaObject()->className());
    settings.setValue("User", ui.user->text());
}

Settings::Settings(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QimsysSettingsWidget(parent)
{
    d = new Private(plugin, this);
}

Settings::~Settings()
{
    delete d;
}

void Settings::save()
{
    d->save();
    QimsysSettingsWidget::save();
}

#include "settings.moc"
