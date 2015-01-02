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

#include "preedit.h"

#include <qimsysapplicationmanager.h>
#include <qimsysconverter.h>
#include <qimsyscandidates.h>
#include <qimsysdebug.h>

using namespace ja_JP;

class Preedit::Private : private QObject
{
    Q_OBJECT
public:
    Private(Preedit *parent);
    ~Private();
    bool init();

    void setPreeditString();
    void cancel();
    void setCurrentText(const QString &text);

private slots:
    void currentCandidateChanged(int currentIndex);

private:
    Preedit *q;
public:
    QimsysApplicationManager manager;
private:
    QimsysCandidates candidates;

public:
    int conversionIndex;
    QList< QimsysConversionItemList > conversionsMap;
};

Preedit::Private::Private(Preedit *parent)
    : QObject(parent)
    , q(parent)
    , conversionIndex(-1)
{
    qimsysDebugIn();
    qimsysDebugOut();
}

Preedit::Private::~Private()
{
}

bool Preedit::Private::init()
{
    qimsysDebugIn();
    bool ret = q->QimsysPreedit::init();
    if (ret) {
        manager.init();
        candidates.init();
        connect(&candidates, SIGNAL(currentIndexChanged(int)), this, SLOT(currentCandidateChanged(int)));
    }
    qimsysDebugOut() << ret;
    return ret;
}

void Preedit::Private::cancel()
{
    qimsysDebugIn();
    QimsysPreeditItem item;
    item.cursor = -1;
    int i = 0;
    foreach(const QimsysConversionItemList &conversion, conversionsMap) {
        item.to.append(conversion.first().from);
        if (i++ == conversionIndex) {
            item.cursor = item.to.length();
        }
    }
    if (item.cursor < 0) {
        item.cursor = item.to.length();
    }
    item.selection = 0;
#ifdef QIMSYS_PLATFORM_MAEMO
    item.underline = QTextCharFormat::SingleUnderline;
#else
    item.underline = QTextCharFormat::WaveUnderline;
#endif
    q->setItems(QimsysPreeditItemList() << item);
    q->setConversions(QimsysConversionItemList());
    qimsysDebugOut();
}

Preedit *Preedit::instance()
{
    static Preedit *ret = 0;
    if (!ret)
        ret = new Preedit;
    return ret;
}

