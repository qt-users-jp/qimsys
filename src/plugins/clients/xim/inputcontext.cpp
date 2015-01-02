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
#include "preeditwidget.h"

#include <qimsyspreeditmanager.h>
#include <qimsysdebug.h>

#include <QX11Info>

#include <X11/X.h>
#include <X11/Xmd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "IMdkit.h"
#include "Xi18n.h"

class InputContext::Private : private QObject
{
    Q_OBJECT
public:
    Private(InputContext *parent);
    ~Private();

    void sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength = 0);
    void setPreeditStringVisible(bool visible);
    void drawPreeditString(const QString &preeditString, int cursorPosition, int selectionLength = 0);

public slots:
    void update();

private slots:
    void rectChanged(const QRect &rect);
    void fontChanged(const QFont &font);

private:
    InputContext *q;
    bool preeditStringVisible;

public:
    XIMS xims;
    PreeditWidget *preeditWidget;
    unsigned long id;
    quint16 connectId;
    qint32 inputStyle;
    unsigned long clientWindow;
    unsigned long focusWindow;
    QRect area;
    QRect areaNeeded;
    QPoint spotLocation;
    unsigned long colorMap;
    unsigned long stdColorMap;
    unsigned long foreground;
    unsigned long background;
    unsigned long backgroundPixmap;
    QString fontSet;
    unsigned long lineSpace;
    unsigned long cursor;

    bool focus;
    Qt::KeyboardModifiers modifiers;
    QimsysPreeditManager preeditManager;
    QString preeditString;
};

InputContext::Private::Private(InputContext *parent)
    : QObject(parent)
    , q(parent)
    , preeditStringVisible(false)
    , preeditWidget(0)
    , connectId(0)
    , inputStyle(0)
    , clientWindow(0)
    , focusWindow(0)
    , area()
    , areaNeeded()
    , spotLocation(-1, -1)
    , colorMap(0)
    , stdColorMap(0)
    , foreground(0)
    , background(0)
    , backgroundPixmap(0)
    , fontSet()
    , lineSpace(0)
    , cursor(0)
    , focus(false)
    , modifiers(Qt::NoModifier)
{
    qimsysDebugIn() << parent;
    static unsigned long counter = 0;
    id = ++counter;
    preeditManager.init();
    connect(&preeditManager, SIGNAL(rectChanged(QRect)), this, SLOT(rectChanged(QRect)));
    connect(&preeditManager, SIGNAL(fontChanged(QFont)), this, SLOT(fontChanged(QFont)));
    qimsysDebugOut();
}

InputContext::Private::~Private()
{
    qimsysDebugIn();
    delete preeditWidget;
    qimsysDebugOut();
}

void InputContext::Private::update()
{
    qimsysDebugIn();
    QFont f;
    if (!fontSet.isEmpty()) {
//   f.setRawMode( true );
        f.setRawName(fontSet);
    }
    QFontMetrics met(f);
    Window window = focusWindow ? focusWindow : clientWindow;
    int x = 0;
    int y = 0;
    forever {
        Window child = 0;
        bool ret = XTranslateCoordinates(QX11Info::display(), window, QX11Info::appRootWindow(), 0, 0, &x, &y, &child);
        if (ret) {
            break;
        } else if (!child) {
            qimsysWarning();
            break;
        } else {
            window = child;
            x = 0;
            y = 0;
            child = 0;
        }
    }
    QRect r;
    if (inputStyle & XIMPreeditCallbacks) {
        r.setLeft(x);
        r.setTop(y);
        XWindowAttributes attr;
        Status status = XGetWindowAttributes(QX11Info::display(), window, &attr);
        if (status) {
            r.setWidth(attr.width);
            r.setHeight(attr.height);
        }
    } else if (inputStyle & XIMPreeditPosition) {
        r.setLeft(spotLocation.x() + x);
        r.setTop(spotLocation.y() + y - met.ascent());
        r.setWidth(met.width(preeditString));
        r.setHeight(met.height() + met.leading());
    }
    preeditManager.setRect(r);
    qimsysDebugOut();
}

void InputContext::Private::rectChanged(const QRect &rect)
{
    qimsysDebugIn() << rect;
    qimsysDebug() << preeditWidget;
    if (preeditWidget) {
        preeditWidget->setGeometry(rect);
    }
    qimsysDebugOut();
}

void InputContext::Private::fontChanged(const QFont &font)
{
    qimsysDebugIn() << font;
    if (preeditWidget) {
        preeditWidget->setFont(font);
    }
    qimsysDebugOut();
}

InputContext::InputContext(XIMS xims, QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    d->xims = xims;
    qimsysDebugOut();
}

InputContext::~InputContext()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

