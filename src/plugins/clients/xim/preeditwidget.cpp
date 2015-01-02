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

#include "preeditwidget.h"

#include <qimsysdebug.h>

#include <QPainter>
#include <QTextLayout>

class PreeditWidget::Private : private QObject
{
    Q_OBJECT
public:
    Private(PreeditWidget *parent);
    ~Private();

private:
    PreeditWidget *q;

public:
    QTextLayout layout;
    QString preeditString;
    int cursorPosition;
    int selectionLength;
};

PreeditWidget::Private::Private(PreeditWidget *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    q->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    qimsysDebugOut();
}

PreeditWidget::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

PreeditWidget::PreeditWidget()
{
    qimsysDebugIn();
    d = new Private(this);
    qimsysDebugOut();
}

PreeditWidget::~PreeditWidget()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

const QString &PreeditWidget::preeditString() const
{
    return d->preeditString;
}

void PreeditWidget::sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength)
{
    qimsysDebugIn() << preeditString << cursorPosition << selectionLength;
    setVisible(!preeditString.isEmpty());
    QFontMetrics met(font());
    d->layout.clearLayout();
    d->layout.setText(preeditString);
    d->layout.beginLayout();
    QTextLine line = d->layout.createLine();
    line.setLineWidth(met.width(preeditString));
    line.setPosition(QPointF(0, 0));
    d->layout.setFont(font());
    d->layout.endLayout();
    d->preeditString = preeditString;
    d->cursorPosition = cursorPosition;
    d->selectionLength = selectionLength;
    setFixedSize(met.width(preeditString), met.height() + met.leading());
    update();
    qimsysDebugOut();
}

void PreeditWidget::paintEvent(QPaintEvent *e)
{
    qimsysDebugIn();
    Q_UNUSED(e)
    QPainter painter(this);
    QVector<QTextLayout::FormatRange> selections;
    QTextLayout::FormatRange selection;
    selection.start = d->cursorPosition;
    selection.length = d->selectionLength;
    selections.append(selection);
    d->layout.draw(&painter, QPointF(0, 0), selections);
    d->layout.drawCursor(&painter, QPointF(0, 0), d->cursorPosition);
    painter.drawText(rect(), Qt::AlignCenter, d->preeditString);
    qimsysDebugOut();
}

void PreeditWidget::reset()
{
    qimsysDebugIn();
    d->preeditString.clear();
    d->cursorPosition = 0;
    d->selectionLength = 0;
    hide();
    qimsysDebugOut();
}

#include "preeditwidget.moc"
