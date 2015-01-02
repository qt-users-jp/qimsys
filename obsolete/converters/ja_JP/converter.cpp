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

#include "converter.h"

#include "japanese.h"

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsyscandidates.h>
#include <qimsyspluginmanager.h>
#include <qimsysengine.h>
#include <qimsysenginedictionary.h>
#include <qimsyskeymanager.h>

#include "preedit.h"
#include "japanesesettings.h"
#include "keyactionmanager.h"
#include "inputmanager.h"
#include "interpreter.h"
#include "translator.h"

#include <QActionGroup>
#include <QIcon>
#include <QKeyEvent>
#include <QLayout>
#include <QPixmap>
#include <QPainter>
#include <QSignalMapper>
#include <QStyle>
#include <QTimer>

namespace ja_JP
{
class Converter::Private : private QObject
{
    Q_OBJECT
public:
    Private(Converter *parent);
    ~Private();

    QIcon icon() const;

private slots:
    void keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat);

    void exec(int action);

    void setCharacterMode(QAction *action);
    void setTyping(QAction *action);
    void currentEngineChanged(const QString &name);

    void setEnabled(int enabled);
    void moveCursor(int offset);
    void changeSelectionLength(int diff);
    void commit();
    void convert();
    void resizeSegment(int diff);
    void selectCandidate(int offset);
    void cancel();
    void setCharacterMode(int character);
    void setTypingMode(int typingMode);
    void convertTo(int character);
    void clear();
    void inputSpace(int width);
    void reset();

    void focusChanged(bool focus);
    void focusChanged();
    void composingChanged(bool composing);
    void stateChanged(State state);
    void characterChanged(int character);
    void typingChanged(Typing typing);
    void predictionsChanged(const QimsysConversionItemList &list);

    void candidateIndexChanged(int index);

    void updateIcon(Typing typing = (Typing)0);

    void setupActions();
private:
    Converter *q;
    InputManager *inputManager;
    QimsysApplicationManager manager;
    Preedit *preedit;
    QimsysKeyManager keyManager;
    QimsysCandidates candidates;
    QimsysEngine *currentEngine;
    Interpreter interpreter;
    KeyActionManager *keyActions;
    QList<QActionGroup*> characterActions;
    QList<QActionGroup*> typingActions;
    bool accept;
    QTimer tempIcon;

public:
    QList<QAction*> actions;
};
}

using namespace ja_JP;

