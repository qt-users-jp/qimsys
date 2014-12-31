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

#include "keyactions.h"
#include "global.h"

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyspreeditmanager.h>
#include <qimsyscandidatemanager.h>
#include <qimsyspluginmanager.h>
#include <qimsysconverter.h>
#include <qimsysinterpreter.h>
#include <qimsyskeyboardmanager.h>
#include <qimsyskeysequence.h>

#include <QSignalMapper>
#include <QFile>
#include <QTextStream>

using namespace Japanese;
using namespace Japanese::Standard;

class Action : public QObject
{
    Q_OBJECT
public:
    Action(QObject *parent = 0) : QObject(parent) {}

public slots:
    void trigger() { emit triggered(); }

signals:
    void triggered();
};

class KeyActions::Private : private QObject
{
    Q_OBJECT
public:
    Private(KeyActions *parent);

    bool contains(const QimsysKeySequence &keySequence) const;
    void trigger(const QimsysKeySequence &keySequence);

private:
    void init();
    void readKeyMap(const QString &fileName);
    void terminate();
    void saveCache();
    void restoreCache();

private slots:
    void stateChanged(uint state);

    void toggleKeyboard();
    void setEnabled(int enabled);
    void composingChanged(bool composing);
    void moveCursor(int offset);
    void changeSelectionLength(int delta);
    void remove(int length = 0);
    void commit(int all = true);
    void convert();
    void resizeSegment(int delta);
    void selectCandidate(int delta);
    void cancel();
    void clear();
    void setCharacterMode(int direction);
    void setTypingMode(int mode);
    void convertTo(const QString &converter);
    void inputSpace(int width);

    void clearSelection();
    void execute(const QString &command);

private:
    KeyActions *q;

    QimsysApplicationManager applicationManager;
    QimsysInputMethodManager inputMethodManager;
    QimsysPreeditManager preeditManager;
    QimsysCandidateManager candidateManager;
    QimsysKeyboardManager keyboardManager;

    State currentState;
    QMap<QString, Action*> actionMap;
    QMap<uint, QMap<QimsysKeySequence, QString> > keyMap;

    QimsysPreeditItem cache;
};

KeyActions::Private::Private(KeyActions *parent)
    : QObject(parent)
    , q(parent)
{
    init();
}

void KeyActions::Private::readKeyMap(const QString &fileName)
{
    qimsysDebugIn() << fileName;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        keyMap.clear();
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains('\t')) {
                QStringList fields = line.split('\t');
                if (fields.isEmpty() || fields.first().isEmpty() || fields.first().startsWith("#")) continue;
                QString state = fields.takeFirst();
                QString key = fields.takeFirst();
                QString action = fields.takeFirst();
                QimsysKeySequence keySequence(key);
                if (key == QLatin1String("Multi_key")) {
                    keySequence = QimsysKeySequence(Qt::Key_Multi_key);
                }
                if (state == QLatin1String("Direct"))
                    keyMap[Direct][keySequence] = action;
                else if (state == QLatin1String("Empty"))
                    keyMap[Empty][keySequence] = action;
                else if (state == QLatin1String("Input"))
                    keyMap[Input][keySequence] = action;
                else if (state == QLatin1String("Convert"))
                    keyMap[Convert][keySequence] = action;
                else if (state == QLatin1String("Select"))
                    keyMap[Select][keySequence] = action;
                else
                    qimsysWarning() << state;
            }
        }
        file.close();
    } else {
        qimsysWarning() << file.error() << file.errorString();
    }
    qimsysDebugOut();
}

bool KeyActions::Private::contains(const QimsysKeySequence &keySequence) const
{
#ifdef QIMSYS_DEBUG_MODE
    qimsysDebugIn() << keySequence.toString();
    qimsysDebug() << currentState;
    bool ret = keyMap[currentState].contains(keySequence);
    qimsysDebugOut() << ret;
    return ret;
#else
    return keyMap[currentState].contains(keySequence);
#endif
}

