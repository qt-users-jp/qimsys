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

#ifndef QIMSYSDECLARATIVEVIEW_H
#define QIMSYSDECLARATIVEVIEW_H

#include <QDeclarativeView>
#include "qimsysglobal.h"

class QIMSYS_EXPORT QimsysDeclarativeView : public QDeclarativeView
{
    Q_OBJECT
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_DISABLE_COPY(QimsysDeclarativeView)
public:
    explicit QimsysDeclarativeView(QWidget *parent = 0);
    ~QimsysDeclarativeView();

    QUrl resolveUrl(const QString &path) const;

signals:
    void quit();
    void geometryChanged(const QRect &geometry);

protected:
    void resizeEvent(QResizeEvent *e);
    void moveEvent(QMoveEvent *e);
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);

private:
    class Private;
    Private *d;
};

#endif // QIMSYSDECLARATIVEVIEW_H
