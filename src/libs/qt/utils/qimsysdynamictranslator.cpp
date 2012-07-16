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

#include "qimsysdynamictranslator.h"
#include "qimsysapplicationmanager.h"
#include "qimsysdebug.h"

#include <QHash>
#include <QPair>
#include <QCoreApplication>

class QimsysDynamicTranslator::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysDynamicTranslator *parent);
    void setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName);

private slots:
    void reset();
    void objectDestroyed();

private:
    QimsysDynamicTranslator *q;
    QimsysApplicationManager applicationManager;

    struct QimsysDynamicTranslatorData {
        QObject *owner;
        const char *source;
        QObject *target;
        const char *propertyName;
    };

    QList<QimsysDynamicTranslatorData> data;
};

QimsysDynamicTranslator::Private::Private(QimsysDynamicTranslator *parent)
    : QObject(parent)
    , q(parent)
{
    applicationManager.init();
    connect(&applicationManager, SIGNAL(displayLanguageChanged(QString)), this, SLOT(reset()), Qt::QueuedConnection);
}

void QimsysDynamicTranslator::Private::setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName)
{
    QimsysDynamicTranslatorData d;
    d.owner = owner;
    connect(owner, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
    d.source = source;
    d.target = target;
    connect(target, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
    d.propertyName = propertyName;
    data.append(d);
    target->setProperty(propertyName, QCoreApplication::translate(owner->metaObject()->className(), source));
}

void QimsysDynamicTranslator::Private::reset()
{
    qimsysDebugIn();

    foreach (const QimsysDynamicTranslatorData &d, data) {
        d.target->setProperty(d.propertyName, QCoreApplication::translate(d.owner->metaObject()->className(), d.source));
    }

    qimsysDebugOut();
}

void QimsysDynamicTranslator::Private::objectDestroyed()
{

}

QimsysDynamicTranslator::QimsysDynamicTranslator(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

QimsysDynamicTranslator &QimsysDynamicTranslator::instance()
{
    static QimsysDynamicTranslator ret;
    return ret;
}

QimsysDynamicTranslator::~QimsysDynamicTranslator()
{
    delete d;
}

void QimsysDynamicTranslator::setTranslation(QObject *owner, const char *source, QObject *target, const char *propertyName)
{
    instance().d->setTranslation(owner, source, target, propertyName);
}

#include "qimsysdynamictranslator.moc"
