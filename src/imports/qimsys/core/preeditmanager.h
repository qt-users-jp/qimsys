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

#ifndef PREEDITMANAGER_H
#define PREEDITMANAGER_H

#include <QObject>
#include <QRect>
#include <QFont>

#include "preedititem.h"

class PreeditManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PreeditItem *item READ item WRITE setItem NOTIFY itemChanged)
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY rectChanged)
#ifndef QIMSYS_NO_GUI
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
#endif
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QString surroundingText READ surroundingText WRITE setSurroundingText NOTIFY surroundingTextChanged)
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY currentSelectionChanged)
    Q_PROPERTY(int maximumTextLength READ maximumTextLength WRITE setMaximumTextLength NOTIFY maximumTextLengthChanged)
    Q_DISABLE_COPY(PreeditManager)
public:
    explicit PreeditManager(QObject *parent = 0);

    Q_INVOKABLE bool isEmpty() const;
    Q_INVOKABLE void insert(const QString &str);
    Q_INVOKABLE void replace(const QString &str);
    Q_INVOKABLE QString text(int length) const;

public slots:
    PreeditItem *item() const;
    QRect rect() const;
#ifndef QIMSYS_NO_GUI
    QFont font() const;
#endif
    int cursorPosition() const;
    QString surroundingText() const;
    QString currentSelection() const;
    int maximumTextLength() const;

public slots:
    void setItem(PreeditItem *item);
    void setRect(const QRect &rect);
#ifndef QIMSYS_NO_GUI
    void setFont(const QFont &font);
#endif
    void setCursorPosition(int cursorPosition);
    void setSurroundingText(const QString &surroundingText);
    void setCurrentSelection(const QString &currentSelection);
    void setMaximumTextLength(int maximumTextLength);

    void commit(const QString &string, qulonglong target);

signals:
    void itemChanged(PreeditItem *item);
    void rectChanged(const QRect &rect);
#ifndef QIMSYS_NO_GUI
    void fontChanged(const QFont &font);
#endif
    void cursorPositionChanged(int cursorPosition);
    void surroundingTextChanged(const QString &surroundingText);
    void currentSelectionChanged(const QString &currentSelection);
    void maximumTextLengthChanged(int maximumTextLength);

    void committed(const QString &commitString, qulonglong target);

private:
    class Private;
    Private *d;
};

#endif // PREEDITMANAGER_H
