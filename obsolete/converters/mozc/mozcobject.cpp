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

#include "mozcobject.h"

#include <QKeyEvent>
#include <QAction>

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsyskeymanager.h>
#include <qimsyspreedit.h>
#include <qimsyscandidates.h>

#include "base/scoped_ptr.h"
#include "client/session.h"
#include "session/commands.pb.h"

namespace mozc {
class MozcObject::Private : private QObject
{
    Q_OBJECT
public:
    Private(MozcObject *parent);
    ~Private();

    QList<QAction*> actions();

private slots:
    void init();

    void keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat);

    void focusChanged(uint focus);
    void setCharacterMode(QAction* action);

private:
    void setCharacterMode(commands::CompositionMode newMode);
    void update(const commands::Output &output);

private:
    MozcObject *q;

    QimsysApplicationManager manager;
    QimsysKeyManager keyManager;
    QimsysPreedit preedit;
    QimsysCandidates candidates;

    scoped_ptr<client::Session> session;
    commands::CompositionMode mode;
};
}

using namespace mozc;

MozcObject::Private::Private(MozcObject *parent)
    : QObject(parent)
    , q(parent)
    , session(new client::Session)
    , mode(commands::DIRECT)
{
    q->setGroups(QStringList() << QLatin1String("X11"));
    q->setCategoryType(MoreThanOne);
    q->setCategoryName(tr("Input/Language"));
    q->setIcon(QIcon(":/icons/mozc.png"));
    q->setName(tr("Mozc"));
    q->setAuthor(tr("Tasuku Suzuki"));
    q->setTranslator(tr("None"));
    q->setDescription(tr("Mozc plugin for qimsys"));

    metaObject()->invokeMethod(this, "init", Qt::QueuedConnection);
}

MozcObject::Private::~Private()
{

}

void MozcObject::Private::init()
{
    manager.init();
    connect(&manager, SIGNAL(focusChanged(uint)), this, SLOT(focusChanged(uint)));

    keyManager.init();
    connect(&keyManager, SIGNAL(keyPressed(QString,int,int,bool)), this, SLOT(keyPressed(QString,int,int,bool)));

    preedit.init();

    candidates.init();

    session->EnsureConnection();
}

void MozcObject::Private::focusChanged(uint focus)
{
    if (!q->isActive()) return;
    qimsysDebugIn() << focus;
    if (focus) {
        if (mode != commands::DIRECT) {
            manager.setComposing(true);
        }
    } else {
        commands::SessionCommand command;
        command.set_type(commands::SessionCommand::REVERT);
        commands::Output output;
        session->SendCommand(command, &output);
        update(output);
    }
    qimsysDebugOut();
}

void MozcObject::Private::setCharacterMode(commands::CompositionMode newMode)
{
    if (mode == newMode) return;
    mode = newMode;

    commands::SessionCommand command;
    commands::Output output;
    switch (mode) {
    case commands::DIRECT:
        command.set_type(commands::SessionCommand::SUBMIT);
        session->SendCommand(command, &output);
        update(output);
        manager.setComposing(false);
        break;
    default:
        command.set_type(commands::SessionCommand::SWITCH_INPUT_MODE);
        command.set_composition_mode(mode);
        session->SendCommand(command, &output);
        manager.setComposing(manager.focus() > 0);
        break;
    }

}

void MozcObject::Private::setCharacterMode(QAction* action)
{
    setCharacterMode((commands::CompositionMode)action->data().toInt());
}

QList<QAction*> MozcObject::Private::actions()
{
    QList<QAction*> ret;

    QActionGroup *group = new QActionGroup(this);
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(setCharacterMode(QAction*)));

    QAction *modes = new QAction(this);
    modes->setText(tr("Mode"));

