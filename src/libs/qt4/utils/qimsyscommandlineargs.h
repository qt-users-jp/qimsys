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

#ifndef QIMSYSCOMMANDLINEARGS_H
#define QIMSYSCOMMANDLINEARGS_H

#include <QObject>
#include <QVariant>

#include "qimsysglobal.h"

class QCommandLineArgsPrivate;
class QIMSYS_EXPORT QCommandLineArgs : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QCommandLineArgs)
public:
    explicit QCommandLineArgs(QObject *parent = 0);
    virtual ~QCommandLineArgs();

    virtual void addOption(const char *name, const char *option, const char *description = 0, const QVariant &defaultValue = QVariant());

    virtual bool parse(int &argc, char** argv);

    virtual QVariant value(const char *name);
private:
    class Private;
    Private *d;
};

class QIMSYS_EXPORT QimsysCommandlineArgs : public QCommandLineArgs
{
    Q_OBJECT
    Q_DISABLE_COPY(QimsysCommandlineArgs)
public:
    static QimsysCommandlineArgs *instance();

private:
    explicit QimsysCommandlineArgs(QObject *parent = 0);

private:
    class Private;
    Private *d;
};

#endif // QIMSYSCOMMANDLINEARGS_H
