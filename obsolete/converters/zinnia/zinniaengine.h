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

#ifndef ZINNIAENGINE_H
#define ZINNIAENGINE_H

#include <QObject>

#include <QSize>
#include <QPolygon>

namespace zinnia {

class ZinniaEngine : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ZinniaEngine)
    Q_PROPERTY(QSize size READ size WRITE setSize)
public:
    explicit ZinniaEngine(QObject *parent = 0);
    ~ZinniaEngine();

    const QSize &size() const;
public slots:
    void setSize(const QSize &size);
    void setData(const QList<QPolygon> &data);

signals:
    void recognized(const QStringList &candidates);

private:
    class Private;
    Private *d;
};

}

#endif // ZINNIAENGINE_H
