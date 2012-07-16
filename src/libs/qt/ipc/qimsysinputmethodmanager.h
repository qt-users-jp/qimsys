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

#ifndef QIMSYSINPUTMETHODMANAGER_H
#define QIMSYSINPUTMETHODMANAGER_H

#include "qimsysglobal.h"
#include "qimsysabstractipcobject.h"

class QIMSYS_EXPORT QimsysInputMethodManager : public QimsysAbstractIpcObject
{
    Q_OBJECT
    Q_PROPERTY(QString locale READ locale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QString converter READ converter WRITE setConverter NOTIFY converterChanged)
    Q_PROPERTY(QString interpreter READ interpreter WRITE setInterpreter NOTIFY interpreterChanged)
    Q_PROPERTY(QString engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(uint state READ state WRITE setState NOTIFY stateChanged)
    Q_CLASSINFO("D-Bus Interface", "local.InputMethodManager")
    Q_DISABLE_COPY(QimsysInputMethodManager)
public:
    explicit QimsysInputMethodManager(QObject *parent = 0, Type type = Client);
    ~QimsysInputMethodManager();

    bool init();

    Q_INVOKABLE QString locale() const;
    Q_INVOKABLE QString identifier() const;
    Q_INVOKABLE QString converter() const;
    Q_INVOKABLE QString interpreter() const;
    Q_INVOKABLE QString engine() const;
    Q_INVOKABLE uint state() const;

public slots:
    void setLocale(const QString &locale);
    void setIdentifier(const QString &identifier);
    void setConverter(const QString &converter);
    void setInterpreter(const QString &interpreter);
    void setEngine(const QString &engine);
    void setState(uint state);
    void execute(const QString &command);

signals:
    void localeChanged(const QString &locale);
    void identifierChanged(const QString &identifier);
    void converterChanged(const QString &converter);
    void interpreterChanged(const QString &interpreter);
    void engineChanged(const QString &engine);
    void stateChanged(uint state);
    void executed(const QString &command);

protected:
    QimsysAbstractIpcObject *server() const;

private:
    class Private;
    Private *d;
};

#endif // QIMSYSINPUTMETHODMANAGER_H