Converter::Private::Private(Converter *parent)
    : QObject(parent)
    , q(parent)
    , inputManager(InputManager::instance())
    , preedit(Preedit::instance())
    , currentEngine(0)
    , keyActions(KeyActionManager::instance())
{
    qimsysDebugIn() << parent;
    inputManager->setParent(0);
    preedit->setParent(q);
    keyActions->setParent(0);

    connect(q, SIGNAL(activeChanged(bool)), this, SLOT(reset()));

    connect(inputManager, SIGNAL(stateChanged(State)), this, SLOT(stateChanged(State)));
    connect(inputManager, SIGNAL(characterChanged(int)), this, SLOT(characterChanged(int)));
    connect(inputManager, SIGNAL(typingChanged(Typing)), this, SLOT(typingChanged(Typing)));

    manager.init();
    connect(&manager, SIGNAL(focusChanged(bool)), this, SLOT(focusChanged(bool)));
    connect(&manager, SIGNAL(composingChanged(bool)), this, SLOT(composingChanged(bool)));
    connect(&manager, SIGNAL(triggered(int)), this, SLOT(exec(int)));
    connect(&manager, SIGNAL(currentEngineChanged(QString)), this, SLOT(currentEngineChanged(QString)));

    keyManager.init();
    connect(&keyManager, SIGNAL(keyPressed(QString,int,int,bool)), this, SLOT(keyPressed(QString,int,int,bool)));

    preedit->init();

    candidates.init();
    connect(&candidates, SIGNAL(currentIndexChanged(int)), this, SLOT(candidateIndexChanged(int)));

    QSignalMapper *setEnabled = new QSignalMapper(this);
    connect(setEnabled, SIGNAL(mapped(int)), this, SLOT(setEnabled(int)));

    {
        KeyAction *action = new KeyAction(Direct, QT_TR_NOOP("Turn on QIMSYS"), this);
        connect(action, SIGNAL(triggered()), setEnabled, SLOT(map()));
        setEnabled->setMapping(action, true);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString, QT_TR_NOOP("Turn off QIMSYS"), this);
        connect(action, SIGNAL(triggered()), setEnabled, SLOT(map()));
        setEnabled->setMapping(action, false);
        keyActions->addAction(action);
    }

    QSignalMapper *moveCursor = new QSignalMapper(this);
    connect(moveCursor, SIGNAL(mapped(int)), this, SLOT(moveCursor(int)));

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Move cursor first"), this);
        connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
        moveCursor->setMapping(action, -2);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Move cursor left"), this);
        connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
        moveCursor->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Move cursor right"), this);
        connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
        moveCursor->setMapping(action, + 1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Move cursor last"), this);
        connect(action, SIGNAL(triggered()), moveCursor, SLOT(map()));
        moveCursor->setMapping(action, + 2);
        keyActions->addAction(action);
    }

    QSignalMapper *changeSelectionLength = new QSignalMapper(this);
    connect(changeSelectionLength, SIGNAL(mapped(int)), this, SLOT(changeSelectionLength(int)));

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Change selection length left"), this);
        connect(action, SIGNAL(triggered()), changeSelectionLength, SLOT(map()));
        changeSelectionLength->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Change selection length right"), this);
        connect(action, SIGNAL(triggered()), changeSelectionLength, SLOT(map()));
        changeSelectionLength->setMapping(action, + 1);
        keyActions->addAction(action);
    }

    QSignalMapper *remove = new QSignalMapper(this);
    connect(remove, SIGNAL(mapped(int)), &interpreter, SLOT(remove(int)));

    {
        KeyAction *action = new KeyAction(InputingString, QT_TR_NOOP("Remove front"), this);
        connect(action, SIGNAL(triggered()), remove, SLOT(map()));
        remove->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString, QT_TR_NOOP("Remove back"), this);
        connect(action, SIGNAL(triggered()), remove, SLOT(map()));
        remove->setMapping(action, + 1);
        keyActions->addAction(action);
    }


    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString | SelectingCandidate | SelectingPrediction, QT_TR_NOOP("Commit"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(commit()));
        keyActions->addAction(action);
    }


    {
        KeyAction *action = new KeyAction(InputingString, QT_TR_NOOP("Convert"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(convert()));
        keyActions->addAction(action);
    }

    QSignalMapper *resizeSegment = new QSignalMapper(this);
    connect(resizeSegment, SIGNAL(mapped(int)), this, SLOT(resizeSegment(int)));

    {
        KeyAction *action = new KeyAction(ConvertingString | SelectingCandidate, QT_TR_NOOP("Resize segment left"), this);
        connect(action, SIGNAL(triggered()), resizeSegment, SLOT(map()));
        resizeSegment->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(ConvertingString | SelectingCandidate, QT_TR_NOOP("Resize segment right"), this);
        connect(action, SIGNAL(triggered()), resizeSegment, SLOT(map()));
        resizeSegment->setMapping(action, + 1);
        keyActions->addAction(action);
    }

    QSignalMapper *selectCandidate = new QSignalMapper(this);
    connect(selectCandidate, SIGNAL(mapped(int)), this, SLOT(selectCandidate(int)));

    {
        KeyAction *action = new KeyAction(ConvertingString | SelectingCandidate, QT_TR_NOOP("Select previous cadidate"), this);
        connect(action, SIGNAL(triggered()), selectCandidate, SLOT(map()));
        selectCandidate->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(ConvertingString | SelectingCandidate, QT_TR_NOOP("Select next cadidate"), this);
        connect(action, SIGNAL(triggered()), selectCandidate, SLOT(map()));
        selectCandidate->setMapping(action, + 1);
        keyActions->addAction(action);
    }


    selectCandidate = new QSignalMapper(this);
    connect(selectCandidate, SIGNAL(mapped(int)), this, SLOT(selectCandidate(int)));

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString | SelectingPrediction, QT_TR_NOOP("Select previous prediction"), this);
        connect(action, SIGNAL(triggered()), selectCandidate, SLOT(map()));
        selectCandidate->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString | SelectingPrediction, QT_TR_NOOP("Select next prediction"), this);
        connect(action, SIGNAL(triggered()), selectCandidate, SLOT(map()));
        selectCandidate->setMapping(action, + 1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(ConvertingString | SelectingCandidate | SelectingPrediction, QT_TR_NOOP("Cancel"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(cancel()));
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString, QT_TR_NOOP("Clear"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(clear()));
        keyActions->addAction(action);
    }

    QSignalMapper *setCharacterMode = new QSignalMapper(this);
    connect(setCharacterMode, SIGNAL(mapped(int)), this, SLOT(setCharacterMode(int)));

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Previous character mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Next character mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, 1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Hiragana mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, Hiragana | FullWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Katakana mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, Katakana | FullWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Halfwidth Katakana mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, Katakana | HalfWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Alphabet mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, Alphabet | HalfWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Fullwidth Alphabet mode"), this);
        connect(action, SIGNAL(triggered()), setCharacterMode, SLOT(map()));
        setCharacterMode->setMapping(action, Alphabet | FullWidth);
        keyActions->addAction(action);
    }

    QSignalMapper *setTypingMode = new QSignalMapper(this);
    connect(setTypingMode, SIGNAL(mapped(int)), this, SLOT(setTypingMode(int)));

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Previous typing mode"), this);
        connect(action, SIGNAL(triggered()), setTypingMode, SLOT(map()));
        setTypingMode->setMapping(action, -1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString, QT_TR_NOOP("Next typing mode"), this);
        connect(action, SIGNAL(triggered()), setTypingMode, SLOT(map()));
        setTypingMode->setMapping(action, 1);
        keyActions->addAction(action);
    }

    QSignalMapper *convertTo = new QSignalMapper(this);
    connect(convertTo, SIGNAL(mapped(int)), this, SLOT(convertTo(int)));

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString, QT_TR_NOOP("Convert to Hiragana"), this);
        connect(action, SIGNAL(triggered()), convertTo, SLOT(map()));
        convertTo->setMapping(action, Hiragana | FullWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString, QT_TR_NOOP("Convert to Katakana"), this);
        connect(action, SIGNAL(triggered()), convertTo, SLOT(map()));
        convertTo->setMapping(action, Katakana | FullWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString, QT_TR_NOOP("Convert to Halfwidth Katakana"), this);
        connect(action, SIGNAL(triggered()), convertTo, SLOT(map()));
        convertTo->setMapping(action, Katakana | HalfWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString, QT_TR_NOOP("Convert to Alphabet"), this);
        connect(action, SIGNAL(triggered()), convertTo, SLOT(map()));
        convertTo->setMapping(action, Alphabet | HalfWidth);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(InputingString | ConvertingString, QT_TR_NOOP("Convert to Fullwidth Alphabet"), this);
        connect(action, SIGNAL(triggered()), convertTo, SLOT(map()));
        convertTo->setMapping(action, Alphabet | FullWidth);
        keyActions->addAction(action);
    }

    QSignalMapper *inputSpace = new QSignalMapper(this);
    connect(inputSpace, SIGNAL(mapped(int)), this, SLOT(inputSpace(int)));

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Input space"), this);
        connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
        inputSpace->setMapping(action, 0);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Input Halfwidth space"), this);
        connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
        inputSpace->setMapping(action, 1);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Input Fullwidth space"), this);
        connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
        inputSpace->setMapping(action, 2);
        keyActions->addAction(action);
    }

    {
        KeyAction *action = new KeyAction(EmptyString | InputingString | ConvertingString | SelectingCandidate, QT_TR_NOOP("Input the other width space"), this);
        connect(action, SIGNAL(triggered()), inputSpace, SLOT(map()));
        inputSpace->setMapping(action, -1);
        keyActions->addAction(action);
    }

    q->setGroups(QStringList() << QLatin1String("X11 Classic"));
    q->setCategoryType(MoreThanOne);
    TR(q, "categoryName", this, QT_TR_NOOP("Input/Language"));
    q->setIcon(QIcon(":/icons/japan.png"));
    metaObject()->invokeMethod(this, "updateIcon", Qt::QueuedConnection);
    TR(q, "name", this, QT_TR_NOOP("Japanese"));
    TR(q, "author", this, QT_TR_NOOP("Tasuku Suzuki"));
    TR(q, "translator", this, QT_TR_NOOP("None"));
    TR(q, "description", this, QT_TR_NOOP("Converter to input Japanese"));

    setupActions();
    tempIcon.setSingleShot(true);
    connect(&tempIcon, SIGNAL(timeout()), this, SLOT(updateIcon()));
    qimsysDebugOut();
}