void KeyActions::Private::trigger(const QimsysKeySequence &keySequence)
{
    qimsysDebugIn() << keySequence.toString();
    if (actionMap.contains(keyMap[currentState][keySequence])) {
        actionMap[keyMap[currentState][keySequence]]->trigger();
    } else {
        qimsysWarning() << keyMap[currentState][keySequence] << actionMap.keys();
    }
    qimsysDebugOut();
}

void KeyActions::Private::execute(const QString &command)
{
    qimsysDebugIn() << command;
    if (actionMap.contains(command)) {
        actionMap[command]->trigger();
    } else {
        qimsysWarning() << command << "not found.";
    }
    qimsysDebugOut();
}

void KeyActions::Private::init()
{
    // toggle keyboard
    {
        Action *action = new Action(this);
        connect(action, SIGNAL(triggered()), this, SLOT(toggleKeyboard()));
        actionMap["Toggle keyboard"] = action;
    }

    // set enabled
    {
        QSignalMapper *setEnabled = new QSignalMapper(this);
        connect(setEnabled, SIGNAL(mapped(int)), this, SLOT(setEnabled(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), setEnabled, SLOT(map()));
            setEnabled->setMapping(action, true);
            actionMap["Turn on qimsys"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), setEnabled, SLOT(map()));
            setEnabled->setMapping(action, false);
            actionMap["Turn off qimsys"] = action;
        }
    }

    // move cursor
    {
        QSignalMapper *moveCursor = new QSignalMapper(this);
        connect(moveCursor, SIGNAL(mapped(int)), this, SLOT(moveCursor(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
            moveCursor->setMapping(action, -2);
            actionMap["Move cursor first"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
            moveCursor->setMapping(action, -1);
            actionMap["Move cursor previous"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
            moveCursor->setMapping(action, +1);
            actionMap["Move cursor next"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
            moveCursor->setMapping(action, +2);
            actionMap["Move cursor last"] = action;
        }
    }

    // change selection length
    {
        QSignalMapper *changeSelectionLength = new QSignalMapper(this);
        connect(changeSelectionLength, SIGNAL(mapped(int)), this, SLOT(changeSelectionLength(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), changeSelectionLength, SLOT(map()));
            changeSelectionLength->setMapping(action, -1);
            actionMap["Change selection length previous"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), changeSelectionLength, SLOT(map()));
            changeSelectionLength->setMapping(action, +1);
            actionMap["Change selection length next"] = action;
        }
    }

    // remove
    {
        QSignalMapper *remove = new QSignalMapper(this);
        connect(remove, SIGNAL(mapped(int)), this, SLOT(remove(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), remove, SLOT(map()));
            remove->setMapping(action, -1);
            actionMap["Remove previous"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), remove, SLOT(map()));
            remove->setMapping(action, +1);
            actionMap["Remove next"] = action;
        }
    }

    // clear selection
    {
        Action *action = new Action(this);
        connect(action, SIGNAL(triggered()), this, SLOT(clearSelection()));
        actionMap["Clear Selection"] = action;
    }

    // commit
    {
        QSignalMapper *commit = new QSignalMapper(this);
        connect(commit, SIGNAL(mapped(int)), this, SLOT(commit(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), commit, SLOT(map()));
            commit->setMapping(action, true);
            actionMap["Commit all"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), commit, SLOT(map()));
            commit->setMapping(action, false);
            actionMap["Commit one"] = action;
        }
    }

    // convert
    {
        Action *action = new Action(this);
        connect(action, SIGNAL(triggered()), this, SLOT(convert()));
        actionMap["Convert"] = action;
    }

    // resize segment
    {
        QSignalMapper *resizeSegment = new QSignalMapper(this);
        connect(resizeSegment, SIGNAL(mapped(int)), this, SLOT(resizeSegment(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), resizeSegment, SLOT(map()));
            resizeSegment->setMapping(action, -1);
            actionMap["Resize segment shorter"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), resizeSegment, SLOT(map()));
            resizeSegment->setMapping(action, +1);
            actionMap["Resize segment longer"] = action;
        }
    }

    // select candidate
    {
        QSignalMapper *selectCandidate = new QSignalMapper(this);
        connect(selectCandidate, SIGNAL(mapped(int)), this, SLOT(selectCandidate(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), selectCandidate, SLOT(map()));
            selectCandidate->setMapping(action, -1);
            actionMap["Select previous candidate"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), selectCandidate, SLOT(map()));
            selectCandidate->setMapping(action, +1);
            actionMap["Select next candidate"] = action;
        }
    }

    // cancel
    {
        Action *action = new Action(this);
        connect(action, SIGNAL(triggered()), this, SLOT(cancel()));
        actionMap["Cancel"] = action;
    }

    // clear
    {
        Action *action = new Action(this);
        connect(action, SIGNAL(triggered()), this, SLOT(clear()));
        actionMap["Clear"] = action;
    }

    // set character mode
    {
        QSignalMapper *setCharacterMode = new QSignalMapper(this);
        connect(setCharacterMode, SIGNAL(mapped(int)), this, SLOT(setCharacterMode(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
            setCharacterMode->setMapping(action, -1);
            actionMap["Previous character mode"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
            setCharacterMode->setMapping(action, +1);
            actionMap["Next character mode"] = action;
        }
    }

    // set typing mode
    {
        QSignalMapper *setTypingMode = new QSignalMapper(this);
        connect(setTypingMode, SIGNAL(mapped(int)), this, SLOT(setTypingMode(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), setTypingMode, SLOT(map()));
            setTypingMode->setMapping(action, -1);
            actionMap["Previous typing mode"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), setTypingMode, SLOT(map()));
            setTypingMode->setMapping(action, +1);
            actionMap["Next typing mode"] = action;
        }
    }

    // convert to
    {
        QSignalMapper *convertTo = new QSignalMapper(this);
        connect(convertTo, SIGNAL(mapped(QString)), this, SLOT(convertTo(QString)));

        QList<QimsysConverter *> converters = QimsysPluginManager::objects<QimsysConverter>();
        foreach(QimsysConverter *converter, converters) {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), convertTo, SLOT(map()));
            convertTo->setMapping(action, converter->identifier());
            actionMap[QString("Convert to %1").arg(converter->identifier())] = action;
        }
    }

    // input space
    {
        QSignalMapper *inputSpace = new QSignalMapper(this);
        connect(inputSpace, SIGNAL(mapped(int)), this, SLOT(inputSpace(int)));

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
            inputSpace->setMapping(action, 0);
            actionMap["Input space"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
            inputSpace->setMapping(action, 1);
            actionMap["Input Halfwidth space"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
            inputSpace->setMapping(action, 2);
            actionMap["Input Fullwidth space"] = action;
        }

        {
            Action *action = new Action(this);
            connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
            inputSpace->setMapping(action, -1);
            actionMap["Input the other width space"] = action;
        }
    }

    applicationManager.init();
    connect(&applicationManager, SIGNAL(composingChanged(bool)), this, SLOT(composingChanged(bool)));
    inputMethodManager.init();
    connect(&inputMethodManager, SIGNAL(stateChanged(uint)), this, SLOT(stateChanged(uint)));
    connect(&inputMethodManager, SIGNAL(executed(QString)), this, SLOT(execute(QString)));
    preeditManager.init();
    candidateManager.init();
    keyboardManager.init();
    readKeyMap(QLatin1String(":/japanese/standard/resources/keymaps/msime.txt"));

    composingChanged(applicationManager.composing());
    stateChanged(inputMethodManager.state());
}

