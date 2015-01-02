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

#include "converter.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <qimsysdebug.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsysdynamictranslator.h>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>

namespace Japanese {
    namespace Katakana {
        namespace Full {

class Converter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Converter *parent);
    ~Private();

private slots:
    void init();

    void enabledChanged(bool isEnabled);
    void activeChanged(bool isActive);
    void stateChanged(uint state);

    void itemChanged(const QimsysPreeditItem &item);


private:
    void readMap(const QString &fileName);

private:
    Converter *q;
    QimsysInputMethodManager *inputMethodManager;
    QimsysPreeditManager *preeditManager;

    QStringList previous;

    QMap<QString, QString> convertMap;
};

        }
    }
}

using namespace Japanese::Katakana::Full;

Converter::Private::Private(Converter *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
{
    qimsysDebugIn() << parent;
    init();
    qimsysDebugOut();
}

Converter::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Converter::Private::init()
{
    qimsysDebugIn();
    q->setIdentifier(QLatin1String("Katakana"));
    q->setPriority(0x20);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese(Standard)");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/japanese/katakana/full/resources/katakana-full.png"));
#endif
    trConnect(this, QT_TR_NOOP("Katakana"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese converter for Katakana"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Converter"), q, "categoryName");

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)));
    enabledChanged(q->isEnabled());

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
    qimsysDebugOut();
}

void Converter::Private::readMap(const QString &fileName)
{
    qimsysDebugIn() << fileName;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        convertMap.clear();
        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains('\t')) {
                QStringList fields = line.split('\t');
                if (fields.isEmpty() || fields.first().isEmpty() || fields.first().startsWith("#")) continue;
                qimsysDebug() << fields.at(0) << fields.at(1);
                convertMap[fields.at(0)] = fields.at(1);
            }
        }
        file.close();
    } else {
        qimsysWarning() << file.error() << file.errorString() << fileName;
    }
    qimsysDebugOut();
}

void Converter::Private::enabledChanged(bool isEnabled)
{
    qimsysDebugIn() << isEnabled;

    if (isEnabled) {
        if (convertMap.isEmpty()) {
            readMap(QLatin1String(":/japanese/katakana/full/resources/katakana-full.txt"));
        }
        if (!inputMethodManager) {
            inputMethodManager = new QimsysInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
        }
        stateChanged(inputMethodManager->state());
    } else {
        convertMap.clear();
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
    }
    qimsysDebugOut();
}

void Converter::Private::activeChanged(bool isActive)
{
    qimsysDebugIn() << isActive;
    if (isActive) {
    } else {
        stateChanged(Reset);
    }
    qimsysDebugOut();
}

void Converter::Private::stateChanged(uint state)
{
    qimsysDebugIn() << state;
    switch(state) {
    case Input:
        if (!preeditManager) {
            preeditManager = new QimsysPreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
        }
        itemChanged(preeditManager->item());
        break;
    case ConvertTo: {
        if (q->isActive()) {
            if (!preeditManager) {
                preeditManager = new QimsysPreeditManager(this);
                preeditManager->init();
                connect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
            }

            QimsysPreeditItem item = preeditManager->item();
            qimsysDebug() << item;
            item.to.clear();
            foreach (const QString &from, item.from) {
                QString to;
                foreach (const QChar &ch, from) {
                    if (convertMap.contains(ch)) {
                        to.append(convertMap[ch]);
                    } else {
                        to.append(ch);
                    }
                }
                item.to.append(to);
            }
            qimsysDebug() << item;
            preeditManager->setItem(item);
        }
        fallthrough;
    }
    default:
        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::itemChanged(const QimsysPreeditItem &item)
{
    if (item.selection != 0) return;
    if (item.from == previous) return;

    qimsysDebugIn() << item;

    QStringList to = item.to;

    int l = 0;
    int i = 0;
    for (i = 0; i < to.length(); i++) {
        l += to.at(i).length();
        if (l > item.cursor - 1) {
            break;
        }
    }

    qimsysDebug() << i;
    int modified = item.modified;
    int k = i;
    for (int j = 0; j < modified; j++) {
        QString str = item.from.at(k);
        int index = str.length() - j - 1 - k + i;
        str = str.mid(index, 1);
        qimsysDebug() << str << index;
        if (q->isActive()) {
            if (convertMap.contains(str)) {
                to.replace(k, to[k].replace(index, 1, convertMap[str]));
            }
        } else {
            QString rawString = item.rawString.at(k);
            qimsysDebug() << rawString;
            if (!rawString.isEmpty() && rawString.at(0).isUpper()) {
                if (convertMap.contains(str)) {
                    to.replace(k, to[k].replace(index, 1, convertMap[str]));
                }
            }
        }
        if (index == 0) k--;
    }

    QimsysPreeditItem newItem = item;
    newItem.to = to;
    preeditManager->blockSignals(true);
    preeditManager->setItem(newItem);
    preeditManager->blockSignals(false);
    previous = item.from;
    qimsysDebugOut();
}

Converter::Converter(QObject *parent)
    : QimsysConverter(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Converter::~Converter()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

#include "converter.moc"
