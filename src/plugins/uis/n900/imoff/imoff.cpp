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

#include "imoff.h"

#include <qimsysdebug.h>

#include <qimsysapplicationmanager.h>

using namespace N900IMOff;

class IMOff::Private : private QObject
{
    Q_OBJECT
public:

    Private(QimsysAbstractPluginObject *object, IMOff *parent);
    ~Private();

private slots:
    void init();

    void windowChanged(qulonglong window);

private:
    IMOff *q;
    QimsysAbstractPluginObject *plugin;

    QimsysApplicationManager manager;
    qulonglong current;
    qulonglong pending;
};

IMOff::Private::Private(QimsysAbstractPluginObject *object, IMOff *parent)
    : QObject(parent)
    , q(parent)
    , plugin(object)
    , current(0)
{
    qimsysDebugIn() << object << parent;
    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    qimsysDebugOut();
}

IMOff::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void IMOff::Private::init()
{
    qimsysDebugIn();

    // system
    manager.init();
    connect(&manager, SIGNAL(windowChanged(qulonglong)), this, SLOT(windowChanged(qulonglong)));

    qimsysDebugOut();
}

void IMOff::Private::windowChanged(qulonglong window) {
    if (current == window) return;
//    if (window > 0) {
        current = window;
        manager.setComposing(false);
//    }
}

IMOff::IMOff(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QWidget(parent)
{
    qimsysDebugIn();
    d = new Private(plugin, this);
    qimsysDebugOut();
}

IMOff::~IMOff()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "imoff.moc"
