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

#include "qximinputmethod.h"

#include <QKeyEvent>
#include <QMap>
#include <QX11Info>

#include <qimsysdebug.h>
#include "inputcontext.h"
#include "qtx11.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <IMdkit.h>
#include <Xi18n.h>

#undef FocusIn
#undef FocusOut

class QXimInputMethod::Private : private QObject
{
    Q_OBJECT
public:
    Private(QXimInputMethod *parent);
    ~Private();

private slots:
    void init();

private:
    bool open(XIMS xims, IMOpenStruct *opts);
    bool close(XIMS xims, IMCloseStruct *opts);
    bool createIc(XIMS xims, IMChangeICStruct *opts);
    bool destroyIc(XIMS xims, IMChangeICStruct *opts);
    bool setIcValues(XIMS xims, IMChangeICStruct *opts);
    bool getIcValues(XIMS xims, IMChangeICStruct *opts);
    bool setIcFocus(XIMS xims, IMChangeFocusStruct *opts);
    bool unsetIcFocus(XIMS xims, IMChangeFocusStruct *opts);
    bool forward(XIMS xims, IMForwardEventStruct *opts);
    bool resetIc(XIMS xims, IMResetICStruct *opts);
    bool preeditStartReply(XIMS xims, IMPreeditCBStruct *opts);

    static bool protocolHandler(XIMS xims, IMProtocol *protocol);
    bool storeIcValues(InputContext *ic, IMChangeICStruct *opts);

    static Private *instance;
    QXimInputMethod *q;
    QMap<quint32, InputContext*> icMap;
public:
    XIMS xims;
    InputContext *currentIc;
};

QXimInputMethod::Private *QXimInputMethod::Private::instance = 0;

QXimInputMethod::Private::Private(QXimInputMethod *parent)
    : QObject(parent)
    , q(parent)
    , xims(0)
    , currentIc(0)
{
    qimsysDebugIn() << parent;
//    Q_ASSERT(instance == 0);
    instance = this;
    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    qimsysDebugOut();
}

QXimInputMethod::Private::~Private()
{
    qimsysDebugIn();
    if (xims) {
        IMCloseIM(xims);
    }
    qimsysDebugOut();
}

void QXimInputMethod::Private::init()
{
    qimsysDebugIn();
    XIMStyle ims_styles_onspot [] = {
        XIMPreeditCallbacks | XIMStatusNothing, // on the spot
        XIMPreeditPosition  | XIMStatusNothing, // over the spot
        XIMPreeditArea      | XIMStatusArea,    // off the spot
        XIMPreeditNothing   | XIMStatusNothing, // root
        0
    };
    XIMEncoding ims_encodings[] = {
        "COMPOUND_TEXT",
        NULL
    };

    XIMStyles styles;
    XIMEncodings encodings;

    styles.count_styles = sizeof(ims_styles_onspot) / sizeof(XIMStyle) - 1;
    styles.supported_styles = ims_styles_onspot;

    encodings.count_encodings = sizeof(ims_encodings) / sizeof(XIMEncoding) - 1;
    encodings.supported_encodings = ims_encodings;

    qimsysDebug() << q->name() << q->locale();
    xims = IMOpenIM(QX11Info::display()
                    , IMModifiers, "Xi18n"
                    , IMServerWindow, (unsigned long)q->winId()
                    , IMServerName, q->name()
                    , IMLocale, q->locale()
                    , IMServerTransport, "X/"
                    , IMInputStyles, &styles
                    , IMEncodingList, &encodings
                    , IMProtocolHandler, protocolHandler
                    , IMFilterEventMask, KeyPressMask | KeyReleaseMask
                    , NULL);
    if (!xims) {
        qFatal("IMOpenIM error");
    }
    qimsysDebugOut();
}

bool QXimInputMethod::Private::open(XIMS xims, IMOpenStruct *opts)
{
    // Nothing to do.
    Q_UNUSED(xims)
    Q_UNUSED(opts)
    return true;
}

