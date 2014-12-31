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

#include "engine.h"
#include "libanthy.h"
#include "dictionary.h"
#include "settings.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <qimsysdebug.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsyscandidatemanager.h>
#include <qimsysdynamictranslator.h>

#include <QIcon>
#include <QSettings>
#include <QTextCodec>

namespace Japanese {
    namespace Anthy {

class Engine::Private : private QObject
{
    Q_OBJECT
public:
    Private(Engine *parent);
    ~Private();

#ifndef QIMSYS_NO_GUI
    QimsysSettingsWidget *settings(const QString &hint, QWidget *parent);
#endif

private slots:
    void activeChanged(bool active);
    void stateChanged(uint state);
    void itemChanged(const QimsysPreeditItem &item);
    void currentIndexChanged(int currentIndex);

    void saved();

private:
    void predict();
    void convert();
    void resize();
    void setCandidates();
    void learn();

    void clearCandidates();

private:
    Engine *q;

    QimsysInputMethodManager *inputMethodManager;
    QimsysPreeditManager *preeditManager;
    QimsysCandidateManager *candidateManager;

    QTextCodec *eucjp;
    LibAnthy *libAnthy;
    LibAnthy::anthy_context_t context;

    State currentState;
    int preeditIndex;
    QimsysConversionItemList predictionItems;
    QimsysConversionItemList conversionItems;

    bool prediction;
    bool predictOnEmpty;
};

    }
}

using namespace Japanese::Anthy;

Engine::Private::Private(Engine *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , libAnthy(0)
    , preeditIndex(-1)
{
    qimsysDebugIn() << parent;

    q->setIdentifier(QLatin1String("anthy"));
    q->setPriority(0x40);

    q->setLocale(QLatin1String("ja_JP"));
    q->setLanguage("Japanese(Standard)");

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Conversion Engine"), q, "categoryName");

#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/icon/anthy.png"));
#endif
    trConnect(this, QT_TR_NOOP("Anthy"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());
    eucjp = QTextCodec::codecForName("EUC-JP");
    Q_ASSERT(eucjp);

    saved();

    qimsysDebugOut();
}

Engine::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Engine::Private::activeChanged(bool active)
{
    qimsysDebugIn() << active;
    if (active) {
        if (!inputMethodManager) {
            inputMethodManager = new QimsysInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));

        }
        if (!libAnthy) {
            libAnthy = new LibAnthy;
            libAnthy->anthy_init();
            context = libAnthy->anthy_create_context();
        }
        stateChanged(inputMethodManager->state());
    } else {
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
        if (libAnthy) {
            libAnthy->anthy_release_context(context);
            libAnthy->anthy_quit();
            delete libAnthy;
            libAnthy = 0;
        }
        stateChanged(Direct);
    }
    qimsysDebugOut();
}

