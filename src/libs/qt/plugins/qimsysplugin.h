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

#ifndef QIMSYSPLUGIN_H
#define QIMSYSPLUGIN_H

#include "qimsysglobal.h"
#include <QtPlugin>

class QimsysAbstractPluginObject;

class QIMSYS_EXPORT QimsysPlugin : public QObject
{
    Q_OBJECT
public:
    QimsysPlugin(QObject *parent = 0);
    ~QimsysPlugin();

    QimsysAbstractPluginObject *object(QObject *parent = 0);

protected:
    virtual QimsysAbstractPluginObject *createObject(QObject *parent) = 0;

private:
//    class Private;
//    Private *d;
};

Q_DECLARE_INTERFACE(QimsysPlugin, "me.qtquick.qimsys.QimsysPlugin/0.1")

#endif//QIMSYSPLUGIN_H