bool QXimInputMethod::Private::close(XIMS xims, IMCloseStruct *opts)
{
    // Nothing to do.
    Q_UNUSED(xims)
    Q_UNUSED(opts)
    return true;
}

bool QXimInputMethod::Private::createIc(XIMS xims, IMChangeICStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = new InputContext(xims, this);
    currentIc->setConnectId(opts->connect_id);
    icMap[currentIc->id()] = currentIc;
    bool ret = storeIcValues(currentIc, opts);
    qimsysDebugOut() << ret;
    return ret;
}

bool QXimInputMethod::Private::destroyIc(XIMS xims, IMChangeICStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    q->updateHandler(QAbstractInputMethod::Destroyed);
    delete icMap.take(opts->icid);
    currentIc = 0;
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::setIcValues(XIMS xims, IMChangeICStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    bool ret = storeIcValues(currentIc, opts);
    qimsysDebugOut() << ret;
    return ret;
}

bool QXimInputMethod::Private::getIcValues(XIMS xims, IMChangeICStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    XICAttribute *ic_attr = opts->ic_attr;

    for (int i = 0; i < opts->ic_attr_num; i++, ic_attr++) {
        if (QString(XNFilterEvents) == ic_attr->name) {
            ic_attr->value = (void*)(new CARD32);
            *(CARD32*)ic_attr->value = KeyPressMask | KeyReleaseMask;
            ic_attr->value_length = sizeof(CARD32);
        } else {
            qimsysWarning() << ic_attr->name;
        }
    }

    XICAttribute *pre_attr = opts->preedit_attr;
    for (int i = 0; i < opts->preedit_attr_num; i++, pre_attr++) {
        if (QLatin1String(XNArea) == pre_attr->name) {
            pre_attr->value = Qt2X11::convert(currentIc->property(pre_attr->name).toRect());
            pre_attr->value_length = sizeof(XRectangle);
        } else if (QLatin1String(XNAreaNeeded) == pre_attr->name) {
            pre_attr->value = Qt2X11::convert(currentIc->property(pre_attr->name).toRect());
            pre_attr->value_length = sizeof(XRectangle);
        } else if (QLatin1String(XNSpotLocation) == pre_attr->name) {
            pre_attr->value = Qt2X11::convert(currentIc->property(pre_attr->name).toPoint());
            pre_attr->value_length = sizeof(XPoint);
        } else if (QLatin1String(XNColormap) == pre_attr->name) {
            pre_attr->value = (Colormap*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        } else if (QLatin1String(XNStdColormap) == pre_attr->name) {
            pre_attr->value = (Colormap*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        } else if (QLatin1String(XNForeground) == pre_attr->name) {
            pre_attr->value = (CARD32*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        } else if (QLatin1String(XNBackground) == pre_attr->name) {
            pre_attr->value = (CARD32*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        } else if (QLatin1String(XNBackgroundPixmap) == pre_attr->name) {
            pre_attr->value = (Pixmap*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        } else if (QLatin1String(XNFontSet) == pre_attr->name) {
            QString font = currentIc->property(pre_attr->name).toString();
            pre_attr->value = Qt2X11::convert(font);
            pre_attr->value_length = font.length();
        } else if (QLatin1String(XNLineSpace) == pre_attr->name) {
            pre_attr->value = (CARD32*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        } else if (QLatin1String(XNCursor) == pre_attr->name) {
            pre_attr->value = (Cursor*)Qt2X11::convert(currentIc->property(pre_attr->name).toULongLong());
            pre_attr->value_length = sizeof(Colormap);
        }
    }
    XICAttribute *sts_attr = opts->status_attr;
    for (int i = 0; i < opts->status_attr_num; i++, sts_attr++) {
        qimsysWarning() << sts_attr->name << "is not supported yet.";
    }
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::setIcFocus(XIMS xims, IMChangeFocusStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    currentIc->setFocus(true);
    q->updateHandler(QAbstractInputMethod::FocusIn);
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::unsetIcFocus(XIMS xims, IMChangeFocusStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    currentIc->setFocus(false);
    q->updateHandler(QAbstractInputMethod::FocusOut);
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::forward(XIMS xims, IMForwardEventStruct *opts)
{
    qimsysDebugIn();
    bool ret = false;
    currentIc = icMap[opts->icid];
    Q_ASSERT(currentIc);
    switch (opts->event.type) {
    case KeyPress:
    case KeyRelease: {
        XKeyEvent *xev = (XKeyEvent*) & opts->event;
        int key = 0;
        int modifiers = 0;
        if (xev->state & ShiftMask) modifiers |= Qt::SHIFT;
        if (xev->state & Mod1Mask) modifiers |= Qt::ALT;
        if (xev->state & ControlMask) modifiers |= Qt::CTRL;
        QChar ch = X112Qt::convert(xev, key);
        switch (key) {
        case Qt::Key_Shift:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
        case Qt::Key_Control:
            ret = true;
            break;
        default:
            ret = q->filter(ch.unicode(), key, modifiers, opts->event.type == KeyPress, false);
            break;
        }
        break;
    }
    default:
        qimsysWarning() << opts->event.type;
        break;
    }
    if (!ret) {
        IMForwardEvent(xims, (XPointer)opts);
    }
    qimsysDebugOut() << ret;
    return ret;
}

bool QXimInputMethod::Private::resetIc(XIMS xims, IMResetICStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    q->reset();
    currentIc->reset();
    q->updateHandler(QAbstractInputMethod::Reset);
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::preeditStartReply(XIMS xims, IMPreeditCBStruct *opts)
{
    qimsysDebugIn();
    Q_UNUSED(xims)
    currentIc = icMap[opts->icid];
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::storeIcValues(InputContext *ic, IMChangeICStruct *opts)
{
    qimsysDebugIn();
    XICAttribute *ic_attr = opts->ic_attr;
    for (int i = 0; i < opts->ic_attr_num; i++, ic_attr++) {
        if (QLatin1String(XNInputStyle) == ic_attr->name) {
            ic->setProperty(ic_attr->name, X112Qt::convert((INT32*)ic_attr->value));
        } else if (QLatin1String(XNClientWindow) == ic_attr->name) {
            ic->setProperty(ic_attr->name, qVariantFromValue(X112Qt::convert((Window*)ic_attr->value)));
        } else if (QLatin1String(XNFocusWindow) == ic_attr->name) {
            ic->setProperty(ic_attr->name, qVariantFromValue(X112Qt::convert((Window*)ic_attr->value)));
        } else {
            qimsysWarning() << ic_attr->name << "is not supported.";
        }
    }
    XICAttribute *pre_attr = opts->preedit_attr;
    for (int i = 0; i < opts->preedit_attr_num; i++, pre_attr++) {
        if (QLatin1String(XNArea) == pre_attr->name) {
            ic->setProperty(pre_attr->name, X112Qt::convert((XRectangle*)pre_attr->value));
        } else if (QLatin1String(XNAreaNeeded) == pre_attr->name) {
            ic->setProperty(pre_attr->name, X112Qt::convert((XRectangle*)pre_attr->value));
        } else if (QLatin1String(XNSpotLocation) == pre_attr->name) {
            ic->setProperty(pre_attr->name, X112Qt::convert((XPoint*)pre_attr->value));
        } else if (QLatin1String(XNColormap) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((Colormap*)pre_attr->value)));
        } else if (QLatin1String(XNStdColormap) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((Colormap*)pre_attr->value)));
        } else if (QLatin1String(XNForeground) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((CARD32*)pre_attr->value)));
        } else if (QLatin1String(XNBackground) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((CARD32*)pre_attr->value)));
        } else if (QLatin1String(XNBackgroundPixmap) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((Pixmap*)pre_attr->value)));
        } else if (QLatin1String(XNFontSet) == pre_attr->name) {
            ic->setProperty(pre_attr->name, X112Qt::convert((char*)pre_attr->value));
        } else if (QLatin1String(XNLineSpace) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((CARD32*)pre_attr->value)));
        } else if (QLatin1String(XNCursor) == pre_attr->name) {
            ic->setProperty(pre_attr->name, qVariantFromValue(X112Qt::convert((Cursor*)pre_attr->value)));
        }
    }
    XICAttribute *sts_attr = opts->status_attr;
    for (int i = 0; i < opts->status_attr_num; i++, sts_attr++) {
        qimsysWarning() << sts_attr->name << "is not supported yet.";
    }
    opts->icid = ic->id();
    qimsysDebugOut();
    return true;
}

