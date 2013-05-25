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

#include "inputcontext.h"

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsyskeymanager.h>
#include <qimsyskeyboardmanager.h>

#include <QApplication>
#include <QSettings>
#include <QTextCharFormat>
#include <QProcess>
#include <QTimer>

class InputContext::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputContext *parent);
    ~Private();

    void setFocusWidget(QWidget *w);
    void widgetDestroyed(QWidget *w);

public slots:
    void update();

private slots:
    void init();
    void itemChanged(const QimsysPreeditItem &item);
    void sendPreeditString();
    void sendCommitString(const QString &commitString, qulonglong target);

private:
    InputContext *q;
    QWidget *focusWidget;
    QTimer timer;

public:
    QimsysApplicationManager *applicationManager;
    QimsysInputMethodManager *inputMethodManager;
    QimsysPreeditManager *preeditManager;
    QimsysKeyManager *keyManager;
    QimsysKeyboardManager *keyboardManager;

    QimsysPreeditItem currentItem;
};

InputContext::Private::Private(InputContext *parent)
    : QObject(parent)
    , q(parent)
    , focusWidget(0)
    , applicationManager(0)
    , inputMethodManager(0)
    , preeditManager(0)
    , keyManager(0)
    , keyboardManager(0)
{
    qimsysDebugIn() << parent;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    init();
    QApplication::restoreOverrideCursor();
    qimsysDebugOut();
}

InputContext::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void InputContext::Private::init()
{
    qimsysDebugIn();
    timer.setInterval(0);
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(sendPreeditString()));
    qimsysDebugOut();
}

void InputContext::Private::setFocusWidget(QWidget *w)
{
    if (qApp->applicationFilePath() == QLatin1String("/usr/bin/qwassr")) {
        if (w && (w->inherits("QTextBrowser") || w->inherits("QListView"))) {
            w = 0;
        }
    }
    if (focusWidget == w) return;
    qimsysDebugIn() << w;
    if (w) {
        focusWidget = w;
        if (!applicationManager) {
            applicationManager = new QimsysApplicationManager(this);
            if (!applicationManager->init()) {
                applicationManager->deleteLater();
                applicationManager = 0;
            }
        }

        if (!inputMethodManager) {
            inputMethodManager = new QimsysInputMethodManager(this);
            if (!inputMethodManager->init()) {
                inputMethodManager->deleteLater();
                inputMethodManager = 0;
            }
        }

        if (!preeditManager) {
            preeditManager = new QimsysPreeditManager(this);
            if (preeditManager->init()) {
                connect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
                connect(preeditManager, SIGNAL(committed(QString, qulonglong)), this, SLOT(sendCommitString(QString, qulonglong)));
            } else {
                preeditManager->deleteLater();
                preeditManager = 0;
            }
        }

        if (!keyManager) {
            keyManager = new QimsysKeyManager(this);
            if (!keyManager->init()) {
                keyManager->deleteLater();
                keyManager = 0;
            }
        }

        if (!keyboardManager) {
            keyboardManager = new QimsysKeyboardManager(this);
            if (!keyboardManager->init()) {
                keyboardManager->deleteLater();
                keyboardManager = 0;
            }
        }

        if (applicationManager) {
            applicationManager->setWindow(w->window()->winId());
            applicationManager->setWidget(w->winId());
            applicationManager->setFocus(true);
        }
        update();
    } else {
        if (!currentItem.to.isEmpty()) {
            sendCommitString(currentItem.to.join(""), focusWidget->winId());
            currentItem = QimsysPreeditItem();
            if (preeditManager)
                preeditManager->setItem(currentItem);
        }
        if (applicationManager) {
            if (applicationManager->widget() == focusWidget->winId()) {
                applicationManager->setWindow(0);
                applicationManager->setWidget(0);
                applicationManager->setFocus(false);
            }
            applicationManager->deleteLater();
            applicationManager = 0;
        }

        if (inputMethodManager) {
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }

        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
            disconnect(preeditManager, SIGNAL(committed(QString, qulonglong)), this, SLOT(sendCommitString(QString, qulonglong)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }

        if (keyManager) {
            keyManager->deleteLater();
            keyManager = 0;
        }

        if (keyboardManager) {
            keyboardManager->setVisible(false);
            keyboardManager->deleteLater();
            keyboardManager = 0;
        }

        focusWidget = w;
    }
    qimsysDebugOut();
}

void InputContext::Private::widgetDestroyed(QWidget *w)
{
    qimsysDebugIn() << w;
    qimsysDebugOut();
}

void InputContext::Private::itemChanged(const QimsysPreeditItem &item)
{
    qimsysDebugIn() << item;
    currentItem = item;
    if (!timer.isActive()) {
        timer.start();
    }
    qimsysDebugOut();
}

void InputContext::Private::sendPreeditString()
{
    qimsysDebugIn();
    QList<QInputMethodEvent::Attribute> attrs;

    bool textFormatAdded = false;
    if (currentItem.cursor > -1) {
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, currentItem.cursor, !currentItem.selection, QVariant()));
        if (currentItem.selection != 0) {
            QTextCharFormat format;
            format.setForeground(QPalette().brush(QPalette::HighlightedText));
            format.setBackground(QPalette().brush(QPalette::Highlight));
            int start = currentItem.cursor;
            start = qMin(start, start + currentItem.selection);
            int len = qAbs(currentItem.selection);
            attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, start, len, format));
            textFormatAdded = true;
        }
    }

    if (!textFormatAdded)
    {
        QTextCharFormat format;
        format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0, currentItem.to.join("").length(), format));
    }

    QInputMethodEvent e(currentItem.to.join(""), attrs);
    q->sendEvent(e);
    update();
    qimsysDebugOut();
}

