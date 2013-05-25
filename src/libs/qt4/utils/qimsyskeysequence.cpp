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

#include "qimsyskeysequence.h"
#include "qimsysdebug.h"

#include <QKeySequence>

class QimsysKeySequence::Private
{
public:
    enum PatchStatus
    {
        NotChecked,
        NotFound,
        Found,
    };

    Private();
    static PatchStatus b19899f81cb7ac3b35e5fad3af777e89f1627474;
    int keyCode;
    QString keyString;
};

QimsysKeySequence::Private::PatchStatus QimsysKeySequence::Private::b19899f81cb7ac3b35e5fad3af777e89f1627474 = QimsysKeySequence::Private::NotChecked;

QimsysKeySequence::Private::Private()
    : keyCode(0)
{
    qimsysDebugIn();
    if (b19899f81cb7ac3b35e5fad3af777e89f1627474 == NotChecked) {
        QKeySequence ks(Qt::Key_Zenkaku_Hankaku);
        if (ks.toString() == QLatin1String("Zenkaku Hankaku"))
            b19899f81cb7ac3b35e5fad3af777e89f1627474 = Found;
        else
            b19899f81cb7ac3b35e5fad3af777e89f1627474 = NotFound;
    }
    qimsysDebugOut() << this;
}

QimsysKeySequence::QimsysKeySequence()
{
    qimsysDebugIn();
    d = new Private;
    qimsysDebugOut();
}

QimsysKeySequence::QimsysKeySequence(const QimsysKeySequence &other)
{
    qimsysDebugIn();
    d = new Private;
    d->keyString = other.d->keyString;
    d->keyCode = other.d->keyCode;
    qimsysDebugOut();
}

QimsysKeySequence::QimsysKeySequence(const QString &key)
{
    qimsysDebugIn() << key;
    d = new Private;
    d->keyString = key;
    if (d->b19899f81cb7ac3b35e5fad3af777e89f1627474 == Private::NotFound) {
#define IF(c, n) \
        if (key == QLatin1String(n)) { \
            d->keyCode = Qt::Key_##c; \
        }
        IF(Kanji, "Kanji")
        else IF(Muhenkan, "Muhenkan")
        else IF(Henkan, "Henkan")
        else IF(Romaji, "Romaji")
        else IF(Hiragana, "Hiragana")
        else IF(Katakana, "Katakana")
        else IF(Hiragana_Katakana, "Hiragana Katakana")
        else IF(Zenkaku, "Zenkaku")
        else IF(Hankaku, "Hankaku")
        else IF(Zenkaku_Hankaku, "Zenkaku Hankaku")
        else {
            d->keyCode = QKeySequence(key)[0];
        }
    } else {
        d->keyCode = QKeySequence(key)[0];
    }
#undef IF
    qimsysDebugOut();
}

QimsysKeySequence::QimsysKeySequence(int k1)
{
    qimsysDebugIn() << k1;
    d = new Private;
    d->keyCode = k1;
    if (d->b19899f81cb7ac3b35e5fad3af777e89f1627474 == Private::NotFound) {
        switch (k1) {
#define CASE(c, n) \
        case Qt::Key_##c: \
            d->keyString = QLatin1String(n); \
            break
        CASE(Kanji, "Kanji");
        CASE(Muhenkan, "Muhenkan");
        CASE(Henkan, "Henkan");
        CASE(Romaji, "Romaji");
        CASE(Hiragana, "Hiragana");
        CASE(Katakana, "Katakana");
        CASE(Hiragana_Katakana, "Hiragana Katakana");
        CASE(Zenkaku, "Zenkaku");
        CASE(Hankaku, "Hankaku");
        CASE(Zenkaku_Hankaku, "Zenkaku Hankaku");
#undef CASE
        default:
            d->keyString = QKeySequence(k1).toString();
            break;
        }
    } else {
        d->keyString = QKeySequence(k1).toString();
    }
    qimsysDebugOut();
}

QimsysKeySequence::~QimsysKeySequence()
{
    qimsysDebugIn() << d;
    delete d;
    qimsysDebugOut();
}

QString QimsysKeySequence::toString() const
{
    qimsysDebugIn();
    QString ret = d->keyString;
    qimsysDebugOut() << ret;
    return ret;
}

QimsysKeySequence &QimsysKeySequence::operator=(const QimsysKeySequence &other)
{
    d->keyString = other.d->keyString;
    d->keyCode = other.d->keyCode;
    return *this;
}

bool QimsysKeySequence::operator==(const QimsysKeySequence &other) const
{
#ifdef QIMSYS_DEBUG
    qimsysDebugIn() << other.toString();
    bool ret = d->keyString == other.d->keyString;
    qimsysDebugOut() << ret;
    return ret;
#else
    return d->keyString == other.d->keyString;
#endif
}

bool QimsysKeySequence::operator<(const QimsysKeySequence &other) const
{
#ifdef QIMSYS_DEBUG
    qimsysDebugIn() << other.toString();
    bool ret = d->keyString < other.d->keyString;
    qimsysDebugOut() << ret;
    return ret;
#else
    return d->keyString < other.d->keyString;
#endif
}

int QimsysKeySequence::operator[](uint index) const
{
    qimsysDebugIn() << index;
    if (index > 0) {
        qimsysWarning() << index << "is not zero.";
    }
    qimsysDebugOut() << d->keyCode;
    return d->keyCode;
}
