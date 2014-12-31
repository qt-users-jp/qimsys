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

#ifndef QIMSYSTEST_H
#define QIMSYSTEST_H

#include <QObject>
#include <QStringList>

#include "qimsysglobal.h"

class QIMSYS_EXPORT QimsysTest : public QObject
{
    Q_OBJECT
public:
    QimsysTest(QObject *parent = 0);
    ~QimsysTest();

public slots:
    virtual void initTestCase();
    virtual void init();
    virtual void cleanup();
    virtual void cleanupTestCase();

protected:
    virtual QStringList activePluginList() const;

private:
    class Private;
    Private *d;
};

#endif // QIMSYSTEST_H
