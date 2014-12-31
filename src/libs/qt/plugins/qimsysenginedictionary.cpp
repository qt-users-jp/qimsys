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

#include "qimsysenginedictionary.h"

#include <QCoreApplication>
#include <QDir>

QimsysEngineDictionary::QimsysEngineDictionary(QObject *parent)
    : QObject(parent)
{
}

QimsysEngineDictionary::~QimsysEngineDictionary()
{
}

QUrl QimsysEngineDictionary::resolveUrl(const QString &path) const
{
    QDir rootDir = QCoreApplication::applicationDirPath();
    // up to root dir
    for (int i = 0; i < QString(QIMSYS_MACRO_TO_STRING(QIMSYS_APP_PATH)).count(QLatin1Char('/')) + 1; i++) {
        rootDir.cdUp();
    }

    rootDir.cd(QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_DATA_PATH)));
    return QUrl(QString("%1/qml/%2").arg(rootDir.absolutePath()).arg(path));
}