void KeyActions::Private::stateChanged(uint state)
{
    currentState = (State)state;
}

void KeyActions::Private::toggleKeyboard()
{
    qimsysDebugIn();
    keyboardManager.setVisible(!keyboardManager.visible());
    qimsysDebugOut();
}

void KeyActions::Private::setEnabled(int enabled)
{
    qimsysDebugIn() << enabled;
    applicationManager.setComposing(enabled);
    qimsysDebugOut();
}

void KeyActions::Private::composingChanged(bool composing)
{
    if (composing)
        inputMethodManager.setState(Empty);
    else
        inputMethodManager.setState(Direct);
}

void KeyActions::Private::moveCursor(int offset)
{
    qimsysDebugIn() << offset;
    QimsysPreeditItem item = preeditManager.item();
    qimsysDebug() << item;

    switch (currentState) {
    case Input: {
        int length = item.to.join("").length();
        int cursor = item.cursor;
        int selection = item.selection;
        switch (offset) {
        case -2: // first
            cursor = 0;
            selection = 0;
            break;
        case 2:  // last
            cursor = length;
            selection = 0;
            break;
        default:
            if (selection != 0) {
                selection = 0;
            } else {
                cursor = (cursor + length + 1 + offset) % (length + 1);
            }
            break;
        }
        item.selection = selection;
        item.cursor = cursor;
        break;
    }
    case Select:
        inputMethodManager.setState(Convert);
        fallthrough;
    case Convert: {
        QStringList to = item.to;
        int cursor = item.cursor;
        int pos = 0;
        int currentIndex = -1;
        for (int i = 0; i < to.length(); i++) {
            if (pos == cursor) {
                currentIndex = i;
            }
            pos += to.at(i).length();
        }

        if (currentIndex < 0) {
            qimsysWarning() << "current item not found." << to << cursor;
        }

        switch (offset) {
        case -2: // first
            currentIndex = 0;
            break;
        case 2:  // last
            currentIndex = to.length() - 1;
            break;
        default:
            currentIndex = (currentIndex + offset + to.length()) % to.length();
            break;
        }

        cursor = 0;
        for (int i = 0; i < currentIndex; i++) {
            cursor += to.at(i).length();
        }
        item.cursor = cursor;
        item.selection = to.at(currentIndex).length();
        break;
    }
    default:
        break;
    }

    preeditManager.blockSignals(true);
    preeditManager.setItem(item);
    preeditManager.blockSignals(false);
    qimsysDebugOut();
}

