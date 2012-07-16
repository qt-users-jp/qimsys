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

#ifndef CONVERSIONITEM_H
#define CONVERSIONITEM_H

#include <QObject>

#include <qimsysconversionitem.h>

class ConversionItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(QString from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QString to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(QString hint READ hint WRITE setHint NOTIFY hintChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_DISABLE_COPY(ConversionItem)
public:
    explicit ConversionItem(QObject *parent = 0);
    ConversionItem(const QimsysConversionItem &item, QObject *parent = 0);
    ~ConversionItem();

    const QimsysConversionItem &item() const { return d; }

    int index() const;
    const QString &from() const;
    const QString &to() const;
    const QString &hint() const;
    const QString &source() const;

public slots:
    void setIndex(int index);
    void setFrom(const QString &from);
    void setTo(const QString &to);
    void setHint(const QString &hint);
    void setSource(const QString &source);

signals:
    void indexChanged(int index);
    void fromChanged(const QString &from);
    void toChanged(const QString &to);
    void hintChanged(const QString &hint);
    void sourceChanged(const QString &source);

private:
    QimsysConversionItem d;
};

#endif // CONVERSIONITEM_H
