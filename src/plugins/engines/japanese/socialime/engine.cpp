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

#include "engine.h"
#include "settings.h"

#include <plugins/inputmethods/japanese/standard/global.h>

#include <qimsysdebug.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsyscandidatemanager.h>
#include <qimsysdynamictranslator.h>

#include <QApplication>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QUrl>
#include <QSettings>

namespace Japanese {
    namespace SocialIME {

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
    void enabledChanged(bool enabled);
    void activeChanged(bool active);
    void stateChanged(uint state);
    void itemChanged(const QimsysPreeditItem &item);

    void clearCandidates();

    void error(QNetworkReply::NetworkError err);
    void sslErrors(QList<QSslError> errs);

    void saved();

    void predict();
    void readPrediction();

    void convert();
    void readConversion();

    void resize();
    void setCandidates();
    void learn();

    void cancel();

private:
    Engine *q;
    QimsysInputMethodManager *inputMethodManager;
    QimsysPreeditManager *preeditManager;
    QimsysCandidateManager *candidateManager;
    QNetworkAccessManager *networkManager;
    QNetworkReply *conversionReply;

    State currentState;
    int preeditIndex;
    QimsysConversionItemList predictionItems;
    QList<QimsysConversionItemList> candidateList;

    bool prediction;
    bool predictOnEmpty;

    QList<int> initialSizes;
    QString user;
};

    }
}

using namespace Japanese::SocialIME;

Engine::Private::Private(Engine *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , networkManager(0)
    , conversionReply(0)
    , prediction(false)
    , predictOnEmpty(false)
{
    qimsysDebugIn() << parent;

    q->setIdentifier(QLatin1String("Social IME"));
    q->setPriority(0x80);

    q->setLocale(QLatin1String("ja_JP"));
    q->setLanguage("Japanese(Standard)");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/icons/socialime.png"));
#endif
    trConnect(this, QT_TR_NOOP("Social IME Engine"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("See <a href=\"http://www.social-ime.com/\">http://www.social-ime.com/</a> for detail."), q, "description");

    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Conversion Engine"), q, "categoryName");

    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    user = settings.value("User").toString();

    connect(q, SIGNAL(enabledChanged(bool)), this, SLOT(enabledChanged(bool)));
    enabledChanged(q->isEnabled());

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(activeChanged(bool)));
    activeChanged(q->isActive());

    qimsysDebugOut();
}

Engine::Private::~Private()
{
}

void Engine::Private::enabledChanged(bool enabled)
{
    qimsysDebugIn() << enabled;
    if (enabled) {

    } else {
    }
    qimsysDebugOut();
}

void Engine::Private::activeChanged(bool active)
{
    qimsysDebugIn() << active;
    if (active) {
        if (!networkManager) {
            networkManager = new QNetworkAccessManager(this);
        }
        if (!inputMethodManager) {
            inputMethodManager = new QimsysInputMethodManager(this);
            inputMethodManager->init();
            connect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));

        }
        stateChanged(inputMethodManager->state());
    } else {
        stateChanged(Direct);
        if (networkManager) {
            networkManager->deleteLater();
            networkManager = 0;
        }
        if (inputMethodManager) {
            disconnect(inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
            inputMethodManager->deleteLater();
            inputMethodManager = 0;
        }
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
        }
    }

    switch (state) {
    case Direct:
        preeditIndex = -1;
        predictionItems.clear();
        candidateList.clear();
        clearCandidates();
        break;
    case Empty:
        learn();
        fallthrough;
    case Input:
        clearCandidates();
        candidateList.clear();
        predict();
        preeditIndex = -1;
        break;
    case Convert:
        predictionItems.clear();
        clearCandidates();
        convert();
        break;
    case Select:
        if (!candidateList.isEmpty()) {
            clearCandidates();
            setCandidates();
        }
        break;
    }

    qimsysDebugOut();
}

void Engine::Private::itemChanged(const QimsysPreeditItem &item)
{
    if (item.rawString.contains(QString())) return;

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
            cancel();
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

void Engine::Private::predict()
{
    if (!prediction) return;
    QimsysPreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    if (!predictOnEmpty && preeditString.isEmpty()) return;
    qimsysDebugIn();

    QString url = QString("http://www.social-ime.com/api2/predict.php?string=%1").arg(preeditString);
    url.append(QLatin1String("&charset=UTF8"));
    url.append(QString("&user=%1").arg(user));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "QIMSYS");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readPrediction()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>))
            , this, SLOT(sslErrors(QList<QSslError>)));

    qimsysDebugOut();
}

