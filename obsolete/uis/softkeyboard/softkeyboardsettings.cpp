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

#include "softkeyboardsettings.h"
#include "ui_softkeyboardsettings.h"
#include "qimsysabstractpluginobject.h"
#include "translator.h"

#include <QSettings>

class SoftKeyboardSettings::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysAbstractPluginObject *plugin, SoftKeyboardSettings *parent);
    ~Private();

    void save();
private:
    SoftKeyboardSettings *q;
    Ui::SoftKeyboardSettings ui;
};

SoftKeyboardSettings::Private::Private(QimsysAbstractPluginObject *plugin, SoftKeyboardSettings *parent)
    : QObject(parent)
    , q(parent)
{
    Translator::begin();
    ui.setupUi(q);
    Translator::ui(q);
    Translator::end();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
// ui.animation->setChecked( settings.value( "Animation", true ).toBool() );
// ui.hide->setChecked( settings.value( "Hide", false ).toBool() );

    ui.tabWidget->addTab(q->about(plugin, "", ui.tabWidget), "");
    TR(ui.tabWidget, 1, this, QT_TR_NOOP("&About Plugin"));
}

SoftKeyboardSettings::Private::~Private()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
// settings.setValue( "Animation", ui.animation->isChecked() );
// settings.setValue( "Hide", ui.hide->isChecked() );
}

void SoftKeyboardSettings::Private::save()
{
}

SoftKeyboardSettings::SoftKeyboardSettings(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QimsysSettingsWidget(parent)
{
    d = new Private(plugin, this);
}

SoftKeyboardSettings::~SoftKeyboardSettings()
{
    delete d;
}

void SoftKeyboardSettings::save()
{
    d->save();
}

#include "softkeyboardsettings.moc"
