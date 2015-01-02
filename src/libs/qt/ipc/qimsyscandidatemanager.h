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

#ifndef QIMSYSCANDIDATEMANAGER_H
#define QIMSYSCANDIDATEMANAGER_H

#include "qimsysglobal.h"
#include "qimsysabstractipcobject.h"
#include "qimsysconversionitem.h"

class QIMSYS_EXPORT QimsysCandidateManager : public QimsysAbstractIpcObject
{
    Q_OBJECT
    Q_PROPERTY(QimsysConversionItemList items READ items WRITE setItems)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_CLASSINFO("D-Bus Interface", "local.CandidateManager")
    Q_DISABLE_COPY(QimsysCandidateManager)
public:
    explicit QimsysCandidateManager(QObject *parent = 0, Type type = Client);
    ~QimsysCandidateManager();

    bool init();

    Q_INVOKABLE QimsysConversionItemList items() const;
    Q_INVOKABLE int currentIndex() const;

public slots:
    void setItems(const QimsysConversionItemList &items);
    void setCurrentIndex(int currentIndex);
    void clear();

signals:
    void itemsChanged(const QimsysConversionItemList &items);
    void currentIndexChanged(int currentIndex);

protected:
    QimsysAbstractIpcObject *server() const;

private:
    class Private;
    Private *d;
};

#endif // QIMSYSCANDIDATEMANAGER_H
