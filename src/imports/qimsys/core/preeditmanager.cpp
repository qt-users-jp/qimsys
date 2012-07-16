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

#include "preeditmanager.h"

#include <qimsysdebug.h>
#include <qimsyspreeditmanager.h>
#include <qimsysinputmethodmanager.h>

#include <QtDeclarative/qdeclarative.h>

class PreeditManager::Private : public QimsysPreeditManager
{
    Q_OBJECT
public:
    Private(PreeditManager *parent);

private slots:
    void slotItemChanged(const QimsysPreeditItem &item);
private:
    PreeditManager *q;

public:
    PreeditItem *m_item;
    QimsysInputMethodManager inputMethodManager;
};

PreeditManager::Private::Private(PreeditManager *parent)
    : QimsysPreeditManager(parent)
    , q(parent)
    , m_item(new PreeditItem(this))
{
    qimsysDebugIn();
    inputMethodManager.init();
    init();
    connect(this, SIGNAL(itemChanged(QimsysPreeditItem)), this, SLOT(slotItemChanged(QimsysPreeditItem)));
    connect(this, SIGNAL(rectChanged(QRect)), q, SIGNAL(rectChanged(QRect)));
#ifndef QIMSYS_NO_GUI
    connect(this, SIGNAL(fontChanged(QFont)), q, SIGNAL(fontChanged(QFont)));
#endif
    connect(this, SIGNAL(cursorPositionChanged(int)), q, SIGNAL(cursorPositionChanged(int)));
    connect(this, SIGNAL(surroundingTextChanged(QString)), q, SIGNAL(surroundingTextChanged(QString)));
    connect(this, SIGNAL(currentSelectionChanged(QString)), q, SIGNAL(currentSelectionChanged(QString)));
    connect(this, SIGNAL(maximumTextLengthChanged(int)), q, SIGNAL(maximumTextLengthChanged(int)));
    connect(this, SIGNAL(committed(QString,qulonglong)), q, SIGNAL(committed(QString,qulonglong)));
    qimsysDebugOut();
}

void PreeditManager::Private::slotItemChanged(const QimsysPreeditItem &item)
{
    qimsysDebugIn();

    m_item = new PreeditItem(item, this);
    emit q->itemChanged(m_item);
    qimsysDebugOut();
}

PreeditManager::PreeditManager(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn();
    d = new Private(this);
    qimsysDebugOut();
}

bool PreeditManager::isEmpty() const
{
    return d->isEmpty();
}

void PreeditManager::insert(const QString &str)
{
    if (d->inputMethodManager.state() > 0x04) {
        d->inputMethodManager.execute(QLatin1String("Commit all"));
    }

    d->inputMethodManager.execute(QLatin1String("Clear Selection"));
    d->inputMethodManager.setState(0x04 /* Input */);
    d->insert(str);
}

void PreeditManager::replace(const QString &str)
{
    if (d->inputMethodManager.state() == 0x04) {
        d->replace(str);
    }
}

QString PreeditManager::text(int length) const
{
    QString ret;
    if (d->inputMethodManager.state() == 0x04) {
        ret = d->text(length);
    }
    return ret;
}

PreeditItem *PreeditManager::item() const
{
    return d->m_item;
}

void PreeditManager::setItem(PreeditItem *item)
{
    qimsysDebugIn() << item;
    if (d->inputMethodManager.state() > 0x04) {
        d->inputMethodManager.execute(QLatin1String("Commit all"));
    }

    d->inputMethodManager.execute(QLatin1String("Clear Selection"));
    if (item->to().isEmpty()) {
        d->inputMethodManager.setState(0x02 /* Empty */);
    } else {
        d->inputMethodManager.setState(0x04 /* Input */);
    }

    d->setItem(item->item());
    qimsysDebugOut();
}

QRect PreeditManager::rect() const { return d->rect(); }
#ifndef QIMSYS_NO_GUI
QFont PreeditManager::font() const { return d->font(); }
#endif
int PreeditManager::cursorPosition() const { return d->cursorPosition(); }
QString PreeditManager::surroundingText() const { return d->surroundingText(); }
QString PreeditManager::currentSelection() const { return d->currentSelection(); }
int PreeditManager::maximumTextLength() const { return d->maximumTextLength(); }

void PreeditManager::setRect(const QRect &rect) { d->setRect(rect); }
#ifndef QIMSYS_NO_GUI
void PreeditManager::setFont(const QFont &font) { d->setFont(font); }
#endif
void PreeditManager::setCursorPosition(int cursorPosition) { d->setCursorPosition(cursorPosition); }
void PreeditManager::setSurroundingText(const QString &surroundingText) { d->setSurroundingText(surroundingText); }
void PreeditManager::setCurrentSelection(const QString &currentSelection) { d->setCurrentSelection(currentSelection); }
void PreeditManager::setMaximumTextLength(int maximumTextLength) { d->setMaximumTextLength(maximumTextLength); }

void PreeditManager::commit(const QString &string, qulonglong target) { d->commit(string, target); }

QML_DECLARE_TYPE(PreeditManager);

#include "preeditmanager.moc"
