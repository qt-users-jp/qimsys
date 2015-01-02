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

#ifndef QIMSYSOBJECT_H
#define QIMSYSOBJECT_H

#include "qimsysabstractipcobject.h"

#include <QDebug>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QRect>
#ifdef QT_GUI_LIB
#include <QFont>
#endif

class AbstractIpcObject : public QimsysAbstractIpcObject
{
    Q_OBJECT
    Q_PROPERTY(bool boolValue READ boolValue WRITE boolValue)
    Q_PROPERTY(int intValue READ intValue WRITE intValue)
    Q_PROPERTY(uint uintValue READ uintValue WRITE uintValue)
    Q_PROPERTY(qulonglong qulonglongValue READ qulonglongValue WRITE qulonglongValue)

    Q_PROPERTY(QString stringValue READ stringValue WRITE stringValue)
    Q_PROPERTY(QPoint pointValue READ pointValue WRITE pointValue)
    Q_PROPERTY(QSize sizeValue READ sizeValue WRITE sizeValue)
    Q_PROPERTY(QRect rectValue READ rectValue WRITE rectValue)
#ifdef QT_GUI_LIB
    Q_PROPERTY(QFont fontValue READ fontValue WRITE fontValue)
#endif

    Q_ENUMS(AbstractIpcObject::Enum)
    Q_PROPERTY(AbstractIpcObject::Enum enumValue READ enumValue WRITE enumValue)

    Q_CLASSINFO("D-Bus Interface", "local.AbstractObject")

public:
    enum Enum {
        Enum0,
        Enum1,
        Enum2,
        Enum3,
        Enum4,
        Enum5,
    };
    AbstractIpcObject(QObject *parent, QimsysAbstractIpcObject::Type type);
    ~AbstractIpcObject();
    virtual bool init();

    bool boolValue() const;
    int intValue() const;
    uint uintValue() const;
    qulonglong qulonglongValue() const;

    QString stringValue() const;
    QPoint pointValue() const;
    QSize sizeValue() const;
    QRect rectValue() const;
#ifdef QT_GUI_LIB
    QFont fontValue() const;
#endif

    AbstractIpcObject::Enum enumValue() const;

public slots:
    void boolValue(bool value);
    void intValue(int value);
    void uintValue(uint value);
    void qulonglongValue(qulonglong value);

    void stringValue(QString value);
    void pointValue(QPoint value);
    void sizeValue(QSize value);
    void rectValue(QRect value);
#ifdef QT_GUI_LIB
    void fontValue(QFont value);
#endif

    void enumValue(AbstractIpcObject::Enum value);

signals:
    void boolValueChanged(bool value);
    void intValueChanged(int value);
    void uintValueChanged(uint value);
    void qulonglongValueChanged(qulonglong value);

    void stringValueChanged(QString value);
    void pointValueChanged(QPoint value);
    void sizeValueChanged(QSize value);
    void rectValueChanged(QRect value);
#ifdef QT_GUI_LIB
    void fontValueChanged(QFont value);
#endif

    void enumValueChanged(AbstractIpcObject::Enum value);

protected:
    QimsysAbstractIpcObject *server() const;

private:
    class Private;
    Private *d;
};

#ifdef QT_GUI_LIB
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
#endif

Q_DECLARE_DBUS_METAENUM(AbstractIpcObject::Enum)

#endif // QIMSYSOBJECT_H
