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

#include "dictionarymodel.h"
#include "libanthydic.h"

#include <qimsysdebug.h>

#include <QTextCodec>

using namespace Japanese::Anthy;

class DictionaryModel::Private : private QObject
{
    Q_OBJECT
    enum DataState {
        NotModified,
        Inserted,
        Modified,
        Deleted,
    };
    struct Data {
        QString sound;
        QString spelling;
        QString wordType;
        int frequency;
        DataState state;
    };
public:
    Private(DictionaryModel *parent);
    ~Private();

    void save();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool insertRows(int row, int count);
    bool removeRows(int row, int count);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    DictionaryModel *q;
    LibAnthyDic libanthydic;
    QTextCodec *eucjp;
    QList<Data> list;
    QList<Data> listDeleted;
};

DictionaryModel::Private::Private(DictionaryModel *parent)
    : QObject(parent)
    , q(parent)
{
    if (!libanthydic.isAvailable()) return;
    eucjp = QTextCodec::codecForName("EUC-JP");
    Q_ASSERT(eucjp);

    libanthydic.anthy_dic_util_init();
    int ret = libanthydic.anthy_priv_dic_select_first_entry();
    switch (ret) {
    case ANTHY_DIC_UTIL_ERROR:
        qimsysWarning() << "ANTHY_DIC_UTIL_ERROR";
        break;
    case ANTHY_DIC_UTIL_DUPLICATE:
        qimsysWarning() << "ANTHY_DIC_UTIL_DUPLICATE";
        break;
    case ANTHY_DIC_UTIL_INVALID:
        qimsysWarning() << "ANTHY_DIC_UTIL_INVALID";
        break;
    }
    char buf[8192];
    while (ret == ANTHY_DIC_UTIL_OK) {
        Data d;
        libanthydic.anthy_priv_dic_get_index(buf, sizeof(buf));
        d.sound = eucjp->toUnicode(buf);
        libanthydic.anthy_priv_dic_get_word(buf, sizeof(buf));
        d.spelling = eucjp->toUnicode(buf);
        libanthydic.anthy_priv_dic_get_wtype(buf, sizeof(buf));
        d.wordType = eucjp->toUnicode(buf);
        d.frequency = libanthydic.anthy_priv_dic_get_freq();
        d.state = NotModified;

        list.append(d);
        ret = libanthydic.anthy_priv_dic_select_next_entry();
    }
}

DictionaryModel::Private::~Private()
{
    if (!libanthydic.isAvailable()) return;
    libanthydic.anthy_dic_util_quit();
}

void DictionaryModel::Private::save()
{
    foreach(const Data &d, listDeleted + list) {
        switch (d.state) {
        case Modified:
        case Inserted:
        case Deleted:
            libanthydic.anthy_priv_dic_add_entry(eucjp->fromUnicode(d.sound).constData(), eucjp->fromUnicode(d.spelling).constData(), d.wordType.toLatin1().constData(), d.frequency);
            break;
        default:
            break;
        }
    }
    for (int i = 0; i < list.count(); i++) {
        list[i].state = NotModified;
    }
    listDeleted.clear();
}

int DictionaryModel::Private::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return list.count();
}

int DictionaryModel::Private::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return Count;
}

QVariant DictionaryModel::Private::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    Data d = list.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case Sound:
            ret = d.sound;
            break;
        case Spelling:
            ret = d.spelling;
            break;
        case WordType:
            ret = d.wordType;
            break;
        case Frequency:
            ret = d.frequency;
            break;
        }
        break;
    case  Sound + Qt::UserRole:
        ret = d.sound;
        break;
    case  Spelling + Qt::UserRole:
        ret = d.spelling;
        break;
    case  WordType + Qt::UserRole:
        ret = d.wordType;
        break;
    case  Frequency + Qt::UserRole:
        ret = d.frequency;
        break;
    default:
        break;
    }
    return ret;
}