Preedit::Preedit(QObject *parent)
    : QimsysPreedit(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Preedit::~Preedit()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool Preedit::init()
{
    qimsysDebugIn();
    bool ret = d->init();
    qimsysDebugOut() << ret;
    return ret;
}

void Preedit::move(int offset, bool selection)
{
    qimsysDebugIn() << offset << selection;
    if (d->conversionIndex < 0) {
        QimsysPreeditItem item = items().first();
        int pos = (item.cursor + offset + item.to.length() + 1) % (item.to.length() + 1);
        if (pos == item.cursor) {
            if (!selection) {
                item.selection = 0;
            }
        } else {
            item.cursor = pos;
            if (selection) {
                item.selection -= offset;
            } else {
                item.selection = 0;
            }
        }
        setItems(QimsysPreeditItemList() << item);
    } else {
        int conversionIndex = d->conversionIndex + offset;
        conversionIndex = (conversionIndex + d->conversionsMap.count()) % (d->conversionsMap.count());
        if (conversionIndex !=  d->conversionIndex) {
            d->conversionIndex = conversionIndex;
            d->setPreeditString();
        }
    }
    qimsysDebugOut();
}

int Preedit::conversionIndex() const
{
    return d->conversionIndex;
}

int Preedit::convercionCount() const
{
    return d->conversionsMap.count();
}

void Preedit::setConversions(const QimsysConversionItemList &conversions)
{
    QimsysConverter *converter = qobject_cast<QimsysConverter*>(parent());
    if (!converter || !converter->isActive()) return;
    qimsysDebugIn() << conversions;
    if (conversions.isEmpty()) {
        d->conversionIndex = -1;
        d->conversionsMap.clear();
    } else if (d->conversionIndex < 0) {
        d->conversionIndex = 0;
        for (int i = 0; i < conversions.count(); i++) {
            QimsysConversionItemList qce;
            qce.append(conversions[i]);
            d->conversionsMap.append(qce);
        }
        d->setPreeditString();
    } else {
        for (int i = 0; i < conversions.count(); i++) {
            if (i > d->conversionsMap.count() - 1) {
                QimsysConversionItemList qce;
                qce.append(conversions[i]);
                d->conversionsMap.append(qce);
            } else {
                Q_ASSERT(!d->conversionsMap[i].isEmpty());
                if (d->conversionsMap[i].first() == conversions[i]) {
                } else {
                    QimsysConversionItemList qce;
                    qce.append(conversions[i]);
                    d->conversionsMap[i] = qce;
                }
            }
        }
        while (conversions.count() < d->conversionsMap.count()) {
            d->conversionsMap.takeLast();
        }
        d->setPreeditString();
    }
    qimsysDebugOut();
}

void Preedit::Private::setPreeditString()
{
    qimsysDebugIn();
    qimsysDebug() << conversionIndex;
    QimsysConversionItemList c = candidates.candidates();
    QimsysPreeditItemList items;
    for (int i = 0; i < conversionsMap.count(); i++) {
        QimsysPreeditItem item;
        item.to = conversionsMap[i].last().to;
        item.cursor = -1;
        item.selection = 0;
        item.underline = QTextCharFormat::SingleUnderline;
        if (conversionIndex < 0 && !c.isEmpty()) {
            item.cursor = 0;
            item.selection = item.to.length();
            item.underline = QTextCharFormat::NoUnderline;
        } else if (i == conversionIndex) {
            item.cursor = 0;
            item.selection = item.to.length();
            item.underline = QTextCharFormat::NoUnderline;
        }
        items.append(item);
    }
    qimsysDebug() << items;
    q->setItems(items);
    qimsysDebugOut();
}

void Preedit::Private::currentCandidateChanged(int currentIndex)
{
    QimsysConverter *converter = qobject_cast<QimsysConverter*>(q->parent());
    if (!converter || !converter->isActive()) return;
    if (currentIndex < 0) return;
    qimsysDebugIn() << currentIndex;
    if (conversionIndex < 0) {
        QimsysConversionItemList list;
        list.append(candidates.candidates()[currentIndex]);
        switch (conversionsMap.count()) {
        case 0:
            conversionsMap.append(list);
            break;
        case 1:
            conversionsMap[0] = list;
            break;
        default:
            qimsysWarning() << currentIndex << "is not handled here.";
            break;
        }
        setPreeditString();
    } else {
        switch (conversionsMap[conversionIndex].count()) {
        case 1:
            conversionsMap[conversionIndex].append(candidates.candidates().at(currentIndex));
            break;
        case 2:
            conversionsMap[conversionIndex][1] = candidates.candidates().at(currentIndex);
            break;
        default:
            qimsysWarning();
        }
        setPreeditString();
    }
    qimsysDebugOut();
}

void Preedit::setCurrentText(const QString &text)
{
    d->setCurrentText(text);
}

void Preedit::Private::setCurrentText(const QString &text)
{
    qimsysDebugIn() << text;
    QimsysConversionItem qce;
    Q_ASSERT(!conversionsMap[conversionIndex].isEmpty());
    qce.from = conversionsMap[conversionIndex].first().from;
    qce.to = text;
    switch (conversionsMap[conversionIndex].count()) {
    case 1:
        conversionsMap[conversionIndex].append(qce);
        break;
    case 2:
        conversionsMap[conversionIndex][1] = qce;
        break;
    default:
        qimsysWarning();
    }
    setPreeditString();
    qimsysDebugOut();
}

void Preedit::commit()
{
    qimsysDebugIn();
    QString commitString;
    foreach (const QimsysPreeditItem &item, items())
        commitString += item.to;
    QimsysPreedit::commit(commitString, d->manager.widget());
    QimsysConversionItemList qce;
    for (int i = 0; i < d->conversionsMap.count(); i++) {
        qce.append(d->conversionsMap.last().last());
    }
    emit committed(qce);
    d->conversionsMap.clear();
    d->conversionIndex = -1;
    qimsysDebugOut();
}

void Preedit::cancel()
{
    d->cancel();
}

#include "preedit.moc"
