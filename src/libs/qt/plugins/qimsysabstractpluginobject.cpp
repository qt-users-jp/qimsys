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

#include "qimsysabstractpluginobject.h"
#include "qimsysdebug.h"
#include "qimsyssettingswidget.h"

#include <QSettings>
#include <QStringList>

class QimsysAbstractPluginObject::Private
{
public:
    QString identifier;
    int priority;

    QString locale;
    QString language;

#ifndef QIMSYS_NO_GUI
    QIcon icon;
#endif
    QString name;
    QString description;
    QString author;
    QString url;
    QString translator;
    QString license;

    bool active;

    QStringList groups;
    CategoryType categoryType;
    QString categoryName;
};

QimsysAbstractPluginObject::QimsysAbstractPluginObject(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d = new Private;
    d->priority = 0;
    d->active = false;
    d->categoryType = Hidden;
    qimsysDebugOut();
}

QimsysAbstractPluginObject::~QimsysAbstractPluginObject()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::identifier() const
{
    return d->identifier;
}

void QimsysAbstractPluginObject::setIdentifier(const QString &identifier)
{
    if (d->identifier == identifier) return;
    d->identifier = identifier;
    emit identifierChanged(identifier);
}

int QimsysAbstractPluginObject::priority() const
{
    return d->priority;
}

void QimsysAbstractPluginObject::setPriority(int priority)
{
    if (d->priority == priority) return;
    d->priority = priority;
    emit priorityChanged(priority);
}


const QString &QimsysAbstractPluginObject::locale() const
{
    return d->locale;
}

void QimsysAbstractPluginObject::setLocale(const QString &locale)
{
    if (d->locale == locale) return;
    qimsysDebugIn() << locale;
    d->locale = locale;
    emit localeChanged(d->locale);
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::language() const
{
    return d->language;
}

void QimsysAbstractPluginObject::setLanguage(const QString &language)
{
    if (d->language == language) return;
    qimsysDebugIn() << language;
    d->language = language;
    emit languageChanged(d->language);
    qimsysDebugOut();
}

#ifndef QIMSYS_NO_GUI
const QIcon &QimsysAbstractPluginObject::icon() const
{
    return d->icon;
}

void QimsysAbstractPluginObject::setIcon(const QIcon &icon)
{
    if (d->icon.cacheKey() == icon.cacheKey()) return;
    qimsysDebugIn() << icon.availableSizes();
    d->icon = icon;
    emit iconChanged(d->icon);
    qimsysDebugOut();
}
#endif

const QString &QimsysAbstractPluginObject::name() const
{
    return d->name;
}

void QimsysAbstractPluginObject::setName(const QString &name)
{
    if (d->name == name) return;
    qimsysDebugIn() << name;
    d->name = name;
    emit nameChanged(d->name);
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::description() const
{
    return d->description;
}

void QimsysAbstractPluginObject::setDescription(const QString &description)
{
    if (d->description == description) return;
    qimsysDebugIn() << description;
    d->description = description;
    emit descriptionChanged(d->description);
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::author() const
{
    return d->author;
}

void QimsysAbstractPluginObject::setAuthor(const QString &author)
{
    if (d->author == author) return;
    qimsysDebugIn() << author;
    d->author = author;
    emit authorChanged(d->author);
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::url() const
{
    return d->url;
}

void QimsysAbstractPluginObject::setUrl(const QString &url)
{
    if (d->url == url) return;
    qimsysDebugIn() << url;
    d->url = url;
    emit urlChanged(d->url);
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::translator() const
{
    return d->translator;
}

void QimsysAbstractPluginObject::setTranslator(const QString &translator)
{
    if (d->translator == translator) return;
    qimsysDebugIn() << translator;
    d->translator = translator;
    emit translatorChanged(d->translator);
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::license() const
{
    return d->license;
}

void QimsysAbstractPluginObject::setLicense(const QString &license)
{
    if (d->license == license) return;
    qimsysDebugIn() << license;
    d->license = license;
    emit licenseChanged(d->license);
    qimsysDebugOut();
}


bool QimsysAbstractPluginObject::isEnabled() const
{
    qimsysDebugIn();
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    bool ret = settings.value("Enabled", true).toBool();
    qimsysDebugOut() << ret;
    return ret;
}

void QimsysAbstractPluginObject::setEnabled(bool enabled)
{
    if (isEnabled() == enabled) return;
    qimsysDebugIn() << enabled;
    QSettings settings;
    settings.beginGroup(metaObject()->className());
    settings.setValue("Enabled", enabled);
    emit enabledChanged(enabled);
    qimsysDebugOut();
}

bool QimsysAbstractPluginObject::isActive() const
{
    return d->active;
}

void QimsysAbstractPluginObject::setActive(bool active)
{
    if (d->active == active) return;
    d->active = active;
    emit activeChanged(active);
}


const QStringList &QimsysAbstractPluginObject::groups() const
{
    return d->groups;
}

void QimsysAbstractPluginObject::setGroups(const QStringList &groups)
{
    if (d->groups == groups) return;
    qimsysDebugIn() << groups;
    d->groups = groups;
    qimsysDebugOut();
}

QimsysAbstractPluginObject::CategoryType QimsysAbstractPluginObject::categoryType() const
{
    return d->categoryType;
}

void QimsysAbstractPluginObject::setCategoryType(QimsysAbstractPluginObject::CategoryType categoryType)
{
    if (d->categoryType == categoryType) return;
    qimsysDebugIn() << categoryType;
    d->categoryType = categoryType;
    qimsysDebugOut();
}

const QString &QimsysAbstractPluginObject::categoryName() const
{
    return d->categoryName;
}

void QimsysAbstractPluginObject::setCategoryName(const QString &categoryName)
{
    if (d->categoryName == categoryName) return;
    qimsysDebugIn() << categoryName;
    d->categoryName = categoryName;
    qimsysDebugOut();
}

#ifndef QIMSYS_NO_GUI
QimsysSettingsWidget *QimsysAbstractPluginObject::settings(const QString &hint, QWidget *parent)
{
    qimsysDebugIn() << parent;
    QimsysSettingsWidget *ret = new QimsysSettingsWidget(this, parent);
    qimsysDebugOut() << ret;
    return ret;
}
#endif // QIMSYS_NO_GUI