void KeyActions::Private::changeSelectionLength(int delta)
{
    qimsysDebugIn() << delta;

    QimsysPreeditItem item = preeditManager.item();
    int length = item.to.join("").length();
    int cursor = item.cursor;
    int selection = item.selection;

    switch (delta) {
    case -2:
        selection = cursor;
        cursor = 0;
        break;
    case 2:
        selection = length - cursor;
        break;
    default:
        if (cursor + delta == qBound(0, cursor + delta, length)) {
            cursor = cursor + delta;
            selection -= delta;
        }
        break;
    }
    item.cursor = cursor;
    item.selection = selection;
    preeditManager.blockSignals(true);
    preeditManager.setItem(item);
    preeditManager.blockSignals(false);
    qimsysDebugOut();
}

void KeyActions::Private::remove(int length)
{
    QimsysPreeditItem item = preeditManager.item();
    if (item.to.isEmpty()) return;
    if (length == 0 && item.selection == 0) return;

    qimsysDebugIn() << length;
    qimsysDebug() << item;

    QStringList to = item.to;
    QStringList from = item.from;
    QStringList rawString = item.rawString;
    int cursor = item.cursor;
    int selection = item.selection;

    if (selection != 0) {
        length = selection;
        selection = 0;
    }

    if (length < 0) {
        cursor += length;
        length = -length;
    }

    if (cursor < 0) {
        length += cursor;
        cursor = 0;
    }

    qimsysDebug() << cursor << length;
    int pos = 0;
    int i = 0;
    while (length > 0) {
        int l = to.at(i).length();
        qimsysDebug() << pos << cursor << l << i;
        if (pos == cursor) {
            qimsysDebug() << l << length;
            if (l <= length) {
                length -= l;
                to.removeAt(i);
                from.removeAt(i);
                rawString.removeAt(i);
                continue;
            } else {
                to.replace(i, to[i].remove(cursor, length));
                from.replace(i, from[i].remove(cursor, length));
                rawString.replace(i, QString());
                break;
            }
        } else if (pos + l > cursor) {
            qimsysDebug() << pos << cursor << length << l;
            to.replace(i, to[i].remove(cursor - pos, length));
            from.replace(i, from[i].remove(cursor - pos, length));
            rawString.replace(i, QString());
            break;
        }
        pos += l;
        i++;
    }

    item.to = to;
    item.from = from;
    item.rawString = rawString;
    item.cursor = cursor;
    item.selection = selection;
    item.modified = 0;
    qimsysDebug() << item;
    preeditManager.setItem(item);
    if (to.isEmpty()) {
        inputMethodManager.setState(Empty);
    }
    qimsysDebugOut();
}

