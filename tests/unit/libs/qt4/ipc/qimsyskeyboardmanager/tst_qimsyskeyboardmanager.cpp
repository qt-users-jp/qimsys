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

#include <qimsyskeyboardmanager.h>

class QimsysKeyboardManagerTest : public QObject
{
    Q_OBJECT
public:
    QimsysKeyboardManagerTest(QObject *parent = 0)
        : QObject(parent)
    {
        bool ret = QDBusConnection::sessionBus().registerService(QIMSYS_DBUS_SERVICE);
        if (ret) {
            keyboardManager = new QimsysKeyboardManager(this, QimsysKeyboardManager::Server);
            keyboardManager->init();
            QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            keyboardManager = new QimsysKeyboardManager(this, QimsysKeyboardManager::Client);
            keyboardManager->init();
        }
    }

    ~QimsysKeyboardManagerTest() {
        if (client.isStarted()) {
            client.waitForFinished();
        }
    }

public slots:
    void initTestCase() {
        qDebug() << keyboardManager->type();
    }

    void init() {
        switch (keyboardManager->type()) {
        case QimsysKeyboardManager::Server:
            break;
        case QimsysKeyboardManager::Client:
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
    void visibleChanged(bool visible) {
        QCOMPARE(visible, expected.toBool());
        signalReceived = true;
    }

private slots:

    void setVisible_data() {
        QTest::addColumn<bool>("data");

        QTest::newRow("true") << true;
        QTest::newRow("false") << true;
    }

    void setVisible() {
        QFETCH(bool, data);
        expected = qVariantFromValue(data);
        connect(keyboardManager, SIGNAL(visibleChanged(bool)), this, SLOT(visibleChanged(bool)));
        switch (keyboardManager->type()) {
        case QimsysKeyboardManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(keyboardManager->visible(), data);
            break;
        case QimsysKeyboardManager::Client:
            wait(200);
            signalReceived = false;
            keyboardManager->setVisible(data);
            QCOMPARE(keyboardManager->visible(), data);
            wait();
            break;
        }
        disconnect(keyboardManager, SIGNAL(visibleChanged(bool)), this, SLOT(visibleChanged(bool)));
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
    QimsysKeyboardManager *keyboardManager;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

QTEST_MAIN(QimsysKeyboardManagerTest)

#include "tst_qimsyskeyboardmanager.moc"