bool DictionaryModel::Private::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = q->QAbstractTableModel::setData(index, value, role);
    int row = index.row();
    int column = index.column();
    Data d = list.at(row);
    switch (role) {
    case Qt::EditRole:
        switch (column) {
        case Sound:
            if (d.sound != value.toString()) {
                d.sound = value.toString();
                d.state = Modified;
            }
            break;
        case Spelling:
            if (d.spelling != value.toString()) {
                d.spelling = value.toString();
                d.state = Modified;
            }
            break;
        case WordType:
            if (d.wordType != value.toString()) {
                d.wordType = value.toString();
                d.state = Modified;
            }
            break;
        case Frequency:
            if (d.frequency != value.toInt()) {
                d.frequency = value.toInt();
                d.state = Modified;
            }
            break;
        }
        break;
    default:
        break;
    }

    Data orig = list.at(row);
    if (orig.state != Inserted &&
            (d.sound != orig.sound ||
             d.spelling != orig.spelling ||
             d.wordType != orig.wordType
            )) {
        orig.state = Deleted;
        orig.frequency = 0;
        listDeleted.append(orig);
    }
    list[row] = d;
    return ret;
}

bool DictionaryModel::Private::insertRows(int row, int count)
{
    for (int i = 0; i < count; i++) {
        Data d;
        d.state = Inserted;
        list.insert(row + i, d);
    }
    return true;
}

bool DictionaryModel::Private::removeRows(int row, int count)
{
    for (int i = 0; i < count; i++) {
        Data d = list.at(row);
        d.state = Deleted;
        d.frequency = 0;
        listDeleted.append(d);
        list.removeAt(row);
    }
    return true;
}

QVariant DictionaryModel::Private::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret = q->QAbstractTableModel::headerData(section, orientation, role);
    switch (orientation) {
    case Qt::Horizontal:
        switch (role) {
        case Qt::DisplayRole:
            switch (section) {
            case Sound:
                ret = tr("Sound");
                break;
            case Spelling:
                ret = tr("Spelling");
                break;
            case WordType:
                ret = tr("Word type");
                break;
            case Frequency:
                ret = tr("Frequency");
                break;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return ret;
}

DictionaryModel::DictionaryModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    d = new Private(this);
}

DictionaryModel::~DictionaryModel()
{
    delete d;
}

void DictionaryModel::save()
{
    d->save();
}

int DictionaryModel::rowCount(const QModelIndex &parent) const
{
    return d->rowCount(parent);
}

int DictionaryModel::columnCount(const QModelIndex &parent) const
{
    return d->columnCount(parent);
}

QVariant DictionaryModel::data(const QModelIndex &index, int role) const
{
    return d->data(index, role);
}

bool DictionaryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->setData(index, value, role);
}

bool DictionaryModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool ret = QAbstractTableModel::insertRows(row, count, parent);
    beginInsertRows(parent, row, row + count - 1);
    ret = d->insertRows(row, count);
    endInsertRows();
    return ret;
}

bool DictionaryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool ret = QAbstractTableModel::removeRows(row, count, parent);
    beginRemoveRows(parent, row, row + count - 1);
    ret = d->removeRows(row, count);
    endRemoveRows();
    return ret;
}

QVariant DictionaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->headerData(section, orientation, role);
}

QHash<int,QByteArray> DictionaryModel::roleNames() const
{
    QHash<int, QByteArray> ret;
    ret[Sound + Qt::UserRole] = "sound";
    ret[Spelling + Qt::UserRole] = "spelling";
    ret[WordType + Qt::UserRole] = "wordType";
    ret[Frequency + Qt::UserRole] = "frequency";
    return ret;
}

QVariantList DictionaryModel::rawData(int row) const
{
    qimsysDebugIn() << row;
    QVariantList ret;
    ret.append(data(index(row, Sound)));
    ret.append(data(index(row, Spelling)));
    ret.append(data(index(row, WordType)));
    ret.append(data(index(row, Frequency)));
    qimsysDebugOut() << ret;
    return ret;
}

void DictionaryModel::setRawData(int row, QVariantList rawData)
{
    qimsysDebugIn() << row;
    if (rawData.isEmpty()) {
        removeRow(row);
    } else {
        if (row == -1) {
            insertRow(0);
            row = 0;
        }
        setData(index(row, Sound), rawData.takeFirst());
        setData(index(row, Spelling), rawData.takeFirst());
        setData(index(row, WordType), rawData.takeFirst());
        setData(index(row, Frequency), rawData.takeFirst());
    }
    save();
    qimsysDebugOut();
}

#include "dictionarymodel.moc"
