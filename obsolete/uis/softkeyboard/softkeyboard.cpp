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

#include "softkeyboard.h"
#include "softkey.h"
#include "qimsysdebug.h"

#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

#include "qimsysapplicationmanager.h"
#include "qimsyspreeditmanager.h"
#include "qimsyskeymanager.h"
#include "qimsyskeysequence.h"

class SoftKeyboard::Private : private QObject
{
    Q_OBJECT
public:
    Private(SoftKeyboard *parent);
    ~Private();

    void press();
    void release();

private slots:
    void focusChanged(bool focus);
    void rectChanged(const QRect &rect);
    void repeatStart();
    void repeat();
    void modifierChanged(Qt::KeyboardModifier modifier, bool on);

private:
    SoftKeyboard *q;
    QimsysApplicationManager manager;
    QimsysPreeditManager preedit;
    QTimer hideTimer;
    QTimer startRepeatTimer;
    QTimer repeatTimer;
    QimsysKeyManager input;

public:
    SoftKey *key;
    Qt::KeyboardModifiers modifiers;
};

SoftKeyboard::Private::Private(SoftKeyboard *parent)
    : QObject(parent)
    , q(parent)
    , key(0)
    , modifiers(Qt::NoModifier)
{
    q->setWindowOpacity(0.75);
    q->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    q->setFocusPolicy(Qt::NoFocus);

    manager.init();
    connect(&manager, SIGNAL(focusChanged(bool)), this, SLOT(focusChanged(bool)));

    preedit.init();
    connect(&preedit, SIGNAL(rectChanged(QRect)), this, SLOT(rectChanged(QRect)));
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    vbox->setSpacing(0);
    QHBoxLayout *hbox = 0;
    QFile file(":/layouts/jp106.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.isEmpty()) {
                vbox->addLayout(hbox);
                hbox = 0;
                continue;
            }
            if (!hbox) {
                hbox = new QHBoxLayout;
                hbox->setMargin(0);
                hbox->setSpacing(0);
            }
            SoftKey *k = new SoftKey(line);
            connect(k, SIGNAL(modifierChanged(Qt::KeyboardModifier, bool)), this, SLOT(modifierChanged(Qt::KeyboardModifier, bool)));
            hbox->addWidget(k);
        }
        file.close();
    }
    if (hbox) {
        vbox->addLayout(hbox);
    }
    q->setLayout(vbox);

    hideTimer.setSingleShot(true);
    connect(&hideTimer, SIGNAL(timeout()), q, SLOT(hide()));

    startRepeatTimer.setSingleShot(true);
    connect(&startRepeatTimer, SIGNAL(timeout()), this, SLOT(repeatStart()));
    connect(&repeatTimer, SIGNAL(timeout()), this, SLOT(repeat()));
}

SoftKeyboard::Private::~Private()
{
}

void SoftKeyboard::Private::focusChanged(bool focus)
{
    if (focus) {
        if (!q->isVisible()) {
            q->setVisible(focus);
        } else if (hideTimer.isActive()) {
            hideTimer.stop();
        }
        rectChanged(preedit.rect());
    } else {
        // delay hiding
        hideTimer.start(100);
    }
}

void SoftKeyboard::Private::rectChanged(const QRect &rect)
{
    QRect desktop = QApplication::desktop()->availableGeometry();
    if (rect.center().y() > desktop.height() / 2) {
        q->move((desktop.width() - q->width()) / 2, desktop.top());
    } else {
        q->move((desktop.width() - q->width()) / 2, desktop.bottom() - q->height());
    }
}

void SoftKeyboard::Private::press()
{
    key->setPressed(true);
    input.process(QimsysKeySequence(key->key()).toString(), key->key(), modifiers, true, false);
    startRepeatTimer.start(250);
}

void SoftKeyboard::Private::release()
{
    if (startRepeatTimer.isActive()) {
        startRepeatTimer.stop();
    }
    if (repeatTimer.isActive()) {
        repeatTimer.stop();
    }
    input.process(QimsysKeySequence(key->key()).toString(), key->key(), modifiers, false, false);
    key->setPressed(false);
    key = 0;
}

void SoftKeyboard::Private::repeatStart()
{
    repeatTimer.start(100);
}

void SoftKeyboard::Private::repeat()
{
    input.process(QimsysKeySequence(key->key()).toString(), key->key(), modifiers, true, true);
}

void SoftKeyboard::Private::modifierChanged(Qt::KeyboardModifier modifier, bool on)
{
    if (on) {
        modifiers |= modifier;
    } else {
        modifiers &= ~modifier;
    }
}

SoftKeyboard::SoftKeyboard(QObject *parent)
    : QWidget()
{
    if (parent)
        connect(parent, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    d = new Private(this);
}

SoftKeyboard::~SoftKeyboard()
{
    delete d;
}

void SoftKeyboard::mousePressEvent(QMouseEvent *e)
{
    qimsysDebugIn() << e->pos();
    d->key = qobject_cast<SoftKey*>(childAt(e->pos()));
    qimsysDebug() << d->key << childAt(e->pos());
    if (d->key) {
        d->press();
    }
    qimsysDebugOut();
}

void SoftKeyboard::mouseMoveEvent(QMouseEvent *e)
{
    SoftKey *key = qobject_cast<SoftKey*>(childAt(e->pos()));
    if (d->key == key) return;
    if (d->key && key) {
        d->release();
        d->key = key;
        d->press();
    }
}

void SoftKeyboard::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if (d->key) {
        d->release();
    }
}

#include "softkeyboard.moc"
