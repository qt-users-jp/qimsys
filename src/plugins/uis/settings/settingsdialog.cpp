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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "qimsysdebug.h"
#include "qimsyspluginmanager.h"

#include "qimsyssettingswidget.h"
#include <QMap>
#include <QSettings>

class SettingsDialog::Private : private QObject
{
    Q_OBJECT
public:
    Private(SettingsDialog *parent);
    ~Private();

private slots:
    void setup();
    void currentItemChanged(QTreeWidgetItem *current);
    void itemChanged(QTreeWidgetItem *item);
    void clicked(QAbstractButton *button);

private:
    QTreeWidgetItem *findItem(const QString &path, QTreeWidgetItem *parentItem = 0);

private:
    SettingsDialog *q;
    Ui::SettingsDialog ui;

    QMap<QTreeWidgetItem*, QimsysAbstractPluginObject*> objects;
};

SettingsDialog::Private::Private(SettingsDialog *parent)
    : QObject(parent)
    , q(parent)
{
    q->setAttribute(Qt::WA_DeleteOnClose);
    ui.setupUi(q);
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    q->resize(settings.value("Size", q->size()).toSize());
    ui.splitter->restoreState(settings.value("Splitter").toByteArray());
    connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(clicked(QAbstractButton*)));
    metaObject()->invokeMethod(this, "setup", Qt::QueuedConnection);
}

SettingsDialog::Private::~Private()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("Size", q->size());
    settings.setValue("Splitter", ui.splitter->saveState());
}

void SettingsDialog::Private::setup()
{
    foreach(QimsysAbstractPluginObject *object, QimsysPluginManager::objects<QimsysAbstractPluginObject>(this)) {
        if (object->categoryType() == QimsysAbstractPluginObject::Hidden) continue;
        QTreeWidgetItem *parentItem = findItem(object->categoryName());
        parentItem->setData(0, Qt::UserRole, object->categoryType());
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
        item->setIcon(0, object->icon());
        item->setText(0, object->name());
        item->setData(0, Qt::UserRole, object->name());
        item->setCheckState(0, Qt::Checked);
        objects[item] = object;
    }
    connect(ui.plugins, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentItemChanged(QTreeWidgetItem*)));
    connect(ui.plugins, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*)));
    foreach(QTreeWidgetItem *item, objects.keys()) {
        QimsysAbstractPluginObject *object = objects[item];
        if (object->isEnabled()) {
            switch (object->categoryType()) {
            case QimsysAbstractPluginObject::CanBeNone:
                item->setCheckState(0, Qt::Checked);
                break;
            case QimsysAbstractPluginObject::AlwaysOne:
                item->setCheckState(0, Qt::PartiallyChecked);
                break;
            case QimsysAbstractPluginObject::MoreThanOne:
                item->setCheckState(0, Qt::PartiallyChecked);
                break;
            case QimsysAbstractPluginObject::OneOrNone:
                item->setCheckState(0, Qt::Checked);
                break;
            case QimsysAbstractPluginObject::All:
                item->setCheckState(0, Qt::PartiallyChecked);
                break;
            default:
                break;
            }
        } else {
            item->setCheckState(0, Qt::Unchecked);
        }
    }
}

QTreeWidgetItem *SettingsDialog::Private::findItem(const QString &path, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *ret = 0;
    QStringList items = path.split("/");
    QString text = items.takeFirst();
    if (parentItem == 0) {
        for (int i = 0; i < ui.plugins->topLevelItemCount(); i++) {
            if (ui.plugins->topLevelItem(i)->text(0) == text) {
                ret = ui.plugins->topLevelItem(i);
                break;
            }
        }
        if (!ret) {
            ret = new QTreeWidgetItem(ui.plugins);
            ret->setText(0, text);
            ret->setExpanded(true);
        }
    } else {
        for (int i = 0; i < parentItem->childCount(); i++) {
            if (parentItem->child(i)->text(0) == text) {
                ret = parentItem->child(i);
                break;
            }
        }
        if (!ret) {
            ret = new QTreeWidgetItem(parentItem);
            ret->setText(0, text);
            ret->setExpanded(true);
        }
    }
    if (!items.isEmpty()) {
        ret = findItem(items.join("/"), ret);
    }
    return ret;
}

