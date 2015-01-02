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

#include "qimsyspreeditmanager.h"

#include "qimsysdebug.h"

class QimsysPreeditManager::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysPreeditManager *parent);
    ~Private();

    bool init();
    void insert(const QString &str);
    void remove(int length);
    QString text(int length) const;

private:
    QimsysPreeditManager *q;

public:
    static QimsysPreeditManager *server;
    QimsysPreeditItem item;
    QRect rect;
#ifndef QIMSYS_NO_GUI
    QFont font;
#endif
    int cursorPosition;
    QString surroundingText;
    QString currentSelection;
    int maximumTextLength;
};

QimsysPreeditManager *QimsysPreeditManager::Private::server = 0;

QimsysPreeditManager::Private::Private(QimsysPreeditManager *parent)
    : QObject(parent)
    , q(parent)
    , item()
    , rect()
#ifndef QIMSYS_NO_GUI
    , font()
#endif
    , cursorPosition(0)
    , maximumTextLength(0)
{
    qimsysDebugIn() << parent;
    qRegisterMetaType<QimsysPreeditItem>("QimsysPreeditItem");
    qRegisterMetaTypeStreamOperators<QimsysPreeditItem>("QimsysPreeditItem");
#ifndef QIMSYS_NO_DBUS
    qDBusRegisterMetaType<QimsysPreeditItem>();
#ifndef QIMSYS_NO_GUI
    qDBusRegisterMetaType<QFont>();
#endif
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

QimsysPreeditManager::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

bool QimsysPreeditManager::Private::init()
{
    qimsysDebugIn();
    bool ret = q->QimsysAbstractIpcObject::init();
    if (ret) {
        switch (q->type()) {
        case Server:
            break;
        case Client:
            connect(q->proxy(), SIGNAL(itemChanged(QimsysPreeditItem)), q, SIGNAL(itemChanged(QimsysPreeditItem)));
            connect(q->proxy(), SIGNAL(rectChanged(QRect)), q, SIGNAL(rectChanged(QRect)));
#ifndef QIMSYS_NO_GUI
            connect(q->proxy(), SIGNAL(fontChanged(QFont)), q, SIGNAL(fontChanged(QFont)));
#endif
            connect(q->proxy(), SIGNAL(cursorPositionChanged(int)), q, SIGNAL(cursorPositionChanged(int)));
            connect(q->proxy(), SIGNAL(surroundingTextChanged(QString)), q, SIGNAL(surroundingTextChanged(QString)));
            connect(q->proxy(), SIGNAL(currentSelectionChanged(QString)), q, SIGNAL(currentSelectionChanged(QString)));
            connect(q->proxy(), SIGNAL(maximumTextLengthChanged(int)), q, SIGNAL(maximumTextLengthChanged(int)));
            connect(q->proxy(), SIGNAL(committed(QString, qulonglong)), q, SIGNAL(committed(QString, qulonglong)));
            break;
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

void QimsysPreeditManager::Private::insert(const QString &str)
{
//    qimsysDebugOn();
    qimsysDebugIn() << str;
    qimsysDebug() << item;

    if (item.to.isEmpty()) {
        item.to.prepend(str);
        item.from.prepend(str);
        item.rawString.prepend(str);
        item.selection = 0;
    } else {
        int pos = 0;
        for (int i = 0; i < item.to.length() + 1; i++) {
            qimsysDebug() << pos << item.cursor;
            if (pos == item.cursor) {
                item.to.insert(i, str);
                item.from.insert(i, str);
                item.rawString.insert(i, str);
                break;
            }
            if (i == item.to.length()) break;
            pos += item.to.at(i).length();
            qimsysDebug() << pos << item.cursor;
            if (pos > item.cursor) {
                QString t = item.to.at(i);
                item.to.replace(i, t.mid(pos - item.cursor));
                item.to.insert(i, str);
                item.to.insert(i, t.left(pos - item.cursor));

                QString f = item.from.at(i);
                item.from.replace(i, f.mid(pos - item.cursor));
                item.from.insert(i, str);
                item.from.insert(i, f.left(pos - item.cursor));

                item.rawString.replace(i, QString());
                item.rawString.insert(i, str);
                item.rawString.insert(i, QString());
                break;
            }
        }
    }
    item.cursor += str.length();
    item.modified = str.length();

    qimsysDebug() << item;
    qimsysDebugOut();
//    qimsysDebugOff();
}

void QimsysPreeditManager::Private::remove(int length)
{
//    qimsysDebugOn();
    qimsysDebugIn() << length;
    qimsysDebug() << item;

    QStringList to = item.to;
    QStringList from = item.from;
    QStringList rawString = item.rawString;
    int cursor = item.cursor;
    int selection = item.selection;

    if (selection != 0) {
        length = selection;
        selection = 0;
    }

    if (length < 0) {
        cursor += length;
        length = -length;
    }

    if (cursor < 0) {
        length += cursor;
        cursor = 0;
    }

    qimsysDebug() << cursor << length;
    int pos = 0;
    int i = 0;
    while (length > 0) {
        int l = to.at(i).length();
        qimsysDebug() << pos << cursor << l << i;
        if (pos == cursor) {
            qimsysDebug() << l << length;
            if (l <= length) {
                length -= l;
                to.removeAt(i);
                from.removeAt(i);
                rawString.removeAt(i);
                continue;
            } else {
                to.replace(i, to[i].remove(cursor, length));
                from.replace(i, from[i].remove(cursor, length));
                rawString.replace(i, QString());
                break;
            }
        } else if (pos + l > cursor) {
            qimsysDebug() << pos << cursor << length << l;
            to.replace(i, to[i].remove(cursor - pos, length));
            from.replace(i, from[i].remove(cursor - pos, length));
            rawString.replace(i, QString());
            break;
        }
        pos += l;
        i++;
    }

    item.to = to;
    item.from = from;
    item.rawString = rawString;
    item.cursor = cursor;
    item.selection = selection;
    item.modified = 0;

    qimsysDebug() << item;
    qimsysDebug() << item;
    qimsysDebugOut();
//    qimsysDebugOff();
}

QString QimsysPreeditManager::Private::text(int length) const
{
    qimsysDebugIn() << length;
    QString ret;
    if (length < 0) {
        ret = item.to.join("").mid(item.cursor + length, -length);
    } else {
        ret = item.to.join("").mid(item.cursor, length);
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysPreeditManager::QimsysPreeditManager(QObject *parent, Type type)
    : QimsysAbstractIpcObject(parent, type)
{
    qimsysDebugIn() << parent << type;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysPreeditManager::~QimsysPreeditManager()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool QimsysPreeditManager::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

QimsysAbstractIpcObject *QimsysPreeditManager::server() const
{
    return d->server;
}

getProp(QimsysPreeditManager, QimsysPreeditItem, item)
setProp(QimsysPreeditManager, const QimsysPreeditItem&, item, setItem)

getProp(QimsysPreeditManager, QRect, rect)
setProp(QimsysPreeditManager, const QRect&, rect, setRect)

#ifndef QIMSYS_NO_GUI
getProp(QimsysPreeditManager, QFont, font)
setProp(QimsysPreeditManager, const QFont&, font, setFont)
#endif

getProp(QimsysPreeditManager, int, cursorPosition)
setProp(QimsysPreeditManager, int, cursorPosition, setCursorPosition)

getProp(QimsysPreeditManager, QString, surroundingText)
setProp(QimsysPreeditManager, const QString &, surroundingText, setSurroundingText)

getProp(QimsysPreeditManager, QString, currentSelection)
setProp(QimsysPreeditManager, const QString &, currentSelection, setCurrentSelection)

getProp(QimsysPreeditManager, int, maximumTextLength)
setProp(QimsysPreeditManager, int, maximumTextLength, setMaximumTextLength)

void QimsysPreeditManager::commit(const QString &string, qulonglong target)
{
    if (string.isEmpty()) return;
    qimsysDebugIn() << string << target;
    switch (type()) {
    case Server:
        emit committed(string, target);
        break;
    case Client:
        if (d->server) {
            d->server->commit(string, target);
#ifndef QIMSYS_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "commit", qVariantFromValue(string), qVariantFromValue(target));
            if (!reply.isValid()) {
                qimsysWarning() << reply.error();
            }
#endif // QIMSYS_NO_DBUS
        }
//   proxy()->metaObject()->invokeMethod( proxy(), "commit", Q_ARG( QString, preeditString() ) );
        break;
    }
//    setItems(QimsysPreeditItemList());
    qimsysDebugOut();
}

bool QimsysPreeditManager::isEmpty() const
{
    qimsysDebugIn();
    bool ret = true;
    switch (type()) {
    case Server:
        ret = d->item.to.isEmpty();
        break;
    case Client:
        if (d->server) {
            ret = d->server->isEmpty();
#ifndef QIMSYS_NO_DBUS
        } else {
            QDBusReply<bool> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "isEmpty");
            if (!reply.isValid()) {
                qimsysWarning() << reply.error();
            } else {
                ret = reply.value();
            }
#endif // QIMSYS_NO_DBUS
        }
        break;
    }
    qimsysDebugOut() << ret;
    return ret;
}

void QimsysPreeditManager::insert(const QString &str)
{
    qimsysDebugIn() << str;
    switch (type()) {
    case Server:
        d->insert(str);
        emit itemChanged(d->item);
        break;
    case Client:
        if (d->server) {
            d->server->insert(str);
#ifndef QIMSYS_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "insert", qVariantFromValue(str));
            if (!reply.isValid()) {
                qimsysWarning() << reply.error();
            }
#endif // QIMSYS_NO_DBUS
        }
        break;
    }
    qimsysDebugOut();
}

void QimsysPreeditManager::replace(const QString &str)
{
    qimsysDebugIn() << str;
    switch (type()) {
    case Server:
        d->remove(-1);
        d->insert(str);
        emit itemChanged(d->item);
        break;
    case Client:
        if (d->server) {
            d->server->replace(str);
#ifndef QIMSYS_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "replace", qVariantFromValue(str));
            if (!reply.isValid()) {
                qimsysWarning() << reply.error();
            }
#endif // QIMSYS_NO_DBUS
        }
        break;
    }
    qimsysDebugOut();
}

void QimsysPreeditManager::remove(int length)
{
    qimsysDebugIn() << length;
    switch (type()) {
    case Server:
        d->remove(length);
        emit itemChanged(d->item);
        break;
    case Client:
        if (d->server) {
            d->server->remove(length);
#ifndef QIMSYS_NO_DBUS
        } else {
            QDBusReply<void> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "remove", qVariantFromValue(length));
            if (!reply.isValid()) {
                qimsysWarning() << reply.error();
            }
#endif // QIMSYS_NO_DBUS
        }
        break;
    }
    qimsysDebugOut();
}

QString QimsysPreeditManager::text(int length) const
{
    qimsysDebugIn() << length;
    QString ret;
    switch (type()) {
    case Server:
        ret = d->text(length);
        break;
    case Client:
        if (d->server) {
            ret = d->server->text(length);
#ifndef QIMSYS_NO_DBUS
        } else {
            QDBusReply<QString> reply = qobject_cast<QDBusAbstractInterface*>(proxy())->call(QDBus::BlockWithGui, "text", qVariantFromValue(length));
            if (!reply.isValid()) {
                qimsysWarning() << reply.error();
            } else {
                ret = reply.value();
            }
#endif // QIMSYS_NO_DBUS
        }
        break;
    }
    qimsysDebugOut() << ret;
    return ret;
}

#include "qimsyspreeditmanager.moc"
