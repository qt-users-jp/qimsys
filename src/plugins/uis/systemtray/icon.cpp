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

#include "icon.h"

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>

#include "inputmethodcontroller.h"

#include <qbinding.h>

#include <QApplication>
#include <QPainter>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QTimeLine>

using namespace SystemTray;

class Icon::Private : private QObject
{
    Q_OBJECT
public:
    Private(Icon *parent);
    ~Private();

private slots:
    void init();
    void activated(QSystemTrayIcon::ActivationReason reason);

    void updateIcon();
    void brendIcon(qreal value);
    void finished();

private:
    Icon *q;

    QimsysApplicationManager applicationManager;

    QTimeLine timeLine;
    QIcon before;
    QIcon after;
};

Icon::Private::Private(Icon *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    connect(q, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activated(QSystemTrayIcon::ActivationReason)));

    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    qimsysDebugOut();
}

Icon::Private::~Private()
{
    if (q->contextMenu()) {
        delete q->contextMenu();
    }
}

void Icon::Private::init()
{
    applicationManager.init();
    connect(&applicationManager, SIGNAL(currentIconChanged(QIcon)), this, SLOT(updateIcon()));

    timeLine.setDuration(500);
    connect(&timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(brendIcon(qreal)));
    connect(&timeLine, SIGNAL(finished()), this, SLOT(finished()));

    QMenu *contextMenu = new QMenu;

    InputMethodController *controller = new InputMethodController(this);

    foreach (QAction *parentAction, controller->actions()) {
        if (parentAction->text() == QLatin1String("-")) {
            contextMenu->addSeparator();
        } else if (parentAction->findChild<QAction*>()){
            QMenu *menu = new QMenu;
            new QBinding(parentAction, "icon", menu, "icon");
            new QBinding(parentAction, "text", menu, "title");
            foreach (QAction *action, parentAction->findChildren<QAction*>()) {
                if (action->text() == QLatin1String("-")) {
                    menu->addSeparator();
                } else {
                    menu->addAction(action);
                }
            }
            contextMenu->addMenu(menu);
        } else {
            contextMenu->addAction(parentAction);
        }
    }

    contextMenu->addSeparator();

    // exit
    {
        QAction *action = new QAction(this);
        action->setIcon(QIcon(":/icons/close.png"));
        action->setText(tr("E&xit"));
        connect(action, SIGNAL(triggered()), qApp, SLOT(quit()), Qt::QueuedConnection);
        contextMenu->addAction(action);
    }

    q->setContextMenu(contextMenu);
    updateIcon();
    q->show();
}

void Icon::Private::activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        q->contextMenu()->popup(q->geometry().topLeft());
        break;
    default:
        break;
    }
}

void Icon::Private::updateIcon()
{
    QIcon icon = applicationManager.currentIcon();
    before = q->icon();
    after = icon;
    if (timeLine.state() == QTimeLine::Running) {
        timeLine.stop();
        timeLine.setCurrentTime(0);
    }
    timeLine.start();
    brendIcon(0.0);
}

void Icon::Private::brendIcon(qreal value)
{
    QPixmap pix(32, 32);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setOpacity(1.0 - value);
    p.drawPixmap(0, 0, before.pixmap(32, 32));
    p.setOpacity(value);
    p.drawPixmap(0, 0, after.pixmap(32, 32));
    p.end();
    q->setIcon(QIcon(pix));
}

void Icon::Private::finished()
{
    q->setIcon(after);
}

Icon::Icon(QObject *parent)
    : QSystemTrayIcon(parent)
{
    d = new Private(this);
}

Icon::~Icon()
{
    delete d;
}

#include "icon.moc"
