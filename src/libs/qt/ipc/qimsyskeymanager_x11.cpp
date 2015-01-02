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

#include "qimsyskeymanager.h"
#include "qimsysdebug.h"

#include "qimsysapplicationmanager.h"

#include <QX11Info>
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_KOREAN
#define XK_XKB_KEYS
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>

// copied from src/gui/kernel/qkeymapper_x11.cpp
// keyboard mapping table
static const unsigned int KeyTbl[] = {

    // misc keys

    XK_Escape,                  Qt::Key_Escape,
    XK_Tab,                     Qt::Key_Tab,
    XK_ISO_Left_Tab,            Qt::Key_Backtab,
    XK_BackSpace,               Qt::Key_Backspace,
    XK_Return,                  Qt::Key_Return,
    XK_Insert,                  Qt::Key_Insert,
    XK_Delete,                  Qt::Key_Delete,
    XK_Clear,                   Qt::Key_Delete,
    XK_Pause,                   Qt::Key_Pause,
    XK_Print,                   Qt::Key_Print,
    0x1005FF60,                 Qt::Key_SysReq,         // hardcoded Sun SysReq
    0x1007ff00,                 Qt::Key_SysReq,         // hardcoded X386 SysReq

    XK_F1,                      Qt::Key_F1,
    XK_F2,                      Qt::Key_F2,
    XK_F3,                      Qt::Key_F3,
    XK_F4,                      Qt::Key_F4,
    XK_F5,                      Qt::Key_F5,
    XK_F6,                      Qt::Key_F6,
    XK_F7,                      Qt::Key_F7,
    XK_F8,                      Qt::Key_F8,
    XK_F9,                      Qt::Key_F9,
    XK_F10,                     Qt::Key_F10,
    XK_F11,                     Qt::Key_F11,
    XK_F12,                     Qt::Key_F12,
    // cursor movement

    XK_Home,                    Qt::Key_Home,
    XK_End,                     Qt::Key_End,
    XK_Left,                    Qt::Key_Left,
    XK_Up,                      Qt::Key_Up,
    XK_Right,                   Qt::Key_Right,
    XK_Down,                    Qt::Key_Down,
    XK_Prior,                   Qt::Key_PageUp,
    XK_Next,                    Qt::Key_PageDown,

    // modifiers

    XK_Shift_L,                 Qt::Key_Shift,
    XK_Shift_R,                 Qt::Key_Shift,
    XK_Shift_Lock,              Qt::Key_Shift,
    XK_Control_L,               Qt::Key_Control,
    XK_Control_R,               Qt::Key_Control,
    XK_Meta_L,                  Qt::Key_Meta,
    XK_Meta_R,                  Qt::Key_Meta,
    XK_Alt_L,                   Qt::Key_Alt,
    XK_Alt_R,                   Qt::Key_Alt,
    XK_Caps_Lock,               Qt::Key_CapsLock,
    XK_Num_Lock,                Qt::Key_NumLock,
    XK_Scroll_Lock,             Qt::Key_ScrollLock,
    XK_Super_L,                 Qt::Key_Super_L,
    XK_Super_R,                 Qt::Key_Super_R,
    XK_Menu,                    Qt::Key_Menu,
    XK_Hyper_L,                 Qt::Key_Hyper_L,
    XK_Hyper_R,                 Qt::Key_Hyper_R,
    XK_Help,                    Qt::Key_Help,
    0x1000FF74,                 Qt::Key_Backtab,        // hardcoded HP backtab
    0x1005FF10,                 Qt::Key_F11,            // hardcoded Sun F36 (labeled F11)
    0x1005FF11,                 Qt::Key_F12,            // hardcoded Sun F37 (labeled F12)

    // International input method support keys

    // International & multi-key character composition
    XK_ISO_Level3_Shift,        Qt::Key_AltGr,
    XK_Multi_key,  Qt::Key_Multi_key,
    XK_Codeinput,  Qt::Key_Codeinput,
    XK_SingleCandidate,  Qt::Key_SingleCandidate,
    XK_MultipleCandidate, Qt::Key_MultipleCandidate,
    XK_PreviousCandidate, Qt::Key_PreviousCandidate,

    // Misc Functions
    XK_Mode_switch,  Qt::Key_Mode_switch,
    XK_script_switch,  Qt::Key_Mode_switch,

    // Japanese keyboard support
    XK_Kanji,   Qt::Key_Kanji,
    XK_Muhenkan,  Qt::Key_Muhenkan,
    //XK_Henkan_Mode,  Qt::Key_Henkan_Mode,
    XK_Henkan_Mode,  Qt::Key_Henkan,
    XK_Henkan,   Qt::Key_Henkan,
    XK_Romaji,   Qt::Key_Romaji,
    XK_Hiragana,  Qt::Key_Hiragana,
    XK_Katakana,  Qt::Key_Katakana,
    XK_Hiragana_Katakana, Qt::Key_Hiragana_Katakana,
    XK_Zenkaku,   Qt::Key_Zenkaku,
    XK_Hankaku,   Qt::Key_Hankaku,
    XK_Zenkaku_Hankaku,  Qt::Key_Zenkaku_Hankaku,
    XK_Touroku,   Qt::Key_Touroku,
    XK_Massyo,   Qt::Key_Massyo,
    XK_Kana_Lock,  Qt::Key_Kana_Lock,
    XK_Kana_Shift,  Qt::Key_Kana_Shift,
    XK_Eisu_Shift,  Qt::Key_Eisu_Shift,
    XK_Eisu_toggle,  Qt::Key_Eisu_toggle,
    //XK_Kanji_Bangou,  Qt::Key_Kanji_Bangou,
    //XK_Zen_Koho,  Qt::Key_Zen_Koho,
    //XK_Mae_Koho,  Qt::Key_Mae_Koho,
    XK_Kanji_Bangou,  Qt::Key_Codeinput,
    XK_Zen_Koho,  Qt::Key_MultipleCandidate,
    XK_Mae_Koho,  Qt::Key_PreviousCandidate,

#ifdef XK_KOREAN
    // Korean keyboard support
    XK_Hangul,   Qt::Key_Hangul,
    XK_Hangul_Start,  Qt::Key_Hangul_Start,
    XK_Hangul_End,  Qt::Key_Hangul_End,
    XK_Hangul_Hanja,  Qt::Key_Hangul_Hanja,
    XK_Hangul_Jamo,  Qt::Key_Hangul_Jamo,
    XK_Hangul_Romaja,  Qt::Key_Hangul_Romaja,
    //XK_Hangul_Codeinput, Qt::Key_Hangul_Codeinput,
    XK_Hangul_Codeinput, Qt::Key_Codeinput,
    XK_Hangul_Jeonja,  Qt::Key_Hangul_Jeonja,
    XK_Hangul_Banja,  Qt::Key_Hangul_Banja,
    XK_Hangul_PreHanja,  Qt::Key_Hangul_PreHanja,
    XK_Hangul_PostHanja, Qt::Key_Hangul_PostHanja,
    //XK_Hangul_SingleCandidate,Qt::Key_Hangul_SingleCandidate,
    //XK_Hangul_MultipleCandidate,Qt::Key_Hangul_MultipleCandidate,
    //XK_Hangul_PreviousCandidate,Qt::Key_Hangul_PreviousCandidate,
    XK_Hangul_SingleCandidate, Qt::Key_SingleCandidate,
    XK_Hangul_MultipleCandidate, Qt::Key_MultipleCandidate,
    XK_Hangul_PreviousCandidate, Qt::Key_PreviousCandidate,
    XK_Hangul_Special,  Qt::Key_Hangul_Special,
    //XK_Hangul_switch,  Qt::Key_Hangul_switch,
    XK_Hangul_switch,  Qt::Key_Mode_switch,
#endif  // XK_KOREAN

    // dead keys
    XK_dead_grave,              Qt::Key_Dead_Grave,
    XK_dead_acute,              Qt::Key_Dead_Acute,
    XK_dead_circumflex,         Qt::Key_Dead_Circumflex,
    XK_dead_tilde,              Qt::Key_Dead_Tilde,
    XK_dead_macron,             Qt::Key_Dead_Macron,
    XK_dead_breve,              Qt::Key_Dead_Breve,
    XK_dead_abovedot,           Qt::Key_Dead_Abovedot,
    XK_dead_diaeresis,          Qt::Key_Dead_Diaeresis,
    XK_dead_abovering,          Qt::Key_Dead_Abovering,
    XK_dead_doubleacute,        Qt::Key_Dead_Doubleacute,
    XK_dead_caron,              Qt::Key_Dead_Caron,
    XK_dead_cedilla,            Qt::Key_Dead_Cedilla,
    XK_dead_ogonek,             Qt::Key_Dead_Ogonek,
    XK_dead_iota,               Qt::Key_Dead_Iota,
    XK_dead_voiced_sound,       Qt::Key_Dead_Voiced_Sound,
    XK_dead_semivoiced_sound,   Qt::Key_Dead_Semivoiced_Sound,
    XK_dead_belowdot,           Qt::Key_Dead_Belowdot,
    XK_dead_hook,               Qt::Key_Dead_Hook,
    XK_dead_horn,               Qt::Key_Dead_Horn,

    // Special multimedia keys
    // currently only tested with MS internet keyboard

    // browsing keys
    XF86XK_Back,                Qt::Key_Back,
    XF86XK_Forward,             Qt::Key_Forward,
    XF86XK_Stop,                Qt::Key_Stop,
    XF86XK_Refresh,             Qt::Key_Refresh,
    XF86XK_Favorites,           Qt::Key_Favorites,
    XF86XK_AudioMedia,          Qt::Key_LaunchMedia,
    XF86XK_OpenURL,             Qt::Key_OpenUrl,
    XF86XK_HomePage,            Qt::Key_HomePage,
    XF86XK_Search,              Qt::Key_Search,

    // media keys
    XF86XK_AudioLowerVolume,    Qt::Key_VolumeDown,
    XF86XK_AudioMute,           Qt::Key_VolumeMute,
    XF86XK_AudioRaiseVolume,    Qt::Key_VolumeUp,
    XF86XK_AudioPlay,           Qt::Key_MediaPlay,
    XF86XK_AudioStop,           Qt::Key_MediaStop,
    XF86XK_AudioPrev,           Qt::Key_MediaPrevious,
    XF86XK_AudioNext,           Qt::Key_MediaNext,
    XF86XK_AudioRecord,         Qt::Key_MediaRecord,

    // launch keys
    XF86XK_Mail,                Qt::Key_LaunchMail,
    XF86XK_MyComputer,          Qt::Key_Launch0,
    XF86XK_Calculator,          Qt::Key_Launch1,
    XF86XK_Standby,             Qt::Key_Standby,

    XF86XK_Launch0,             Qt::Key_Launch2,
    XF86XK_Launch1,             Qt::Key_Launch3,
    XF86XK_Launch2,             Qt::Key_Launch4,
    XF86XK_Launch3,             Qt::Key_Launch5,
    XF86XK_Launch4,             Qt::Key_Launch6,
    XF86XK_Launch5,             Qt::Key_Launch7,
    XF86XK_Launch6,             Qt::Key_Launch8,
    XF86XK_Launch7,             Qt::Key_Launch9,
    XF86XK_Launch8,             Qt::Key_LaunchA,
    XF86XK_Launch9,             Qt::Key_LaunchB,
    XF86XK_LaunchA,             Qt::Key_LaunchC,
    XF86XK_LaunchB,             Qt::Key_LaunchD,
    XF86XK_LaunchC,             Qt::Key_LaunchE,
    XF86XK_LaunchD,             Qt::Key_LaunchF,

    0,                          0
};

