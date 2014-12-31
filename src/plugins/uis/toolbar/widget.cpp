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

#include "widget.h"
#include "ui_widget.h"

#include "inputmethodcontroller.h"

#include <qimsysdebug.h>

#include <qimsysapplicationmanager.h>
#include <qimsysabstractpluginobject.h>
#include <qbinding.h>

#include <QDesktopWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include <QToolButton>

#include <QStateMachine>
#include <QSignalTransition>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsBlurEffect>
#include <QEasingCurve>
#include "qboolsignal.h"

using namespace ToolBar;

class Widget::Private : private QObject
{
    Q_OBJECT
public:
    Private(QimsysAbstractPluginObject *object, Widget *parent);
    ~Private();

    void setEnter(bool enter);

public slots:
    void settingsUpdated();
    void positionChanged();

private slots:
    void init();
    void setupUi();
    void setupStateMachine();
    void aboutToHide();

    void setFocus(bool focus);

signals:
    void enterChanged(bool on);
    void focusChanged(bool on);
    void aboutToShow();
    void minimize();
    void normalize();

private:
    Widget *q;
    Ui::Widget ui;
    QStateMachine *machine;
    QimsysAbstractPluginObject *plugin;

    QimsysApplicationManager applicationManager;

    QBoolSignal *enter;
public:
    QPoint lastPos;
    QSize normalSize;
};

Widget::Private::Private(QimsysAbstractPluginObject *object, Widget *parent)
    : QObject(parent)
    , q(parent)
    , machine(0)
    , plugin(object)
{
    ui.setupUi(q);

    ui.icon->setPixmap(qApp->windowIcon().pixmap(22));

    q->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    q->setFocusPolicy(Qt::NoFocus);

    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
}

Widget::Private::~Private()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("Geometry", q->saveGeometry());
}

void Widget::Private::init()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());

    applicationManager.init();
    connect(&applicationManager, SIGNAL(focusChanged(bool)), this, SLOT(setFocus(bool)));

    setupUi();
    settingsUpdated();

    setFocus(applicationManager.focus());

    q->restoreGeometry(settings.value("Geometry", q->saveGeometry()).toByteArray());
    setupStateMachine();

    q->restoreGeometry(settings.value("Geometry", q->saveGeometry()).toByteArray());
    q->show();
    q->restoreGeometry(settings.value("Geometry", q->saveGeometry()).toByteArray());
}

