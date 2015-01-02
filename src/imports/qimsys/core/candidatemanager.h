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

#ifndef CANDIDATEMANAGER_H
#define CANDIDATEMANAGER_H

#include <QObject>
#include "conversionitem.h"

class CandidateManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> items READ items WRITE setItems NOTIFY itemsChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_DISABLE_COPY(CandidateManager)
public:
    explicit CandidateManager(QObject *parent = 0);

    Q_INVOKABLE const QList<QObject*> &items() const;
    Q_INVOKABLE int currentIndex();

public slots:
    void setItems(const QList<QObject*> &items);
    void setCurrentIndex(int currentIndex);

signals:
    void itemsChanged(const QList<QObject*> &items);
    void currentIndexChanged(int currentIndex);

private:
    class Private;
    Private *d;
};

#endif // CANDIDATEMANAGER_H
