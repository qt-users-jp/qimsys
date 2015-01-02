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

#include "qimsystest.h"
#include "qimsysdebug.h"
#include "qimsysserver.h"
#include "qimsyspluginmanager.h"

class QimsysTest::Private
{
public:
    Private(QimsysTest *parent);
    ~Private();

private:
    QimsysTest *q;

public:
    QMap<QimsysAbstractPluginObject*, bool> enabled;
};

QimsysTest::Private::Private(QimsysTest *parent)
    : q(parent)
{
    if (!qgetenv("QIMSYS_DEBUG").isEmpty()) qimsysDebugOn();
}

QimsysTest::Private::~Private()
{

}

QimsysTest::QimsysTest(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

QimsysTest::~QimsysTest()
{
    delete d;
}

void QimsysTest::initTestCase()
{
    qimsysDebugIn();
    new QimsysServer(this);

#ifdef QIMSYS_APPLICATION_DIR_PATH
    QDir rootDir(QIMSYS_MACRO_TO_STRING2(QIMSYS_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    qimsysDebug() << rootDir;

    // up to root dir
    QString appPath(QIMSYS_MACRO_TO_STRING(QIMSYS_APP_PATH));
    if (!appPath.isEmpty()) {
        for (int i = 0; i < appPath.count(QLatin1Char('/')) + 1; i++) {
            rootDir.cdUp();
        }
    }
    qimsysDebug() << rootDir << QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_PLUGIN_PATH));

    rootDir.cd(QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_PLUGIN_PATH)));
    qimsysDebug() << rootDir;

    qimsysDebug() << rootDir.absolutePath();
    setQimsysPluginPath(rootDir.absolutePath());
#ifndef Q_OS_LINUX
    if (rootDir.exists(QLatin1String("inputmethods"))) {
        q->addLibraryPath(rootDir.absolutePath());
    }
#endif

    QStringList identifiers = activePluginList();
    if (!identifiers.isEmpty()) {
        foreach (QimsysAbstractPluginObject* object, QimsysPluginManager::objects<QimsysAbstractPluginObject>(this)) {
            d->enabled[object] = object->isEnabled();
            object->setEnabled(identifiers.contains(object->identifier()));
            object->setActive(identifiers.contains(object->identifier()));
        }
    }
    qimsysDebugOut();
}

void QimsysTest::init()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void QimsysTest::cleanup()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void QimsysTest::cleanupTestCase()
{
    qimsysDebugIn();
    foreach (QimsysAbstractPluginObject* object, QimsysPluginManager::objects<QimsysAbstractPluginObject>(this)) {
        object->setEnabled(d->enabled[object]);
    }
    qimsysDebugOut();
}

QStringList QimsysTest::activePluginList() const
{
    return QStringList();
}
