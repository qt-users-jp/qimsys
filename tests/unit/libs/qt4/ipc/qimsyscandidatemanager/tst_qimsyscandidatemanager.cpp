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

#include <QtTest>
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>
#include <QtConcurrentRun>

#include <qimsyscandidatemanager.h>

class QimsysCandidateManagerTest : public QObject
{
    Q_OBJECT
public:
    QimsysCandidateManagerTest(QObject *parent = 0)
        : QObject(parent)
    {
        bool ret = QDBusConnection::sessionBus().registerService(QIMSYS_DBUS_SERVICE);
        if (ret) {
            candidateManager = new QimsysCandidateManager(this, QimsysCandidateManager::Server);
            candidateManager->init();
            QStringList args = QCoreApplication::arguments();
            args.removeFirst();
            client = QtConcurrent::run(QProcess::execute, QCoreApplication::applicationFilePath(), args);
        } else {
            candidateManager = new QimsysCandidateManager(this, QimsysCandidateManager::Client);
            candidateManager->init();
        }
    }

    ~QimsysCandidateManagerTest() {
        if (client.isStarted()) {
            client.waitForFinished();
        }
    }

public slots:
    void initTestCase() {
        qDebug() << candidateManager->type();
    }

    void init() {
        switch (candidateManager->type()) {
        case QimsysCandidateManager::Server:
            break;
        case QimsysCandidateManager::Client:
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
    void itemsChanged(const QimsysConversionItemList &candidates) {
        QCOMPARE(candidates, expected.value<QimsysConversionItemList>());
        signalReceived = true;
    }

    void currentIndexChanged(int currentIndex) {
        QCOMPARE(currentIndex, expected.toInt());
        signalReceived = true;
    }

private slots:
    void setItems_data() {
        QTest::addColumn<QimsysConversionItemList>("data");

        QimsysConversionItemList candidates;
        QString alphabet = "abcde";
        for (int i = 0; i < alphabet.length() + 1; i++) {
            QTest::newRow(QString("candidates(%1)").arg(i).toLocal8Bit().data()) << candidates;
            QimsysConversionItem elem;
            elem.from = QLatin1String("from:") + alphabet.left(i + 1);
            elem.to = QLatin1String("to:") + alphabet.left(i + 1);
            candidates.append(elem);
        }
    }

    void setItems() {
        QFETCH(QimsysConversionItemList, data);
        expected.setValue(data);
        connect(candidateManager, SIGNAL(itemsChanged(QimsysConversionItemList)), this, SLOT(itemsChanged(QimsysConversionItemList)));
        switch (candidateManager->type()) {
        case QimsysCandidateManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(candidateManager->items(), data);
            break;
        case QimsysCandidateManager::Client:
            wait(200);
            signalReceived = false;
            candidateManager->setItems(data);
            QCOMPARE(candidateManager->items(), data);
            wait();
            break;
        }
        disconnect(candidateManager, SIGNAL(itemsChanged(QimsysConversionItemList)), this, SLOT(itemsChanged(QimsysConversionItemList)));
    }

    void setCurrentIndex_data() {
        QTest::addColumn<int>("data");

        for (int i = 0; i < 2; i++) {
            QTest::newRow(QString::number(i).toLocal8Bit().data()) << i;
        }
    }

    void setCurrentIndex() {
        QFETCH(int, data);
        expected = data;
        connect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        switch (candidateManager->type()) {
        case QimsysCandidateManager::Server:
            signalReceived = false;
            wait();
            QCOMPARE(candidateManager->currentIndex(), data);
            break;
        case QimsysCandidateManager::Client:
            wait(200);
            signalReceived = false;
            candidateManager->setCurrentIndex(data);
            QCOMPARE(candidateManager->currentIndex(), data);
            wait();
            break;
        }
        disconnect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
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
    QimsysCandidateManager *candidateManager;
    QFuture<int> client;
    bool signalReceived;
    QVariant expected;
};

QTEST_MAIN(QimsysCandidateManagerTest)

#include "tst_qimsyscandidatemanager.moc"
