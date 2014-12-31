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

#ifndef PREEDITITEM_H
#define PREEDITITEM_H

#include <QObject>

#include <qimsyspreedititem.h>

class PreeditItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(QStringList from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QStringList rawString READ rawString WRITE setRawString NOTIFY rawStringChanged)
    Q_PROPERTY(int cursor READ cursor WRITE setCursor NOTIFY cursorChanged)
    Q_PROPERTY(int selection READ selection WRITE setSelection NOTIFY selectionChanged)
    Q_PROPERTY(int modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_DISABLE_COPY(PreeditItem)
public:
    explicit PreeditItem(QObject *parent = 0);
    PreeditItem(const QimsysPreeditItem &item, QObject *parent = 0);
    ~PreeditItem();

    const QStringList &to() const;
    const QStringList &from() const;
    const QStringList &rawString() const;
    int cursor() const;
    int selection() const;
    int modified() const;

    const QimsysPreeditItem &item() const { return d; }

public slots:
    void setTo(const QStringList &to);
    void setFrom(const QStringList &from);
    void setRawString(const QStringList &rawString);
    void setCursor(int cursor);
    void setSelection(int selection);
    void setModified(int modified);

signals:
    void toChanged(const QStringList &to);
    void fromChanged(const QStringList &from);
    void rawStringChanged(const QStringList &rawString);
    void cursorChanged(int cursor);
    void selectionChanged(int selection);
    void modifiedChanged(int modified);

private:
    QimsysPreeditItem d;
};

#endif // PREEDITITEM_H
