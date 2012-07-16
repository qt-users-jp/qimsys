/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010 by Tasuku Suzuki <stasuku@gmail.com>                 *
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
    namespace Hiragana {
        namespace Full {

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

private:
    Converter *q;
    QimsysInputMethodManager *inputMethodManager;
};

        }
    }
}

using namespace Japanese::Hiragana::Full;

Converter::Private::Private(Converter *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
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
    q->setIdentifier(QLatin1String("Hiragana"));
    q->setPriority(0x10);

    q->setLocale("ja_JP");
    q->setLanguage("Japanese(Standard)");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/japanese/hiragana/full/resources/hiragana-full.png"));
#endif
    trConnect(this, QT_TR_NOOP("Hiragana"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("Japanese converter for Hiragana"), q, "description");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    trConnect(this, QT_TR_NOOP("Input/Converter"), q, "categoryName");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
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
    case ConvertTo: {
        QimsysPreeditManager preeditManager;
        preeditManager.init();

        QimsysPreeditItem item = preeditManager.item();
        item.to = item.from;
        preeditManager.setItem(item);
        break;
    }
    default:
        break;
    }
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
