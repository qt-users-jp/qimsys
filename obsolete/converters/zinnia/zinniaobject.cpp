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

#include "zinniaobject.h"

#include <QAction>

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsyspreedit.h>
#include <qimsyscandidates.h>

#include "zinniaengine.h"
#include "zinniawidget.h"

namespace zinnia {
class ZinniaObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(ZinniaObject *parent);
    ~Private();

    QList<QAction*> actions();

private slots:
    void init();

    void inputLanguageChanged(const QString &inputLanguage);
    void recognized(const QStringList &data);
    void focusChanged(uint focus);
    void currentIndexChanged(int currentIndex);

private:
    ZinniaObject *q;

    ZinniaEngine *engine;
    ZinniaWidget *widget;
    QimsysApplicationManager manager;
    QimsysPreedit preedit;
    QimsysCandidates candidates;
};
}

using namespace zinnia;

ZinniaObject::Private::Private(ZinniaObject *parent)
    : QObject(parent)
    , q(parent)
    , engine(0)
    , widget(0)
{
    qimsysDebugIn() << parent;
    q->setGroups(QStringList() << QLatin1String("X11"));
    q->setCategoryType(MoreThanOne);
    q->setCategoryName(tr("Input/Language"));
    q->setIcon(QIcon(":/icons/zinnia.png"));
    q->setName(tr("Zinnia"));
    q->setAuthor(tr("Tasuku Suzuki"));
    q->setTranslator(tr("None"));
    q->setDescription(tr("Zinnia plugin for qimsys"));

    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
    qimsysDebugOut();
}

ZinniaObject::Private::~Private()
{
    delete widget;
}

void ZinniaObject::Private::init()
{
    if (widget) return;
    qimsysDebugIn();
    manager.init();
    connect(&manager, SIGNAL(inputLanguageChanged(QString)), this, SLOT(inputLanguageChanged(QString)));
    connect(&manager, SIGNAL(focusChanged(uint)), this, SLOT(focusChanged(uint)));

    preedit.init();
    candidates.init();
    connect(&candidates, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));

    engine = new ZinniaEngine(this);
    widget = new ZinniaWidget;
    widget->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    widget->setVisible(manager.inputLanguage() == q->identifier());

    engine->setSize(widget->size());
    connect(widget, SIGNAL(dataChanged(QList<QPolygon>)), engine, SLOT(setData(QList<QPolygon>)));
    connect(engine, SIGNAL(recognized(QStringList)), this, SLOT(recognized(QStringList)));
    qimsysDebugOut();
}

void ZinniaObject::Private::recognized(const QStringList &data)
{
    qimsysDebugIn() << data;
    QimsysConversionItemList items;
    int i = 0;
    foreach (const QString &candidate, data) {
        QimsysConversionItem item;
        item.index = i++;
        item.to = candidate;
        items.append(item);
    }
    candidates.setCandidates(items);
    qimsysDebugOut();
}

void ZinniaObject::Private::currentIndexChanged(int currentIndex)
{
    if (manager.inputLanguage() != q->identifier()) return;
    if (currentIndex < 0) return;
    qimsysDebugIn() << currentIndex;
    preedit.commit(manager.widget());
    widget->clear();
    qimsysDebugOut();
}

void ZinniaObject::Private::inputLanguageChanged(const QString &inputLanguage)
{
    qimsysDebugIn() << inputLanguage;
    widget->setVisible(inputLanguage == q->identifier() && manager.focus() > 0);
    engine->setSize(widget->size());
    manager.setCurrentIcon(q->icon());
    qimsysDebugOut();
}

void ZinniaObject::Private::focusChanged(uint focus)
{
    if (manager.inputLanguage() != q->identifier()) return;
    qimsysDebugIn() << focus;
    widget->setVisible(manager.inputLanguage() == q->identifier() && focus > 0);
    engine->setSize(widget->size());
    qimsysDebugOut();
}

QList<QAction*> ZinniaObject::Private::actions()
{
    static QList<QAction*> ret;
    if (!ret.isEmpty()) return ret;
    if (!widget) {
        init();
    }

    QAction *on = new QAction(this);
    on->setIcon(QIcon(":/icons/zinniawidget.png"));
    on->setText(tr("Show"));
    on->setCheckable(true);
    on->setChecked(widget->isVisible());
    connect(on, SIGNAL(toggled(bool)), widget, SLOT(setVisible(bool)));

    ret.append(on);
    return ret;
}

ZinniaObject::ZinniaObject(QObject *parent)
    : QimsysConverter(parent)
{
    d = new Private(this);
}

ZinniaObject::~ZinniaObject()
{
    delete d;
}

QString ZinniaObject::identifier() const
{
    return QLatin1String("Zinnia");
}

QStringList ZinniaObject::locales() const
{
    return QStringList() << "ja_JP";
}

bool ZinniaObject::useEngine() const
{
    return false;
}

QList<QAction*> ZinniaObject::actions()
{
    return d->actions();
}

#include "zinniaobject.moc"
