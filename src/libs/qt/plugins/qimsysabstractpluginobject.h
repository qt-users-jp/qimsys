/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#ifndef QIMSYSABSTRACTPLUGINOBJECT_H
#define QIMSYSABSTRACTPLUGINOBJECT_H

#include "qimsysglobal.h"
#include <QObject>
#include <QStringList>
#ifndef QIMSYS_NO_GUI
#include <QIcon>

class QimsysSettingsWidget;
#endif

class QIMSYS_EXPORT QimsysAbstractPluginObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged)

    Q_PROPERTY(QString locale READ locale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

#ifndef QIMSYS_NO_GUI
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
#endif
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString translator READ translator WRITE setTranslator NOTIFY translatorChanged)
    Q_PROPERTY(QString license READ license WRITE setLicense NOTIFY licenseChanged)

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

    Q_PROPERTY(QStringList groups READ groups WRITE setGroups)
    Q_PROPERTY(CategoryType categoryType READ categoryType WRITE setCategoryType)
    Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
public:
    enum CategoryType {
        Hidden = 0x00,
        CanBeNone = 0x01,
        AlwaysOne = 0x02,
        MoreThanOne = 0x04,
        OneOrNone = 0x08,
        All = 0x10,
    };
    QimsysAbstractPluginObject(QObject *parent = 0);
    ~QimsysAbstractPluginObject();

    const QString &identifier() const;
    int priority() const;

    const QString &locale() const;
    const QString &language() const;

#ifndef QIMSYS_NO_GUI
    const QIcon &icon() const;
#endif
    const QString &name() const;
    const QString &description() const;
    const QString &author() const;
    const QString &url() const;
    const QString &translator() const;
    const QString &license() const;

    bool isEnabled() const;
    bool isActive() const;

    virtual const QStringList &groups() const;
    virtual CategoryType categoryType() const;
    virtual const QString &categoryName() const;

#ifndef QIMSYS_NO_GUI
    virtual QimsysSettingsWidget *settings(const QString &hint, QWidget *parent = 0);
#endif

public slots:
    void setEnabled(bool enabled);
    void setActive(bool active);

signals:
    void identifierChanged(const QString &identifier);
    void priorityChanged(int priority);

    void localeChanged(const QString &locale);
    void languageChanged(const QString &language);

#ifndef QIMSYS_NO_GUI
    void iconChanged(const QIcon &icon);
#endif
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void authorChanged(const QString &author);
    void urlChanged(const QString &url);
    void translatorChanged(const QString &translator);
    void licenseChanged(const QString &license);

    void enabledChanged(bool enabled);
    void activeChanged(bool active);

protected slots:
    void setIdentifier(const QString &identifier);
    void setPriority(int priority);

    void setLanguage(const QString &language);
    void setLocale(const QString &locale);

#ifndef QIMSYS_NO_GUI
    void setIcon(const QIcon &icon);
#endif
    void setName(const QString &name);
    void setDescription(const QString &description);
    void setAuthor(const QString &author);
    void setUrl(const QString &url);
    void setTranslator(const QString &translator);
    void setLicense(const QString &license);

    void setGroups(const QStringList &groups);
    void setCategoryType(CategoryType categoryType);
    void setCategoryName(const QString &categoryName);

private:
    class Private;
    Private *d;
};

#endif//QIMSYSABSTRACTPLUGINOBJECT_H
