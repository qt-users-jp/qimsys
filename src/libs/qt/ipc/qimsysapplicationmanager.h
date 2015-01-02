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

#ifndef QIMSYSAPPLICATIONMANAGER_H
#define QIMSYSAPPLICATIONMANAGER_H

#include "qimsysglobal.h"
#include "qimsysabstractipcobject.h"

#ifndef QIMSYS_NO_GUI
#include <QIcon>
#endif

class QIMSYS_EXPORT QimsysApplicationManager : public QimsysAbstractIpcObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayLanguage READ displayLanguage WRITE setDisplayLanguage NOTIFY displayLanguageChanged)
    Q_PROPERTY(bool focus READ focus WRITE setFocus NOTIFY focusChanged)
    Q_PROPERTY(qulonglong window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(qulonglong widget READ widget WRITE setWidget NOTIFY widgetChanged)
    Q_PROPERTY(bool composing READ composing WRITE setComposing NOTIFY composingChanged)
#ifndef QIMSYS_NO_GUI
    Q_PROPERTY(QIcon currentIcon READ currentIcon WRITE setCurrentIcon NOTIFY currentIconChanged)
#endif
    Q_ENUMS(QimsysApplicationManager::ActionType)
    Q_CLASSINFO("D-Bus Interface", "local.ApplicationManager")
    Q_DISABLE_COPY(QimsysApplicationManager)
public:
    enum ActionType {
        Reset,
        ShowDictionary,
        ShowSettings,
        ShowAboutQimsys,
        ShowController,
    };

    explicit QimsysApplicationManager(QObject *parent = 0, Type type = Client);
    ~QimsysApplicationManager();
    virtual bool init();

    Q_INVOKABLE bool focus() const;
    Q_INVOKABLE qulonglong window() const;
    Q_INVOKABLE qulonglong widget() const;
    Q_INVOKABLE bool composing() const;
    Q_INVOKABLE QString displayLanguage() const;
#ifndef QIMSYS_NO_GUI
    Q_INVOKABLE QIcon currentIcon() const;
#endif

public slots:
    void setFocus(bool focus);
    void setWindow(qulonglong window);
    void setWidget(qulonglong widget);
    void setComposing(bool composing);
    void setDisplayLanguage(const QString &displayLanguage);
#ifndef QIMSYS_NO_GUI
    void setCurrentIcon(const QIcon &icon);
#endif

    void exec(int action);
    void settingsUpdate(const QString &name);

signals:
    void focusChanged(bool focus);
    void windowChanged(qulonglong window);
    void widgetChanged(qulonglong widget);
    void composingChanged(bool composing);
    void displayLanguageChanged(const QString &displayLanguage);
#ifndef QIMSYS_NO_GUI
    void currentIconChanged(const QIcon &currentIcon);
#endif

    void triggered(int action);
    void settingsUpdated(const QString &name);

protected:
    QimsysAbstractIpcObject *server() const;

private:
    class Private;
    Private *d;
};

#ifndef QIMSYS_NO_DBUS

#ifndef QIMSYS_NO_GUI
DBUSARGUMENT_IN(QIcon &e,
                arg.beginStructure();
                QByteArray ba;
                arg >> ba;
{
    QPixmap pix;
    QDataStream stream(ba);
    stream >> pix;
    e.addPixmap(pix);
}
arg.endStructure();
               )

#include <QBuffer>

DBUSARGUMENT_OUT(const QIcon &e,
                 arg.beginStructure();
                 QByteArray ba;
{
    QBuffer buffer(&ba);
    buffer.open(QBuffer::WriteOnly);
    QDataStream stream(&buffer);
    stream << e.pixmap(32);
}
arg << ba;
arg.endStructure();
                )

#endif // QIMSYS_NO_GUI

#endif // QIMSYS_NO_DBUS

#endif // QIMSYSAPPLICATIONMANAGER_H
