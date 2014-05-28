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

#ifndef QIMSYSDEBUG_H
#define QIMSYSDEBUG_H

#include <glib-object.h>
#include <syslog.h>
#include <stdio.h>

#include "qimsysglobal.h"

#ifdef __cplusplus
extern "C"
{
#endif

QIMSYS_EXPORT int QimsysLogLevelCounter;
QIMSYS_EXPORT int QimsysLogEnabled;
QIMSYS_EXPORT int QimsysLogLevel;

#define qimsys_debug_on() QimsysLogEnabled++;
#define qimsys_debug_off() QimsysLogEnabled--
#define qimsys_debug_in() if(QimsysLogEnabled <= 0){} else g_print("+ %s\n", __PRETTY_FUNCTION__)
#define qimsys_debug_out() if(QimsysLogEnabled <= 0){} else g_print("- %s\n", __PRETTY_FUNCTION__)
#define qimsys_debug(...) if(QimsysLogEnabled <= 0){} else g_print(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // QIMSYSDEBUG_H