void QimsysKeyManager::processKey(QString text, int keycode, int modifiers, bool isPress, bool autoRepeat)
{
    static QimsysApplicationManager *manager = 0;
    if (!manager) {
        manager = new QimsysApplicationManager(this);
        manager->init();
    }

    qimsysDebugIn() << text << QString("%1").arg(keycode, 0, 16) << modifiers << isPress << autoRepeat;
    Display *display = QX11Info::display();

    KeySym keysym = 0;
    for (int i = 0; KeyTbl[i]; i += 2) {
        if (KeyTbl[i+1] == (unsigned int)keycode) {
            keysym = KeyTbl[i];
            break;
        }
    }
    qimsysDebug() << keysym;
    if (keysym == 0) {
        keysym = keycode;
    }
    qimsysDebug() << keysym;

    XKeyEvent ev;
    ev.display = display;
    ev.root = QX11Info::appRootWindow();
    ev.window = (Window)manager->widget();
    ev.subwindow = None;
    ev.same_screen = True;
    ev.time = CurrentTime;
    ev.x = 1;
    ev.y = 1;
    ev.x_root = 1;
    ev.y_root = 1;

    ev.keycode = XKeysymToKeycode(display, keysym);
    qimsysDebug() << ev.keycode;
    ev.state = 0;
    if (modifiers & Qt::ShiftModifier) ev.state |= ShiftMask;
    if (modifiers & Qt::ControlModifier) ev.state |= ControlMask;
    if (modifiers & Qt::AltModifier) ev.state |= Mod1Mask;
    ev.type = (isPress ? KeyPress : KeyRelease);

    Status status = XSendEvent(display, ev.window, True, isPress ? KeyPressMask : KeyReleaseMask, reinterpret_cast<XEvent*>(&ev));
    qimsysDebug() << "XSendEvent =" << status;
    int ret = XFlush(display);
    qimsysDebug() << "XFlush =" << ret;
    qimsysDebugOut();
}