bool QXimInputMethod::Private::protocolHandler(XIMS xims, IMProtocol *opts)
{
    qimsysDebugIn();
    int ret = false;
    Private *d = QXimInputMethod::Private::instance;
    switch (opts->major_code) {
    case XIM_OPEN:
        ret = d->open(xims, (IMOpenStruct*)opts);
        break;
    case XIM_CLOSE:
        ret = d->close(xims, (IMCloseStruct*)opts);
        break;
    case XIM_CREATE_IC:
        ret = d->createIc(xims, (IMChangeICStruct*)opts);
        break;
    case XIM_DESTROY_IC:
        ret = d->destroyIc(xims, (IMChangeICStruct*)opts);
        break;
    case XIM_SET_IC_VALUES:
        ret = d->setIcValues(xims, (IMChangeICStruct*)opts);
        break;
    case XIM_GET_IC_VALUES:
        ret = d->getIcValues(xims, (IMChangeICStruct*)opts);
        break;
    case XIM_SET_IC_FOCUS:
        ret = d->setIcFocus(xims, (IMChangeFocusStruct*)opts);
        break;
    case XIM_UNSET_IC_FOCUS:
        ret = d->unsetIcFocus(xims, (IMChangeFocusStruct*)opts);
        break;
    case XIM_FORWARD_EVENT:
        ret = d->forward(xims, (IMForwardEventStruct*)opts);
        break;
    case XIM_RESET_IC:
        ret = d->resetIc(xims, (IMResetICStruct*)opts);
        break;
    case XIM_PREEDIT_START_REPLY:
        ret = d->preeditStartReply(xims, (IMPreeditCBStruct*)opts);
        break;
    default:
        qimsysWarning() << opts->major_code << " is not supported yet.";
        break;
    }
    qimsysDebugOut() << ret;
    return ret;
}