void KeyActions::Private::commit(int all)
{
    QimsysPreeditItem item = preeditManager.item();
    if (item.to.isEmpty()) return;
    qimsysDebugIn() << all;

    terminate();

    QStringList to = item.to;
    QStringList from = item.from;
    QStringList rawString = item.rawString;
    int cursor = item.cursor;
    int selection = item.selection;

    if (!all) {
        if (selection == 0 || cursor == 0) {
            qimsysWarning() << "first segment should be selected" << to << cursor << selection;
        }
    }

    QString commitString = to.takeFirst();
    if (!from.isEmpty()) from.removeFirst();
    if (!rawString.isEmpty()) rawString.removeFirst();

    if (all) {
        commitString.append(to.join(""));
        to.clear();
        from.clear();
        rawString.clear();
    }
    cursor = 0;
    selection = to.isEmpty() ? 0 : to.first().length();

    item.to = to;
    item.from = from;
    item.rawString = rawString;
    item.cursor = cursor;
    item.selection = selection;
    item.modified = 0;
    qimsysDebug() << item;
    preeditManager.setItem(item);
    preeditManager.commit(commitString, applicationManager.widget());
    if (all || to.isEmpty()) {
        inputMethodManager.setState(Empty);
    }
    saveCache();
    candidateManager.setCurrentIndex(-1);
    qimsysDebugOut();
}

void KeyActions::Private::convert()
{
    qimsysDebugIn();
    terminate();
    saveCache();
    candidateManager.setItems(QimsysConversionItemList());
    inputMethodManager.setState(Convert);
    qimsysDebugOut();
}

