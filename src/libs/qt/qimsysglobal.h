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

#ifndef QIMSYSGLOBAL_H
#define QIMSYSGLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QIMSYS_NO_DBUS
static const char *const QIMSYS_DBUS_SERVICE = "net.linux-life.qimsys";
#endif

#ifdef QIMSYS_LIBRARY
# define QIMSYS_EXPORT Q_DECL_EXPORT
#else
# define QIMSYS_EXPORT Q_DECL_IMPORT
#endif

//#define QIMSYS_DEBUG_MODE

#define fallthrough

#ifdef QIMSYS_PLATFORM_SYMBIAN
#ifndef QIMSYS_VERSION
#  define QIMSYS_VERSION "0.2.0"
#endif

#ifndef QIMSYS_APP_PATH
#  define QIMSYS_APP_PATH ""
#endif

#ifndef QIMSYS_LIBRARY_PATH
#define QIMSYS_LIBRARY_PATH ""
#endif

#ifndef QIMSYS_PLUGIN_PATH
#define QIMSYS_PLUGIN_PATH "/resource/qimsys/plugins"
#endif

#ifndef QIMSYS_IMPORTS_PATH
#define QIMSYS_IMPORTS_PATH "/resource/qimsys/imports"
#endif

#ifndef QIMSYS_DATA_PATH
#define QIMSYS_DATA_PATH "share/qimsys"
#endif

#ifndef QIMSYS_TRANSLATIONS_PATH
#define QIMSYS_TRANSLATIONS_PATH QIMSYS_DATA_PATH "/translations"
#endif
#endif//QIMSYS_PLATFORM_SYMBIAN

#define QIMSYS_MACRO_TO_STRING(x) x
#define QIMSYS_MACRO_TO_STRING2(x) #x

#endif // QIMSYSGLOBAL_H