void Engine::Private::readPrediction()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    QimsysPreeditItem item = preeditManager->item();
    QString preeditString = reply->url().queryItemValue("string");
    if (preeditString != item.from.join("")) {
        return;
    }
    qimsysDebugIn();

    QStringList result = QString::fromUtf8(reply->readAll()).split("\t");
    result.removeAll(QLatin1String("\n"));

    predictionItems = candidateManager->items();
    foreach (const QimsysConversionItem &item, predictionItems) {
        if (item.source == q->identifier()) {
            predictionItems.removeOne(item);
        }
    }

    foreach(const QString &p, result) {
        QimsysConversionItem convertionItem;
        convertionItem.index = -1;
        convertionItem.to = p;
        convertionItem.from = preeditString;
        convertionItem.source = q->identifier();
        predictionItems.append(convertionItem);
    }

    qimsysDebug() << predictionItems;
    candidateManager->setItems(predictionItems);
    reply->deleteLater();
    qimsysDebugOut();
}


void Engine::Private::convert()
{
    if (!candidateList.isEmpty()) return;
    qimsysDebugIn();

    QimsysPreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    qimsysDebug() << preeditString;

    QString url = QString("http://www.social-ime.com/api/?string=%1").arg(preeditString);
    url.append(QLatin1String("&charset=UTF8"));
    url.append(QString("&user=%1").arg(user));
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "QIMSYS");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>))
            , this, SLOT(sslErrors(QList<QSslError>)));

    cancel();
    conversionReply = reply;
    initialSizes.clear();
    preeditIndex = 0;

    qimsysDebugOut();
}

void Engine::Private::readConversion()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (conversionReply != reply) return;
    conversionReply = 0;
    if (reply->error() != QNetworkReply::NoError) return;
    QimsysPreeditItem item = preeditManager->item();
    if (reply->url().queryItemValue("string") != item.from.join("")) {
        qimsysDebug() << reply->url().queryItemValue("string") << item.from;
        return;
    }

    qimsysDebugIn();

    QStringList all = QString::fromUtf8(reply->readAll()).split("\n");
    qimsysDebug() << all;
    qimsysDebug() << item;
    qimsysDebug() << preeditIndex;

    QStringList from = item.from;
    QStringList rawString = item.rawString;

    while (item.to.length() > preeditIndex) {
        qimsysDebug() << item;
        item.to.removeLast();
        item.from.removeLast();
        item.rawString.removeLast();
    }

    qimsysDebug() << from;
    foreach (QString f, item.from) {
        while (f.startsWith(from.first())) {
            f = f.mid(from.length());
            from.removeFirst();
            rawString.removeFirst();
        }
    }
    qimsysDebug() << from;

    qimsysDebug() << candidateList;
    while (candidateList.length() > preeditIndex) {
        candidateList.removeLast();
    }
    qimsysDebug() << candidateList;

    for (int i = 0; i < preeditIndex; i++) {
        all.removeFirst();
    }
    qimsysDebug() << all;
    qimsysDebug() << item;
    item.cursor = item.to.join("").length();

    foreach(const QString &line, all) {
        QStringList data = line.split("\t");
        qimsysDebug() << data;
        Q_ASSERT(!data.isEmpty());
        if (data.first().isEmpty()) continue;

        item.to.append(data.first());

        QString f;
        QimsysConversionItemList cand;
        for (int i = 0; i < data.length(); i++) {
            if (data.at(i).isEmpty()) continue;
            QimsysConversionItem c;
            c.to = data.at(i);
            c.index = i;
            c.source = q->identifier();
            cand.append(c);

            if (f.isNull() && !from.isEmpty() && from.join("").startsWith(data.at(i))) {
                f = data.at(i);
                qimsysDebug() << f << from;
                while (!f.isEmpty() && !from.isEmpty()) {
                    qimsysDebug() << f << from.first() << item;
                    if (f.startsWith(from.first())) {
                        f = f.mid(from.first().length());
                        int l = item.to.length();
                        if (item.from.length() < l) {
                            item.from.append(from.takeFirst());
                            item.rawString.append(rawString.takeFirst());
                        } else {
                            item.from.replace(l - 1, item.from.at(l - 1) + from.takeFirst());
                            item.rawString.replace(l - 1, item.rawString.at(l - 1) + rawString.takeFirst());
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

        }
        qimsysDebug() << item;
        for (int i = 0; i < cand.count(); i++) {
            cand[i].from = item.from.last();
        }
        candidateList.append(cand);
    }
    qimsysDebug() << item;

    item.selection = item.to.at(preeditIndex).length();
    qimsysDebug() << item;

    for (int i = initialSizes.length(); i < item.from.size(); i++) {
        initialSizes.append(item.from.at(i).length());
    }

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);

    qimsysDebugOut();
}

void Engine::Private::resize()
{
    QimsysPreeditItem item = preeditManager->item();
    if (item.to.isEmpty()) return;
    QStringList from = item.from;

    int delta = item.from.at(preeditIndex).length() - initialSizes.at(preeditIndex);

    if (initialSizes.at(preeditIndex) + delta < 1) return;
    if (delta > 0 && preeditIndex == initialSizes.count() - 1) return;

    qimsysDebugIn() << delta;

    qimsysDebug() << item;
    qimsysDebug() << initialSizes;

    QString url = QString("http://www.social-ime.com/api/?string=%1").arg(from.join(""));
    while (initialSizes.count() - 1 > preeditIndex) {
        initialSizes.takeLast();
    }
    for (int i = 0; i < preeditIndex; i++) {
        if (item.from.at(i).length() != initialSizes.at(i))
            url.append(QString("&resize[%1]=%2").arg(i).arg(item.from.at(i).length() - initialSizes.at(i)));
    }
    url.append(QString("&resize[%1]=%2").arg(preeditIndex).arg(item.from.at(preeditIndex).length() - initialSizes.at(preeditIndex)));
    url.append(QLatin1String("&charset=UTF8"));
    url.append(QString("&user=%1").arg(user));
    qimsysDebug() << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", QString("%1(%2)").arg(QApplication::applicationName()).arg(QApplication::applicationVersion()).toUtf8());

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>))
            , this, SLOT(sslErrors(QList<QSslError>)));
    cancel();
    conversionReply = reply;
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

    qimsysDebug() << item << currentIndex;
    qimsysDebug() << candidateList;
    candidateManager->setItems(candidateList.at(currentIndex));
    qimsysDebugOut();
}

