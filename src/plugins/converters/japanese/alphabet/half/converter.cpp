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

#include "converter.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <qimsysdebug.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsysdynamictranslator.h>

namespace Japanese {
    namespace Alphabet {
        namespace Half {

class Converter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Converter *parent);
    ~Private();

private slots:
    void init();

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
};

        }
    }
}

using namespace Japanese::Alphabet::Half;

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
    q->setIdentifier(QLatin1String("Alphabet"));
    q->setPriority(0x31);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese(Standard)");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/japanese/alphabet/half/resources/alphabet-half.png"));
#endif
    trConnect(this, QT_TR_NOOP("Alphabet"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese converter for Alphabet"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Converter"), q, "categoryName");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
    qimsysDebugOut();
}

void Converter::Private::activeChanged(bool isActive)
{
    qimsysDebugIn() << isActive;
    if (isActive) {
        if (!inputMethodManager) {
            inputMethodManager = new QimsysInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
        }
        stateChanged(inputMethodManager->state());
    } else {
        stateChanged(Reset);
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
    }
    qimsysDebugOut();
}

void Converter::Private::stateChanged(uint state)
{
    qimsysDebugIn() << state;
    switch (state) {
    case Input:
        if (!preeditManager) {
            preeditManager = new QimsysPreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
        }
        itemChanged(preeditManager->item());
        break;
    case ConvertTo: {
        if (!preeditManager) {
            preeditManager = new QimsysPreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
        }

        QimsysPreeditItem item = preeditManager->item();
        item.to = item.rawString;
        preeditManager->setItem(item);
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
    int cursor = item.cursor;

    int l = 0;
    int i = 0;
    for (i = 0; i < to.length(); i++) {
        l += to.at(i).length();
        if (l > cursor - 1) {
            break;
        }
    }

    qimsysDebug() << i;
    int modified = item.modified;
    int k = i;
    for (int j = 0; j < modified; j++) {
        QString str = item.rawString.at(k);
        j += str.length() - 1;
        qimsysDebug() << str << j;
        cursor += str.length() - to.at(k).length();
        to.replace(k, str);
        k--;
    }
    QimsysPreeditItem newItem = item;
    newItem.to = to;
    newItem.cursor = cursor;
    newItem.modified = 0;
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