void InputContext::sendCommitString(const QString &commitString)
{
    qimsysDebugIn() << commitString;
    IMCommitStruct opts;
    opts.major_code = XIM_COMMIT;
    opts.minor_code = 0;
    opts.icid = id();
    opts.connect_id = connectId();
    opts.flag = XimLookupChars;
    QByteArray utf8 = commitString.toUtf8();
    char *str = utf8.data();
    XTextProperty tp;
    Xutf8TextListToTextProperty(QX11Info::display(), (char **)&str, 1, XCompoundTextStyle, &tp);
    opts.commit_string = (char*)tp.value;
    IMCommitString(d->xims, (XPointer)&opts);
    XFree(tp.value);
    qimsysDebugOut();
}

void InputContext::sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength)
{
    qimsysDebugIn() << preeditString << cursorPosition << selectionLength;
    d->preeditString = preeditString;
    d->update();
    if (d->inputStyle & XIMPreeditCallbacks) {
        d->sendPreeditString(preeditString, cursorPosition, selectionLength);
    } else {
        if (!d->preeditWidget) {
            d->preeditWidget = new PreeditWidget;
            QFont f;
            if (!d->fontSet.isEmpty()) {
//    f.setRawMode( true );
                f.setRawName(d->fontSet);
            }
            f.setUnderline(true);
            d->preeditWidget->setFont(f);
        }
        d->preeditWidget->sendPreeditString(preeditString, cursorPosition, selectionLength);
    }
    qimsysDebugOut();
}

void InputContext::Private::sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength)
{
    qimsysDebugIn() << preeditString << cursorPosition << selectionLength;
    if (preeditStringVisible) {
        if (preeditString.isEmpty()) {
            drawPreeditString(preeditString, cursorPosition, selectionLength);
            setPreeditStringVisible(false);
        } else {
            drawPreeditString(preeditString, cursorPosition, selectionLength);
        }
    } else {
        if (preeditString.isEmpty()) {
            // nothing to do.
        } else {
            setPreeditStringVisible(true);
            drawPreeditString(preeditString, cursorPosition, selectionLength);
        }
    }
    qimsysDebugOut();
}

void InputContext::Private::setPreeditStringVisible(bool visible)
{
    qimsysDebugIn() << visible;
    IMPreeditCBStruct opts;
    if (visible) {
        opts.major_code = XIM_PREEDIT_START;
    } else {
        opts.major_code = XIM_PREEDIT_DONE;
    }
    opts.minor_code = 0;
    opts.connect_id = connectId;
    opts.icid = id;
    opts.todo.return_value = 0;
    IMCallCallback(xims, (XPointer)&opts);
    preeditStringVisible = visible;
    qimsysDebugOut();
}

void InputContext::Private::drawPreeditString(const QString &preeditString, int cursorPosition, int selectionLength)
{
    qimsysDebugIn() << preeditString << cursorPosition << selectionLength;
    static int changeLength = 0;
    int length = preeditString.length();
    IMPreeditCBStruct opts;
    opts.major_code = XIM_PREEDIT_DRAW;
    opts.minor_code = 0;
    opts.connect_id = connectId;
    opts.icid = id;
    opts.todo.draw.caret = cursorPosition;
    opts.todo.draw.chg_first = 0;
    opts.todo.draw.chg_length = changeLength;
    changeLength = length;

    XIMText text;
    text.encoding_is_wchar = 0;
    QByteArray utf8 = preeditString.toUtf8();
    char *str = utf8.data();
    XTextProperty tp;
    Xutf8TextListToTextProperty(QX11Info::display(), (char **)&str, 1, XCompoundTextStyle, &tp);
    text.length = strlen((char*)tp.value);
    text.string.multi_byte = (char*)tp.value;

    XIMFeedback feedback[length+1];
    int min = qMin(cursorPosition, cursorPosition + selectionLength);
    int max = qMax(cursorPosition, cursorPosition + selectionLength) - 1;
    for (int i = 0; i < length; i++) {
        if (selectionLength != 0 && qBound(min, i, max) == i) {
            feedback[i] |= XIMReverse;
        } else {
            feedback[i] = XIMUnderline;
        }
    }
    feedback[length] = 0;
    text.feedback = feedback;
    opts.todo.draw.text = &text;
    IMCallCallback(xims, (XPointer)&opts);
    XFree(tp.value);
    qimsysDebugOut();
}

unsigned long InputContext::id() const
{
    return d->id;
}

quint16 InputContext::connectId() const
{
    return d->connectId;
}

void InputContext::setConnectId(quint16 connectId)
{
    if (d->connectId == connectId) return;
    d->connectId = connectId;
}

qint32 InputContext::inputStyle() const
{
    return d->inputStyle;
}

void InputContext::setInputStyle(qint32 inputStyle)
{
    if (d->inputStyle == inputStyle) return;
    d->inputStyle = inputStyle;
}

