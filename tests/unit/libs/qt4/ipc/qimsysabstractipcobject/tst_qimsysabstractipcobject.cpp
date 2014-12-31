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

#include <QtTest>
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>
#include <QtConcurrentRun>

#include <qimsysdebug.h>
#include <qimsysabstractipcobject.h>
#include "qimsysobject.h"

static const int loopCount = 3;

class QimsysAbstractIpcObjectTest : public QObject
{
    Q_OBJECT
public:
    QimsysAbstractIpcObjectTest(QObject *parent = 0)
        : QObject(parent)
    {
        qimsysDebugIn() << parent;
        bool ret = QDBusConnection::sessionBus().registerService(QIMSYS_DBUS_SERVICE);
        if (ret) {
            object = new AbstractIpcObject(this, QimsysAbstractIpcObject::Server);
            object->init();
            QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            object = new AbstractIpcObject(this, QimsysAbstractIpcObject::Client);
            object->init();
        }
        qimsysDebugOut() << object->type();
    }

    ~QimsysAbstractIpcObjectTest() {
        qimsysDebugIn() << object->type();
        if (client.isStarted()) {
            client.waitForFinished();
        }
        qimsysDebugOut();
    }

public slots:
    void initTestCase() {
        qimsysDebugIn() << object->type();
        qimsysDebugOut();
    }

    void init() {
        qimsysDebugIn() << object->type();
        expected.clear();
        signalReceived = false;
        qimsysDebugOut();
    }

    void cleanup() {
        qimsysDebugIn() << object->type();
        signalReceived = false;
        qimsysDebugOut();
    }

    void cleanupTestCase() {
        qimsysDebugIn() << object->type();
        signalReceived = false;
        wait();
        qimsysDebugOut();
    }

protected slots:
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

private slots:
    void boolValue_data();
    void boolValue();
    void intValue_data();
    void intValue();
    void uintValue_data();
    void uintValue();
    void qulonglongValue_data();
    void qulonglongValue();

    void stringValue_data();
    void stringValue();
    void pointValue_data();
    void pointValue();
    void sizeValue_data();
    void sizeValue();
    void rectValue_data();
    void rectValue();
#ifdef QT_GUI_LIB
    void fontValue_data();
    void fontValue();
#endif

    void enumValue_data();
    void enumValue();

private:
    bool wait(int timeout = 1000) {
        bool ret = false;
        QTime timer;
        timer.start();
        while (!signalReceived) {
            QTest::qWait(50);
            if (timer.elapsed() > timeout) {
                ret = true;
                break;
            }
        }
        return ret;
    }
private:
    AbstractIpcObject *object;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

#define TEST(T, func, ...) \
    void QimsysAbstractIpcObjectTest::func##ValueChanged(T func##Value) \
    { \
        qimsysDebugIn() << object->type(); \
        T value = expected.value<T>(); \
        QCOMPARE(func##Value, value); \
        signalReceived = true; \
        qimsysDebugOut(); \
    } \
    void QimsysAbstractIpcObjectTest::func##Value_data() \
    { \
       qimsysDebugIn() << object->type(); \
        QTest::addColumn<T>("data"); \
        \
        __VA_ARGS__ \
        qimsysDebugOut(); \
    } \
    void QimsysAbstractIpcObjectTest::func##Value() \
    { \
        QFETCH(T, data); \
        qimsysDebugIn() << object->type(); \
        expected.setValue(data); \
        connect(object, SIGNAL(func##ValueChanged(T)), this, SLOT(func##ValueChanged(T))); \
        switch(object->type()) \
        { \
        case QimsysAbstractIpcObject::Server: \
            signalReceived = false; \
            wait(); \
            QCOMPARE(object->func##Value(), data); \
            break; \
        case QimsysAbstractIpcObject::Client: \
            wait(200); \
            signalReceived = false; \
            object->func##Value(data); \
            QCOMPARE(object->func##Value(), data); \
            wait(); \
            break;\
        } \
        disconnect(object, SIGNAL(func##ValueChanged(T)), this, SLOT(func##ValueChanged(T))); \
        qimsysDebugOut(); \
    }

TEST(bool, bool,
        QTest::newRow("True") << true;
        QTest::newRow("False") << false;
    )
TEST(int, int,
        for (int i = 0; i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << i;
        }
    )
TEST(uint, uint,
        for (int i = 0; i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << (uint)i;
        }
    )
TEST(qulonglong, qulonglong,
        for (int i = 0; i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << (qulonglong)i;
        }
    )

TEST(QString, string,
        QString str("I am a pen.");
        for (int i = 0; i < str.length() && i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << str.left(i);
        }
    )
TEST(QPoint, point,
        for (int i = 0; i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << QPoint(i, i);
        }
    )
TEST(QSize, size,
        for (int i = 0; i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << QSize(i, i);
        }
    )
TEST(QRect, rect,
        for (int i = 0; i < loopCount; i++) {
            QTest::newRow(QString::number(i).toUtf8().data()) << QRect(i, i, i, i);
        }
    )

#ifdef QT_GUI_LIB
TEST(QFont, font,
        QTest::newRow("default") << QFont();
        QTest::newRow("arial") << QFont("Arial");
    )
#endif

TEST(AbstractIpcObject::Enum, enum,
        QTest::newRow("0") << AbstractIpcObject::Enum0;
        QTest::newRow("1") << AbstractIpcObject::Enum1;
        QTest::newRow("2") << AbstractIpcObject::Enum2;
        QTest::newRow("3") << AbstractIpcObject::Enum3;
    )

#undef TEST

QTEST_MAIN(QimsysAbstractIpcObjectTest)
#include "tst_qimsysabstractipcobject.moc"
