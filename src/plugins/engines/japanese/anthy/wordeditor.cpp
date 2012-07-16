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

#include "wordeditor.h"
#include "ui_wordeditor.h"

using namespace Japanese::Anthy;

struct WordTypeDef {
    const char *wordType;
    bool yn[5];
};

static const WordTypeDef NounTable[] = {
    { "#T00", { true , true , true , true , true  } },
    { "#T02", { true , true , true , true , false } },
    { "#T03", { true , true , true , false, true  } },
    { "#T04", { true , true , true , false, false } },
    { "#T05", { true , true , false, true , true  } },
    { "#T07", { true , true , false, true , false } },
    { "#T08", { true , true , true , false, false } },
    { "#T09", { true , true , false, false, false } },
    { "#T10", { true , false, true , true , true  } },
    { "#T12", { true , false, true , true , false } },
    { "#T13", { true , false, true , false, true  } },
    { "#T14", { true , false, true , false, false } },
    { "#T15", { true , false, false, true , true  } },
    { "#T17", { true , false, false, true , false } },
    { "#T18", { true , false, false, false, true  } },
    { "#T19", { true , false, false, false, false } },
    { "#T20", { false, true , true , true , true  } },
    { "#T22", { false, true , true , true , false } },
    { "#T23", { false, true , true , false, true  } },
    { "#T24", { false, true , true , false, false } },
    { "#T25", { false, true , false, true , true  } },
    { "#T27", { false, true , false, true , false } },
    { "#T28", { false, true , false, false, true  } },
    { "#T29", { false, true , false, false, false } },
    { "#T30", { false, false, true , true , true  } },
    { "#T32", { false, false, true , true , false } },
    { "#T33", { false, false, true , false, true  } },
    { "#T34", { false, false, true , false, false } },
    { "#T35", { false, false, false, true , true  } },
    { "#T37", { false, false, false, true , false } },
    { "#T38", { false, false, false, false, true  } },
    { "#T39", { false, false, false, false, false } },
    { 0, { 0, 0, 0, 0, 0 } },
};

static const WordTypeDef AdjectiveTable[] = {
    { "#F00", { true , true , true , true , true  } },
    { "#F01", { true , true , true , false, true  } },
    { "#F02", { true , true , false, true , true  } },
    { "#F03", { true , true , false, false, true  } },
    { "#F04", { true , false, true , true , true  } },
    { "#F05", { true , false, true , false, true  } },
    { "#F06", { true , false, false, true , true  } },
    { "#F07", { true , false, false, false, true  } },
    { "#F08", { false, true , true , true , true  } },
    { "#F09", { false, true , true , false, true  } },
    { "#F10", { false, true , false, true , true  } },
    { "#F11", { false, true , false, false, true  } },
    { "#F12", { false, false, true , true , true  } },
    { "#F13", { false, false, true , false, true  } },
    { "#F14", { false, false, false, true , true  } },
    { 0, { 0, 0, 0, 0, 0 } },
};

class WordEditor::Private : private QObject
{
    Q_OBJECT
public:
    Private(WordEditor *parent);
    ~Private();

    QString wordType() const;
    void setWordType(const QString &wordType);

private slots:
    void spellingChanged(const QString &spelling);
    void wordTypeChanged(int index);

private:
    enum WordType {
        GeneralNoun,
        NameOfPeople,
        GeographicName,
        Adjective,
        Adverb,
        SingleKanjiCharacter,
    };
    WordEditor *q;
public:
    Ui::WordEditor ui;
};

WordEditor::Private::Private(WordEditor *parent)
    : QObject(parent)
    , q(parent)
{
    ui.setupUi(q);
    connect(ui.spelling, SIGNAL(textChanged(QString)), this, SLOT(spellingChanged(QString)));
    connect(ui.wordType, SIGNAL(currentIndexChanged(int)), this, SLOT(wordTypeChanged(int)));
}

WordEditor::Private::~Private()
{
}

void WordEditor::Private::spellingChanged(const QString &spelling)
{
    ui.nounNa->setText(tr("%1na").arg(spelling));
    ui.nounSa->setText(tr("%1sa").arg(spelling));
    ui.nounSuru->setText(tr("%1suru").arg(spelling));
    ui.nounOnly->setText(tr("%1").arg(spelling));
    ui.nounGa->setText(tr("%1ga").arg(spelling));
    ui.adjectiveTo->setText(tr("%1to").arg(spelling));
    ui.adjectiveSuru->setText(tr("%1suru").arg(spelling));
    ui.adjectiveTaru->setText(tr("%1taru").arg(spelling));
    ui.adjectiveOnly->setText(tr("%1").arg(spelling));
}

void WordEditor::Private::wordTypeChanged(int index)
{
    switch (index) {
    case GeneralNoun:
        ui.stackedWidget->setCurrentWidget(ui.generalNoun);
        ui.stackedWidget->show();
        break;
    case Adjective:
        ui.stackedWidget->setCurrentWidget(ui.adjective);
        ui.stackedWidget->show();
        break;
    default:
        ui.stackedWidget->hide();
        break;
    }
}

