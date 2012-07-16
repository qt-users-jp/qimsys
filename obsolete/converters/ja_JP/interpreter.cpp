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

#include "interpreter.h"
#include "preedit.h"
#include "inputmanager.h"
#include <qimsysdebug.h>

#include <QDir>
#include <QFile>
#include <QMap>
#include <QPair>
#include <QTextStream>
#include <QTextCodec>

using namespace ja_JP;

class Interpreter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Interpreter *parent);
    ~Private();

    QString rawString() const;
    QString inputString(int character = -1) const;

    void input(const QString &string);
    void remove(int length);
    void termiateInput();
    void clear();

private:
    QString rawStringFor(const QString str);

private:
    enum RemovingState {
        Before,
        Removing,
        After,
    };
    Interpreter *q;
    InputManager *state;
    Preedit *preedit;
    QMap< Typing, QMap<QString, QString> > from2to;
    QMap< Typing, QMap<QString, QString> > to2from;
    QStringList rawList;
    QStringList inputList;
    QMap< QPair<int, int>, QMap<QString, QString> > ch2ch;
public:
};

Interpreter::Private::Private(Interpreter *parent)
    : QObject(parent)
    , q(parent)
    , state(InputManager::instance())
    , preedit(Preedit::instance())
{
    qimsysDebugIn() << parent;
    QMap<QString, Typing> typingMap;
    typingMap["roma"] = Roma;
    typingMap["kana"] = Kana;
    typingMap["kyuriex"] = KyuriEx;
    QDir dir(":/typing/");
    qimsysDebug();
    foreach(const QString &text, dir.entryList(QStringList() << "*.txt", QDir::Files, QDir::Name)) {
        qimsysDebug() << text;
        QString baseName = QFileInfo(text).baseName();
        QFile file(dir.absoluteFilePath(text));
        Typing typing = static_cast<Typing>(typingMap[baseName]);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream s(&file);
            s.setCodec(QTextCodec::codecForName("UTF-8"));
            qimsysDebug() << s.codec();
            while (!s.atEnd()) {
                QStringList fields = s.readLine().split('\t');
                if (fields.isEmpty() || fields.first().isEmpty() || fields.first().startsWith("//")) continue;
                QString from = fields.takeFirst();
                QString to = fields.takeFirst();
                bool priority = !fields.isEmpty();

                from2to[typing][from] = to;
                if (to2from[typing].contains(to)) {
                    if (priority) {
                        to2from[typing][to] = from;
                    }
                } else {
                    to2from[typing][to] = from;
                }
            }
            file.close();
        }
    }
    qimsysDebug();
    dir.setPath(":/character/");
    QMap<QString, QPair<int, int> > characterMap;
    characterMap["katakana_full"] = QPair<int, int>(Hiragana | FullWidth, Katakana | FullWidth);
    characterMap["katakana_half"] = QPair<int, int>(Hiragana | FullWidth, Katakana | HalfWidth);
    characterMap["alphabet"] = QPair<int, int>(Alphabet | HalfWidth, Alphabet | FullWidth);
    foreach(const QString &text, dir.entryList(QStringList() << "*.txt", QDir::Files, QDir::Name)) {
        qimsysDebug() << text;
        QString baseName = QFileInfo(text).baseName();
        QFile file(dir.absoluteFilePath(text));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream s(&file);
            s.setCodec(QTextCodec::codecForName("UTF-8"));
            qimsysDebug() << s.codec()->name();
            while (!s.atEnd()) {
                QString line = s.readLine();
                QStringList fields = line.split('\t');
                qimsysDebug() << fields;
                if (fields.isEmpty() || fields.first().isEmpty() || fields.first().startsWith("//")) continue;
                QString from = fields.takeFirst();
                QString to = fields.takeFirst();
                ch2ch[QPair<int,int>(characterMap[baseName].first, characterMap[baseName].second)][from] = to;
                ch2ch[QPair<int,int>(characterMap[baseName].second, characterMap[baseName].first)][to] = from;
            }
        }
    }
    qimsysDebugOut();
}

Interpreter::Private::~Private()
{
}

