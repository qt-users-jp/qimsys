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

#ifndef INPUTCONTEXT_H
#define INPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>

class InputContext : public QPlatformInputContext
{
    Q_OBJECT
    Q_DISABLE_COPY(InputContext)
public:
    explicit InputContext();
    ~InputContext();

    virtual bool isValid() const;

    virtual void reset();
    virtual void commit();
    virtual void update(Qt::InputMethodQueries queries);
//    virtual void invokeAction(QInputMethod::Action, int cursorPosition);
    virtual bool filterEvent(const QEvent *event);
//    virtual QRectF keyboardRect() const;
//    virtual bool isAnimating() const;
//    virtual void showInputPanel();
//    virtual void hideInputPanel();
//    virtual bool isInputPanelVisible() const;
//    virtual QLocale locale() const;
//    virtual Qt::LayoutDirection inputDirection() const;

    virtual void setFocusObject(QObject *object);

private:
    class Private;
    Private *d;
};

#endif//INPUTCONTEXT_H