void KeyActions::Private::resizeSegment(int delta)
{
//    qimsysDebugOn();
    qimsysDebugIn() << delta;

    inputMethodManager.setState(Convert);

    QimsysPreeditItem item = preeditManager.item();

    qimsysDebug() << item;

    int currentIndex = -1;
    QList<int> length;

    // find current index
    int pos = 0;
    for (int i = 0; i < item.to.length(); i++) {
        if (pos == item.cursor) {
            currentIndex = i;
        }
        pos += item.to.at(i).length();
        length.append(item.from.at(i).length());
    }

    qimsysDebug() << length << currentIndex;

    if (delta < 0) {
        int len = length.at(currentIndex) + delta;
        if (len < 1) {
            qimsysDebugOut();
            return;
        }
        length.replace(currentIndex, len);
    } else {
        bool changed = false;
        while (delta > 0) {
            if (length.length() == currentIndex + 1) {
                if (!changed) {
                    qimsysDebugOut();
                    return;
                }
                break;
            }
            int len = length.at(currentIndex + 1);
            if (len > delta) {
                length.replace(currentIndex, length.at(currentIndex) + delta);
                break;
            } else {
                length.replace(currentIndex, length.at(currentIndex) + len);
                delta -= len;
            }
            changed = true;
        }
    }

    qimsysDebug() << length;
    qimsysDebug() << item;

    while (item.to.length() > currentIndex) {
        item.to.removeLast();
        if (!item.from.isEmpty()) item.from.removeLast();
        if (!item.rawString.isEmpty()) item.rawString.removeLast();
    }

    QimsysPreeditItem previous = cache;
    qimsysDebug() << item << previous;

    for (int i = 0; i < currentIndex + 2; i++) {
        qimsysDebug() << i << currentIndex;
        qimsysDebug() << item << previous;
        if (i < currentIndex) {
            for (int j = 0; j < length.at(i); j++) {
                QString to = previous.to.first();
                QString from = previous.from.first();
                if (from.length() == 1) {
                    previous.to.removeFirst();
                    if (!previous.from.isEmpty()) previous.from.removeFirst();
                    if (!previous.rawString.isEmpty()) previous.rawString.removeFirst();
                } else {
                    previous.to.replace(i, to.mid(1));
                    if (!previous.from.isEmpty()) previous.from.replace(i, from.mid(1));
                    if (!previous.rawString.isEmpty()) previous.rawString.replace(i, QString());
                }
            }
        } else {
            if (i == currentIndex) {
                item.cursor = item.to.join("").length();
                for (int j = 0; j < length.at(i); j++) {
                    qimsysDebug() << previous;
                    QString to = previous.to.first();
                    QString from = previous.from.first();
                    if (from.length() == 1) {
                        qimsysDebug();
                        if (item.to.length() - 1 < currentIndex) {
                            qimsysDebug();
                            item.to.append(previous.to.takeFirst());
                            if (!previous.from.isEmpty()) item.from.append(previous.from.takeFirst());
                            if (!previous.rawString.isEmpty()) item.rawString.append(previous.rawString.takeFirst());
                            qimsysDebug() << item;
                        } else {
                            qimsysDebug() << item << previous;
                            item.to.replace(currentIndex, item.to.at(currentIndex) + previous.to.takeFirst());
                            if (!previous.from.isEmpty()) item.from.replace(currentIndex, item.from.at(currentIndex) + previous.from.takeFirst());
                            if (!previous.rawString.isEmpty()) item.rawString.replace(currentIndex, item.rawString.at(currentIndex) + previous.rawString.takeFirst());
                            qimsysDebug() << item;
                        }
                    } else {
                        qimsysDebug() << item;
                        if (item.to.length() - 1 < currentIndex) {
                            item.to.append(to.left(1));
                            item.from.append(from.left(1));
                            item.rawString.append(QString());
                            qimsysDebug() << item;
                        } else {
                            item.to.replace(currentIndex, item.to.at(currentIndex) + to.left(1));
                            item.from.replace(currentIndex, item.from.at(currentIndex) + from.left(1));
                            qimsysDebug() << item;
                        }

                        previous.to.replace(0, to.mid(1));
                        previous.from.replace(0, from.mid(1));
                        previous.rawString.replace(0, QString());
                        qimsysDebug() << item;
                    }
                }
                item.selection = item.to.join("").length() - item.cursor;
                item.modified = 0;
                qimsysDebug() << item;
            } else if (!previous.from.isEmpty()){
                item.to.append(previous.to);
                item.from.append(previous.from);
                item.rawString.append(previous.rawString);
                qimsysDebug() << item;
            }
        }
    }

    qimsysDebug() << item;
    preeditManager.setItem(item);
    qimsysDebugOut();
//    qimsysDebugOff();
}

void KeyActions::Private::selectCandidate(int delta)
{
    qimsysDebugIn() << delta;
    switch (inputMethodManager.state()) {
    case Empty:
    case Input:
        if (!candidateManager.items().isEmpty()) {
            saveCache();
            inputMethodManager.setState(Select);
            candidateManager.setCurrentIndex((candidateManager.currentIndex() + delta + candidateManager.items().count()) % candidateManager.items().count());
        }
        break;
    case Convert:
        candidateManager.setItems(QimsysConversionItemList());
        inputMethodManager.setState(Select);
        if (candidateManager.currentIndex() == -1 && delta == 1) {
            delta = 2;
        }
        fallthrough;
    case Select:
        if (!candidateManager.items().isEmpty()) {
            candidateManager.setCurrentIndex((candidateManager.currentIndex() + delta + candidateManager.items().count()) % candidateManager.items().count());
        }
        break;
    default:
        break;
    }
    qimsysDebugOut();
}

void KeyActions::Private::cancel()
{
    qimsysDebugIn();
    qimsysDebug() << cache;
    if (cache.to.isEmpty()) {
        inputMethodManager.setState(Empty);
    } else {
        inputMethodManager.setState(Input);
    }
    restoreCache();
    candidateManager.setCurrentIndex(-1);
    qimsysDebugOut();
}

