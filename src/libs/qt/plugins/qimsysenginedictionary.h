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

#ifndef QIMSYSENGINEDICTIONARY_H
#define QIMSYSENGINEDICTIONARY_H

#include "qimsysglobal.h"

#include <QObject>
#include <QUrl>

class QWidget;
class QAbstractTableModel;
class QModelIndex;

class QIMSYS_EXPORT QimsysEngineDictionary : public QObject
{
    Q_OBJECT
public:
    QimsysEngineDictionary(QObject *parent = 0);
    ~QimsysEngineDictionary();

    virtual QAbstractTableModel *model() = 0;
    virtual void save() = 0;
    virtual void add(QWidget *parent = 0) = 0;
    virtual void edit(const QModelIndex &index, QWidget *parent = 0) = 0;
    virtual void del(const QModelIndex &index, QWidget *parent = 0) = 0;

protected:
    QUrl resolveUrl(const QString &path) const;
};

#endif//QIMSYSENGINEDICTIONARY_H
