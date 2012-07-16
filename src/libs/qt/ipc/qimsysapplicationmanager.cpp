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

#include "qimsysapplicationmanager.h"

#include "qimsysdebug.h"

#ifndef QIMSYS_NO_GUI
inline bool operator==(const QIcon &icon1, const QIcon &icon2)
{
    return icon1.cacheKey() == icon2.cacheKey();
}
#endif

class QimsysApplicationManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysApplicationManager *parent);
    ~Private();

    bool init();

private:
    QimsysApplicationManager *q;

public:
    static QimsysApplicationManager *server;

    bool focus;
    qulonglong window;
    qulonglong widget;
    bool composing;
    QString displayLanguage;
#ifndef QIMSYS_NO_GUI
    QIcon currentIcon;
#endif
};

QimsysApplicationManager *QimsysApplicationManager::Private::server = 0;

QimsysApplicationManager::Private::Private(QimsysApplicationManager *parent)
    : QObject(parent)
    , q(parent)
    , focus(false)
    , window(0)
    , widget(0)
    , composing(false)
    , displayLanguage()
#ifndef QIMSYS_NO_GUI
    , currentIcon("qrc:/icons/qimsys_32x32.png")
#endif
{
    qimsysDebugIn() << parent;
#ifndef QIMSYS_NO_DBUS
# ifndef QIMSYS_NO_GUI
    qDBusRegisterMetaType<QIcon>();
# endif
#endif
    switch (q->type()) {
    case Server:
        server = q;
        break;
    case Client:
        break;
    }
    qimsysDebugOut();
}

QimsysApplicationManager::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

bool QimsysApplicationManager::Private::init()
{
    qimsysDebugIn() << q->type();
    bool ret = q->QimsysAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(focusChanged(bool)), q, SIGNAL(focusChanged(bool)));
            connect(q->proxy(), SIGNAL(windowChanged(qulonglong)), q, SIGNAL(windowChanged(qulonglong)));
            connect(q->proxy(), SIGNAL(widgetChanged(qulonglong)), q, SIGNAL(widgetChanged(qulonglong)));
            connect(q->proxy(), SIGNAL(composingChanged(bool)), q, SIGNAL(composingChanged(bool)));
            connect(q->proxy(), SIGNAL(displayLanguageChanged(QString)), q, SIGNAL(displayLanguageChanged(QString)));
#ifndef QIMSYS_NO_GUI
            connect(q->proxy(), SIGNAL(currentIconChanged(QIcon)), q, SIGNAL(currentIconChanged(QIcon)));
#endif
            connect(q->proxy(), SIGNAL(triggered(int)), q, SIGNAL(triggered(int)));
            connect(q->proxy(), SIGNAL(settingsUpdated(QString)), q, SIGNAL(settingsUpdated(QString)));
            break;
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysApplicationManager::QimsysApplicationManager(QObject *parent, Type type)
    : QimsysAbstractIpcObject(parent, type)
{
    qimsysDebugIn() << parent << type;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysApplicationManager::~QimsysApplicationManager()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool QimsysApplicationManager::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject *QimsysApplicationManager::server() const
{
    return d->server;
}

getProp(QimsysApplicationManager, QString, displayLanguage)
setProp(QimsysApplicationManager, const QString&, displayLanguage, setDisplayLanguage)

getProp(QimsysApplicationManager, bool, focus)
setProp(QimsysApplicationManager, bool, focus, setFocus)

getProp(QimsysApplicationManager, qulonglong, window)
setProp(QimsysApplicationManager, qulonglong, window, setWindow)

getProp(QimsysApplicationManager, qulonglong, widget)
setProp(QimsysApplicationManager, qulonglong, widget, setWidget)

getProp(QimsysApplicationManager, bool, composing)
setProp(QimsysApplicationManager, bool, composing, setComposing)

#ifndef QIMSYS_NO_GUI
getProp(QimsysApplicationManager, QIcon, currentIcon)
setProp(QimsysApplicationManager, const QIcon&, currentIcon, setCurrentIcon)
#endif

void QimsysApplicationManager::exec(int action)
{
    qimsysDebugIn() << action;
    switch (type()) {
    case Server:
        emit triggered(action);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "exec", Q_ARG(int, action));
        break;
    }
    qimsysDebugOut();
}

void QimsysApplicationManager::settingsUpdate(const QString &name)
{
    qimsysDebugIn() << name;
    switch (type()) {
    case Server:
        emit settingsUpdated(name);
        break;
    case Client:
        proxy()->metaObject()->invokeMethod(proxy(), "settingsUpdated", Q_ARG(QString, name));
        break;
    }
    qimsysDebugOut();
}

#include "qimsysapplicationmanager.moc"
