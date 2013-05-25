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

#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include "qimsysglobal.h"

extern QIMSYS_EXPORT int QimsysLogEnabled;
extern QIMSYS_EXPORT int QimsysLogLevel;

#define qimsysDebugOn() QimsysLogEnabled++
#define qimsysDebugOff() QimsysLogEnabled--
#if defined(Q_CC_GNU)
#define qimsysDebugIn() if(QimsysLogEnabled <= 0){} else qDebug() << QString(2*QimsysLogLevel++, ' ') + QString("+ %1").arg(__PRETTY_FUNCTION__)
#define qimsysDebugOut() if(QimsysLogEnabled <= 0){} else qDebug() << QString(2*--QimsysLogLevel, ' ') + QString("- %1").arg(__PRETTY_FUNCTION__)
#define qimsysDebug() if(QimsysLogEnabled <= 0){} else qDebug() << QString(2*QimsysLogLevel, ' ') + QString(" %1 %2>").arg(__PRETTY_FUNCTION__).arg(__LINE__)
#define qimsysWarning() qWarning() << QString(2*QimsysLogLevel, '#') + QString(" %1 %2>").arg(__PRETTY_FUNCTION__).arg(__LINE__)
#else
#define qimsysDebugIn() if(QimsysLogEnabled <= 0){} else qDebug() << QString(2*QimsysLogLevel++, ' ') + QString("+ %1").arg(__FUNCTION__)
#define qimsysDebugOut() if(QimsysLogEnabled <= 0){} else qDebug() << QString(2*--QimsysLogLevel, ' ') + QString("- %1").arg(__FUNCTION__)
#define qimsysDebug() if(QimsysLogEnabled <= 0){} else qDebug() << QString(2*QimsysLogLevel, ' ') + QString(" %1 %2>").arg(__FUNCTION__).arg(__LINE__)
#define qimsysWarning() qWarning() << QString(2*QimsysLogLevel, '#') + QString(" %1 %2>").arg(__FUNCTION__).arg(__LINE__)
#endif
#endif // DEBUG_H