QXimInputMethod::QXimInputMethod(QObject *parent)
    : QAbstractInputMethod(parent)
{
    qimsysDebugIn();
    d = new Private(this);
    qimsysDebugOut();
}

QXimInputMethod::~QXimInputMethod()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

void QXimInputMethod::reset()
{
}

void QXimInputMethod::sendCommitString(const QString &commitString)
{
    if (!d->currentIc) return;
    if (!d->currentIc->hasFocus()) return;
    qimsysDebugIn() << commitString;
    d->currentIc->sendCommitString(commitString);
    qimsysDebugOut();
}

void QXimInputMethod::sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength)
{
    if (!d->currentIc) return;
    if (!d->currentIc->hasFocus()) return;
    qimsysDebugIn() << preeditString << cursorPosition << selectionLength;
    d->currentIc->sendPreeditString(preeditString, cursorPosition, selectionLength);
    qimsysDebugOut();
}

qulonglong QXimInputMethod::clientWindow() const
{
    return d->currentIc ? d->currentIc->clientWindow() : 0;
}

qulonglong QXimInputMethod::focusWindow() const
{
    return d->currentIc ? d->currentIc->focusWindow() : 0;
}

void QXimInputMethod::updateHandler(UpdateType type)
{
    Q_UNUSED(type)
}

#include "qximinputmethod.moc"
