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

#include <plugins/inputmethods/japanese/standard/global.h>

#include <qimsysdebug.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsyscandidatemanager.h>
#include <qimsysdynamictranslator.h>

#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTextCodec>

namespace Japanese {
    namespace GoogleIME {

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

    void convert();
    void readConversion();

    void resize();
    void setCandidates();

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
    QList<QimsysConversionItemList> candidateList;
};

    }
}

using namespace Japanese::GoogleIME;

Engine::Private::Private(Engine *parent)
    : QObject(parent)
    , q(parent)
    , inputMethodManager(0)
    , preeditManager(0)
    , candidateManager(0)
    , networkManager(0)
    , conversionReply(0)
{
    qimsysDebugIn() << parent;

    q->setIdentifier(QLatin1String("Google IME"));
    q->setPriority(0x100);

    q->setLocale(QLatin1String("ja_JP"));
    q->setLanguage("Japanese(Standard)");
#ifndef QIMSYS_NO_GUI
    q->setIcon(QIcon(":/icons/googleime.png"));
#endif
    trConnect(this, QT_TR_NOOP("Google IME Engine"), q, "name");
    trConnect(this, QT_TR_NOOP("Tasuku Suzuki"), q, "author");
    trConnect(this, QT_TR_NOOP(""), q, "translator");
    trConnect(this, QT_TR_NOOP("See <a href=\"http://www.google.com/intl/ja/ime/cgiapi.html\">http://www.google.com/intl/ja/ime/cgiapi.html</a> for detail."), q, "description");

    q->setCategoryType(CanBeNone);
    trConnect(this, QT_TR_NOOP("Input/Conversion Engine"), q, "categoryName");

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
        candidateList.clear();
        clearCandidates();
        break;
    case Empty:
        fallthrough;
    case Input:
        clearCandidates();
        candidateList.clear();
        preeditIndex = -1;
        break;
    case Convert:
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
    qimsysDebugIn() << item;
    switch (currentState) {
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
    default:
        break;
    }

    qimsysDebugOut();
}

void Engine::Private::convert()
{
    if (!candidateList.isEmpty()) return;
    qimsysDebugIn();

    QimsysPreeditItem item = preeditManager->item();
    QString preeditString = item.from.join("");

    qimsysDebug() << preeditString;

    QString url = QString("http://www.google.com/transliterate?langpair=ja-Hira|ja&text=%1").arg(preeditString);
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "QIMSYS");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));

    cancel();
    conversionReply = reply;
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

    qimsysDebugIn();
    qimsysDebug() << "preeditIndex =" << preeditIndex;

    candidateList.clear();
    QStringList cache = item.to;
    while (cache.length() > preeditIndex) cache.takeLast();
    qimsysDebug() << cache;

    item.to.clear();
    item.from.clear();
    item.rawString.clear();

    QTextCodec *textCodec = 0;
    foreach (const QByteArray &headerName, reply->rawHeaderList()) {
        if (headerName == "Content-Type") {
            QRegExp charset("charset=([A-Za-z0-9\\-_]+)");
            if (charset.indexIn(reply->rawHeader(headerName))) {
                qimsysDebug() << headerName << reply->rawHeader(headerName) << charset.cap(1);
                textCodec = QTextCodec::codecForName(charset.cap(1).toUtf8());
            }
        }
    }

    QString data = textCodec ? textCodec->toUnicode(reply->readAll()) : QString::fromUtf8(reply->readAll());
    qimsysDebug() << data;

    int level = 0;
    foreach (QString line, QString(data).split(QLatin1String("\n"))) {
        if (line.isEmpty()) continue;
        qimsysDebug() << line;
        switch (line.at(0).unicode()) {
        case '[':
            level++;
            break;
        case ']':
            level--;
            break;
        default:
            switch (level) {
            case 2:
                item.from.append(line.mid(1, line.length() - 3));
                item.rawString.append(QString());
                break;
            case 3: {
                QStringList tos = line.mid(1, line.length() - 2).split(QLatin1String("\",\""));
                qimsysDebug() << tos;
                QString to = tos.first();
                if (preeditIndex == candidateList.length()) {
                    item.cursor = item.to.join("").length();
                    item.selection = to.length();
                }
                qimsysDebug() << item.to << cache;
                if (item.to.length() < cache.length()) {
                    item.to.append(cache.at(item.to.length()));
                } else {
                    item.to.append(to);
                }
                qimsysDebug() << item.to << cache;

                QimsysConversionItemList candidates;
                for (int i = 0; i < tos.length(); i++) {
                    QimsysConversionItem candidate;
                    candidate.from = item.from.last();
                    candidate.to = tos.at(i);
                    candidate.index = i;
                    candidate.source = q->identifier();
                    candidates.append(candidate);
                }
                candidateList.append(candidates);
                break; }
            default:
                qimsysWarning() << level << line;
                break;
            }
            break;
        }
    }
    qimsysDebug() << item;

    preeditManager->blockSignals(true);
    preeditManager->setItem(item);
    preeditManager->blockSignals(false);
    qimsysDebugOut();
}

void Engine::Private::resize()
{
    QimsysPreeditItem item = preeditManager->item();
    if (item.to.isEmpty()) return;

    qimsysDebugIn();

    QString preeditString;
    qimsysDebug() << item.from << preeditIndex;
    while (item.from.length() > preeditIndex + 2) {
        qimsysDebug() << item.from.last();
        preeditString.prepend(item.from.takeLast());
    }
    preeditString.prepend(item.from.join(","));

    qDebug() << preeditString;

    QString url = QString("http://www.google.com/transliterate?langpair=ja-Hira|ja&text=%1").arg(preeditString);
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "QIMSYS");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(readConversion()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError))
            , this, SLOT(error(QNetworkReply::NetworkError)));

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

Engine::Engine(QObject *parent)
    : QimsysEngine(parent)
{
    d = new Private(this);
}

Engine::~Engine()
{
    delete d;
}

#include "engine.moc"