void Engine::Private::learn()
{
    QimsysPreeditItem item = preeditManager->item();
    if (candidateList.isEmpty()) return;
    if (item.to.length() != initialSizes.length()) return;
    qimsysDebugIn();
    QString preeditString = item.from.join("");
    QString url = QString("http://www.social-ime.com/api/?string=%1").arg(preeditString);
    for (int i = 0; i < initialSizes.length(); i++) {
        url.append(QString("&resize[%1]=%2").arg(i).arg(item.from.at(i).length() - initialSizes.at(i)));
    }
    initialSizes.clear();
    for (int i = 0; i < item.to.length(); i++) {
        int k = 0;
        for (int j = 0; j < candidateList.at(i).length(); j++) {
            if (candidateList.at(i).at(j).source == q->identifier()) {
                if (item.to.at(i) == candidateList.at(i).at(j).to) {
                    url.append(QString("&commit[%1]=%2").arg(i).arg(k));
                    break;
                }
                k++;
            }
        }
    }
    url.append(QString("&user=%1").arg(user));
    qimsysDebug() << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "QIMSYS");

    networkManager->get(request);
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

void Engine::Private::cancel()
{
    if (conversionReply) {
        qimsysWarning() << conversionReply;
        conversionReply->abort();
        conversionReply = 0;
    }
}

void Engine::Private::error(QNetworkReply::NetworkError err)
{
    qimsysDebugIn() << err;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qimsysDebug() << reply;
    reply->deleteLater();
    qimsysWarning() << err;
    qimsysDebugOut();
}

void Engine::Private::sslErrors(QList<QSslError> errs)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    qimsysWarning() << errs.count();
}

#ifndef QIMSYS_NO_GUI
QimsysSettingsWidget *Engine::Private::settings(const QString &hint, QWidget *parent)
{
    Q_UNUSED(hint);
    Settings *settings = new Settings(q, parent);
    connect(settings, SIGNAL(saved()), this, SLOT(saved()));
    return settings;
}
#endif

void Engine::Private::saved()
{
    QSettings settings;
    settings.beginGroup(q->metaObject()->className());
    user = settings.value("User").toString();
}

Engine::Engine(QObject *parent)
    : QimsysEngine(parent)
{
    d = new Private(this);
}

Engine::~Engine()
{
    delete d;
}

#ifndef QIMSYS_NO_GUI
QimsysSettingsWidget *Engine::settings(const QString &hint, QWidget *parent)
{
    return d->settings(hint, parent);
}
#endif

#include "engine.moc"
