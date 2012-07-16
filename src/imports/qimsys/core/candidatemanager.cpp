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

#include "candidatemanager.h"

#include <qimsysdebug.h>
#include <qimsyscandidatemanager.h>
#include <QtDeclarative/qdeclarative.h>

class CandidateManager::Private : public QimsysCandidateManager
{
    Q_OBJECT
public:
    Private(CandidateManager *parent);

private slots:
    void slotItemsChanged(const QimsysConversionItemList &items);

private:
    CandidateManager *q;

public:
    QList<QObject*> m_items;
};

CandidateManager::Private::Private(CandidateManager *parent)
    : QimsysCandidateManager(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    init();
    connect(this, SIGNAL(itemsChanged(QimsysConversionItemList)), this, SLOT(slotItemsChanged(QimsysConversionItemList)));
    connect(this, SIGNAL(currentIndexChanged(int)), q, SIGNAL(currentIndexChanged(int)));
    qimsysDebugOut();
}

void CandidateManager::Private::slotItemsChanged(const QimsysConversionItemList &items)
{
    qimsysDebugIn() << items;
    qDeleteAll(m_items);
    m_items.clear();
    foreach (const QimsysConversionItem &item, items) {
        m_items.append(new ConversionItem(item, this));
    }
    qimsysDebug() << m_items.length();
    emit q->itemsChanged(m_items);
    qimsysDebug() << m_items.length();
    qimsysDebugOut();
}

CandidateManager::CandidateManager(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

const QList<QObject*> &CandidateManager::items() const
{
    return d->m_items;
}

void CandidateManager::setItems(const QList<QObject*> &items)
{
    QimsysConversionItemList list;
    foreach (const QObject *object, items) {
        list.append(qobject_cast<const ConversionItem*>(object)->item());
    }

    d->setItems(list);
}

int CandidateManager::currentIndex()
{
    return d->currentIndex();
}

void CandidateManager::setCurrentIndex(int currentIndex)
{
    d->setCurrentIndex(currentIndex);
}

QML_DECLARE_TYPE(CandidateManager)

#include "candidatemanager.moc"
