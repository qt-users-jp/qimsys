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

#ifndef WORDEDITOR_H
#define WORDEDITOR_H

#include <QDialog>

namespace Japanese {
    namespace Anthy {

class WordEditor : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString sound READ sound WRITE setSound)
    Q_PROPERTY(QString spelling READ spelling WRITE setSpelling)
    Q_PROPERTY(QString wordType READ wordType WRITE setWordType)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency)
public:
    WordEditor(QWidget *parent = 0);
    ~WordEditor();

    QString sound() const;
    void setSound(const QString &sound);

    QString spelling() const;
    void setSpelling(const QString &spelling);

    QString wordType() const;
    void setWordType(const QString &wordType);

    int frequency() const;
    void setFrequency(int frequency);

private:
    class Private;
    Private *d;
};

    }
}

#endif // WORDEDITOR_H
