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

#include "widget.h"
#include "ui_widget.h"
#include "inputmethodcontroller.h"

#include <qimsysdebug.h>
#include <qbinding.h>

#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>

#include <QSettings>
#include <QSignalMapper>

using namespace N900::Dialog;

static const int defaultIconSize = 32;

class Widget::Private : private QObject
{
    Q_OBJECT
public:

    Private(QimsysAbstractPluginObject *object, Widget *parent);
    ~Private();

private slots:
    void init();

    void currentIndexChanged(int index);
    void currentIconChanged(const QIcon &icon);
    void toggled(bool checked);
    void triggered(int action);

private:
    void setupUi();

private:
    Widget *q;
    Ui::Widget ui;
    QimsysAbstractPluginObject *plugin;

    QFormLayout *layout;
};

Widget::Private::Private(QimsysAbstractPluginObject *object, Widget *parent)
    : QObject(parent)
    , q(parent)
    , plugin(object)
{
    qimsysDebugIn() << object << parent;
    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    qimsysDebugOut();
}

Widget::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Widget::Private::init()
{
    qimsysDebugIn();

    setupUi();

    qimsysDebugOut();
}

void Widget::Private::setupUi()
{
    qimsysDebugIn();
    ui.setupUi(q);

    layout = new QFormLayout;

    InputMethodController *controller = new InputMethodController(this);

    foreach (QAction *parentAction, controller->actions()) {
        qimsysDebug() << parentAction->text();
        if (parentAction->text() != QLatin1String("-")) {
            if (parentAction->findChild<QAction*>()){
                QComboBox *combobox = new QComboBox;
                combobox->setIconSize(QSize(defaultIconSize, defaultIconSize));
                foreach (QAction *action, parentAction->findChildren<QAction*>()) {
                    if (action->text() != QLatin1String("-")) {
                        connect(action, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
                        combobox->addItem(action->icon(), action->text(), qVariantFromValue(qobject_cast<QObject*>(action)));
                        if (action->isChecked()) {
                            combobox->setCurrentIndex(combobox->count() - 1);
                        }
                    }
                }
                connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
                layout->addRow(parentAction->text(), combobox);
            } else {
                QPushButton *button = new QPushButton;
                button->setIconSize(QSize(defaultIconSize, defaultIconSize));
                button->setIcon(parentAction->icon());
                button->setText(tr("Open..."));
                connect(button, SIGNAL(clicked()), parentAction, SLOT(trigger()));
                connect(button, SIGNAL(clicked()), q, SLOT(close()), Qt::QueuedConnection);
                QString label = parentAction->text();
                if (label.endsWith("...")) {
                    label = label.left(label.length() - 3);
                }
                layout->addRow(label, button);
                new QBinding(parentAction, "enabled", button, "enabled");
            }
        }
    }

    ui.scrollAreaWidgetContents->setLayout(layout);
    qimsysDebugOut();
}

void Widget::Private::currentIndexChanged(int index)
{
    qimsysDebugIn() << index;
    QComboBox *combobox = qobject_cast<QComboBox*>(sender());
    QAction *action = qobject_cast<QAction*>(combobox->itemData(index).value<QObject*>());
    action->trigger();
    qimsysDebugOut();
}

void Widget::Private::toggled(bool checked)
{
    if (!checked) return;
    qimsysDebugIn() << checked;
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action->data().isNull()) {
        foreach (QComboBox *combobox, q->findChildren<QComboBox*>()) {
            bool found = false;
            for (int i = 0; i < combobox->count(); i++) {
                if (action == qobject_cast<QAction*>(combobox->itemData(i).value<QObject*>())) {
                    combobox->blockSignals(true);
                    combobox->setCurrentIndex(i);
                    combobox->blockSignals(false);
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
    }
    qimsysDebugOut();
}

Widget::Widget(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QDialog(parent)
{
    qimsysDebugIn();
    d = new Private(plugin, this);
    qimsysDebugOut();
}

Widget::~Widget()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "widget.moc"