void InputContext::Private::sendCommitString(const QString &commitString, qulonglong target)
{
    if (!focusWidget || focusWidget->winId() != target) return;
    qimsysDebugIn() << commitString << target;

    {
        QList<QInputMethodEvent::Attribute> attrs;
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, 0, 1, QVariant()));
        QInputMethodEvent e(QString::null, attrs);
        e.setCommitString(commitString);
        QApplication::sendEvent(focusWidget, &e);
//  q->sendEvent( e );
    }
    metaObject()->invokeMethod(this, "update", Qt::QueuedConnection);
    qimsysDebugOut();
}

void InputContext::Private::update()
{
    qimsysDebugIn();
    QWidget *widget = q->focusWidget();
    if (widget && q->isComposing()) {
        QRect r = widget->inputMethodQuery(Qt::ImMicroFocus).toRect();
        QPoint topleft = widget->mapToGlobal(QPoint(0, 0));
        r.translate(topleft);
        if (preeditManager) {
            preeditManager->setRect(r);
            preeditManager->setFont(widget->inputMethodQuery(Qt::ImFont).value<QFont>());
            preeditManager->setCursorPosition(widget->inputMethodQuery(Qt::ImCursorPosition).toInt());
            preeditManager->setSurroundingText(widget->inputMethodQuery(Qt::ImSurroundingText).toString());
            preeditManager->setCurrentSelection(widget->inputMethodQuery(Qt::ImCurrentSelection).toString());
            preeditManager->setMaximumTextLength(widget->inputMethodQuery(Qt::ImMaximumTextLength).toInt());
        }
    }
    qimsysDebugOut();
}

InputContext::InputContext(QObject *parent)
    : QInputContext(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

InputContext::~InputContext()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool InputContext::filterEvent(const QEvent *event)
{
    qimsysDebugIn();
    bool ret = false;
    switch (event->type()) {
    case QEvent::KeyPress:
        {
            qimsysDebugIn() << event;
            const QKeyEvent *e = static_cast<const QKeyEvent*>(event);
            if (d->keyManager) {
                ret = d->keyManager->keyPress(e->text(), e->key(), e->modifiers(), e->isAutoRepeat());
                qimsysDebug() << e->text() << e->key() << e->modifiers() << e->isAutoRepeat() << ret;
            }
            qimsysDebugOut() << ret;
            break;
        }
    case QEvent::KeyRelease:
        {
            qimsysDebugIn() << event;
            const QKeyEvent *e = static_cast<const QKeyEvent*>(event);
            if (d->keyManager) {
                ret = d->keyManager->keyRelease(e->text(), e->key(), e->modifiers(), e->isAutoRepeat());
                qimsysDebug() << e->text() << e->key() << e->modifiers() << e->isAutoRepeat() << ret;
            }
            qimsysDebugOut() << ret;
            break;
        }
    case QEvent::RequestSoftwareInputPanel:
        {
            qimsysDebugIn() << event;
            if (d->keyboardManager)
                d->keyboardManager->setVisible(true);
            ret = true;
            qimsysDebugOut() << ret;
            break;
        }
    case QEvent::CloseSoftwareInputPanel:
        {
            qimsysDebugIn() << event;
            if (d->keyboardManager)
                d->keyboardManager->setVisible(false);
            ret = true;
            qimsysDebugOut() << ret;
            break;
        }
    default:
        break;
    }
    update();
    qimsysDebugOut();
    return ret;
}

QFont InputContext::font() const
{
    QFont ret;
// if( focusWidget() )
// {
//  ret = focusWidget()->inputMethodQuery( Qt::ImFont ).value<QFont>();
// }
    return ret;
}

QString InputContext::identifierName()
{
    return "qimsys";
}

bool InputContext::isComposing() const
{
    if (d && d->applicationManager) {
        return d->applicationManager->composing();
    }
    return false;
}

QString InputContext::language()
{
    return d->inputMethodManager->locale();
}

void InputContext::update()
{
    d->update();
}

void InputContext::reset()
{
    qimsysDebugIn();
    if (d->applicationManager) {
        d->applicationManager->exec(QimsysApplicationManager::Reset);
    }
    update();
    qimsysDebugOut();
}

void InputContext::setFocusWidget(QWidget *w)
{
    qimsysDebugIn() << w;
    d->setFocusWidget(w);
    QInputContext::setFocusWidget(w);
    update();
    qimsysDebugOut();
}

void InputContext::widgetDestroyed(QWidget *w)
{
    d->widgetDestroyed(w);
}

#include "inputcontext.moc"
