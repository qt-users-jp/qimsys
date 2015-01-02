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

#include <qimsysdebug.h>
#include <qimsysabstractpluginobject.h>

#include <QSettings>

using namespace ToolBar;

class Settings::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysAbstractPluginObject *object, Settings *parent);
    ~Private();

    void save();

private slots:
    void toggled();

private:
    Settings *q;
    Ui::Settings ui;
    QimsysAbstractPluginObject *plugin;
};

Settings::Private::Private(QimsysAbstractPluginObject *object, Settings *parent)
    : QObject(parent)
    , q(parent)
    , plugin(object)
{
    ui.setupUi(q);

    connect(ui.always, SIGNAL(toggled(bool)), this, SLOT(toggled()));
    connect(ui.available, SIGNAL(toggled(bool)), this, SLOT(toggled()));
    connect(ui.active, SIGNAL(toggled(bool)), this, SLOT(toggled()));
    connect(ui.inactive, SIGNAL(toggled(bool)), this, SLOT(toggled()));
    connect(ui.unavailable, SIGNAL(toggled(bool)), this, SLOT(toggled()));

    QSettings settings;
    settings.beginGroup(plugin->metaObject()->className());
    ui.active->setChecked(settings.value("Active", ui.active->isChecked()).toBool());
    ui.available->setChecked(settings.value("Available", ui.available->isChecked()).toBool());
    ui.always->setChecked(settings.value("Always", ui.always->isChecked()).toBool());
    ui.unavailable->setChecked(settings.value("Unavailable", ui.unavailable->isChecked()).toBool());
    ui.inactive->setChecked(settings.value("Inactive", ui.inactive->isChecked()).toBool());
    toggled();

    ui.tabWidget->addTab(q->about(plugin, "", ui.tabWidget), tr("&About Plugin"));
}

Settings::Private::~Private()
{
}

void Settings::Private::save()
{
    QSettings settings;
    settings.beginGroup(plugin->metaObject()->className());
    settings.setValue("Active", ui.active->isChecked());
    settings.setValue("Available", ui.available->isChecked());
    settings.setValue("Always", ui.always->isChecked());
    settings.setValue("Unavailable", ui.unavailable->isChecked());
    settings.setValue("Inactive", ui.inactive->isChecked());
}

void Settings::Private::toggled()
{
    bool always = ui.always->isChecked();
    bool available = ui.available->isChecked();
    ui.inactive->setVisible(always | available);
    ui.line->setVisible(always | available);
    ui.unavailable->setVisible(always);
    ui.unavailable->setEnabled(ui.inactive->isEnabled() && !ui.inactive->isChecked());
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