unsigned long InputContext::clientWindow() const
{
    return d->clientWindow;
}

void InputContext::setClientWindow(unsigned long clientWindow)
{
    if (d->clientWindow == clientWindow) return;
    d->clientWindow = clientWindow;
}

unsigned long InputContext::focusWindow() const
{
    return d->focusWindow;
}

void InputContext::setFocusWindow(unsigned long focusWindow)
{
    if (d->focusWindow == focusWindow) return;
    d->focusWindow = focusWindow;
}

QRect InputContext::area() const
{
    return d->area;
}

void InputContext::setArea(const QRect &area)
{
    if (d->area == area) return;
    qimsysDebug() << area;
    d->area = area;
}

QRect InputContext::areaNeeded() const
{
    return d->areaNeeded;
}

void InputContext::setAreaNeeded(const QRect &areaNeeded)
{
    if (d->areaNeeded == areaNeeded) return;
    qimsysDebug() << areaNeeded;
    d->areaNeeded = areaNeeded;
}

const QPoint &InputContext::spotLocation() const
{
    return d->spotLocation;
}

void InputContext::setSpotLocation(const QPoint &spotLocation)
{
    if (d->spotLocation == spotLocation) return;
    d->spotLocation = spotLocation;
    d->update();
}

unsigned long InputContext::colorMap() const
{
    return d->colorMap;
}

void InputContext::setColorMap(unsigned long colorMap)
{
    if (d->colorMap == colorMap) return;
    qimsysDebug() << colorMap;
    d->colorMap = colorMap;
}

unsigned long InputContext::stdColorMap() const
{
    return d->stdColorMap;
}

void InputContext::setStdColorMap(unsigned long stdColorMap)
{
    if (d->stdColorMap == stdColorMap) return;
    qimsysDebug() << stdColorMap;
    d->stdColorMap = stdColorMap;
}

unsigned long InputContext::foreground() const
{
    return d->foreground;
}

void InputContext::setForeground(unsigned long foreground)
{
    if (d->foreground == foreground) return;
    d->foreground = foreground;
}

unsigned long InputContext::background() const
{
    return d->background;
}

void InputContext::setBackground(unsigned long background)
{
    if (d->background == background) return;
    qimsysDebug() << background;
    d->background = background;
}

unsigned long InputContext::backgroundPixmap() const
{
    return d->backgroundPixmap;
}

void InputContext::setBackgroundPixmap(unsigned long backgroundPixmap)
{
    if (d->backgroundPixmap == backgroundPixmap) return;
    qimsysDebug() << backgroundPixmap;
    d->backgroundPixmap = backgroundPixmap;
}

const QString &InputContext::fontSet() const
{
    return d->fontSet;
}

void InputContext::setFontSet(const QString &fontSet)
{
    if (d->fontSet == fontSet) return;
    d->fontSet = fontSet;
    QFont f;
// f.setRawMode( true );
    f.setRawName(d->fontSet);
// f.setUnderline( true );
    d->preeditManager.setFont(f);
    d->update();
}

unsigned long InputContext::lineSpace() const
{
    return d->lineSpace;
}

void InputContext::setLineSpace(unsigned long lineSpace)
{
    if (d->lineSpace == lineSpace) return;
    qimsysDebug() << lineSpace;
    d->lineSpace = lineSpace;
}

unsigned long InputContext::cursor() const
{
    return d->cursor;
}

void InputContext::setCursor(unsigned long cursor)
{
    if (d->cursor == cursor) return;
    qimsysDebug() << cursor;
    d->cursor = cursor;
}

bool InputContext::hasFocus() const
{
    return d->focus;
}

void InputContext::setFocus(bool focus)
{
    if (d->focus == focus) return;
    d->focus = focus;
}

Qt::KeyboardModifiers InputContext::modifiers() const
{
    return d->modifiers;
}

void InputContext::setModifiers(Qt::KeyboardModifiers modifiers)
{
    d->modifiers = modifiers;
}

void InputContext::setModifier(Qt::KeyboardModifier modifier, bool on)
{
    Qt::KeyboardModifiers before = d->modifiers;
    if (on) {
        d->modifiers |= modifier;
    } else {
        d->modifiers ^= modifier;
    }
}

void InputContext::reset()
{
    // TODO
    // reset all properties
    d->modifiers = Qt::NoModifier;
    if (d->preeditWidget) {
        QString preeditString = d->preeditWidget->preeditString();
        if (!preeditString.isEmpty()) {
            sendCommitString(preeditString);
        }
        d->preeditWidget->reset();
    } else {
        d->sendPreeditString("", 0);
        d->setPreeditStringVisible(false);
    }
}

#include "inputcontext.moc"