#include <QApplication>

Converter::Private::~Private()
{
    qimsysDebugIn();
    qimsysDebugOut();
}

void Converter::Private::reset()
{
    commit();
    setEnabled(false);
}

void Converter::Private::exec(int action)
{
    if (!q->isActive()) return;
    qimsysDebugIn() << action;
    switch (action) {
    case QimsysApplicationManager::Reset:
        commit();
        break;
    default:
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::setEnabled(int enabled)
{
    qimsysDebugIn() << enabled;
    manager.setComposing(enabled);
    qimsysDebugOut();
}

void Converter::Private::moveCursor(int offset)
{
    qimsysDebugIn() << offset;
    switch (inputManager->state()) {
    case InputingString: {
        QimsysPreeditItemList items = preedit->items();
        if (items.count() != 1) {
            qimsysWarning() << items;
        }
        QimsysPreeditItem item = items.first();
        switch (offset) {
        case -2: // first
            offset = -item.cursor;
            break;
        case 2:  // last
            offset = item.to.length() - item.cursor;
            break;
        }
        break;
    }
    case SelectingCandidate:
        inputManager->setState(ConvertingString);
    case ConvertingString:
        switch (offset) {
        case -2: // first
            offset = -preedit->conversionIndex();
            break;
        case 2:  // last
            offset = preedit->convercionCount() - preedit->conversionIndex() - 1;
            break;
        }
        break;
    default:
        break;
    }
    preedit->move(offset, false);
    qimsysDebugOut();
}

void Converter::Private::changeSelectionLength(int diff)
{
    qimsysDebugIn() << diff;
    preedit->move(diff, true);
    qimsysDebugOut();
}

void Converter::Private::convert()
{
    qimsysDebugIn() << currentEngine;
    if (currentEngine) {
        QimsysPreeditItemList items = preedit->items();
        if (items.count() != 1) {
            qimsysWarning() << items;
        }
        if (items.first().to == interpreter.inputString()) {
            interpreter.termiateInput();
        }
        inputManager->setState(ConvertingString);
        currentEngine->convert();
    } else {
        accept = false;
    }
    qimsysDebugOut();
}

void Converter::Private::resizeSegment(int diff)
{
    qimsysDebugIn() << diff;
    if (currentEngine) {
        inputManager->setState(ConvertingString);
        currentEngine->resize(preedit->conversionIndex(), diff);
    } else {
        accept = false;
    }
    qimsysDebugOut();
}

void Converter::Private::selectCandidate(int offset)
{
    qimsysDebugIn() << offset;
    switch (inputManager->state()) {
    case EmptyString:
    case InputingString:
    case SelectingPrediction:
        if (currentEngine) {
            if (candidates.candidates().isEmpty()) {
                accept = false;
                qimsysDebugOut();
                return;
            }

            inputManager->setState(SelectingPrediction);
            int currentIndex = candidates.currentIndex();
            if (currentIndex == -1 && offset == -1) offset++;
            candidates.blockSignals(true);
            candidates.setCurrentIndex(currentIndex + offset);
            candidates.blockSignals(false);
        } else {
            accept = false;
        }
        break;
    case ConvertingString:
    case SelectingCandidate:
        if (currentEngine) {
            inputManager->setState(SelectingCandidate);
            int currentIndex = candidates.currentIndex();
            if (currentIndex == -1) offset++;
            candidates.blockSignals(true);
            candidates.setCurrentIndex(currentIndex + offset);
            candidates.blockSignals(false);
        } else {
            accept = false;
        }
        break;
    default:
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::candidateIndexChanged(int index)
{
    if (index < 0) return;
    qimsysDebugIn() << index;
    switch (inputManager->state()) {
    case ConvertingString:
    case SelectingCandidate:
        moveCursor(1);
        break;
    case EmptyString:
    case InputingString:
    case SelectingPrediction:
        inputManager->setState(SelectingPrediction);
        commit();
        break;
    default:
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::cancel()
{
    qimsysDebugIn();
    switch (inputManager->state()) {
    case ConvertingString:
    case SelectingCandidate:
        currentEngine->cancel();
    case SelectingPrediction:
        preedit->cancel();
        if (interpreter.inputString().isEmpty()) {
            inputManager->setState(EmptyString);
        } else {
            inputManager->setState(InputingString);
        }
        break;
    default:
        qimsysWarning() << inputManager->state() << "is not handled here.";
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::setCharacterMode(int character)
{
    qimsysDebugIn() << character;
    switch (character) {
    case 1:
        switch (inputManager->character()) {
        case Hiragana | FullWidth:
            character = Katakana | FullWidth;
            break;
        case Katakana | FullWidth:
            character = Katakana | HalfWidth;
            break;
        case Katakana | HalfWidth:
            character = Alphabet | FullWidth;
            break;
        case Alphabet | FullWidth:
            character = Alphabet | HalfWidth;
            break;
        case Alphabet | HalfWidth:
            character = Hiragana | FullWidth;
            break;
        default:
            qimsysWarning() << character << "not found";
            break;
        }
        break;
    case -1:
        switch (inputManager->character()) {
        case Hiragana | FullWidth:
            character = Alphabet | HalfWidth;
            break;
        case Katakana | FullWidth:
            character = Hiragana | FullWidth;
            break;
        case Katakana | HalfWidth:
            character = Katakana | FullWidth;
            break;
        case Alphabet | FullWidth:
            character = Katakana | HalfWidth;
            break;
        case Alphabet | HalfWidth:
            character = Alphabet | FullWidth;
            break;
        default:
            qimsysWarning() << character << "not found";
            break;
        }
        break;
    }
    inputManager->setCharacter(character);
    qimsysDebugOut();
}

void Converter::Private::setTypingMode(int typingMode)
{
    qimsysDebugIn() << typingMode;
    if (inputManager->character() & (Hiragana | Katakana)) {
        switch (typingMode) {
        case 1:
            switch (inputManager->typing()) {
            case Kana:
                typingMode = Roma;
                break;
            case Roma:
                typingMode = KyuriEx;
                break;
            case KyuriEx:
                typingMode = Kana;
                break;
            default:
                qimsysWarning() << typingMode << "not found";
                break;
            }
            break;
        case -1:
            switch (inputManager->typing()) {
            case Kana:
                typingMode = KyuriEx;
                break;
            case Roma:
                typingMode = Kana;
                break;
            case KyuriEx:
                typingMode = Roma;
                break;
            default:
                qimsysWarning() << typingMode << "not found";
                break;
            }
            break;
        }
        inputManager->setTyping((Typing)typingMode);
    }
    qimsysDebugOut();
}

void Converter::Private::convertTo(int character)
{
    qimsysDebugIn() << character;
    switch (character) {
    case 1:
        switch (inputManager->character()) {
        case Hiragana | FullWidth:
            character = Katakana | FullWidth;
            break;
        case Katakana | FullWidth:
            character = Katakana | HalfWidth;
            break;
        case Katakana | HalfWidth:
            character = Alphabet | FullWidth;
            break;
        case Alphabet | FullWidth:
            character = Alphabet | HalfWidth;
            break;
        case Alphabet | HalfWidth:
            character = Hiragana | FullWidth;
            break;
        default:
            qimsysWarning() << character << "not found";
            break;
        }
        break;
    case -1:
        switch (inputManager->character()) {
        case Hiragana | FullWidth:
            character = Alphabet | HalfWidth;
            break;
        case Katakana | FullWidth:
            character = Hiragana | FullWidth;
            break;
        case Katakana | HalfWidth:
            character = Katakana | FullWidth;
            break;
        case Alphabet | FullWidth:
            character = Katakana | HalfWidth;
            break;
        case Alphabet | HalfWidth:
            character = Alphabet | FullWidth;
            break;
        default:
            qimsysWarning() << character << "not found";
            break;
        }
        break;
    }
    switch (inputManager->state()) {
    case InputingString: {
        QString str = interpreter.inputString(character);
        QimsysPreeditItem item;
        item.to = str;
        item.cursor = str.length();
        item.selection = 0;
#ifdef QIMSYS_PLATFORM_MAEMO
        item.underline = QTextCharFormat::SingleUnderline;
#else
        item.underline = QTextCharFormat::WaveUnderline;
#endif
        preedit->setItems(QimsysPreeditItemList() << item);
        break;
    }
    case ConvertingString:
//   preedit->setCurrentText( interpreter.inputString( character ) );
        break;
    default:
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::inputSpace(int width)
{
    qimsysDebugIn();
    int current = inputManager->character();
    bool temp = inputManager->isTemp();
    int character = Alphabet;
    switch (width) {
    case -1:
        if (current & FullWidth) {
            character |= HalfWidth;
        } else {
            character |= FullWidth;
        }
        break;
    case 0:
        character |= (current & WidthMask);
        break;
    case 1:
        character |= HalfWidth;
        break;
    case 2:
        character |= FullWidth;
        break;
    }
    State state = inputManager->state();
    inputManager->setCharacter(character, true);
    keyPressed(" ", 0, 0, false);
    inputManager->setCharacter(current, temp);
    switch (state) {
    case EmptyString:
        commit();
        break;
    case InputingString:
    case ConvertingString:
    case SelectingCandidate:
    case SelectingPrediction:
        break;
    default:
        qimsysWarning() << state << "is not handled here.";
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::predictionsChanged(const QimsysConversionItemList &list)
{
    if (!q->isActive()) return;
    qimsysDebugIn() << list;
    switch (inputManager->state()) {
    case EmptyString:
    case InputingString:
        candidates.setCandidates(list);
    case SelectingPrediction:
    case ConvertingString:
        // do nothing
        break;
    default:
        qimsysWarning() << inputManager->state() << "not handled.";
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat)
{
    if (!q->isActive() || keyManager.isAccepted()) return;
    int k1 = 0;
    switch (keycode) {
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
    case Qt::Key_Shift:
        break;
    default:
        k1 += keycode;
        break;
    }
    if (modifiers & Qt::ControlModifier) k1 += Qt::CTRL;
    if (modifiers & Qt::AltModifier) k1 += Qt::ALT;
    if (modifiers & Qt::ShiftModifier) k1 += Qt::SHIFT;
    QimsysKeySequence seq(k1);
    qimsysDebugIn() << seq.toString();

    const KeyActionDataList &list = keyActions->list();
    for (int i = 0; i < list.count(); i++) {
        const KeyActionData &data = list.at(i);
        if (data.state == inputManager->state() && data.key == seq) {
            QimsysPreeditItemList items = preedit->items();
            QString preeditString;
            foreach(const QimsysPreeditItem &item, items) {
                preeditString.append(item.to);
            }
            switch (inputManager->state()) {
            case InputingString:
                if (!inputManager->isTemp() && preeditString == interpreter.inputString()) {
                    interpreter.termiateInput();
                }
                break;
            default:
                break;
            }
            accept = true;
            data.action->execute();
            if (accept) {
                keyManager.accept();
            }
            break;
        }
    }
    if (keyManager.isAccepted()) {
        if (inputManager->state() == InputingString && preedit->items().isEmpty()) {
            inputManager->setState(EmptyString);
        }
    } else {
        bool hasInput = false;
        if (!text.isEmpty()) {
            if (text.at(0).isPrint()) {
                hasInput = true;
            } else if (text.at(0).isSpace() && !preedit->items().isEmpty()) {
                hasInput = true;
            }
        }
        bool isModifiers = (modifiers & (Qt::CTRL | Qt::ALT));
        switch (inputManager->state()) {
        case Direct:
            break;
        case ConvertingString:
        case SelectingCandidate:
        case SelectingPrediction:
            if (hasInput) {
                commit();
                inputManager->setState(EmptyString);
                interpreter.clear();
            } else {
                keyManager.accept();
                break;
            }
        case EmptyString:
            if (!hasInput || isModifiers) {
                break;
            }
        case InputingString:
            if (hasInput) {
                inputManager->setState(InputingString);
                interpreter.input(text);

                QRegExp capitals("[A-Z]$");
                // avoid to go into temp mode for KyuriEx
                switch (inputManager->typing()) {
                case Roma:
                case Kana:
                    if ((inputManager->character() & CharacterMask) != Alphabet && capitals.indexIn(interpreter.inputString()) > -1) {
                        interpreter.termiateInput();
                        inputManager->setCharacter(Alphabet | HalfWidth, true);
                    }
                    break;
                 default:
                    break;
                }
                keyManager.accept();
            } else if (isModifiers) {
                keyManager.accept();
            } else {
                keyManager.accept();
            }
            break;
        }
    }
    qimsysDebugOut() << keyManager.isAccepted();
}

void Converter::Private::focusChanged(bool focus)
{
    if (!q->isActive()) return;
    qimsysDebugIn() << focus;
    Q_UNUSED(focus)
    QTimer::singleShot(100, this, SLOT(focusChanged()));
    qimsysDebugOut();
}

void Converter::Private::focusChanged()
{
    qimsysDebugIn();
    updateIcon();
    qimsysDebugOut();
}

void Converter::Private::composingChanged(bool composing)
{
    if (!q->isActive()) return;
    qimsysDebugIn() << composing;
    if (composing) {
        inputManager->setState(EmptyString);
    } else {
        inputManager->setState(Direct);
    }
    foreach(QActionGroup *group, characterActions) {
        foreach(QAction *action, group->actions()) {
            if (action->data().toInt() == inputManager->character()) {
                action->setChecked(composing);
                break;
            }
        }
    }
    updateIcon();
    qimsysDebugOut();
}

void Converter::Private::stateChanged(State state)
{
    qimsysDebugIn() << state;
    manager.setComposing(state != Direct);
    switch (state) {
    case Direct:
        candidates.clear();
        preedit->setConversions(QimsysConversionItemList());
        break;
    case EmptyString:
        candidates.clear();
        preedit->setConversions(QimsysConversionItemList());
        break;
    case InputingString:
        candidates.clear();
        preedit->setConversions(QimsysConversionItemList());
        break;
    case ConvertingString:
        candidates.clear();
        break;
    case SelectingCandidate:
        Q_ASSERT(currentEngine);
        candidates.setCandidates(currentEngine->candidates(preedit->conversionIndex()));
        break;
    case SelectingPrediction:
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::characterChanged(int character)
{
    qimsysDebugIn() << character;
    Q_UNUSED(character);
    updateIcon();
    qimsysDebugOut();
}

void Converter::Private::typingChanged(Typing typing)
{
    qimsysDebugIn() << typing;
    updateIcon(typing);

    // TODO use QTimer and reset it while running
    if (tempIcon.isActive()) {
        tempIcon.setInterval(1000);
    } else {
        tempIcon.start(1000);
    }
    qimsysDebugOut();
}

void Converter::Private::updateIcon(Typing typing)
{
    qimsysDebugIn() << typing;
    bool focus = manager.focus();
    bool composing = manager.composing();

    foreach(QActionGroup *group, characterActions) {
        foreach(QAction *action, group->actions()) {
            if (action->data().toInt() == inputManager->character()) {
                action->setChecked(composing);
                QAction *parent = qobject_cast<QAction*>(action->parent());
                if (parent) {
                    QIcon icon;
                    foreach (const QSize &size, action->icon().availableSizes()) {
                        icon.addPixmap(action->icon().pixmap(size, (composing ? QIcon::Normal : QIcon::Disabled)));
                    }
                    parent->setIcon(icon);
                }
                break;
            }
        }
    }

    foreach(QActionGroup *group, typingActions) {
        foreach(QAction *action, group->actions()) {
            if ((Typing)action->data().toInt() == inputManager->typing()) {
                if (!action->isChecked()) {
                    action->setChecked(true);
                }
                QAction *parent = qobject_cast<QAction*>(action->parent());
                if (parent) {
                    parent->setIcon(action->icon());
                }
                break;
            }
        }
    }

    QPixmap pix(":/icons/japan.png");
    QPainter p(&pix);
    QString png;
    switch (typing) {
    case Roma:
        png = "roma";
        break;
    case Kana:
        png = "kana";
        break;
    case KyuriEx:
        png = "kyuriex";
        break;
    default:
        switch (inputManager->state()) {
        case Direct:
            break;
        default:
            switch (inputManager->character() & CharacterMask) {
            case Alphabet:
                png = "alphabet";
                break;
            case Hiragana:
                png = "hiragana";
                break;
            case Katakana:
                png = "katakana";
                break;
            default:
                qimsysWarning() << inputManager->character();
                break;
            }
            switch (inputManager->character() & WidthMask) {
            case HalfWidth:
                png += "_half";
                break;
            case FullWidth:
                png += "_full";
                break;
            default:
                qimsysWarning() << inputManager->character();
                break;
            }
            break;
        }
        break;
    }
    p.setOpacity(0.75);
    QPixmap override(QString(":/icons/%1.png").arg(png));
    if (!override.isNull()) {
        p.scale((qreal)pix.width() / (qreal)override.width(), (qreal)pix.height() / (qreal)override.height());
        p.drawPixmap(0, 0, override);
    }
    p.end();
    manager.setCurrentIcon(QIcon(pix));
    qimsysDebugOut();
}

void Converter::Private::commit()
{
    if (preedit->items().isEmpty()) return;
    qimsysDebugIn();
    switch (inputManager->state()) {
    case InputingString: {
        QimsysPreeditItemList items = preedit->items();
        if (items.count() != 1) {
            qimsysWarning() << items;
        }
        if (items.first().to == interpreter.inputString()) {
            interpreter.termiateInput();
        }
        // fall through
    }
    case ConvertingString:
    case SelectingCandidate:
    case SelectingPrediction:
        preedit->commit();
        interpreter.clear();
        inputManager->setState(EmptyString);
        break;

    default:
        qimsysWarning() << inputManager->state() << "is not handled here" << preedit->items();
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::clear()
{
    qimsysDebugIn();
    switch (inputManager->state()) {
    case EmptyString:
        candidates.clear();
        break;
    case InputingString:
        preedit->cancel();
        interpreter.clear();
        inputManager->setState(EmptyString);
        break;
    default:
//   qimsysWarning() << inputManager->state();
        break;
    }
    qimsysDebugOut();
}

void Converter::Private::setupActions()
{
    qimsysDebugIn();

    // Character types
    {
        QActionGroup *group = new QActionGroup(this);
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setCharacterMode(QAction*)));
        characterActions.append(group);

        QAction *parent = new QAction(this);
        TR(parent, "text", this, QT_TR_NOOP("Character"));
#define ADDACTION( text, data, png ) \
    { \
        QAction *action = new QAction(parent); \
        TR(action, "text", this, text); \
        action->setData(data); \
        action->setIcon(QIcon(":/icons/" #png ".png")); \
        action->setCheckable(true); \
        if (inputManager->character() == (data)) { \
            parent->setIcon(action->icon()); \
            action->setChecked(true); \
        } \
        group->addAction(action); \
    }
        ADDACTION(QT_TR_NOOP("Direct"), 0,);
        new QAction("-", parent);
        ADDACTION(QT_TR_NOOP("Hiragana"), Hiragana | FullWidth, hiragana_full);
        ADDACTION(QT_TR_NOOP("Fullwidth Katakana"), Katakana | FullWidth, katakana_full);
        ADDACTION(QT_TR_NOOP("Halfwidth Katakana"), Katakana | HalfWidth, katakana_half);
        ADDACTION(QT_TR_NOOP("Halfwidth Alphabet"), Alphabet | HalfWidth, alphabet_half);
        ADDACTION(QT_TR_NOOP("Fullwidth Alphabet"), Alphabet | FullWidth, alphabet_full);
#undef ADDACTION
        foreach(QAction *action, group->actions()) {
            if (!action->data().toInt()) {
                action->setCheckable(false);
            }
        }
        actions.append(parent);
    }

    // Typing type
    {
        QActionGroup *group = new QActionGroup(this);
        connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setTyping(QAction*)));
        typingActions.append(group);

        QAction *parent = new QAction(this);
        TR(parent, "text", this, QT_TR_NOOP("Typing"));
#define ADDACTION( text, data, png ) \
    { \
        QAction *action = new QAction(parent); \
        TR( action, "text", this, text ); \
        action->setData( data ); \
        action->setIcon( QIcon( ":/icons/" #png ".png" ) ); \
        action->setCheckable( true ); \
        action->setChecked( inputManager->typing() == data ); \
        if( action->isChecked() ) parent->setIcon( action->icon() ); \
        group->addAction(action); \
    }
        ADDACTION(QT_TR_NOOP("Romaji"), Roma, roma);
        ADDACTION(QT_TR_NOOP("Kana"), Kana, kana);
        ADDACTION(QT_TR_NOOP("KyuriEx"), KyuriEx, kyuriex);
#undef ADDACTION
        actions.append(parent);
    }

    qimsysDebugOut();
}

void Converter::Private::setCharacterMode(QAction *action)
{
    qimsysDebugIn() << action;
    int character = action->data().toInt();
    if (character > 0) {
        if (inputManager->state() == Direct) {
            inputManager->setState(EmptyString);
            updateIcon();
        }
        inputManager->setCharacter(character);
    } else if (inputManager->state() != Direct) {
        inputManager->setState(Direct);
        updateIcon();
    }
    qimsysDebugOut();
}

void Converter::Private::setTyping(QAction *action)
{
    qimsysDebugIn() << action;
    inputManager->setTyping((Typing)action->data().toInt());
    updateIcon();
    qimsysDebugOut();
}

void Converter::Private::currentEngineChanged(const QString &name)
{
    qimsysDebugIn() << name;
    foreach(QimsysEngine *engine, QimsysPluginManager::objects<QimsysEngine>(this)) {
        if (engine->identifier() == name) {
            if (currentEngine) {
                disconnect(&interpreter, SIGNAL(rawStringChanged(QString)), currentEngine, SLOT(setRawString(QString)));
                disconnect(&interpreter, SIGNAL(inputStringChanged(QString)), currentEngine, SLOT(setInputString(QString)));
                disconnect(currentEngine, SIGNAL(predicted(QimsysConversionItemList)), this, SLOT(predictionsChanged(QimsysConversionItemList)));
                disconnect(currentEngine, SIGNAL(conversionsChanged(QimsysConversionItemList)), preedit, SLOT(setConversions(QimsysConversionItemList)));
                disconnect(preedit, SIGNAL(committed(QimsysConversionItemList)), currentEngine, SLOT(commit(QimsysConversionItemList)));
            }
            currentEngine = engine;
            connect(&interpreter, SIGNAL(rawStringChanged(QString)), currentEngine, SLOT(setRawString(QString)));
            connect(&interpreter, SIGNAL(inputStringChanged(QString)), currentEngine, SLOT(setInputString(QString)));
            connect(currentEngine, SIGNAL(predicted(QimsysConversionItemList)), this, SLOT(predictionsChanged(QimsysConversionItemList)), Qt::QueuedConnection);
            connect(currentEngine, SIGNAL(conversionsChanged(QimsysConversionItemList)), preedit, SLOT(setConversions(QimsysConversionItemList)));
            connect(preedit, SIGNAL(committed(QimsysConversionItemList)), currentEngine, SLOT(commit(QimsysConversionItemList)));
            return;
        }
    }

    // not found
    if (currentEngine) {
        disconnect(&interpreter, SIGNAL(rawStringChanged(QString)), currentEngine, SLOT(setRawString(QString)));
        disconnect(&interpreter, SIGNAL(inputStringChanged(QString)), currentEngine, SLOT(setInputString(QString)));
        disconnect(currentEngine, SIGNAL(predicted(QimsysConversionItemList)), this, SLOT(predictionsChanged(QimsysConversionItemList)));
        disconnect(currentEngine, SIGNAL(conversionsChanged(QimsysConversionItemList)), preedit, SLOT(setConversions(QimsysConversionItemList)));
        disconnect(preedit, SIGNAL(committed(QimsysConversionItemList)), currentEngine, SLOT(commit(QimsysConversionItemList)));
    }
    currentEngine = 0;
    qimsysDebugOut();
}

Converter::Converter(QObject *parent)
    : QimsysConverter(parent)
{
    qimsysDebugIn() << parent;
    d = new Private(this);
    qimsysDebugOut();
}

Converter::~Converter()
{
    qimsysDebugIn();
    delete d;
    qimsysDebugOut();
}

QString Converter::identifier() const
{
    return QLatin1String("ja_JP");
}

QStringList Converter::locales() const
{
    return QStringList() << "ja_JP";
}

bool Converter::useEngine() const
{
    return true;
}

QList<QAction*> Converter::actions()
{
    return d->actions;
}

QimsysSettingsWidget *Converter::settings(const QString &hint, QWidget *parent)
{
    return new JapaneseSettings(this, parent);
}

#include "converter.moc"
