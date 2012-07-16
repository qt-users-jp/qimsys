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

#include "keyactionedit.h"
#include "ui_keyactionedit.h"
#include "keyactionmanager.h"
#include "translator.h"
#include "qimsysdebug.h"

using namespace ja_JP;

class KeyActionEdit::Private : private QObject
{
    Q_OBJECT
public:
    Private(KeyActionEdit *parent);
    ~Private();

    void setState(State state);
    void setDataList(KeyActionDataList *list);

private slots:
    void add();
    void remove();
    void currentItemChanged(QTreeWidgetItem *item);
    void sequenceChanged(const QimsysKeySequence &key);
    void actionChanged(int index);

private:
    KeyActionEdit *q;
    Ui::KeyActionEdit ui;
    KeyActionManager *actions;
    State state;
    KeyActionDataList *list;
};

KeyActionEdit::Private::Private(KeyActionEdit *parent)
    : QObject(parent)
    , q(parent)
    , actions(KeyActionManager::instance())
    , list(0)
{
    Translator::begin();
    ui.setupUi(q);
    Translator::ui(q);
    Translator::end();

    connect(ui.add, SIGNAL(clicked()), this, SLOT(add()));
    connect(ui.remove, SIGNAL(clicked()), this, SLOT(remove()));
    connect(ui.list, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentItemChanged(QTreeWidgetItem*)));
    connect(ui.key, SIGNAL(sequenceChanged(QimsysKeySequence)), this, SLOT(sequenceChanged(QimsysKeySequence)));
    connect(ui.action, SIGNAL(currentIndexChanged(int)), this, SLOT(actionChanged(int)));
}

KeyActionEdit::Private::~Private()
{
}

void KeyActionEdit::Private::setState(State s)
{
    state = s;
    ui.action->clear();
    foreach(KeyAction *action, actions->actions()) {
        if (action->states() & state) {
            ui.action->addItem(action->name(), action->name());
            TR(ui.action, ui.action->count() - 1, "Converter::Private", action->name());
        }
    }
}

void KeyActionEdit::Private::setDataList(KeyActionDataList *l)
{
    list = l;

    ui.list->clear();
    for (int i = 0; i < list->count(); i++) {
        KeyActionData data = list->at(i);
        if (data.state == state) {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui.list);
            QString key = data.key.toString();
            item->setText(0, key);
            item->setData(0, Qt::UserRole, data.user);
            item->setText(1, data.action->name());
            item->setData(1, Qt::UserRole, data.action->name());
            TR(ui.list, QPoint(ui.list->topLevelItemCount() - 1, 1), "Converter::Private", data.action->name());
        }
    }
    currentItemChanged(ui.list->currentItem());
}

void KeyActionEdit::Private::add()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui.list);
    item->setText(0, "");
    item->setData(0, Qt::UserRole, true);
    item->setText(1, ui.action->itemText(0));
    item->setData(1, Qt::UserRole, ui.action->itemData(0));
    ui.list->clearSelection();
    ui.list->setCurrentItem(item);
// ui.list->scrollToItem( item );

    KeyActionData data;
    data.state = state;
    data.action = KeyActionManager::instance()->action(ui.action->itemData(0).toString());
    data.user = true;
    list->append(data);
}

void KeyActionEdit::Private::remove()
{
    QTreeWidgetItem *item = ui.list->currentItem();
    if (!item) return;
    QimsysKeySequence key(item->text(0));
    KeyAction *action = KeyActionManager::instance()->action(item->text(1));
    for (int i = 0; i < list->count(); i++) {
        KeyActionData data = list->at(i);
        if (data.state == state
                && data.key == key
                && data.action == action) {
            list->removeAt(i);
            break;
        }
    }
    int index = ui.list->indexOfTopLevelItem(item);
    delete ui.list->takeTopLevelItem(index);
}

void KeyActionEdit::Private::currentItemChanged(QTreeWidgetItem *item)
{
    ui.label->setEnabled(item);
    ui.key->setEnabled(item);
    ui.action->setEnabled(item);
    if (item) {
        QString key = item->text(0);
        ui.key->setSequence(QimsysKeySequence(key));
        int index = ui.action->findData(item->data(1, Qt::UserRole).toString());
        ui.action->setCurrentIndex(qMax(0, index));
    }
}

void KeyActionEdit::Private::sequenceChanged(const QimsysKeySequence &key)
{
    QTreeWidgetItem *item = ui.list->currentItem();
    if (!item) return;
    QList<QTreeWidgetItem*> items = ui.list->findItems(key.toString(), Qt::MatchExactly);
    items.removeAll(item);
    if (items.isEmpty()) {
        if (item->text(0) != key.toString()) {
            QimsysKeySequence oldKey(item->text(0));
            KeyAction *action = KeyActionManager::instance()->action(item->text(1));
            for (int i = 0; i < list->count(); i++) {
                KeyActionData data = list->at(i);
                if (data.state == state
                        && data.key == oldKey
                        && data.action == action) {
                    data.key = key;
                    data.user = true;
                    list->replace(i, data);
                    break;
                }
            }
            item->setText(0, key.toString());
//            ui.key->clearFocus();
        }
    } else {
        ui.key->reset();
    }
}

void KeyActionEdit::Private::actionChanged(int index)
{
    QTreeWidgetItem *item = ui.list->currentItem();
    if (!item) return;

    QimsysKeySequence key(item->text(0));
    KeyAction *action = KeyActionManager::instance()->action(item->text(1));
    for (int i = 0; i < list->count(); i++) {
        KeyActionData data = list->at(i);
        if (data.state == state
                && data.key == key
                && data.action == action) {
            data.action = KeyActionManager::instance()->action(ui.action->itemData(index).toString());
            data.user = true;
            list->replace(i, data);
            break;
        }
    }
    item->setText(1, ui.action->currentText());
}

KeyActionEdit::KeyActionEdit(QWidget *parent)
    : QWidget(parent)
{
    d = new Private(this);
}

KeyActionEdit::~KeyActionEdit()
{
    delete d;
}

void KeyActionEdit::setState(State state)
{
    d->setState(state);
}

void KeyActionEdit::setDataList(KeyActionDataList *list)
{
    d->setDataList(list);
}

#include "keyactionedit.moc"
