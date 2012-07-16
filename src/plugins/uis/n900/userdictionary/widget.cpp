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

#include "widget.h"

#include <qimsysdebug.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspluginmanager.h>
#include <qimsysengine.h>
#include <qimsysenginedictionary.h>

#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>

using namespace N900::UserDictionary;

Widget::Widget(QObject *parent)
    : QimsysDeclarativeView(qobject_cast<QWidget*>(parent))
{
    setWindowTitle(tr("User Dictionary"));

//    QSortFilterProxyModel *model = new QSortFilterProxyModel(this);

    QAbstractItemModel *model = 0;
    QimsysInputMethodManager inputMethodManager;
    inputMethodManager.init();

    QString currentEngine = inputMethodManager.engine();
    QUrl editor;

    foreach(QimsysEngine *engine, QimsysPluginManager::objects<QimsysEngine>(this)) {
        if (engine->identifier() == currentEngine) {
            QimsysEngineDictionary *currentDictionary = engine->dictionary();
            if (currentDictionary) {
                editor = currentDictionary->editor();
//                model->setSourceModel(currentDictionary->model());
                model = currentDictionary->model();
            } else {
                QMessageBox::warning(this, engine->name(), tr("%1 does not support user dictoinary.").arg(engine->name()));
                return;
            }
            break;
        }
    }

    QDeclarativeContext *context = engine()->rootContext();
    context->setContextProperty(QLatin1String("userDictionaryModel"), model);
    context->setContextProperty(QLatin1String("userDictionaryEditor"), editor);
    setSource(resolveUrl("N900/UserDictionary/userdictionary.qml"));

#ifdef QIMSYS_PLATFORM_MAEMO
    setAttribute(Qt::WA_Maemo5AutoOrientation, true);
#endif
    connect(this, SIGNAL(quit()), this, SLOT(close()));
}

Widget::~Widget()
{
}
