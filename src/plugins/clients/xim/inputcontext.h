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

#ifndef INPUTCONTEXT_H
#define INPUTCONTEXT_H

#include <QWidget>
typedef struct _XIMS *XIMS;

class InputContext : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(InputContext)
    Q_PROPERTY(quint16 connectId READ connectId WRITE setConnectId)
    Q_PROPERTY(qint32 inputStyle READ inputStyle WRITE setInputStyle)
    Q_PROPERTY(unsigned long clientWindow READ clientWindow WRITE setClientWindow)
    Q_PROPERTY(unsigned long focusWindow READ focusWindow WRITE setFocusWindow)
    Q_PROPERTY(QRect area READ area WRITE setArea)
    Q_PROPERTY(QRect areaNeeded READ areaNeeded WRITE setAreaNeeded)
    Q_PROPERTY(QPoint spotLocation READ spotLocation WRITE setSpotLocation)
    Q_PROPERTY(unsigned long colorMap READ colorMap WRITE setColorMap)
    Q_PROPERTY(unsigned long stdColorMap READ stdColorMap WRITE setStdColorMap)
    Q_PROPERTY(unsigned long foreground READ foreground WRITE setForeground)
    Q_PROPERTY(unsigned long background READ background WRITE setBackground)
    Q_PROPERTY(unsigned long backgroundPixmap READ backgroundPixmap WRITE setBackgroundPixmap)
    Q_PROPERTY(QString fontSet READ fontSet WRITE setFontSet)
    Q_PROPERTY(unsigned long lineSpace READ lineSpace WRITE setLineSpace)
    Q_PROPERTY(unsigned long cursor READ cursor WRITE setCursor)
    Q_PROPERTY(bool hasFocus READ hasFocus WRITE setFocus)

public:
    explicit InputContext(XIMS xims, QObject *parent = 0);
    ~InputContext();

    void sendCommitString(const QString &commitString);
    void sendPreeditString(const QString &preeditString, int cursorPosition, int selectionLength = 0);

    unsigned long id() const;
    quint16 connectId() const;
    void setConnectId(quint16 connectId);

    qint32 inputStyle() const;
    void setInputStyle(qint32 inputStyle);

    unsigned long clientWindow() const;
    void setClientWindow(unsigned long clientWindow);

    unsigned long focusWindow() const;
    void setFocusWindow(unsigned long focusWindow);

    QRect area() const;
    void setArea(const QRect &area);

    QRect areaNeeded() const;
    void setAreaNeeded(const QRect &areaNeeded);

    const QPoint &spotLocation() const;
    void setSpotLocation(const QPoint &spotLocation);

    unsigned long colorMap() const;
    void setColorMap(unsigned long colorMap);

    unsigned long stdColorMap() const;
    void setStdColorMap(unsigned long stdColorMap);

    unsigned long foreground() const;
    void setForeground(unsigned long foreground);

    unsigned long background() const;
    void setBackground(unsigned long background);

    unsigned long backgroundPixmap() const;
    void setBackgroundPixmap(unsigned long backgroundPixmap);

    const QString &fontSet() const;
    void setFontSet(const QString &fontSet);

    unsigned long lineSpace() const;
    void setLineSpace(unsigned long lineSpace);

    unsigned long cursor() const;
    void setCursor(unsigned long cursor);

    bool hasFocus() const;
    void setFocus(bool focus);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);
    void setModifier(Qt::KeyboardModifier modifier, bool on);

    void reset();

private:
    class Private;
    Private *d;
};

#endif // INPUTCONTEXT_H
