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

#include "inputcontext.h"

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsyskeymanager.h>
#include <qimsyskeyboardmanager.h>

#include <QCursor>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QPointer>
#include <QSettings>
#include <QTextCharFormat>
#include <QProcess>
#include <QTimer>

#include <QWindow>
#include <QWidget>
#include <QQuickItem>
#include <QQuickWindow>

class InputContext::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputContext *parent);
    ~Private();

    void setFocusObject(QObject *o);

public slots:
    void update(Qt::InputMethodQueries queries = Qt::ImQueryInput);
    void commit();

private slots:
    void init();
    void itemChanged(const QimsysPreeditItem &item);
    void sendPreeditString();
    void sendCommitString(const QString &commitString, qulonglong target);

private:
    InputContext *q;
    QPointer<QObject> focusObject;
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
    , applicationManager(0)
    , inputMethodManager(0)
    , preeditManager(0)
    , keyManager(0)
    , keyboardManager(0)
{
    qimsysDebugIn() << parent;
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    init();
    QGuiApplication::restoreOverrideCursor();
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

static void widgetAndWindowFor(const QObject *object, qulonglong *widgetId, qulonglong *windowId)
{
    qimsysDebugIn() << object;

    *widgetId = (qulonglong)object;

    qimsysDebug();
    if (windowId) {
        QWindow *window = 0;
        const QWidget *widget = qobject_cast<const QWidget *>(object);
        const QQuickItem *item = qobject_cast<const QQuickItem *>(object);
        if (widget) {
            window = widget->windowHandle();
        } else if (item) {
            window = item->window();
        }

        if (window) {
            *windowId = window->winId();
            qimsysDebug() << *windowId;
        }
    }

    qimsysDebugOut() << *widgetId;
}

void InputContext::Private::setFocusObject(QObject *o)
{
    if (focusObject == o) return;
    qimsysDebugIn() << o;

    qulonglong nextWindow = 0;
    qulonglong nextWidget = 0;
    widgetAndWindowFor(o, &nextWidget, &nextWindow);

    if (o) {
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
            applicationManager->setWindow(nextWindow);
            applicationManager->setWidget(nextWidget);
            applicationManager->setFocus(true);
        }
        //qimsysDebugOn();
        //update();
        //qimsysDebugOff();
    } else {
        if (applicationManager) {
            if (applicationManager->widget() == (qulonglong)focusObject.data()) {
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
    }
    focusObject = o;
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
    if (!focusObject) return;
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
    QCoreApplication::sendEvent(focusObject, &e);
    //update();
    qimsysDebugOut();
}

void InputContext::Private::sendCommitString(const QString &commitString, qulonglong target)
{
    if (!focusObject || (qulonglong)focusObject.data() != target) return;
    qimsysDebugIn() << commitString << target;

    {
        QList<QInputMethodEvent::Attribute> attrs;
        attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, 0, 1, QVariant()));
        QInputMethodEvent e(QString::null, attrs);
        e.setCommitString(commitString);
        QCoreApplication::sendEvent(focusObject, &e);
    }
    //metaObject()->invokeMethod(this, "update", Qt::QueuedConnection);
    qimsysDebugOut();
}

void InputContext::Private::update(Qt::InputMethodQueries queries)
{
    if (!focusObject) return;
    qimsysDebugIn();
    QInputMethodQueryEvent query(queries);
    QCoreApplication::sendEvent(focusObject, &query);
    if (queries & Qt::ImEnabled) {
        if (!query.value(Qt::ImEnabled).toBool()) {
            setFocusObject(0);
        }
    }

    if (preeditManager && queries & Qt::ImCursorRectangle) {
        QRect r = query.value(Qt::ImCursorRectangle).toRect();
        qimsysDebug() << r;
        QPoint topLeft;
        if (QWidget *widget = qobject_cast<QWidget *>(focusObject)) {
            topLeft = widget->mapToGlobal(QPoint(0, 0));
        } else if (QQuickItem *item = qobject_cast<QQuickItem *>(focusObject)) {
            topLeft = item->mapToScene(QPointF(0, 0)).toPoint() + item->window()->geometry().topLeft();
        }
        r.translate(topLeft);
        preeditManager->setRect(r);
    }
    if (preeditManager && queries & Qt::ImFont) {
        preeditManager->setFont(query.value(Qt::ImFont).value<QFont>());
    }
    if (preeditManager && queries & Qt::ImCursorPosition) {
        preeditManager->setCursorPosition(query.value(Qt::ImCursorPosition).toInt());
    }
    if (preeditManager && queries & Qt::ImSurroundingText) {
        preeditManager->setSurroundingText(query.value(Qt::ImSurroundingText).toString());
    }
    if (preeditManager && queries & Qt::ImCurrentSelection) {
        preeditManager->setCurrentSelection(query.value(Qt::ImCurrentSelection).toString());
    }
    if (preeditManager && queries & Qt::ImMaximumTextLength) {
        preeditManager->setMaximumTextLength(query.value(Qt::ImMaximumTextLength).toInt());
    }
    qimsysDebugOut();
}

void InputContext::Private::commit()
{
    if (currentItem.to.isEmpty()) return;
    qimsysDebugIn();
    sendCommitString(currentItem.to.join(""), (qulonglong)focusObject.data());
    currentItem = QimsysPreeditItem();
    if (preeditManager)
        preeditManager->setItem(currentItem);
    qimsysDebugOut();
}

InputContext::InputContext()
    : QPlatformInputContext()
{
    qimsysDebugIn();
    d = new Private(this);
    qimsysDebugOut();
}

InputContext::~InputContext()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

bool InputContext::isValid() const
{
    qimsysDebugIn();
    qimsysDebugOut();
    return true;
}

void InputContext::reset()
{
    qimsysDebugIn();
    if (d->applicationManager) {
        d->applicationManager->exec(QimsysApplicationManager::Reset);
    }
    update(Qt::ImQueryAll);
    qimsysDebugOut();
}

void InputContext::commit()
{
    qimsysDebugIn();
    d->commit();
    qimsysDebugOut();
}

void InputContext::update(Qt::InputMethodQueries queries)
{
    qimsysDebugIn();
    d->update(queries);
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
    d->update();
    qimsysDebugOut();
    return ret;
}

void InputContext::setFocusObject(QObject *object)
{
    qimsysDebugIn() << object;
    d->setFocusObject(object);
    QPlatformInputContext::setFocusObject(object);
    d->update();
    qimsysDebugOut() << object;
}

#include "inputcontext.moc"
