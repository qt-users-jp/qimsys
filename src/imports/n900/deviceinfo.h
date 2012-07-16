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
#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <QSize>
#include <QRect>

namespace N900 {
    namespace Device {

class DeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize screenSize READ screenSize NOTIFY screenSizeChanged)
    Q_PROPERTY(QRect desktopRect READ desktopRect NOTIFY desktopRectChanged)
    Q_PROPERTY(bool slideClosed READ slideClosed NOTIFY slideClosedChanged)
    Q_DISABLE_COPY(DeviceInfo)
public:
    explicit DeviceInfo(QObject *parent = 0);
    ~DeviceInfo();

    const QSize &screenSize() const;
    const QRect &desktopRect() const;
    bool slideClosed() const;

signals:
    void screenSizeChanged(const QSize &size);
    void desktopRectChanged(const QRect &rect);
    void slideClosedChanged(bool slideClosed);

private:
    class Private;
    Private *d;
};

    }
}
#endif // DEVICEINFO_H
