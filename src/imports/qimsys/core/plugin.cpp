/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2009-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#include "plugin.h"

#include <qimsysdebug.h>

#include "applicationmanager.h"
#include "inputmethodmanager.h"
#include "preeditmanager.h"
#include "preedititem.h"
#include "candidatemanager.h"
#include "conversionitem.h"
#include "keymanager.h"
#include "keyboardmanager.h"

using namespace Qimsys::Core;

void Plugin::registerTypes(const char *uri)
{
//    qimsysDebugOn();
    qimsysDebugIn() << uri;
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Qimsys.Core"));
    qmlRegisterType<ApplicationManager>(uri, 1, 0, "ApplicationManager");
    qmlRegisterType<InputMethodManager>(uri, 1, 0, "InputMethodManager");
    qmlRegisterType<PreeditManager>(uri, 1, 0, "PreeditManager");
    qmlRegisterType<PreeditItem>(uri, 1, 0, "PreeditItem");
    qmlRegisterType<CandidateManager>(uri, 1, 0, "CandidateManager");
    qmlRegisterType<ConversionItem>(uri, 1, 0, "ConversionItem");
    qmlRegisterType<KeyManager>(uri, 1, 0, "KeyManager");
    qmlRegisterType<KeyboardManager>(uri, 1, 0, "KeyboardManager");
    qimsysDebugOut();
}