void Widget::Private::setupStateMachine()
{
    qimsysDebugIn();

    if (machine) {
        machine->stop();
        machine->deleteLater();
    }
    QStateMachine *machine = new QStateMachine(this);

    QBoolSignal *focus = new QBoolSignal(this, SIGNAL(focusChanged(bool)), applicationManager.focus(), machine);
    enter = new QBoolSignal(this, SIGNAL(enterChanged(bool)), false, machine);
    QBoolSignal *composing = new QBoolSignal(&applicationManager, SIGNAL(composingChanged(bool)), applicationManager.composing(), machine);
    QBoolsSignal *combination = new QBoolsSignal(focus, composing, QBoolsSignal::And, machine);

    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect;
    ui.widget->setGraphicsEffect(effect);

    QState *parentState = new QState(QState::ParallelStates);
    machine->addState(parentState);
    machine->setInitialState(parentState);

    QState *sizeStates = new QState(parentState);
    QState *visibleStates = new QState(parentState);
    QState *activeStates = new QState(parentState);

    QState *invisibleState = new QState(visibleStates);
    QState *visibleState = new QState(visibleStates);
    QState *smallState = new QState(sizeStates);
    QState *hoverState = new QState(sizeStates);
    QState *popupState = new QState(sizeStates);
    QState *normalState = new QState(sizeStates);
    QState *activeState = new QState(activeStates);
    QState *inactiveState = new QState(activeStates);

    invisibleState->assignProperty(q, "visible", false);
    visibleState->assignProperty(q, "visible", true);

    QSize smallSize = ui.icon->size();
    smallState->setProperty("smallness", true);
    smallState->assignProperty(q, "size", smallSize);
    smallState->assignProperty(effect, "blurRadius", 5);
    hoverState->setProperty("smallness", false);
    new QBinding(q, "normalSize", hoverState, q, "size");
    hoverState->assignProperty(effect, "blurRadius", 0);
    normalState->setProperty("smallness", false);
    new QBinding(q, "normalSize", normalState, q, "size");
    normalState->assignProperty(effect, "blurRadius", 0);

    inactiveState->assignProperty(ui.icon, "enabled", false);
    activeState->assignProperty(ui.icon, "enabled", true);

    QSettings settings;

    settings.beginGroup(plugin->metaObject()->className());
    if (settings.value("Always", true).toBool()) {
        qimsysDebug();
        visibleStates->setInitialState(visibleState);
    } else if (settings.value("Available", true).toBool()) {
        qimsysDebug();
        visibleStates->setInitialState(invisibleState);
        invisibleState->addTransition(focus, SIGNAL(on()), visibleState);
        visibleState->addTransition(focus, SIGNAL(off()), invisibleState);
    } else if (settings.value("Active", true).toBool()) {
        qimsysDebug();
        visibleStates->setInitialState(invisibleState);
        invisibleState->addTransition(composing, SIGNAL(on()), visibleState);
        visibleState->addTransition(composing, SIGNAL(off()), invisibleState);
    }

#define TRANSITION(t) \
    { \
        QAbstractTransition *transition = t; \
        QSequentialAnimationGroup *seq = new QSequentialAnimationGroup; \
        QParallelAnimationGroup *para = new QParallelAnimationGroup; \
        QPropertyAnimation *animation = new QPropertyAnimation(q, "size", transition); \
        animation->setEasingCurve(QEasingCurve::InOutCirc); \
        para->addAnimation(animation); \
        animation = new QPropertyAnimation(q, "pos"); \
        animation->setEasingCurve(QEasingCurve::InOutCirc); \
        para->addAnimation(animation); \
        seq->addAnimation(para); \
        seq->addAnimation(new QPropertyAnimation(effect, "blurRadius")); \
        transition->addAnimation(seq); \
    }

    if (settings.value("Inactive", true).toBool()) {
        qimsysDebug();
        sizeStates->setInitialState(smallState);
        TRANSITION(smallState->addTransition(combination, SIGNAL(on()), normalState));
        TRANSITION(smallState->addTransition(enter, SIGNAL(on()), hoverState));
        TRANSITION(smallState->addTransition(this, SIGNAL(aboutToShow()), popupState));
        TRANSITION(hoverState->addTransition(enter, SIGNAL(off()), smallState));
        TRANSITION(hoverState->addTransition(this, SIGNAL(aboutToShow()), popupState));
        TRANSITION(popupState->addTransition(this, SIGNAL(minimize()), smallState));
        TRANSITION(popupState->addTransition(this, SIGNAL(normalize()), hoverState));
        TRANSITION(normalState->addTransition(combination, SIGNAL(off()), smallState));
    } else {
        if (settings.value("Unavailable", true).toBool()) {
            qimsysDebug();
            sizeStates->setInitialState(smallState);
            TRANSITION(smallState->addTransition(focus, SIGNAL(on()), normalState));
            TRANSITION(smallState->addTransition(enter, SIGNAL(on()), hoverState));
            TRANSITION(smallState->addTransition(this, SIGNAL(aboutToShow()), popupState));
            TRANSITION(hoverState->addTransition(enter, SIGNAL(off()), smallState));
            TRANSITION(hoverState->addTransition(this, SIGNAL(aboutToShow()), popupState));
            TRANSITION(popupState->addTransition(this, SIGNAL(minimize()), smallState));
            TRANSITION(popupState->addTransition(this, SIGNAL(normalize()), hoverState));
            TRANSITION(normalState->addTransition(focus, SIGNAL(off()), smallState));
        } else {
            qimsysDebug();
            sizeStates->setInitialState(normalState);
        }
    }
#undef TRANSITION
    activeStates->setInitialState(inactiveState);
    inactiveState->addTransition(focus, SIGNAL(on()), activeState);
    activeState->addTransition(focus, SIGNAL(off()), inactiveState);
    machine->start();
    positionChanged();
    qimsysDebugOut();
}