QString WordEditor::Private::wordType() const
{
    QString ret;
    switch (ui.wordType->currentIndex()) {
    case GeneralNoun: {
        int i = 0;
        while (NounTable[i].wordType) {
            if (NounTable[i].yn[0] == ui.nounNa->isChecked() &&
                    NounTable[i].yn[1] == ui.nounSa->isChecked() &&
                    NounTable[i].yn[2] == ui.nounSuru->isChecked() &&
                    NounTable[i].yn[3] == ui.nounOnly->isChecked() &&
                    NounTable[i].yn[4] == ui.nounGa->isChecked()) {
                ret = NounTable[i].wordType;
                break;
            }
            i++;
        }
        break;
    }
    case NameOfPeople:
        ret = QLatin1String("#JNM");
        break;
    case GeographicName:
        ret = QLatin1String("#CN");
        break;
    case Adjective: {
        int i = 0;
        while (AdjectiveTable[i].wordType) {
            if (AdjectiveTable[i].yn[0] == ui.adjectiveTo->isChecked() &&
                    AdjectiveTable[i].yn[1] == ui.adjectiveTaru->isChecked() &&
                    AdjectiveTable[i].yn[2] == ui.adjectiveSuru->isChecked() &&
                    AdjectiveTable[i].yn[3] == ui.adjectiveOnly->isChecked()) {
                ret = AdjectiveTable[i].wordType;
                break;
            }
            i++;
        }
        break;
    }
    case Adverb:
        ret = QLatin1String("#KY");
        break;
    case SingleKanjiCharacter:
        ret = QLatin1String("#KJ");
        break;
    }
    return ret;
}

void WordEditor::Private::setWordType(const QString &wordType)
{
    if (wordType.startsWith(QLatin1String("#T"))) {
        ui.wordType->setEditable(false);
        ui.wordType->setCurrentIndex(GeneralNoun);
        ui.wordType->setEnabled(true);
        int i = 0;
        while (NounTable[i].wordType) {
            if (wordType == NounTable[i].wordType) {
                ui.nounNa->setCheckState(NounTable[i].yn[0] ? Qt::Checked : Qt::Unchecked);
                ui.nounSa->setCheckState(NounTable[i].yn[1] ? Qt::Checked : Qt::Unchecked);
                ui.nounSuru->setCheckState(NounTable[i].yn[2] ? Qt::Checked : Qt::Unchecked);
                ui.nounOnly->setCheckState(NounTable[i].yn[3] ? Qt::Checked : Qt::Unchecked);
                ui.nounGa->setCheckState(NounTable[i].yn[4] ? Qt::Checked : Qt::Unchecked);
            }
            i++;
        }
    } else if (wordType == QLatin1String("#JNM")) {
        ui.wordType->setEditable(false);
        ui.wordType->setCurrentIndex(NameOfPeople);
        ui.wordType->setEnabled(true);
    } else if (wordType == QLatin1String("#CN")) {
        ui.wordType->setEditable(false);
        ui.wordType->setCurrentIndex(GeographicName);
        ui.wordType->setEnabled(true);
    } else if (wordType.startsWith(QLatin1String("#F"))) {
        ui.wordType->setEditable(false);
        ui.wordType->setCurrentIndex(Adjective);
        ui.wordType->setEnabled(true);
        int i = 0;
        while (AdjectiveTable[i].wordType) {
            if (wordType == AdjectiveTable[i].wordType) {
                ui.adjectiveTo->setCheckState(AdjectiveTable[i].yn[0] ? Qt::Checked : Qt::Unchecked);
                ui.adjectiveTaru->setCheckState(AdjectiveTable[i].yn[1] ? Qt::Checked : Qt::Unchecked);
                ui.adjectiveSuru->setCheckState(AdjectiveTable[i].yn[2] ? Qt::Checked : Qt::Unchecked);
                ui.adjectiveOnly->setCheckState(AdjectiveTable[i].yn[3] ? Qt::Checked : Qt::Unchecked);
            }
            i++;
        }
    } else if (wordType == QLatin1String("#KY")) {
        ui.wordType->setEditable(false);
        ui.wordType->setCurrentIndex(Adverb);
        ui.wordType->setEnabled(true);
    } else if (wordType == QLatin1String("#KJ")) {
        ui.wordType->setEditable(false);
        ui.wordType->setCurrentIndex(SingleKanjiCharacter);
        ui.wordType->setEnabled(true);
    } else {
        ui.wordType->setEditable(true);
        ui.wordType->setEditText(wordType);
        ui.wordType->setEnabled(false);
    }
}

WordEditor::WordEditor(QWidget *parent)
    : QDialog(parent)
{
    d = new Private(this);
}

WordEditor::~WordEditor()
{
    delete d;
}

QString WordEditor::sound() const
{
    return d->ui.sound->text();
}

void WordEditor::setSound(const QString &sound)
{
    d->ui.sound->setText(sound);
}

QString WordEditor::spelling() const
{
    return d->ui.spelling->text();
}

void WordEditor::setSpelling(const QString &spelling)
{
    d->ui.spelling->setText(spelling);
}

QString WordEditor::wordType() const
{
    return d->wordType();
}

void WordEditor::setWordType(const QString &wordType)
{
    d->setWordType(wordType);
}

int WordEditor::frequency() const
{
    return d->ui.frequency->value();
}

void WordEditor::setFrequency(int frequency)
{
    d->ui.frequency->setValue(frequency);
}

#include "wordeditor.moc"
