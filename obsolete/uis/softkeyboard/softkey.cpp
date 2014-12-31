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

#include "softkey.h"
#include "qimsysdebug.h"
#include "qimsyskeysequence.h"
#include <QPainter>
#include <QMouseEvent>

class SoftKey::Private
{
public:
    Qt::Key key;
    Qt::KeyboardModifier modifier;
    bool pressed;
    QStringList texts;
    qreal stretch;
};

SoftKey::SoftKey(const QString &line, QWidget *parent)
    : QWidget(parent)
{
    d = new Private;
    d->pressed = false;
    d->texts = line.split("\t");
    QimsysKeySequence seq(d->texts.takeFirst());
    d->key = (Qt::Key)seq[0];
    if (d->key & Qt::SHIFT) {
        d->modifier = Qt::ShiftModifier;
        d->key = (Qt::Key)0;
    } else if (d->key & Qt::CTRL) {
        d->modifier = Qt::ControlModifier;
        d->key = (Qt::Key)0;
    } else if (d->key & Qt::ALT) {
        d->modifier = Qt::AltModifier;
        d->key = (Qt::Key)0;
    } else {
        d->modifier = Qt::NoModifier;
    }

    d->stretch = 1.0;
    switch (d->texts.count()) {
    case 5:
        d->stretch = d->texts.takeLast().toDouble();
        break;
    }
}

SoftKey::~SoftKey()
{
    delete d;
}

QSize SoftKey::sizeHint() const
{
    QFontMetricsF met(font());
    return QSize(met.height() * 2.0 * d->stretch, met.height() * 1.75);
}

void SoftKey::setPressed(bool pressed)
{
    d->pressed = pressed;
    update();
}

Qt::Key SoftKey::key() const
{
    return d->key;
}

void SoftKey::mousePressEvent(QMouseEvent *e)
{
    if (d->modifier == Qt::NoModifier) {
        QWidget::mousePressEvent(e);
    } else {
        d->pressed = !d->pressed;
        emit modifierChanged(d->modifier, d->pressed);
        update();
    }
}

void SoftKey::mouseMoveEvent(QMouseEvent *e)
{
    if (d->modifier == Qt::NoModifier) {
        QWidget::mouseMoveEvent(e);
    }
}

void SoftKey::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->modifier == Qt::NoModifier) {
        QWidget::mouseReleaseEvent(e);
    }
}

void SoftKey::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    if (d->pressed) {
        p.setPen(palette().highlightedText().color());
        p.fillRect(e->rect(), palette().highlight());
    }
    QRect r = rect();
    r.setRight(r.right() - 1);
    r.setBottom(r.bottom() - 1);
    p.drawRect(r);
    if (d->texts[0].length() == 1) {
        p.drawText(0, 0, width() / 2, height() / 2, Qt::AlignCenter, d->texts[0]);
    } else if (d->texts.join("") == d->texts[0]) {
        p.drawText(rect(), Qt::AlignCenter, d->texts[0]);
    } else if (d->texts[1].isEmpty()) {
        p.drawText(0, 0, width(), height() / 2, Qt::AlignCenter, d->texts[0]);
    } else {
        p.drawText(rect(), Qt::AlignTop | Qt::AlignLeft, d->texts[0]);
    }

    if (d->texts[1].length() == 1) {
        p.drawText(width() / 2, 0, width() / 2, height() / 2, Qt::AlignCenter, d->texts[1]);
    } else if (d->texts[0].isEmpty()) {
        p.drawText(0, 0, width(), height() / 2, Qt::AlignCenter, d->texts[1]);
    } else {
        p.drawText(rect(), Qt::AlignTop | Qt::AlignRight, d->texts[1]);
    }

    if (d->texts[2].length() == 1) {
        p.drawText(0, height() / 2, width() / 2, height() / 2, Qt::AlignCenter, d->texts[2]);
    } else if (d->texts[3].isEmpty()) {
        p.drawText(0, height() / 2, width(), height() / 2, Qt::AlignCenter, d->texts[2]);
    } else {
        p.drawText(rect(), Qt::AlignBottom | Qt::AlignLeft, d->texts[2]);
    }

    if (d->texts[3].length() == 1) {
        p.drawText(width() / 2, height() / 2, width() / 2, height() / 2, Qt::AlignCenter, d->texts[3]);
    } else if (d->texts[2].isEmpty()) {
        p.drawText(0, height() / 2, width(), height() / 2, Qt::AlignCenter, d->texts[3]);
    } else {
        p.drawText(rect(), Qt::AlignBottom | Qt::AlignRight, d->texts[3]);
    }
}