void SettingsDialog::Private::currentItemChanged(QTreeWidgetItem *current)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (ui.stackedWidget->count()) {
        QWidget *w = ui.stackedWidget->widget(0);
        ui.stackedWidget->removeWidget(w);
        delete w;
    }
    foreach(QimsysAbstractPluginObject *object, QimsysPluginManager::objects<QimsysAbstractPluginObject>(this)) {
        if (object->name() == current->data(0, Qt::UserRole)) {
            QWidget *w = object->settings(QLatin1String("desktop"), ui.stackedWidget);
            if (w) {
                ui.stackedWidget->addWidget(w);
            }
        }
    }
    QApplication::restoreOverrideCursor();
}

void SettingsDialog::Private::itemChanged(QTreeWidgetItem *item)
{
    QTreeWidgetItem *parentItem = item->parent();
    QimsysAbstractPluginObject::CategoryType type = (QimsysAbstractPluginObject::CategoryType)parentItem->data(0, Qt::UserRole).toInt();
    QList<QTreeWidgetItem*> checked;
    for (int i = 0; i < parentItem->childCount(); i++) {
        if (parentItem->child(i)->checkState(0) != Qt::Unchecked) {
            checked.append(parentItem->child(i));
        }
    }
    switch (type) {
    case QimsysAbstractPluginObject::CanBeNone:
        break;
    case QimsysAbstractPluginObject::MoreThanOne: {
        switch (checked.count()) {
        case 0:
            item->setCheckState(0, Qt::PartiallyChecked);
            break;
        case 1:
            foreach(QTreeWidgetItem *i, checked) {
                i->setCheckState(0, Qt::PartiallyChecked);
            }
            break;
        case 2:
            foreach(QTreeWidgetItem *i, checked) {
                i->setCheckState(0, Qt::Checked);
            }
            break;
        }
        break;
    }
    case QimsysAbstractPluginObject::AlwaysOne: {
        switch (checked.count()) {
        case 0:
            item->setCheckState(0, Qt::PartiallyChecked);
            break;
        case 1:
            checked.first()->setCheckState(0, Qt::PartiallyChecked);
            break;
        case 2:
            foreach(QTreeWidgetItem *i, checked) {
                i->setCheckState(0, item == i  ? Qt::PartiallyChecked : Qt::Unchecked);
            }
            break;
        }
        break;
    }
    case QimsysAbstractPluginObject::OneOrNone: {
        switch (checked.count()) {
        case 0:
            break;
        case 1:
            checked.first()->setCheckState(0, Qt::Checked);
            break;
        case 2:
            foreach(QTreeWidgetItem *i, checked) {
                i->setCheckState(0, item == i  ? Qt::Checked : Qt::Unchecked);
            }
            break;
        }
        break;
    }
    case QimsysAbstractPluginObject::All: {
        item->setCheckState(0, Qt::PartiallyChecked);
        break;
    }
    default:
        break;
    }
}

void SettingsDialog::Private::clicked(QAbstractButton *button)
{
    if (ui.buttonBox->buttonRole(button) != QDialogButtonBox::ApplyRole) return;
    QimsysSettingsWidget *widget = qobject_cast<QimsysSettingsWidget*>(ui.stackedWidget->currentWidget());
    if (widget) {
        widget->save();
    }
    foreach(QTreeWidgetItem *item, objects.keys()) {
        objects[item]->setEnabled(item->checkState(0) != Qt::Unchecked);
    }
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    d = new Private(this);
}

SettingsDialog::~SettingsDialog()
{
    delete d;
}

bool SettingsDialog::event(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        emit languageChanged();
    }
    return QDialog::event(e);
}

#include "settingsdialog.moc"
