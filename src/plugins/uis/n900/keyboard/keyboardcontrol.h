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

#ifndef KEYBOARDCONTROL_H
#define KEYBOARDCONTROL_H

#include <QObject>
#include <QRect>

class KeyboardControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ x WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ x WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ x WRITE setHeight NOTIFY heightChanged)
public:
    explicit KeyboardControl(QObject *parent = 0);
    ~KeyboardControl();

    bool isVisible() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;

public slots:
    void setVisible(bool visible);
    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);

signals:
    void visibleChanged(bool visible);
    void xChanged(int x);
    void yChanged(int y);
    void widthChanged(int width);
    void heightChanged(int height);

private:
    class Private;
    Private *d;
};

#endif // KEYBOARDCONTROL_H
