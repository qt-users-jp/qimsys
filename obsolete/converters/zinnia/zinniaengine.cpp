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

#include "zinniaengine.h"

#include <qimsysdebug.h>

#include <QStringList>

#include <zinnia.h>

namespace zinnia {

class ZinniaEngine::Private
{
public:
    Private(ZinniaEngine *parent);
    ~Private();

    void setSize(const QSize &size);
    void recognize(const QList<QPolygon> &data);
private:
    ZinniaEngine *q;
    Recognizer *recognizer;
public:
    QSize size;
};

}

using namespace zinnia;

ZinniaEngine::Private::Private(ZinniaEngine *parent)
    : q(parent)
    , size(100, 100)
{
    qimsysDebugIn() << parent;
    recognizer = Recognizer::create();
    if (!recognizer->open(QIMSYS_ZINNIA_DEFAULT_MODEL_PATH)) {
        delete recognizer;
        recognizer = 0;
    }
    qimsysDebugOut();
}

ZinniaEngine::Private::~Private()
{
    qimsysDebugIn();
    delete recognizer;
    qimsysDebugOut();
}

void ZinniaEngine::Private::setSize(const QSize &s)
{
    qimsysDebugIn() << s;
    size = s;
    qimsysDebugOut();
}

void ZinniaEngine::Private::recognize(const QList<QPolygon> &data)
{
    if (!recognizer) return;
    qimsysDebugIn() << data;
    Character *character = Character::create();
    character->clear();
    character->set_width(size.width());
    character->set_height(size.height());

    for (int i = 0; i < data.size(); i++) {
        foreach (const QPoint &p, data.at(i)) {
            character->add(i, p.x(), p.y());
        }
    }

    Result *result = recognizer->classify(*character, 20);
    if (!result) {
        emit q->recognized(QStringList());
    } else {
        QStringList candidates;
        for (uint i = 0; i < result->size(); i++) {
            candidates.append(QString::fromUtf8(result->value(i)));
        }
        qimsysDebug() << candidates;
        emit q->recognized(candidates);
        delete result;
    }
    delete character;
    qimsysDebugOut();
}

ZinniaEngine::ZinniaEngine(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

ZinniaEngine::~ZinniaEngine()
{
    delete d;
}

const QSize &ZinniaEngine::size() const
{
    return d->size;
}

void ZinniaEngine::setSize(const QSize &size)
{
    d->setSize(size);
}

void ZinniaEngine::setData(const QList<QPolygon> &data)
{
    d->recognize(data);
}
