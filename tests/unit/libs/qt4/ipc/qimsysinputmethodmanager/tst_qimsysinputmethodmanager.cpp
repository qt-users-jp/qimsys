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

#include <qimsysinputmethodmanager.h>

class QimsysInputMethodManagerTest : public QObject
{
    Q_OBJECT
public:
    QimsysInputMethodManagerTest(QObject *parent = 0)
        : QObject(parent)
    {
        bool ret = QDBusConnection::sessionBus().registerService(QIMSYS_DBUS_SERVICE);
        if (ret) {
            inputmethodManager = new QimsysInputMethodManager(this, QimsysInputMethodManager::Server);
            inputmethodManager->init();
            QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            inputmethodManager = new QimsysInputMethodManager(this, QimsysInputMethodManager::Client);
            inputmethodManager->init();
        }
    }

    ~QimsysInputMethodManagerTest() {
        if (client.isStarted()) {
            client.waitForFinished();
        }
    }

public slots:
    void initTestCase() {
        qDebug() << inputmethodManager->type();
    }

    void init() {
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            break;
        case QimsysInputMethodManager::Client:
            break;
        }
        expected.clear();
        signalReceived = false;
    }

    void cleanup() {
        signalReceived = false;
    }

    void cleanupTestCase() {
        wait();
    }

protected slots:
    void localeChanged(const QString &locale) {
        QCOMPARE(locale, expected.toString());
        signalReceived = true;
    }

    void identifierChanged(const QString &identifier) {
        QCOMPARE(identifier, expected.toString());
        signalReceived = true;
    }

    void converterChanged(const QString &converter) {
        QCOMPARE(converter, expected.toString());
        signalReceived = true;
    }

    void interpreterChanged(const QString &interpreter) {
        QCOMPARE(interpreter, expected.toString());
        signalReceived = true;
    }

    void engineChanged(const QString &engine) {
        QCOMPARE(engine, expected.toString());
        signalReceived = true;
    }

    void stateChanged(uint state) {
        QCOMPARE(state, expected.toUInt());
        signalReceived = true;
    }

    void executed(const QString &command) {
        QCOMPARE(command, expected.toString());
        signalReceived = true;
    }

private slots:
    void setLocale_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("locale %1").arg(i);
        }
    }

    void setLocale() {
        QFETCH(QString, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(localeChanged(QString)), this, SLOT(localeChanged(QString)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(inputmethodManager->locale(), data);
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->setLocale(data);
            QCOMPARE(inputmethodManager->locale(), data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(localeChanged(QString)), this, SLOT(localeChanged(QString)));
    }

    void setIdentifier_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("identifier %1").arg(i);
        }
    }

    void setIdentifier() {
        QFETCH(QString, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(identifierChanged(QString)), this, SLOT(identifierChanged(QString)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(inputmethodManager->identifier(), data);
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->setIdentifier(data);
            QCOMPARE(inputmethodManager->identifier(), data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(identifierChanged(QString)), this, SLOT(identifierChanged(QString)));
    }

    void setConverter_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("converter %1").arg(i);
        }
    }

    void setConverter() {
        QFETCH(QString, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(converterChanged(QString)), this, SLOT(converterChanged(QString)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(inputmethodManager->converter(), data);
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->setConverter(data);
            QCOMPARE(inputmethodManager->converter(), data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(converterChanged(QString)), this, SLOT(converterChanged(QString)));
    }

    void setInterpreter_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("interpreter %1").arg(i);
        }
    }

    void setInterpreter() {
        QFETCH(QString, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(interpreterChanged(QString)), this, SLOT(interpreterChanged(QString)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(inputmethodManager->interpreter(), data);
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->setInterpreter(data);
            QCOMPARE(inputmethodManager->interpreter(), data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(interpreterChanged(QString)), this, SLOT(interpreterChanged(QString)));
    }

    void setEngine_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("engine %1").arg(i);
        }
    }

    void setEngine() {
        QFETCH(QString, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(engineChanged(QString)), this, SLOT(engineChanged(QString)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(inputmethodManager->engine(), data);
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->setEngine(data);
            QCOMPARE(inputmethodManager->engine(), data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(engineChanged(QString)), this, SLOT(engineChanged(QString)));
    }

    void setState_data() {
        QTest::addColumn<uint>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << (uint)i;
        }
    }

    void setState() {
        QFETCH(uint, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(inputmethodManager->state(), data);
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->setState(data);
            QCOMPARE(inputmethodManager->state(), data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
    }

    void execute_data() {
        QTest::addColumn<QString>("data");

        for (int i = 0; i < 10; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << QString("command %1").arg(i);
        }
    }

    void execute() {
        QFETCH(QString, data);
        expected = qVariantFromValue(data);
        connect(inputmethodManager, SIGNAL(executed(QString)), this, SLOT(executed(QString)));
        switch (inputmethodManager->type()) {
        case QimsysInputMethodManager::Server:
            signalReceived = false;
            wait();
            break;
        case QimsysInputMethodManager::Client:
            wait(200);
            signalReceived = false;
            inputmethodManager->execute(data);
            wait();
            break;
        }
        disconnect(inputmethodManager, SIGNAL(executed(QString)), this, SLOT(executed(QString)));
    }

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
    QimsysInputMethodManager *inputmethodManager;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

QTEST_MAIN(QimsysInputMethodManagerTest)
#include "tst_qimsysinputmethodmanager.moc"
