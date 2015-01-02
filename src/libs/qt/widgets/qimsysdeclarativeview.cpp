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

#include "qimsysdeclarativeview.h"
#include "qimsysdebug.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QMoveEvent>
#include <QtGui/QResizeEvent>

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

class QimsysDeclarativeView::Private
{
public:
    Private(QimsysDeclarativeView *parent);

private:
    QString read(const QString &fileName);

private:
    QimsysDeclarativeView *q;

};

QimsysDeclarativeView::Private::Private(QimsysDeclarativeView *parent)
    : q(parent)
{
    qimsysDebugIn() << parent;
    q->setResizeMode(QDeclarativeView::SizeRootObjectToView);

#ifdef QIMSYS_APPLICATION_DIR_PATH
    QDir rootDir(QIMSYS_MACRO_TO_STRING2(QIMSYS_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    // up to root dir
    for (int i = 0; i < QString(QIMSYS_MACRO_TO_STRING(QIMSYS_APP_PATH)).count(QLatin1Char('/')) + 1; i++) {
        rootDir.cdUp();
    }

    rootDir.cd(QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_IMPORTS_PATH)));

    q->engine()->addImportPath(rootDir.absolutePath());

    QDeclarativeContext *context = q->rootContext();
    context->setContextProperty("version", QCoreApplication::applicationVersion());
    context->setContextProperty("lgpl", read(":/LICENSE.LGPL"));
    context->setContextProperty("lgpl_exception", read(":/LGPL_EXCEPTION.TXT"));
    context->setContextProperty("self", q);

    connect(q->engine(), SIGNAL(quit()), q, SIGNAL(quit()));
    qimsysDebugOut();
}

QString QimsysDeclarativeView::Private::read(const QString &fileName)
{
    qimsysDebugIn() << fileName;
    QString ret;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        ret = file.readAll();
        file.close();
    } else {
        ret = file.errorString();
    }
    qimsysDebugOut() << ret;
    return ret;
}

QimsysDeclarativeView::QimsysDeclarativeView(QWidget *parent)
    : QDeclarativeView(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

QimsysDeclarativeView::~QimsysDeclarativeView()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

QUrl QimsysDeclarativeView::resolveUrl(const QString &path) const
{
    qimsysDebugIn() << path;
    QUrl ret;
#ifdef QIMSYS_APPLICATION_DIR_PATH
    QDir rootDir(QIMSYS_MACRO_TO_STRING2(QIMSYS_APPLICATION_DIR_PATH));
#else
    QDir rootDir = QCoreApplication::applicationDirPath();
#endif
    // up to root dir
    for (int i = 0; i < QString(QIMSYS_MACRO_TO_STRING(QIMSYS_APP_PATH)).count(QLatin1Char('/')) + 1; i++) {
        rootDir.cdUp();
    }

    rootDir.cd(QLatin1String(QIMSYS_MACRO_TO_STRING(QIMSYS_DATA_PATH)));
    ret = QUrl(QString("%1/qml/%2").arg(rootDir.absolutePath()).arg(path));
    qimsysDebugOut() << ret;
    return ret;
}

void QimsysDeclarativeView::resizeEvent(QResizeEvent *e)
{
//    qimsysDebugOn();
    qimsysDebugIn() << e->size() << e->oldSize();
    QDeclarativeView::resizeEvent(e);
    emit geometryChanged(geometry());
    qimsysDebugOut() << geometry();
//    qimsysDebugOff();
}

void QimsysDeclarativeView::moveEvent(QMoveEvent *e)
{
//    qimsysDebugOn();
    qimsysDebugIn() << e->pos() << e->oldPos();
    QDeclarativeView::moveEvent(e);
    emit geometryChanged(geometry());
    qimsysDebugOut() << geometry();
//    qimsysDebugOff();
}

void QimsysDeclarativeView::hideEvent(QHideEvent *e)
{
//    qimsysDebugOn();
    qimsysDebugIn() << geometry();
    QDeclarativeView::hideEvent(e);
    qimsysDebugOut() << geometry();
//    qimsysDebugOff();
}

void QimsysDeclarativeView::showEvent(QShowEvent *e)
{
//    qimsysDebugOn();
    qimsysDebugIn() << geometry();
    QDeclarativeView::showEvent(e);
    if (!parent()) raise();
    qimsysDebugOut() << geometry();
//    qimsysDebugOff();
}
