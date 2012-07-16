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

#include "softkeyboardobject.h"
#include "softkeyboard.h"
#include "softkeyboardsettings.h"
#include "translator.h"

#include <QSettings>

class SoftKeyboardObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(SoftKeyboardObject *parent);
    ~Private();

private slots:
    void enabledChanged(bool enabled);

private:
    SoftKeyboardObject *q;
    SoftKeyboard *keyboard;
};

SoftKeyboardObject::Private::Private(SoftKeyboardObject *parent)
    : QObject(parent)
    , q(parent)
    , keyboard(0)
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    // disabled by default
    settings.setValue("Enabled", settings.value("Enabled", false).toBool());

    q->setCategoryType(OneOrNone);
    q->setCategoryName(tr("User Interface/Input Window"));

    q->setIcon(QIcon(":/softkeyboard.png"));
    q->setName(tr("Software Keyboard"));
    q->setAuthor(tr("Tasuku Suzuki"));
    q->setTranslator(tr("None"));
    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)), Qt::QueuedConnection);
    enabledChanged(q->isEnabled());
}

SoftKeyboardObject::Private::~Private()
{
    delete keyboard;
}

void SoftKeyboardObject::Private::enabledChanged(bool enabled)
{
    if (enabled && !keyboard) {
        keyboard = new SoftKeyboard(this);
    } else if (!enabled && keyboard) {
        delete keyboard;
        keyboard = 0;
    }
}

SoftKeyboardObject::SoftKeyboardObject(QObject *parent)
    : QimsysAbstractPluginObject(parent)
{
    d = new Private(this);
}

SoftKeyboardObject::~SoftKeyboardObject()
{
    delete d;
}

QimsysSettingsWidget *SoftKeyboardObject::settings(const QString &hint, QWidget *parent)
{
    return QimsysAbstractPluginObject::settings(hint, parent);
// return new SoftKeyboardSettings( this, parent );
}

#include "softkeyboardobject.moc"