Interpreter::Interpreter(QObject *parent)
    : QObject(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Interpreter::~Interpreter()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

QString Interpreter::rawString() const
{
    return d->rawString();
}

QString Interpreter::Private::rawString() const
{
    return rawList.join("");
}

QString Interpreter::inputString(int character) const
{
    return d->inputString(character);
}

QString Interpreter::Private::inputString(int character) const
{
    qimsysDebugIn() << character;
    QString ret;
    QMap<QString, QString> map;
    if (character < 0) {
        ret = inputList.join("");
    } else {
        QMap<QString, QString> map;
        switch (character) {
        case Alphabet | HalfWidth:
            ret = rawList.join("");
            break;
        case Alphabet | FullWidth:
            map = ch2ch[QPair<int,int>(Alphabet | HalfWidth, Alphabet | FullWidth)];
            foreach(const QChar &ch, rawList.join("")) {
                ret.append(map[ch]);
            }
            break;
        case Hiragana | FullWidth:
        case Katakana | FullWidth:
        case Katakana | HalfWidth:
            map = ch2ch[QPair<int,int>(state->character(), character)];
            foreach(const QString &str, inputList) {
                if (map.contains(str)) {
                    ret.append(map[str]);
                } else {
                    foreach(const QChar &ch, str) {
                        if (map.contains(ch)) {
                            ret.append(map[ch]);;
                        } else {
                            ret.append(ch);
                        }
                    }
                }
            }
            break;
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

void Interpreter::input(const QString &string)
{
    d->input(string);
}

void Interpreter::Private::input(const QString &string)
{
    qimsysDebugIn() << string;
    remove(0);

    QimsysPreeditItemList items = preedit->items();
    if (items.count() > 1) {
        qimsysWarning() << items;
    }
    int cursorPosition = (items.isEmpty() ? 0 : items.first().cursor);
    QMap<QString, QString> map = ch2ch[ QPair<int,int>(Hiragana | FullWidth, state->character())];
    if (cursorPosition == 0) {
        QString str = string;
        switch (state->character()) {
        case Alphabet | HalfWidth:
            inputList.prepend(string);
            rawList.prepend(string);
            cursorPosition += string.length();
            break;
        case Alphabet | FullWidth: {
            QString ch = ch2ch[QPair<int,int>(Alphabet | HalfWidth, Alphabet | FullWidth)][string];
            if (ch.isEmpty()) {
                qimsysWarning() << ch << "not found in" << ch2ch[QPair<int,int>(Alphabet | HalfWidth, Alphabet | FullWidth)];
            }
            inputList.prepend(ch);
            rawList.prepend(string);
            qimsysWarning() << inputList;
            cursorPosition += inputList.first().length();
            break;
        }
        default: {
            bool found = false;
            for (int i = str.length(); i > 0; i--) {
                if (from2to[state->typing()].contains(str.right(i))) {
                    QString from = str.right(i);
                    QString to = from2to[state->typing()][from];
                    if (map.contains(to)) {
                        to = map[to];
                    }
                    found = true;
                    str.replace(from, to);
                    if (cursorPosition == 0) {
                        inputList.prepend(str);
                        rawList.prepend(string);
                    } else {
                        qimsysWarning() << "######" << inputList;
                        qFatal("No route??");
                        inputList.append(str);
                        rawList.append(string);
                    }
                    cursorPosition += (string.length() + to.length() - from.length());
                    break;
                }
            }
            if (!found) {
                if (cursorPosition == 0) {
                    inputList.prepend(string);
                    rawList.prepend(string);
                } else {
                    qimsysWarning() << "######" << inputList;
                    qFatal("No route??");
                    inputList.append(string);
                    rawList.append(string);
                }
                cursorPosition += string.length();
            }
            break;
        }
        }
    } else {
        int total = 0;
        for (int index = 0; index < inputList.length(); index++) {
            QString str = inputList[index];
            QString raw = rawList[index];
            int l = str.length();
            if (total + l < cursorPosition) {
            } else if (total + l == cursorPosition) {
                raw.append(string);
                switch (state->character()) {
                case Alphabet | HalfWidth:
                    inputList.insert(index + 1, string);
                    rawList.insert(index + 1, string);
                    cursorPosition ++;
                    break;
                case Alphabet | FullWidth:
                    inputList.insert(index + 1, ch2ch[QPair<int,int>(Alphabet | HalfWidth, Alphabet | FullWidth)][string]);
                    rawList.insert(index + 1, string);
                    cursorPosition += inputList[index+1].length();
                    break;
                default: {
                    str.append(string);
                    bool found = false;
                    for (int i = str.length(); i > 0; i--) {
                        if (from2to[state->typing()].contains(str.right(i))) {
                            QString from = str.right(i);
                            QString to = from2to[state->typing()][from];
                            if (map.contains(to)) {
                                to = map[to];
                            }
                            found = true;
                            if (str == from) {
                                inputList[index] = to;
                                rawList[index] = raw;
                            } else if (raw.length() == 3 && raw[0] == raw[1]) {
                                str.replace(str.lastIndexOf(from), from.length(), to);
                                inputList[index] = str;
                                rawList[index] = raw;
                            } else {
                                inputList[index] = str.left(str.length() - from.length());
                                inputList.insert(index + 1, to);
                                rawList[index] = raw.left(raw.length() - from.length());
                                rawList.insert(index + 1, from);
                            }
                            cursorPosition += (string.length() + to.length() - from.length());
                            break;
                        }
                    }
                    if (!found) {
                        if (inputList[index][inputList[index].length() - 1] == rawList[index][rawList[index].length() - 1]) {
                            inputList[index].append(string);
                            rawList[index].append(string);
                        } else {
                            inputList.insert(index + 1, string);
                            rawList.insert(index + 1, string);
                        }
                        cursorPosition += string.length();
                    }
                    break;
                }
                }
                break;
            }
            total += l;
        }
    }
    emit q->inputStringChanged(inputString());
    emit q->cursorPositionChanged(cursorPosition);
    QimsysPreeditItem item;
    item.to = inputString();
    item.cursor = cursorPosition;
    item.selection = 0;
#ifdef QIMSYS_PLATFORM_MAEMO
    item.underline = QTextCharFormat::SingleUnderline;
#else
    item.underline = QTextCharFormat::WaveUnderline;
#endif
    preedit->setItems(QimsysPreeditItemList() << item);
    qimsysDebugOut();
}

void Interpreter::remove(int length)
{
    d->remove(length);
}

QString Interpreter::Private::rawStringFor(const QString str)
{
    qimsysDebugIn() << str;
    QString ret;
    foreach(const QChar &ch, str) {
        if (to2from[state->typing()].contains(ch)) {
            ret.append(to2from[state->typing()][ch]);
        } else {
            ret.append(ch);
        }
    }
    qimsysDebugOut() << ret;
    return ret;
}

void Interpreter::Private::remove(int length)
{
    if (qAbs(length) > 1) {
        qimsysWarning() << "can not remove" << length << "string";
        return;
    }

    QimsysPreeditItemList items = preedit->items();
    if (items.isEmpty()) return;
    qimsysDebugIn() << length;
    if (items.count() != 1) {
        qimsysWarning() << items;
    }
    QimsysPreeditItem item = items.first();
    int cursorPosition = item.cursor;
    int selectionLength = item.selection;
    int min = cursorPosition;
    min = qMin(min, min + selectionLength);
    int max = cursorPosition;
    max = qMax(max, max + selectionLength);

    if (!selectionLength) {
        switch (length) {
        case -1:
            min = qMax(0, min - 1);
            break;
        case 1:
            max = qMin(inputString().length(), max + 1);
            break;
        }
    }
    if (min == max && min == 0) {
        qimsysDebugOut();
        return;
    }

    QStringList input;
    QStringList raw;
    int total = 0;
    RemovingState removing = Before;
    int count = inputList.length();
    for (int index = 0; index < count; index++) {
        QString str = inputList[index];
        int l = str.length();
        switch (removing) {
        case Before:
            if (total + l < min) {
                input.append(str);
                raw.append(rawList[index]);
                total += l;
                break;
            } else if (total + l == min) {
                input.append(str);
                raw.append(rawList[index]);
                removing = Removing;
            } else if (total + l > min) {
                QString s = str.left(min - total);
                if (!s.isEmpty()) {
                    input.append(s);
                    raw.append(rawStringFor(s));
                }
                removing = Removing;
            }
            // fall through
        case Removing:
            if (total + l < max) {
                total += l;
            } else if (total + l == max) {
                total += l;
                removing = After;
            } else if (total + l > max) {
                QString s = str.mid(total + l - max);
                input.append(s);
                raw.append(rawStringFor(s));
                removing = After;
                total += l;
            }
            break;
        case After:
            input.append(str);
            raw.append(rawList[index]);
            break;
        }
    }

    inputList = input;
    rawList = raw;
    cursorPosition = min;
    selectionLength = 0;
    emit q->inputStringChanged(inputString());
    emit q->cursorPositionChanged(cursorPosition);
    emit q->selectionLengthChanged(selectionLength);
    item.to = inputString();
    item.cursor = cursorPosition;
    item.selection = 0;
#ifdef QIMSYS_PLATFORM_MAEMO
    item.underline = QTextCharFormat::SingleUnderline;
#else
    item.underline = QTextCharFormat::WaveUnderline;
#endif
    if (item.to.isEmpty()) {
        items.clear();
    } else {
        items.replace(0, item);
    }
    preedit->setItems(items);
    qimsysDebugOut();
}

void Interpreter::termiateInput()
{
    d->termiateInput();
}

void Interpreter::Private::termiateInput()
{
    static QString terminator = QLatin1String("\\0");
    if (inputString() == rawString()) return;
    qimsysDebugIn();
    QMap<QString, QString> map = ch2ch[ QPair<int,int>(Hiragana | FullWidth, state->character())];
    for (int index = 0; index < inputList.count(); index++) {
        QString str = inputList[index] + terminator;
        if (from2to[state->typing()].contains(str)) {
            QString to = from2to[state->typing()][str];
            if (map.contains(to)) {
                to = map[to];
            }
            inputList[index] = to;
        }
    }
    emit q->inputStringChanged(inputString());
    QimsysPreeditItemList items = preedit->items();
    if (items.count() != 1) {
        qimsysWarning() << items;
    }
    QimsysPreeditItem item = items.first();
    item.to = inputString();
    items.replace(0, item);
    preedit->setItems(items);
    qimsysDebugOut();
}

void Interpreter::clear()
{
    d->clear();
}

void Interpreter::Private::clear()
{
    qimsysDebugIn();
    inputList.clear();
    rawList.clear();
    emit q->inputStringChanged(inputString());
    qimsysDebugOut();
}

#include "interpreter.moc"
