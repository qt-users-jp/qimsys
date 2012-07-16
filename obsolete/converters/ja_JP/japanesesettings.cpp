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

#include "japanesesettings.h"
#include "ui_japanesesettings.h"
#include "keyactionmanager.h"
#include "inputmanager.h"
#include "qimsysdebug.h"
#include "translator.h"

#include <QSettings>

using namespace ja_JP;

class JapaneseSettings::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysAbstractPluginObject *plugin, JapaneseSettings *parent);
    ~Private();

    void save();

private slots:
    void setStyle(int index);

private:
    JapaneseSettings *q;
    Ui::JapaneseSettings ui;
    KeyActionManager *actions;
    KeyActionDataList list;
};

JapaneseSettings::Private::Private(QimsysAbstractPluginObject *plugin, JapaneseSettings *parent)
    : QObject(parent)
    , q(parent)
    , actions(KeyActionManager::instance())
{
    Translator::begin();
    ui.setupUi(q);
    Translator::ui(q, "JapaneseSettings");
    Translator::end();

    list = actions->list();
    ui.style->addItem("MS IME", ":/keyassigns/msime.txt");
    ui.style->addItem("ATOK", ":/keyassigns/atok.txt");
    for (int i = 0; i < ui.style->count(); i++) {
        if (ui.style->itemData(i).toString() == list.fileName()) {
            ui.style->setCurrentIndex(i);
            break;
        }
    }
    connect(ui.style, SIGNAL(currentIndexChanged(int)), this, SLOT(setStyle(int)));

    ui.direct->setState(Direct);
    ui.empty->setState(EmptyString);
    ui.input->setState(InputingString);
    ui.convert->setState(ConvertingString);
    ui.select->setState(SelectingCandidate);
    ui.predict->setState(SelectingPrediction);

    ui.direct->setDataList(&list);
    ui.empty->setDataList(&list);
    ui.input->setDataList(&list);
    ui.convert->setDataList(&list);
    ui.select->setDataList(&list);
    ui.predict->setDataList(&list);

    ui.tabWidget->addTab(q->about(plugin, "", q), QString::null);
    TR(ui.tabWidget, ui.tabWidget->count() - 1, this, QT_TR_NOOP("&About Plugin"));

    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    q->resize(settings.value("size", q->size()).toSize());
    settings.endGroup();
}

JapaneseSettings::Private::~Private()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("size", q->size());
    settings.endGroup();
}

void JapaneseSettings::Private::setStyle(int index)
{
    list = KeyActionDataList(ui.style->itemData(index).toString());

    ui.direct->setDataList(&list);
    ui.empty->setDataList(&list);
    ui.input->setDataList(&list);
    ui.convert->setDataList(&list);
    ui.select->setDataList(&list);
    ui.predict->setDataList(&list);
}

void JapaneseSettings::Private::save()
{
    KeyActionManager::instance()->setList(list);
}

JapaneseSettings::JapaneseSettings(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QimsysSettingsWidget(parent)
{
    d = new Private(plugin, this);
}

JapaneseSettings::~JapaneseSettings()
{
    delete d;
}

void JapaneseSettings::save()
{
    d->save();
    QimsysSettingsWidget::save();
}

#include "japanesesettings.moc"
