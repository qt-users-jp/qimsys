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
#include "ui_widget.h"

#include <qimsysdebug.h>

#include <QFile>
#include <QSettings>
#include <QTextStream>

namespace AboutQimsys {

class Widget::Private : private QObject
{
    Q_OBJECT
public:
    Private(Widget *parent);
    ~Private();

private:
    QString read(const QString &fileName);

private:
    Widget *q;
    Ui::Widget ui;
};

}

using namespace AboutQimsys;

Widget::Private::Private(Widget *parent)
    : QObject(parent)
    , q(parent)
{
    qimsysDebugIn() << parent;
    ui.setupUi(q);

    ui.aboutQimsys->setText(
       tr("<span style=\"font-size: 15px; font-weight: bold;\">qimsys version %1</span><br /><br />\n"
            "An input method for Qt<br /><br />\n"
            "Copyright &copy; 2009-2011 Tasuku Suzuki &lt;<a href=\"mailto:stasuku@gmail.com\">stasuku@gmail.com</a>&gt;<br />\n"
       ).arg(QIMSYS_MACRO_TO_STRING(QIMSYS_VERSION))
   );

    ui.lgpl->setPlainText(read(":/LICENSE.LGPL"));
    ui.lgplException->setPlainText(read(":/LGPL_EXCEPTION.TXT"));
    ui.tabWidget->setCornerWidget(ui.buttonBox, Qt::BottomRightCorner);

    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    q->resize(settings.value("Size", q->size()).toSize());
    qimsysDebugOut();
}

Widget::Private::~Private()
{
    qimsysDebugIn();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    settings.setValue("Size", q->size());
    qimsysDebugOut();
}

QString Widget::Private::read(const QString &fileName)
{
    QString ret;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        ret = file.readAll();
        file.close();
    } else {
        ret = file.errorString();
    }
    return ret;
}

Widget::Widget(QObject *parent)
    : QWidget(qobject_cast<QWidget*>(parent))
{
    d = new Private(this);
}

Widget::~Widget()
{
    delete d;
}

bool Widget::event(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        emit languageChanged();
    }
    return QWidget::event(e);
}

#include "widget.moc"
