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

#ifndef QIMSYSPLUGINMANAGER_H
#define QIMSYSPLUGINMANAGER_H

#include <QCoreApplication>
#include <QList>
#include <QDir>
#include <QPluginLoader>
#include <QPointer>
#include <QLibraryInfo>
#include "qimsysdebug.h"
#include "qimsysglobal.h"
#include "qimsysabstractpluginobject.h"
#include "qimsysplugin.h"

const QIMSYS_EXPORT QString &qimsysPluginPath();
void QIMSYS_EXPORT setQimsysPluginPath(const QString &);

bool priorityLessThan(const QimsysAbstractPluginObject *o1, const QimsysAbstractPluginObject *o2);

class QIMSYS_EXPORT QimsysPluginManager
{
public:
    template<class T>
    static QList<T*> objects(QObject *parent = 0) {
//        qimsysDebugIn();
        QList<T*> ret;
//        qimsysDebug() << instance();
//        qimsysDebug() << instance()->m_plugins.count();
//        qimsysDebug() << instance()->m_plugins;
        foreach(QimsysPlugin *plugin, instance()->m_plugins) {
//            qimsysDebug() << plugin;
            QimsysAbstractPluginObject *o = plugin->object(parent);
            T *t = qobject_cast<T*>(o);
//            qimsysDebug() << o << t;
            if (t) ret.append(t);
        }

        qSort(ret.begin(), ret.end(), priorityLessThan);
//        qimsysDebugOut() << ret;
        return ret;
    }

private:
    static QimsysPluginManager *instance() {
        static QimsysPluginManager *manager = 0;
        if (!manager)
            manager = new QimsysPluginManager;
        return manager;
    }

    QimsysPluginManager() {
        loadPlugins();
    }

public:
    ~QimsysPluginManager() {
//        qimsysDebugIn();
        qDeleteAll(m_plugins);
//        qimsysDebugOut();
    }

private:
    void loadPlugins() {
//        qimsysDebugOn();
        qimsysDebugIn();
        QDir dir(qimsysPluginPath() + QLatin1String("/"));
        qimsysDebug() << dir;
        if (!dir.exists()) {
            qWarning() << QString::fromLatin1("No such file or directory: %1").arg(dir.absolutePath());
        }
        QStringList pluginFilter;
#if defined(QIMSYS_PLATFORM_WINDOWS)
        pluginFilter << QLatin1String("*.dll");
#elif defined(QIMSYS_PLATFORM_MAC)
        pluginFilter << QLatin1String("*.dylib");
#elif defined(QIMSYS_PLATFORM_LINUX)
        pluginFilter << QLatin1String("*.so");
#else
        pluginFilter << QLatin1String("*");
#endif

        foreach(const QString &lib, dir.entryList(pluginFilter, QDir::Files)) {
            qimsysDebug() << "file =" << lib;
            QPluginLoader loader(dir.absoluteFilePath(lib));
            QObject *object = loader.instance();
            qimsysDebug() << "object =" << object;
            if (object) {
                QimsysPlugin *plugin = qobject_cast<QimsysPlugin*>(object);
                qimsysDebug() << "plugin =" << plugin;
                if (plugin) {
                    m_plugins.append(plugin);
                    plugin->setObjectName(lib);
                } else {
                    delete object;
                }
            } else {
                qimsysWarning() << dir.absoluteFilePath(lib) << loader.errorString();
            }
        }
        qimsysDebugOut();
//        qimsysDebugOff();
    }

    QList< QPointer<QimsysPlugin> > m_plugins;
};

#endif // QIMSYSPLUGINMANAGER_H