#define ADDACTION( text, data, png ) \
    { \
        QAction *action = new QAction(modes); \
        action->setText(text); \
        action->setData(data); \
        action->setIcon(QIcon(":/icons/" #png ".png")); \
        action->setCheckable(true); \
        if (mode == (data)) { \
            modes->setIcon(action->icon()); \
            action->setChecked(true); \
        } \
        group->addAction(action); \
    }
    ADDACTION(QT_TR_NOOP("&Hiragana"), commands::HIRAGANA, hiragana_full);
    ADDACTION(QT_TR_NOOP("Fullwidth &Katakana"), commands::FULL_KATAKANA, katakana_full);
    ADDACTION(QT_TR_NOOP("&Halfwidth Katakana"), commands::HALF_KATAKANA, katakana_half);
    ADDACTION(QT_TR_NOOP("Halfwidth &Ascii"), commands::HALF_ASCII, alphabet_half);
    ADDACTION(QT_TR_NOOP("&Fullwidth Ascii"), commands::FULL_ASCII, alphabet_full);
    new QAction("-", modes);
    ADDACTION(QT_TR_NOOP("&Direct"), commands::DIRECT,);
#undef ADDACTION

    ret.append(modes);
    return ret;
}

void MozcObject::Private::keyPressed(const QString &text, int keycode, int modifiers, bool autoRepeat)
{
    if (!q->isActive() || keyManager.isAccepted()) return;
    qimsysDebugIn() << text << keycode << modifiers << autoRepeat;
    if (keycode == Qt::Key_Zenkaku_Hankaku) {
        if (mode == commands::DIRECT) {
            setCharacterMode(commands::HIRAGANA);
        } else {
            setCharacterMode(commands::DIRECT);
        }
        qimsysDebugOut();
        return;
    }
    if (mode == commands::DIRECT) {
        qimsysDebugOut();
        return;
    }
    commands::KeyEvent key;
    if (!text.isEmpty() && text.at(0).isPrint() && !text.at(0).isSpace()) {
        key.set_key_code(text.at(0).unicode());
    } else if (modifiers > 0) {
        int m = 0;
        if (modifiers & Qt::CTRL)
            m |= commands::KeyEvent::CTRL;
        if (modifiers & Qt::SHIFT)
            m |= commands::KeyEvent::SHIFT;
        if (modifiers & Qt::ALT)
            m |= commands::KeyEvent::ALT;
        key.set_modifiers(m);
    } else {
        commands::KeyEvent::SpecialKey specialKey = commands::KeyEvent::NO_SPECIALKEY;
        switch (keycode) {
#define CASE(qtkey, mozckey) \
        case Qt::Key_##qtkey: \
            specialKey = commands::KeyEvent::mozckey; \
            break
            CASE(Space, SPACE);
            CASE(Return, ENTER);
            CASE(Left, LEFT);
            CASE(Right, RIGHT);
            CASE(Up, UP);
            CASE(Down, DOWN);
            CASE(Escape, ESCAPE);
            CASE(Delete, DEL);
            CASE(Backspace, BACKSPACE);
            CASE(Insert, INSERT);
            CASE(Henkan, HENKAN);
            CASE(Muhenkan, MUHENKAN);
            CASE(Hiragana, KANA);
            CASE(Katakana, KANA);
            CASE(Eisu_toggle, EISU);
            CASE(Home, HOME);
            CASE(End, END);
            CASE(Tab, TAB);
            CASE(F1, F1);
            CASE(F2, F2);
            CASE(F3, F3);
            CASE(F4, F4);
            CASE(F5, F5);
            CASE(F6, F6);
            CASE(F7, F7);
            CASE(F8, F8);
            CASE(F9, F9);
            CASE(F10, F10);
            CASE(F11, F11);
            CASE(F12, F12);
            CASE(F13, F13);
            CASE(F14, F14);
            CASE(F15, F15);
            CASE(F16, F16);
            CASE(F17, F17);
            CASE(F18, F18);
            CASE(F19, F19);
            CASE(F20, F20);
            CASE(F21, F21);
            CASE(F22, F22);
            CASE(F23, F23);
            CASE(F24, F24);
            CASE(PageUp, PAGE_UP);
            CASE(PageDown, PAGE_DOWN);

#undef CASE
        default:
            break;
        }
        key.set_special_key(specialKey);
    }

    commands::Output output;
    if (session->SendKey(key, &output)) {
//        qDebug() << QString::fromUtf8(output.DebugString().c_str());
        update(output);
        if (output.consumed()) {
            keyManager.accept();
        }
    }
    qimsysDebugOut();
}

void MozcObject::Private::update(const commands::Output &output)
{
    qimsysDebugIn();
    if (output.has_result()) {
        const commands::Result &r = output.result();
        QString commitText = QString::fromUtf8(r.value().c_str());
        preedit.commit(manager.widget());
        qimsysDebug() << commitText;
    }

    QimsysConversionItemList candidateItems;
    if (output.has_candidates()) {
        const commands::Candidates &c = output.candidates();
        for (int i = 0; i < c.candidate_size(); i++) {
            const commands::Candidates_Candidate cc = c.candidate(i);
            QimsysConversionItem item;
            item.to = QString::fromUtf8(cc.value().c_str());
            item.index = i;

            candidateItems.append(item);
        }


        if (c.has_focused_index()) {
            candidates.setCurrentIndex(c.focused_index());
        } else {
            candidates.setCurrentIndex(0);
        }
    }
    candidates.setCandidates(candidateItems);

    QimsysPreeditItemList preeditItems;
    if (output.has_preedit()) {
        const commands::Preedit &p = output.preedit();
        int cursor = p.has_highlighted_position() ? p.highlighted_position() : p.cursor();
        int length = 0;
        for (int i = 0; i < p.segment_size(); i++ ) {
            const commands::Preedit::Segment &s = p.segment(i);
            QimsysPreeditItem item;
            item.text = QString::fromUtf8(s.value().c_str());
            item.underline = QTextCharFormat::NoUnderline;
            if (length <= cursor && cursor < length + item.text.length()) {
                item.cursor = cursor - length;
            } else {
                item.cursor = -1;
            }
            item.selection = 0;
            switch (s.annotation()) {
            case commands::Preedit::Segment::NONE:
                break;
            case commands::Preedit::Segment::UNDERLINE:
                item.underline = QTextCharFormat::WaveUnderline;
                break;
            case commands::Preedit::Segment::HIGHLIGHT:
                item.cursor = item.text.length();
                item.selection = -item.cursor;
                break;
            }

            preeditItems.append(item);
            length += item.text.length();
        }
    }
    preedit.setItems(preeditItems);

    if (output.has_mode()) {
        const commands::CompositionMode &m = output.mode();
        setCharacterMode(m);
    }
    qimsysDebugOut();
}

MozcObject::MozcObject(QObject *parent)
    : QimsysConverter(parent)
{
    d = new Private(this);
}

MozcObject::~MozcObject()
{
    delete d;
}

QString MozcObject::identifier() const
{
    return QLatin1String("Mozc");
}

QStringList MozcObject::locales() const
{
    return QStringList() << "ja_JP";
}

bool MozcObject::useEngine() const
{
    return false;
}

QList<QAction*> MozcObject::actions()
{
    return d->actions();
}

#include "mozcobject.moc"
