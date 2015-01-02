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

#include "qimsyswidget.h"
#include <QPainter>
#include <QTimer>
#include <math.h>

class QimsysWidget::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysWidget *parent);
    void paint(QPainter &painter);

private slots:
    void update();

private:
    QimsysWidget *q;
    QTimer timer;
    int angle;
};

QimsysWidget::Private::Private(QimsysWidget *parent)
    : QObject(parent)
    , q(parent)
    , angle(0)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(100);
}

void QimsysWidget::Private::update()
{
    angle = (angle + 5) % 360;
    if (q->isVisible()) {
        q->update();
    }
}

void QimsysWidget::Private::paint(QPainter &painter)
{
    //! \todo don't paint everytime
    painter.save();
    qreal w = painter.device()->width();
    qreal h = painter.device()->height();
    qreal min = qMin(w, h)  *1.0;
    qreal penWidth = min / 20.0;
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(w / 2.0, h / 2.0);
    painter.scale(0.9, 0.9);
    painter.rotate(-45);

    {
        QLinearGradient grad(-min / 15, 0, min / 15, 0);
        grad.setColorAt(0.0, Qt::darkGray);
        grad.setColorAt(1.0, Qt::black);
        painter.setPen(QPen(Qt::black, penWidth));
        painter.setBrush(grad);
        painter.drawRoundedRect(-min / 7.5, 0, min / 7.5, min  *7 / 12, min / 20, min / 20);
    }
    painter.rotate(0);

    painter.save();
    QList<QColor> wing;
    wing << QColor(255, 0, 0);
    wing << QColor(255, 255, 0);
    wing << QColor(0, 255, 0);
    wing << QColor(0, 255, 255);
    wing << QColor(0, 0, 255);
    wing << QColor(255, 0, 255);
    painter.rotate(angle);
    foreach(const QColor &c, wing) {
        painter.save();
        {
            QPainterPath clip;
            clip.moveTo(0.0, 0.0);
            clip.arcTo(-min, -min, min  *2.0, min  *2.0, 0.0, -365.0 / wing.count());
            clip.closeSubpath();
            painter.setClipPath(clip);
        }
        QPen pen(c.darker(255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        QPainterPath path;
        path.moveTo(penWidth / 2, penWidth / 2);
        path.lineTo(min / 2, penWidth / 2);
        path.cubicTo(min / 4, min / 3, min / 5, min / 3, penWidth / 2, penWidth / 2);
        {
            QLinearGradient grad(0, 0, 0, path.boundingRect().height());
            grad.setColorAt(0.0, c.darker(128));
            grad.setColorAt(1.0, c.darker(255));
            painter.setBrush(grad);
            painter.drawRect(penWidth / 2.0, penWidth / 2.0, min / 4.0 - penWidth, path.boundingRect().height());
        }
        {
            QLinearGradient grad(min / 4, 0, 0, path.boundingRect().height());
            grad.setColorAt(0.0, c);
            grad.setColorAt(1.0, c.darker());
            painter.setBrush(grad);
            painter.drawPath(path);
            painter.drawLine(penWidth / 2.0, path.boundingRect().height() + penWidth / 2.0, min / 5.0, path.boundingRect().height() + penWidth / 2.0);
        }
        painter.restore();
        painter.rotate(360 / wing.count());
    }
    painter.restore();
    painter.rotate(225);
    {
        QRadialGradient grad(0, 0, min / 10, min / 10, min / 15);
        grad.setColorAt(0.0, Qt::white);
        grad.setColorAt(1.0, Qt::black);
        painter.setPen(QPen(Qt::black, penWidth));
        painter.setBrush(grad);
        painter.drawEllipse(QPointF(0, 0), min / 10, min / 10);
    }
    painter.restore();
}

QimsysWidget::QimsysWidget(QWidget *parent)
    : QWidget(parent)
{
    d = new Private(this);
}

QimsysWidget::~QimsysWidget()
{
    delete d;
}

QSize QimsysWidget::sizeHint() const
{
    return QSize(128, 128);
}

void QimsysWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    d->paint(p);
}

#include "qimsyswidget.moc"
