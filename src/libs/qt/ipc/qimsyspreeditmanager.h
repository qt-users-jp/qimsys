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

#ifndef QIMSYSPREEDITMANAGER_H
#define QIMSYSPREEDITMANAGER_H

#include "qimsysglobal.h"
#include "qimsysabstractipcobject.h"
#include "qimsyspreedititem.h"
#include <QRect>

#ifndef QIMSYS_NO_GUI
#include <QFont>
#endif

class QIMSYS_EXPORT QimsysPreeditManager : public QimsysAbstractIpcObject
{
    Q_OBJECT
    Q_PROPERTY(QimsysPreeditItem item READ item WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY rectChanged)
#ifndef QIMSYS_NO_GUI
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
#endif
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QString surroundingText READ surroundingText WRITE setSurroundingText NOTIFY surroundingTextChanged)
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY currentSelectionChanged)
    Q_PROPERTY(int maximumTextLength READ maximumTextLength WRITE setMaximumTextLength NOTIFY maximumTextLengthChanged)
    Q_CLASSINFO("D-Bus Interface", "local.PreeditManager")
    Q_DISABLE_COPY(QimsysPreeditManager)
public:
    explicit QimsysPreeditManager(QObject *parent = 0, Type type = Client);
    ~QimsysPreeditManager();

    virtual bool init();

    Q_INVOKABLE bool isEmpty() const;
    Q_INVOKABLE void insert(const QString &str);
    Q_INVOKABLE void replace(const QString &str);
    Q_INVOKABLE void remove(int length);
    Q_INVOKABLE QString text(int length) const;

    Q_INVOKABLE QimsysPreeditItem item() const;
    Q_INVOKABLE QRect rect() const;
#ifndef QIMSYS_NO_GUI
    Q_INVOKABLE QFont font() const;
#endif
    Q_INVOKABLE int cursorPosition() const;
    Q_INVOKABLE QString surroundingText() const;
    Q_INVOKABLE QString currentSelection() const;
    Q_INVOKABLE int maximumTextLength() const;

public slots:
    void setItem(const QimsysPreeditItem &item);
    void setRect(const QRect &rect);
#ifndef QIMSYS_NO_GUI
    void setFont(const QFont &font);
#endif
    void setCursorPosition(int cursorPosition);
    void setSurroundingText(const QString &surroundingText);
    void setCurrentSelection(const QString &currentSelection);
    void setMaximumTextLength(int maximumTextLength);

    void commit(const QString &string, qulonglong target = 0);

signals:
    void itemChanged(const QimsysPreeditItem &item);
    void rectChanged(const QRect &rect);
#ifndef QIMSYS_NO_GUI
    void fontChanged(const QFont &font);
#endif
    void cursorPositionChanged(int cursorPosition);
    void surroundingTextChanged(const QString &surroundingText);
    void currentSelectionChanged(const QString &currentSelection);
    void maximumTextLengthChanged(int maximumTextLength);

    void committed(const QString &commitString, qulonglong target);

protected:
    QimsysAbstractIpcObject *server() const;

private:
    class Private;
    Private *d;
};

#ifndef QIMSYS_NO_DBUS
#ifndef QIMSYS_NO_GUI
DBUSARGUMENT_OUT(QFont value,
                 arg.beginStructure();
                 arg << value.toString();
                 arg.endStructure();
                )

DBUSARGUMENT_IN(QFont &value,
                arg.beginStructure();
                value.fromString(qdbus_cast<QString>(arg));
                arg.endStructure();
               )
#endif // QIMSYS_NO_GUI
#endif // QIMSYS_NO_DBUS

#endif // QIMSYSPREEDITMANAGER_H
