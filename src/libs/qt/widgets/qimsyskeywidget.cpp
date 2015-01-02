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

#include "qimsyskeywidget.h"
#include "qimsysdebug.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMenu>

class QimsysKeyWidget::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysKeyWidget *parent);
    ~Private();

    void setSequence(const QimsysKeySequence &sequence);

public slots:
    void reset();
    void clear();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    QimsysKeyWidget *q;

public:
    QimsysKeySequence original;
    QimsysKeySequence sequence;
};

QimsysKeyWidget::Private::Private(QimsysKeyWidget *parent)
    : QObject(parent)
    , q(parent)
{
    q->setText(QLatin1String(""));
    q->setFocusPolicy(Qt::ClickFocus);

    QMenu *menu = new QMenu;
    QAction *action = new QAction(this);
    action->setText(tr("&Reset"));
    connect(action, SIGNAL(triggered()), this, SLOT(reset()));
    menu->addAction(action);

    action = new QAction(this);
    action->setText(tr("&Clear"));
    connect(action, SIGNAL(triggered()), this, SLOT(clear()));
    menu->addAction(action);

    q->setMenu(menu);

    q->installEventFilter(this);
}

QimsysKeyWidget::Private::~Private()
{
}

bool QimsysKeyWidget::Private::eventFilter(QObject *object, QEvent *event)
{
    bool ret = QObject::eventFilter(object, event);
    switch (event->type()) {
    case QEvent::KeyPress: {
        q->setCheckable(true);
        q->setChecked(true);
        QKeyEvent *e = static_cast<QKeyEvent*>(event);
        int k1 = 0;
        switch (e->key()) {
        case Qt::Key_Control:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
        case Qt::Key_Shift:
        case Qt::Key_Meta:
            break;
        default:
            k1 += e->key();
            break;
        }
        if (e->modifiers() & Qt::ControlModifier) k1 += Qt::CTRL;
        if (e->modifiers() & Qt::AltModifier) k1 += Qt::ALT;
        if (e->modifiers() & Qt::ShiftModifier) k1 += Qt::SHIFT;
        if (e->modifiers() & Qt::MetaModifier) k1 += Qt::META;
        QimsysKeySequence seq(k1);
        q->setText(seq.toString());
        ret = true;
        break;
    }
    case QEvent::KeyRelease:
        if (q->isChecked()) {
            QKeyEvent *e = static_cast<QKeyEvent*>(event);
            switch (e->key()) {
            case Qt::Key_Control:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
            case Qt::Key_Shift:
            case Qt::Key_Meta:
                q->setText(sequence.toString());
                break;
            default:
                break;
            }
            q->setChecked(false);
            q->setCheckable(false);
            setSequence(QimsysKeySequence(q->text()));
        }
        ret = true;
        break;
    default:
        break;
    }
    return ret;
}

void QimsysKeyWidget::Private::setSequence(const QimsysKeySequence &seq)
{
    if (sequence == seq) return;
    sequence = seq;
    q->setText(sequence.toString());
    emit q->sequenceChanged(sequence);
}

void QimsysKeyWidget::Private::reset()
{
    setSequence(original);
}

void QimsysKeyWidget::Private::clear()
{
    setSequence(QimsysKeySequence());
}

QimsysKeyWidget::QimsysKeyWidget(QWidget *parent)
    : QToolButton(parent)
{
    d = new Private(this);
}

QimsysKeyWidget::~QimsysKeyWidget()
{
    delete d;
}

QSize QimsysKeyWidget::sizeHint() const
{
    QSize ret = QToolButton::sizeHint();
    QFontMetrics met(font());
    ret.setWidth(ret.width() - met.width(text()) + met.width(QLatin1String("Ctrl+Alt+Shift+W")));
    return ret;
}

QimsysKeySequence QimsysKeyWidget::sequence() const
{
    return QimsysKeySequence(text());
}

void QimsysKeyWidget::setSequence(const QimsysKeySequence &sequence)
{
    d->setSequence(sequence);
    d->original = sequence;
}

void QimsysKeyWidget::reset()
{
    d->reset();
}

void QimsysKeyWidget::clear()
{
    d->clear();
}

#include "qimsyskeywidget.moc"
