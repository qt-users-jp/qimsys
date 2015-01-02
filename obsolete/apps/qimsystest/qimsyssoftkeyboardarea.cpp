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

#include "qimsyssoftkeyboardarea.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QScrollArea>

namespace Qimsys{
    enum {
        ShowSoftwareInputPanel = QEvent::User + 1234,
        HideSoftwareInputPanel,
        SoftwareInputPanelShown,
        SoftwareInputPanelHidden
    };
}

QimsysSoftKeyboardArea::QimsysSoftKeyboardArea(QWidget *parent)
    : QWidget(parent)
    , _visible(false)
    , _buddy(0)
{
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(resetGeometry()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(resetGeometry()));
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
    qApp->installEventFilter(this);
    focusChanged(0, qApp->focusWidget());
}

void QimsysSoftKeyboardArea::setBuddy(QScrollArea *buddy)
{
    if (_buddy == buddy) return;
    _buddy = buddy;
 }

bool QimsysSoftKeyboardArea::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case Qimsys::SoftwareInputPanelShown:
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "Qimsys::SoftwareInputPanelShown";
        _visible = true;
        show();
        QMetaObject::invokeMethod(this, "ensureWidgetVisible", Qt::QueuedConnection);
        return true;
    case Qimsys::SoftwareInputPanelHidden:
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "Qimsys::SoftwareInputPanelHidden";
        _visible = false;
        hide();
        return true;
    default:
        break;
    }
    return QWidget::eventFilter(o, e);
}

void QimsysSoftKeyboardArea::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
//    QEvent event((QEvent::Type)Qimsys::ShowSoftwareInputPanel);
//    QApplication::sendEvent(QApplication::instance(), &event);
}

void QimsysSoftKeyboardArea::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
//    QEvent event((QEvent::Type)Qimsys::HideSoftwareInputPanel);
//    QApplication::sendEvent(QApplication::instance(), &event);
}

void QimsysSoftKeyboardArea::resetGeometry()
{
    QRect desktop = QApplication::desktop()->screenGeometry();
    desktop.setTop(desktop.top() + desktop.height() / 2);
    setFixedSize(desktop.size());
}

void QimsysSoftKeyboardArea::focusChanged(QWidget *old, QWidget *now)
{
    if (old) {
//        old->removeEventFilter(this);
    }

    if (now) {
//        now->installEventFilter(this);
        if (_visible) show();
    } else {
        hide();
    }
}

void QimsysSoftKeyboardArea::ensureWidgetVisible()
{
    if (_buddy) {
        _buddy->ensureWidgetVisible(QApplication::focusWidget());
    }
}
