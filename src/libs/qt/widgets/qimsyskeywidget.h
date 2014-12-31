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

#ifndef QIMSYSKEYWIDGET_H
#define QIMSYSKEYWIDGET_H

#include <QToolButton>
#include "qimsysglobal.h"
#include "qimsyskeysequence.h"

class QIMSYS_EXPORT QimsysKeyWidget : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QimsysKeySequence sequence READ sequence WRITE setSequence NOTIFY sequenceChanged)
    Q_DISABLE_COPY(QimsysKeyWidget)
public:
    explicit QimsysKeyWidget(QWidget *parent = 0);
    ~QimsysKeyWidget();

    virtual QSize sizeHint() const;

    QimsysKeySequence sequence() const;
    void setSequence(const QimsysKeySequence &sequence);

public slots:
    void reset();
    void clear();

signals:
    void sequenceChanged(QimsysKeySequence sequence);

private:
    class Private;
    Private *d;
};
#endif//QIMSYSKEYWIDGET_H
