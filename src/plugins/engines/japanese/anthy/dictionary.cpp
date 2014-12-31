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

#include "dictionary.h"
#include "dictionarymodel.h"
#include "wordeditor.h"

using namespace Japanese::Anthy;

class Dictionary::Private
{
public:
    DictionaryModel *model;
};

Dictionary::Dictionary(QObject *parent)
    : QimsysEngineDictionary(parent)
{
    d = new Private;
    d->model = 0;
}

Dictionary::~Dictionary()
{
    delete d;
}

void Dictionary::save()
{
    d->model->save();
}

QAbstractTableModel *Dictionary::model()
{
    if (!d->model) {
        d->model = new DictionaryModel(this);
    }
    return d->model;
}

void Dictionary::add(QWidget *parent)
{
    WordEditor editor(parent);
    if (editor.exec()) {
        int row = d->model->rowCount();
        d->model->insertRow(row);
        d->model->setData(d->model->index(row, DictionaryModel::Sound), editor.sound());
        d->model->setData(d->model->index(row, DictionaryModel::Spelling), editor.spelling());
        d->model->setData(d->model->index(row, DictionaryModel::WordType), editor.wordType());
        d->model->setData(d->model->index(row, DictionaryModel::Frequency), editor.frequency());
    }
}

void Dictionary::edit(const QModelIndex &index, QWidget *parent)
{
    WordEditor editor(parent);
    int row = index.row();
    editor.setSound(d->model->data(d->model->index(row, DictionaryModel::Sound)).toString());
    editor.setSpelling(d->model->data(d->model->index(row, DictionaryModel::Spelling)).toString());
    editor.setWordType(d->model->data(d->model->index(row, DictionaryModel::WordType)).toString());
    editor.setFrequency(d->model->data(d->model->index(row, DictionaryModel::Frequency)).toInt());
    if (editor.exec()) {
        d->model->setData(d->model->index(row, DictionaryModel::Sound), editor.sound());
        d->model->setData(d->model->index(row, DictionaryModel::Spelling), editor.spelling());
        d->model->setData(d->model->index(row, DictionaryModel::WordType), editor.wordType());
        d->model->setData(d->model->index(row, DictionaryModel::Frequency), editor.frequency());
    }
}

void Dictionary::del(const QModelIndex &index, QWidget *parent)
{
    Q_UNUSED(parent)
    int row = index.row();
    d->model->removeRow(row, index.parent());
}
