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

#include "settingsobject.h"
#include "settingsdialog.h"

#include <qimsysapplicationmanager.h>
#include <qimsysdebug.h>
#include <qimsysdynamictranslator.h>

#include <QPointer>

class SettingsObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(SettingsObject *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);
    void languageChanged();
    void triggered(int type);

private:
    SettingsObject *q;
    QPointer<SettingsDialog> dialog;
    QimsysApplicationManager manager;
};

SettingsObject::Private::Private(SettingsObject *parent)
    : QObject(parent)
    , q(parent)
{
    q->setCategoryType(AlwaysOne);
    trConnect(this, QT_TR_NOOP("User Interface/Settings Dialog"), q, "categoryName");
    q->setIcon(QIcon(":/icons/configure.png"));
    trConnect(this, QT_TR_NOOP("Default settings dialog"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "description");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
    manager.init();
}

SettingsObject::Private::~Private()
{
    delete dialog;
}

void SettingsObject::Private::enabledChanged(bool enabled)
{
    if (enabled) {
        connect(&manager, SIGNAL(triggered(int)), this, SLOT(triggered(int)));
    } else {
        disconnect(&manager, SIGNAL(triggered(int)), this, SLOT(triggered(int)));
    }
}

void SettingsObject::Private::triggered(int type)
{
    if (type == QimsysApplicationManager::ShowSettings) {
        if (!dialog) {
            languageChanged();
        } else {
            dialog->activateWindow();
        }
    }
}

void SettingsObject::Private::languageChanged()
{
    qimsysDebugIn();
    delete dialog;
    dialog = new SettingsDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(languageChanged()), this, SLOT(languageChanged()), Qt::QueuedConnection);
    dialog->open();
    qimsysDebugOut();
}

SettingsObject::SettingsObject(QObject *parent)
    : QimsysAbstractPluginObject(parent)
{
    d = new Private(this);
}

SettingsObject::~SettingsObject()
{
    delete d;
}

#include "settingsobject.moc"
