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

#ifndef QIMSYS_NO_GUI

#include "qimsyssettingswidget.h"
#include "ui_qimsyssettingswidget.h"
#include "qimsysabstractpluginobject.h"
#include "qimsysdebug.h"

class QimsysSettingsWidget::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysSettingsWidget *parent);
    void setPlugin(QWidget *widget, QimsysAbstractPluginObject *plugin);

private:
    QimsysSettingsWidget *q;

public:
    bool modified;
    Ui::QimsysSettingsWidget ui;
};

QimsysSettingsWidget::Private::Private(QimsysSettingsWidget *parent)
    : QObject(parent)
    , q(parent)
    , modified(false)
{
    qimsysDebugIn() << parent;
    qimsysDebugOut();
}

void QimsysSettingsWidget::Private::setPlugin(QWidget *widget, QimsysAbstractPluginObject *plugin)
{
    qimsysDebugIn() << widget << plugin;
    ui.setupUi(widget);
    QIcon icon(plugin->icon());
    if (!icon.isNull()) {
        ui.icon->setPixmap(icon.pixmap(32, 32));
    } else {
        ui.icon->hide();
    }
    ui.name->setText(plugin->name());
    if (plugin->description().isEmpty()) {
        ui.descriptionLabel->hide();
        ui.description->hide();
    } else {
        ui.description->setText(plugin->description());
    }
    if (plugin->author().isEmpty()) {
        ui.author->hide();
        ui.authorLabel->hide();
    } else {
        ui.author->setText(plugin->author());
    }
    if (plugin->url().isEmpty()) {
        ui.url->hide();
        ui.urlLabel->hide();
    } else {
        ui.url->setText(plugin->url());
    }
    if (plugin->translator().isEmpty()) {
        ui.translator->hide();
        ui.translatorLabel->hide();
    } else {
        ui.translator->setText(plugin->translator());
    }
    if (plugin->license().isEmpty()) {
        ui.license->hide();
        ui.licenseLabel->hide();
    } else {
        ui.license->setText(plugin->license());
    }
    qimsysDebugOut();
}

QimsysSettingsWidget::QimsysSettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysSettingsWidget::QimsysSettingsWidget(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QWidget(parent)
{
    qimsysDebugIn() << plugin << parent;
    d = new Private(this);
    d->setPlugin(this, plugin);
    qimsysDebugOut();
}

QimsysSettingsWidget::~QimsysSettingsWidget()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

void QimsysSettingsWidget::save()
{
    qimsysDebugIn();
    emit saved();
    qimsysDebugOut();
}

bool QimsysSettingsWidget::isModified() const
{
    return d->modified;
}

void QimsysSettingsWidget::setModified(bool modified)
{
    if (d->modified == modified) return;
    qimsysDebugIn() << modified;
    d->modified = modified;
    emit modifiedChanged(d->modified);
    qimsysDebugOut();
}

QWidget *QimsysSettingsWidget::about(QimsysAbstractPluginObject *plugin, const QString &spec, QWidget *parent)
{
    qimsysDebugIn() << plugin << spec << parent;
    QWidget *ret = new QWidget(parent);
    d->setPlugin(ret, plugin);
    qimsysDebugOut() << ret;
    return ret;
}

#include "qimsyssettingswidget.moc"

#endif // QIMSYS_NO_GUI
