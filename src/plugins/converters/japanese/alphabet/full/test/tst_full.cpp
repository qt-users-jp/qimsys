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

#include <qimsysdebug.h>
#include <qimsystest.h>
#include <qimsysapplicationmanager.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>

namespace Japanese {
    namespace Alphabet {
        namespace Full {

class Test : public QimsysTest
{
    Q_OBJECT
public slots:
    void initTestCase();
    void cleanup();

protected:
    QStringList activePluginList() const { return QStringList() << QLatin1String("Fullwidth Alphabet"); }

private slots:
    void test_data();
    void test();

private:
    QimsysApplicationManager applicationManadatager;
    QimsysInputMethodManager inputMethodManager;
    QimsysPreeditManager preeditManager;
};

        }
    }
}

using namespace Japanese::Alphabet::Full;

void Test::initTestCase()
{
    QimsysTest::initTestCase();

    applicationManadatager.init();
    inputMethodManager.init();
    preeditManager.init();

    applicationManadatager.setComposing(true);
}

void Test::cleanup()
{
    preeditManager.setItem(QimsysPreeditItem());
}

void Test::test_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");

    QFile file(":/test.txt");
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QTextStream stream(&file);

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.isEmpty()) continue;
        if (line.startsWith("//")) continue;
        QStringList fields = line.split('\t');
        if (fields.count() < 2) continue;
        QTest::newRow(fields.at(0).toLatin1().data()) << fields.at(0) << fields.at(1);
    }

    file.close();
}

void Test::test()
{
    QFETCH(QString, from);
    QFETCH(QString, to);

    inputMethodManager.setState(0x04);
    foreach (const QChar &ch, from) {
        preeditManager.insert(ch);
    }

    QCOMPARE(preeditManager.item().to.join(""), to);
}

QTEST_MAIN(Test)

#include "tst_full.moc"
