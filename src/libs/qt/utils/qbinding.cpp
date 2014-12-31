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

#include "qbinding.h"

#include "qimsysdebug.h"

#include <QMetaProperty>
#include <QStateMachine>

class QBinding::Private
{
public:
    const QObject *sourceObject;
    const char *sourcePropertyName;
    QState *state;
    QObject *targetObject;
    const char *targetPropertyName;
};

QBinding::QBinding(const QObject *sourceObject, const char *sourcePropertyName, QObject *targetObject, const char *targetPropertyName)
{
    qimsysDebugIn() << sourceObject << sourcePropertyName << targetObject << targetPropertyName;
    d = new Private;
    if (!sourceObject || !sourcePropertyName || !targetObject || !targetPropertyName) {
        qimsysWarning() << sourceObject << sourcePropertyName << targetObject << targetPropertyName;
        deleteLater();
        qimsysDebugOut();
        return;
    }
    d->sourceObject = sourceObject;
    d->sourcePropertyName = sourcePropertyName;
    d->state = 0;
    d->targetObject = targetObject;
    d->targetPropertyName = targetPropertyName;

    const QMetaObject *sourceMetaObject = sourceObject->metaObject();
    int sourcePropertyIndex = sourceMetaObject->indexOfProperty(sourcePropertyName);
    QMetaProperty sourceProperty = d->sourceObject->metaObject()->property(sourcePropertyIndex);
    if (sourceProperty.hasNotifySignal()) {
        connect(sourceObject, QByteArray("2") + sourceProperty.notifySignal().methodSignature(), this, SLOT(setProperty()));
    } else {
        qWarning() << sourceObject <<  "doesn't notify change for" << sourcePropertyName;
        deleteLater();
    }

    connect(sourceObject, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(targetObject, SIGNAL(destroyed()), this, SLOT(deleteLater()));

    setProperty();
    qimsysDebugOut();
}

QBinding::QBinding(const QObject *sourceObject, const char *sourcePropertyName, QState *state, QObject *targetObject, const char *targetPropertyName)
{
    qimsysDebugIn() << sourceObject << sourcePropertyName << state << targetObject << targetPropertyName;
    d = new Private;
    if (!sourceObject || !sourcePropertyName || !targetObject || !targetPropertyName) {
        qWarning() << sourceObject << sourcePropertyName << targetObject << targetPropertyName;
        deleteLater();
        qimsysDebugOut();
        return;
    }
    d->sourceObject = sourceObject;
    d->sourcePropertyName = sourcePropertyName;
    d->state = state;
    d->targetObject = targetObject;
    d->targetPropertyName = targetPropertyName;

    const QMetaObject *sourceMetaObject = sourceObject->metaObject();
    int sourcePropertyIndex = sourceMetaObject->indexOfProperty(sourcePropertyName);
    QMetaProperty sourceProperty = d->sourceObject->metaObject()->property(sourcePropertyIndex);
    if (sourceProperty.hasNotifySignal()) {
        connect(sourceObject, QByteArray("2") + sourceProperty.notifySignal().methodSignature(), this, SLOT(setProperty()));
    } else {
        qWarning() << sourceObject <<  "doesn't notify change for" << sourcePropertyName;
        deleteLater();
    }

    connect(sourceObject, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(state, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(targetObject, SIGNAL(destroyed()), this, SLOT(deleteLater()));

    setProperty();
    qimsysDebugOut();
}

QBinding::~QBinding()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

void QBinding::setProperty()
{
    qimsysDebugIn();
    if (d->state) {
        d->state->assignProperty(d->targetObject, d->targetPropertyName, d->sourceObject->property(d->sourcePropertyName));
        if (d->state->machine()->configuration().contains(d->state)) {
            d->targetObject->setProperty(d->targetPropertyName, d->sourceObject->property(d->sourcePropertyName));
        }
    } else {
        d->targetObject->setProperty(d->targetPropertyName, d->sourceObject->property(d->sourcePropertyName));
    }
    qimsysDebugOut();
}
