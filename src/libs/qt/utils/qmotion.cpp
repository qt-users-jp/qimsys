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

#include "qmotion.h"

#include <math.h>

// qt headers
#include <QTimer>

#define UPDATE_SPAN 100

class QMotion::Private : private QObject
{
    Q_OBJECT
public:
    Private(QMotion *parent)
        : QObject(parent)
        , q(parent)
        , value(0)
        , minimum(0)
        , maximum(100)
        , target(0)
        , velocity(0.0f)
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    ~Private() {
    }

    void setTarget(int t) {
        if (target == t) return;
        target = t;
        if (!timer.isActive()) {
            velocity = 0.0f;
            timer.start(UPDATE_SPAN);
        }
    }

    void setValue(int v) {
        if (value == v) return;
        value = v;
        target = value;
        emit q->valueChanged(v);
    }

private slots:
    void timeout() {
        static qreal t = (qreal)UPDATE_SPAN / 1000.0f;
        qreal a = ((qreal)target - value) * 40.0f - velocity * 10.0f * (qreal)(UPDATE_SPAN / 100);
        qreal v = velocity + a * t;
        qreal dv = (velocity + a * t / 2.0f) * t;
        velocity = v;
        value += dv;
        if (fabs(v) < 10.0f && fabs((qreal)target - value) < 1.0f) {
            value = target;
            timer.stop();
            emit q->valueChanged((int)value);
            emit q->finished();
        } else {
            emit q->valueChanged((int)value);
        }
    }

private:
    QMotion *q;

public:
    qreal value;
    int minimum;
    int maximum;

private:
    QTimer timer;
    int target;
    qreal velocity;
};


QMotion::QMotion(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

QMotion::~QMotion()
{
    delete d;
}

int QMotion::value() const
{
    return (int)d->value;
}

int QMotion::minimum() const
{
    return d->minimum;
}

int QMotion::maximum() const
{
    return d->maximum;
}

void QMotion::setTarget(int target)
{
    d->setTarget(target);
}

void QMotion::setValue(int value)
{
    d->setValue(value);
}

void QMotion::setMinimum(int minimum)
{
    d->minimum = minimum;
    d->setTarget(qMax(value(), minimum));
}

void QMotion::setMaximum(int maximum)
{
    d->maximum = maximum;
    d->setTarget(qMin(value(), maximum));
}

#include "qmotion.moc"