void KeyActions::Private::clear()
{
    // TBD
}

void KeyActions::Private::setCharacterMode(int direction)
{
    qimsysDebugIn() << direction;
    QString current = inputMethodManager.converter();
    qimsysDebug() << current;

    QList<QimsysConverter *> converters = QimsysPluginManager::objects<QimsysConverter>();
    int count = converters.length();
    int index = 0;
    //! TODO locale and/or available
    for (int i = 0; i < count; i++) {
        if (converters.at(i)->identifier() == current) {
            index = i;
            break;
        }
    }

    index = (index + direction + count) % count;
    current = converters.at(index)->identifier();
    qimsysDebug() << current;
    terminate();
    inputMethodManager.setConverter(current);
    qimsysDebugOut();
}

void KeyActions::Private::setTypingMode(int mode)
{
    qimsysDebugIn() << mode;
    QString current = inputMethodManager.interpreter();
    qimsysDebug() << current;

    QList<QimsysInterpreter *> interpreters = QimsysPluginManager::objects<QimsysInterpreter>();
    int count = interpreters.length();
    int index = 0;
    //! TODO locale and/or available
    for (int i = 0; i < count; i++) {
        if (interpreters.at(i)->identifier() == current) {
            index = i;
            break;
        }
    }

    index = (index + mode + count) % count;
    current = interpreters.at(index)->identifier();
    qimsysDebug() << current;
    terminate();
    inputMethodManager.setInterpreter(current);
    qimsysDebugOut();
}

void KeyActions::Private::convertTo(const QString &converter)
{
    qimsysDebugOn();
    qimsysDebugIn() << converter;
    QString current = inputMethodManager.converter();
    inputMethodManager.setConverter(converter);
    inputMethodManager.setState(ConvertTo);
    inputMethodManager.setState(Input);
    inputMethodManager.setConverter(current);
    qimsysDebugOut();
    qimsysDebugOff();
}

void KeyActions::Private::inputSpace(int width)
{
    switch (width) {
    case -1:
        break;
    case 0:
        if (inputMethodManager.state() == Empty) {
            preeditManager.commit(QLatin1String(" "), applicationManager.widget());
        } else {
            preeditManager.insert(QLatin1String(" "));
        }
        break;
    case 1:
        if (inputMethodManager.state() == Empty) {
            preeditManager.commit(QLatin1String(" "), applicationManager.widget());
        } else {
            preeditManager.insert(QLatin1String(" "));
        }
        break;
    case 2:
        if (inputMethodManager.state() == Empty) {
            preeditManager.commit(QString::fromUtf8("　"), applicationManager.widget());
        } else {
            preeditManager.insert(QString::fromUtf8("　"));
        }
        break;
    }
}

void KeyActions::Private::clearSelection()
{
    qimsysDebugIn();
    remove();
    qimsysDebugOut();
}

void KeyActions::Private::terminate()
{
    if (inputMethodManager.state() != Input) return;
    if (preeditManager.item().modified == 0) return;

    qimsysDebugIn();

    static QString terminator = QString();
    preeditManager.insert(terminator);
    QimsysPreeditItem item = preeditManager.item();
    item.to.removeOne(QString());
    item.from.removeOne(QString());
    item.rawString.removeOne(QString());
    item.modified = 0;
    preeditManager.blockSignals(true);
    preeditManager.setItem(item);
    preeditManager.blockSignals(false);

    qimsysDebugOut();
}

void KeyActions::Private::saveCache()
{
    cache = preeditManager.item();
}

void KeyActions::Private::restoreCache()
{
    preeditManager.setItem(cache);
}

KeyActions::KeyActions(QObject *parent)
    : QObject(parent)
{
    d = new Private(this);
}

bool KeyActions::contains(const QimsysKeySequence &keySequence) const
{
    return d->contains(keySequence);
}

void KeyActions::trigger(const QimsysKeySequence &keySequence)
{
    d->trigger(keySequence);
}

#include "keyactions.moc"
