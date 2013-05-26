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

#ifndef QIMSYSKEYMANAGER_H
#define QIMSYSKEYMANAGER_H

#include "qimsysglobal.h"
#include "qimsysabstractipcobject.h"

class QIMSYS_EXPORT QimsysKeyManager : public QimsysAbstractIpcObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.KeyManager")
    Q_PROPERTY(bool accepted READ isAccepted);
    Q_DISABLE_COPY(QimsysKeyManager)
public:
    explicit QimsysKeyManager(QObject *parent = 0, Type type = Client);
    ~QimsysKeyManager();
    virtual bool init();

    Q_INVOKABLE bool isAccepted() const;

public slots:
    void process(QString text, int keycode, int modifiers, bool isPress, bool autoRepeat);
    bool keyPress(const QString &text, int keycode, int modifiers, bool autoRepeat);
    bool keyRelease(const QString &text, int keycode, int modifiers, bool autoRepeat);

    void accept();

signals:
    void keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat);
    void keyReleased(const QString &text, int keycode, int modifiers, bool autoRepeat);

protected:
    QimsysAbstractIpcObject *server() const;

private:
    void processKey(QString text, int keycode, int modifiers, bool isPress, bool autoRepeat);

private:
    class Private;
    Private *d;
};

#endif//QIMSYSKEYMANAGER_H