void Widget::Private::positionChanged()
{
    qimsysDebugIn();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("Position", q->pos());

    foreach (QState *state, findChildren<QState*>()) {
        if (state->dynamicPropertyNames().contains("smallness")) {
            QRect desktop = QApplication::desktop()->availableGeometry();
            QRect rect = q->geometry();
            if (state->property("smallness").toBool()) {
                if (q->width() > ui.icon->width()) {
                    if (rect.center().x() < desktop.center().x()) {
                        qimsysDebug() << q->pos();
                        state->assignProperty(q, "pos", q->pos());
                    } else {
                        qimsysDebug() << QPoint(q->x() + ui.widget->width(), q->y());
                        state->assignProperty(q, "pos", QPoint(q->x() + ui.widget->width(), q->y()));
                    }
                } else {
                    qimsysDebug() << q->pos();
                    state->assignProperty(q, "pos", q->pos());
                }
            } else {
                if (q->width() > ui.icon->width()) {
                    qimsysDebug() << q->pos();
                    state->assignProperty(q, "pos", q->pos());
                } else {
                    if (rect.center().x() < desktop.center().x()) {
                        qimsysDebug() << q->pos();
                        state->assignProperty(q, "pos", q->pos());
                    } else {
                        qimsysDebug() << QPoint(q->x() - ui.widget->width(), q->y());
                        state->assignProperty(q, "pos", QPoint(q->x() - ui.widget->width(), q->y()));
                    }
                }
            }
        }
    }
    qimsysDebugOut();
}

void Widget::Private::aboutToHide()
{
    if (enter->value())
        emit normalize();
    else
        QTimer::singleShot(1000, this, SIGNAL(minimize()));
}

void Widget::Private::setEnter(bool enter)
{
    emit enterChanged(enter);
}

void Widget::Private::setFocus(bool focus)
{
    emit focusChanged(focus > 0);
}

void Widget::Private::settingsUpdated()
{
    QSettings settings;
    settings.beginGroup(plugin->metaObject()->className());
    ui.widget->resize(ui.widget->sizeHint());
    q->setNormalSize(QSize(ui.widget->x() + ui.widget->width(), ui.widget->height()));
    setupStateMachine();
}

void Widget::Private::setupUi()
{
    qimsysDebugIn();
    InputMethodController *controller = new InputMethodController(this);

    foreach (QAction *parentAction, controller->actions()) {
        qimsysDebug() << parentAction->text();
        if (parentAction->text() == QLatin1String("-")) {
            QFrame *separator = new QFrame;
            separator->setFrameShape(QFrame::VLine);
            separator->setFrameShadow(QFrame::Sunken);
            separator->setLineWidth(1);
            ui.widget->layout()->addWidget(separator);
        } else {
            QToolButton *button = new QToolButton;
            button->setAutoRaise(true);
            button->setIconSize(QSize(16, 16));
            ui.widget->layout()->addWidget(button);
            new QBinding(parentAction, "icon", button, "icon");
            new QBinding(parentAction, "text", button, "tooltip");
            if (parentAction->findChild<QAction*>()){
                QMenu *menu = new QMenu(button);
                connect(menu, SIGNAL(aboutToShow()), this, SIGNAL(aboutToShow()));
                connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide()));
                foreach (QAction *action, parentAction->findChildren<QAction*>()) {
                    if (action->text() == QLatin1String("-")) {
                        menu->addSeparator();
                    } else {
                        menu->addAction(action);
                    }
                }
                button->setMenu(menu);
                button->setPopupMode(QToolButton::InstantPopup);
            } else {
                connect(button, SIGNAL(clicked()), parentAction, SLOT(trigger()));
            }
        }
    }
    ui.widget->resize(ui.widget->sizeHint());
    metaObject()->invokeMethod(this, "positionChanged", Qt::QueuedConnection);
    qimsysDebugOut();
}

Widget::Widget(QimsysAbstractPluginObject *plugin, QWidget *parent)
    : QFrame(parent)
{
    d = new Private(plugin, this);
}

Widget::~Widget()
{
    delete d;
}

const QSize &Widget::normalSize() const
{
    return d->normalSize;
}

void Widget::setNormalSize(const QSize &normalSize)
{
    if (d->normalSize == normalSize) return;
    d->normalSize = normalSize;
    emit normalSizeChanged(normalSize);
}

void Widget::settingsUpdated()
{
    d->settingsUpdated();
}

bool Widget::event(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        emit languageChanged();
    }
    return QFrame::event(e);
}

void Widget::enterEvent(QEvent *e)
{
    d->setEnter(true);
    QFrame::enterEvent(e);
}

void Widget::leaveEvent(QEvent *e)
{
    QFrame::leaveEvent(e);
    d->setEnter(false);
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    d->lastPos = e->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if (d->lastPos.isNull()) return;
    move(pos() - d->lastPos + e->pos());
}

void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    mouseMoveEvent(e);
    d->lastPos = QPoint();
    d->positionChanged();
}

#include "widget.moc"
