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

#include <qpa/qplatforminputcontextplugin_p.h>
#include <QtCore/QStringList>
#include <qimsysdebug.h>
#include "inputcontext.h"

class InputContextPlugin : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPlatformInputContextFactoryInterface" FILE "qimsys.json")

public:
    QPlatformInputContext *create(const QString&, const QStringList&);
};

QPlatformInputContext *InputContextPlugin::create(const QString& system, const QStringList& paramList)
{
    if (!qgetenv("QIMSYS_DEBUG").isEmpty()) {
        qimsysDebugOn();
    }
    qimsysDebugIn() << system << paramList;
    Q_UNUSED(paramList);
    QPlatformInputContext *ret  = 0;

    if (system.compare(system, QStringLiteral("qimsys"), Qt::CaseInsensitive) == 0)
        ret = new InputContext;
    qimsysDebugOut() << ret;
    return ret;
}

#include "main.moc"
