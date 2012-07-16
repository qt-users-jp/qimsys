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
#include "deviceinfo.h"

#include <qimsysdebug.h>

#include <QApplication>
#include <QDesktopWidget>

#include <QDBusConnection>
#include <QDBusInterface>

using namespace N900::Device;

class DeviceInfo::Private : private QObject
{
    Q_OBJECT
public:
    Private(DeviceInfo *parent);

private slots:
    void resized();
    void workAreaResized();
    void slideClosedChanged();

private:
    DeviceInfo *q;
    QDesktopWidget *desktop;
public:
    QSize screenSize;
    QRect desktopRect;
    bool slideClosed;
};

DeviceInfo::Private::Private(DeviceInfo *parent)
    : QObject(parent)
    , q(parent)
    , desktop(QApplication::desktop())
    , slideClosed(true)
{
    qimsysDebugIn() << parent;
    connect(desktop, SIGNAL(resized(int)), this, SLOT(resized()));
    connect(desktop, SIGNAL(workAreaResized(int)), this, SLOT(workAreaResized()));
    QDBusConnection::systemBus().connect(QLatin1String("org.freedesktop.Hal"), QLatin1String("/org/freedesktop/Hal/devices/platform_slide"), QLatin1String("org.freedesktop.Hal.Device"), QString("PropertyModified"), this, SLOT(slideClosedChanged()));

    resized();
    workAreaResized();
    slideClosedChanged();
    qimsysDebugOut();
}

void DeviceInfo::Private::resized()
{
    qimsysDebugIn();
    screenSize = desktop->screenGeometry().size();
    emit q->screenSizeChanged(screenSize);
    workAreaResized();
    qimsysDebugOut() << screenSize;
}

void DeviceInfo::Private::workAreaResized()
{
    qimsysDebugIn();
    desktopRect = desktop->availableGeometry();
    emit q->desktopRectChanged(desktopRect);
    qimsysDebugOut() << desktopRect;
}

void DeviceInfo::Private::slideClosedChanged()
{
#ifdef QIMSYS_PLATFORM_MAEMO
    QDBusInterface propertyInterface(QLatin1String("org.freedesktop.Hal"), QLatin1String("/org/freedesktop/Hal/devices/platform_slide"), QLatin1String("org.freedesktop.Hal.Device"), QDBusConnection::systemBus());
    slideClosed = propertyInterface.call(QLatin1String("GetProperty"), QLatin1String("button.state.value")).arguments().at(0).toBool();
    emit q->slideClosedChanged(slideClosed);
#endif
}

DeviceInfo::DeviceInfo(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

DeviceInfo::~DeviceInfo()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

const QSize &DeviceInfo::screenSize() const
{
    return d->screenSize;
}

const QRect &DeviceInfo::desktopRect() const
{
    return d->desktopRect;
}

bool DeviceInfo::slideClosed() const
{
    return d->slideClosed;
}

#include "deviceinfo.moc"