void Engine::Private::stateChanged(uint state)
{
    qimsysDebugIn() << state;
    currentState = (State)state;
    if (state == Direct) {
        if (preeditManager) {
            disconnect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
            preeditManager->deleteLater();
            preeditManager = 0;
        }
        if (candidateManager) {
            disconnect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            candidateManager->deleteLater();
            candidateManager = 0;
        }
    } else {
        if (!preeditManager) {
            preeditManager = new QimsysPreeditManager(this);
            preeditManager->init();
            connect(preeditManager, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(itemChanged(QimsysPreeditItem)));
        }
        if (!candidateManager) {
            candidateManager = new QimsysCandidateManager(this);
            candidateManager->init();
            connect(candidateManager, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        }
    }

    switch (state) {
    case Direct:
        preeditIndex = -1;
        predictionItems.clear();
        conversionItems.clear();
        clearCandidates();
        break;
    case Empty:
        learn();
        fallthrough;
    case Input:
        clearCandidates();
        predict();
        preeditIndex = -1;
        conversionItems.clear();
        break;
    case Convert:
        predictionItems.clear();
        clearCandidates();
        convert();
        break;
    case Select:
        if (!conversionItems.isEmpty()) {
            clearCandidates();
            setCandidates();
        }
        break;
    }

    qimsysDebugOut();
}

void Engine::Private::itemChanged(const QimsysPreeditItem &item)
{
    if (item.to.contains(QString())) return;

    qimsysDebugIn() << item;
    switch (currentState) {
    case Empty:
        if (!predictOnEmpty) {
            break;
        }
        fallthrough;
    case Input:
        if (!prediction) {
            break;
        }
//        qimsysWarning() << "PREDICT:" << items;
        predict();
        break;
    case Convert: {
        int currentIndex = -1;
        int pos = 0;
        for (int i = 0; i < item.to.length(); i++) {
            if (pos == item.cursor) {
                currentIndex = i;
                break;
            }
            pos += item.to.at(i).length();
        }
        qimsysDebug() << currentIndex << preeditIndex;
        if (currentIndex == preeditIndex) {
            resize();
        } else {
            preeditIndex = currentIndex;
//            move();
        }
        break;
    }
    case Select:
        // nothing to do ?
        break;
    default:
        break;
    }

    qimsysDebugOut();
}

void Engine::Private::currentIndexChanged(int currentIndex)
{
    if (currentIndex == -1) return;
    if (conversionItems.isEmpty()) return;
    qimsysDebugIn() << currentIndex;

    QimsysPreeditItem item = preeditManager->item();
    int index = -1;
    int pos = 0;
    for (int i = 0; i < item.to.length(); i++) {
        if (pos == item.cursor) {
            index = i;
            break;
        }
        pos += item.to.at(i).length();
    }

    QString to = candidateManager->items().at(currentIndex).to;

    QimsysConversionItem convertionItem = conversionItems.at(index);
    convertionItem.to = to;
    if (candidateManager->items().at(currentIndex).source == q->identifier()) {
        convertionItem.index = candidateManager->items().at(currentIndex).index;
    } else {
        convertionItem.index = -1;
    }
    conversionItems.replace(index, convertionItem);

    qimsysDebugOut();
}

void Engine::Private::predict()
{
    if (!prediction) return;
    QimsysPreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    if (!predictOnEmpty && preeditString.isEmpty()) return;
    qimsysDebugIn();

    predictionItems = candidateManager->items();
    foreach (const QimsysConversionItem &item, predictionItems) {
        if (item.source == q->identifier()) {
            predictionItems.removeOne(item);
        }
    }

    int ret = libAnthy->anthy_set_prediction_string(context, eucjp->fromUnicode(preeditString).data());
    Q_ASSERT(ret >= 0);
    LibAnthy::anthy_prediction_stat stat;
    ret = libAnthy->anthy_get_prediction_stat(context, &stat);
    Q_ASSERT(ret >= 0);
    for (int i = 0; i < stat.nr_prediction; i++) {
        char buf[8192];
        int len = libAnthy->anthy_get_prediction(context, i, buf, sizeof(buf));
        Q_ASSERT(len > 0);

        QimsysConversionItem convertionItem;
        convertionItem.to = eucjp->toUnicode(buf, len);
        convertionItem.from = preeditString;
        convertionItem.index = i;
        convertionItem.source = q->identifier();
        predictionItems.append(convertionItem);
    }

    qimsysDebug() << predictionItems;
    candidateManager->setItems(predictionItems);
    qimsysDebugOut();
}

void Engine::Private::convert()
{
    if (!conversionItems.isEmpty()) return;
    qimsysDebugIn();
    QimsysPreeditItem item = preeditManager->item();
    QStringList from = item.from;
    QString preeditString = from.join("");

    qimsysDebug() << from << preeditString;

    int ret = libAnthy->anthy_set_string(context, eucjp->fromUnicode(preeditString).data());
    Q_ASSERT(ret >= 0);
    LibAnthy::anthy_conv_stat stat;
    ret = libAnthy->anthy_get_stat(context, &stat);
    Q_ASSERT(ret >= 0);
    conversionItems.clear();
    for (int i = 0; i < stat.nr_segment; i++) {
        QimsysConversionItem conversionItem;
        conversionItem.index = 0;
        char buf[8192];
        int len = libAnthy->anthy_get_segment(context, i, 0, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        conversionItem.to = eucjp->toUnicode(buf, len);
        len = libAnthy->anthy_get_segment(context, i, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        conversionItem.from = eucjp->toUnicode(buf, len);
        qimsysDebug() << conversionItem;
        conversionItems.append(conversionItem);
    }

    item.to.clear();
    item.from.clear();
    item.rawString.clear();
    item.cursor = 0;
    foreach (const QimsysConversionItem &conversionItem, conversionItems) {
        qimsysDebug() << conversionItem;
        if (item.to.isEmpty()) {
            item.selection = conversionItem.to.length();
        }
        item.to.append(conversionItem.to);
        QString f = conversionItem.from;
        while (!f.isEmpty()) {
            qimsysDebug() << f << from.first() << item;
            if (f.startsWith(from.first())) {
                f = f.mid(from.first().length());
                int l = item.to.length();
                if (item.from.length() < l) {
                    item.from.append(from.takeFirst());
                    item.rawString.append(QString());
                } else {
                    item.from.replace(l - 1, item.from.at(l - 1) + from.takeFirst());
                }
            } else {
                QString f = from.first();
                QString ch = f.left(1);
                qimsysDebug() << f << ch;
                if (f.startsWith(ch)) {
                    f = f.mid(1);
                    item.from.append(ch);
                    item.rawString.append(QString());
                }
                from.replace(0, f.mid(1));
            }
        }
    }
    qimsysDebug() << item;

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);
    preeditIndex = 0;

    qimsysDebugOut();
}

void Engine::Private::resize()
{
    QimsysPreeditItem item = preeditManager->item();
    if (item.to.isEmpty()) return;
    qimsysDebugIn();
    QStringList from = item.from;

    qimsysDebug() << item << preeditIndex;
    int delta = item.from.at(preeditIndex).length() - conversionItems.at(preeditIndex).from.length();
    qimsysDebug() << delta;
    if (delta == 0) {
        qimsysDebugOut();
        return;
    }
    libAnthy->anthy_resize_segment(context, preeditIndex, delta);
    LibAnthy::anthy_conv_stat stat;
    int ret = libAnthy->anthy_get_stat(context, &stat);
    Q_ASSERT(ret >= 0);

    while (conversionItems.length() > preeditIndex) {
        conversionItems.removeLast();
    }

    for (int i = preeditIndex; i < stat.nr_segment; i++) {
        QimsysConversionItem convertionItem;
        convertionItem.index = 0;
        char buf[8192];
        int len = libAnthy->anthy_get_segment(context, i, 0, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        convertionItem.to = eucjp->toUnicode(buf, len);
        len = libAnthy->anthy_get_segment(context, i, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        convertionItem.from = eucjp->toUnicode(buf, len);
        conversionItems.append(convertionItem);
    }

    item.to.clear();
    item.from.clear();
    item.rawString.clear();
    foreach (const QimsysConversionItem &conversionItem, conversionItems) {
        if (item.to.length() == preeditIndex) {
            item.cursor = item.to.join("").length();
            item.selection = conversionItem.to.length();
        }
        item.to.append(conversionItem.to);
        item.from.append(QString());
        item.rawString.append(QString());
        QString f = conversionItem.from;
        qimsysDebug() << f << from;
        while (!f.isEmpty()) {
            if (f.startsWith(from.first())) {
                f= f.mid(from.first().length());
                item.from[item.from.length() - 1] = item.from.last() + from.takeFirst();
            } else {
                break;
            }
        }
    }

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);

    qimsysDebugOut();
}

void Engine::Private::setCandidates()
{
    qimsysDebugIn();

    QimsysPreeditItem item = preeditManager->item();
    int currentIndex = -1;
    int pos = 0;
    for (int i = 0; i < item.to.length(); i++) {
        if (pos == item.cursor) {
            currentIndex = i;
            break;
        }
        pos += item.to.at(i).length();
    }

    QimsysConversionItemList candidateItemList = candidateManager->items();

    LibAnthy::anthy_segment_stat stat;
    int ret = libAnthy->anthy_get_segment_stat(context, currentIndex, &stat);
    Q_ASSERT(ret >= 0);

    for (int i = 0; i < stat.nr_candidate; i++) {
        QimsysConversionItem convertionItem;
        convertionItem.index = i;
        char buf[8192];
        int len = libAnthy->anthy_get_segment(context, currentIndex, i, buf, sizeof(buf));
        Q_ASSERT(len > 0);
        convertionItem.to = eucjp->toUnicode(buf, len);
        convertionItem.from = conversionItems[currentIndex].from;
        convertionItem.source = q->identifier();
        candidateItemList.append(convertionItem);
    }

    candidateManager->setItems(candidateItemList);
    qimsysDebugOut();
}

void Engine::Private::learn()
{
    qimsysDebugIn() << conversionItems << predictionItems;
    if (!conversionItems.isEmpty()) {
        for (int i = 0; i < conversionItems.count(); i++) {
            if (conversionItems[i].index < 0) continue;
            libAnthy->anthy_commit_segment(context, i, conversionItems[i].index);
        }
        conversionItems.clear();
    } else if (!predictionItems.isEmpty()) {
        for (int i = 0; i < conversionItems.count(); i++) {
            if (conversionItems[i].index < 0) continue;
            libAnthy->anthy_commit_prediction(context, predictionItems[i].index);
        }
        predictionItems.clear();
    }
    qimsysDebugOut();
}

void Engine::Private::clearCandidates()
{
    if (!candidateManager) return;
    qimsysDebugIn();

    QimsysConversionItemList candidateItemList = candidateManager->items();
    foreach (QimsysConversionItem candidateItem, candidateItemList) {
        if (candidateItem.source == q->identifier()) {
            candidateItemList.removeOne(candidateItem);
        }
    }
    candidateManager->setItems(candidateItemList);

    qimsysDebugOut();
}

#ifndef QIMSYS_NO_GUI
QimsysSettingsWidget *Engine::Private::settings(const QString &hint, QWidget *parent)
{
    qimsysDebugIn() << hint << parent;
    Q_UNUSED(hint);
    Settings *settings = new Settings(q, parent);
    connect(settings, SIGNAL(saved()), this, SLOT(saved()));
    qimsysDebugOut() << settings;
    return settings;
}
#endif

void Engine::Private::saved()
{
    qimsysDebugIn();
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    prediction = settings.value("Prediction", false).toBool();
    predictOnEmpty = settings.value("Empty", false).toBool();
    qimsysDebugOut();
}

Engine::Engine(QObject *parent)
    : QimsysEngine(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Engine::~Engine()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}


QimsysEngineDictionary *Engine::dictionary(QObject *parent)
{
    return new Dictionary(parent);
}

#ifndef QIMSYS_NO_GUI
QimsysSettingsWidget *Engine::settings(const QString &hint, QWidget *parent)
{
    return d->settings(hint, parent);
}
#endif

#include "engine.moc"
