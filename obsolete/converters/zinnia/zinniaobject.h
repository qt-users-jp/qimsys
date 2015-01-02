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

#ifndef ZINNIA_OBJECT_H
#define ZINNIA_OBJECT_H

#include <qimsysconverter.h>

namespace zinnia {

class ZinniaObject : public QimsysConverter
{
	Q_OBJECT
    Q_DISABLE_COPY(ZinniaObject)
public:
    explicit ZinniaObject(QObject *parent = 0);
    ~ZinniaObject();

    QString identifier() const;
    QStringList locales() const;

    bool useEngine() const;
    QList<QAction*> actions();

//    QimsysSettingsWidget *settings(const QString &hint, QWidget *parent = 0);

private:
    class Private;
    Private *d;
};

}

#endif // ZINNIA_OBJECT_H
