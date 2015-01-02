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

#include "libanthydic.h"

#include <qimsysdebug.h>

#include <QLibrary>

struct LibAnthyDic::Private {
public:
    Private(LibAnthyDic *parent);
    ~Private();

    void *handler;
private:
    LibAnthyDic *q;

public:
    QLibrary libanthy;
};

LibAnthyDic::Private::Private(LibAnthyDic *parent)
    : q(parent)
    , libanthy("libanthydic", 0)
{
    qimsysDebugIn() << parent;
    if (!libanthy.load()) {
        qimsysWarning() << libanthy.errorString();
        libanthy.unload();
    } else {
        qimsysDebug() << "libanthy loaded.";
#define RESOLV( func, ... ) \
        q->func = (__VA_ARGS__)libanthy.resolve( #func );
        RESOLV(anthy_dic_util_init, void (*)(void));
        RESOLV(anthy_dic_util_quit, void (*)(void));
        RESOLV(anthy_dic_util_set_personality, void (*)(const char*));
        RESOLV(anthy_dic_util_get_anthydir, const char*(*)(void));
        RESOLV(anthy_dic_util_set_encoding, int (*)(int));
        RESOLV(anthy_priv_dic_delete, void (*)(void));
        RESOLV(anthy_priv_dic_select_first_entry, int (*)(void));
        RESOLV(anthy_priv_dic_select_next_entry, int (*)(void));
        RESOLV(anthy_priv_dic_get_index, char*(*)(char *buf, int len));
        RESOLV(anthy_priv_dic_get_freq, int (*)(void));
        RESOLV(anthy_priv_dic_get_wtype, char*(*)(char *buf, int len));
        RESOLV(anthy_priv_dic_get_word, char*(*)(char *buf, int len));
        RESOLV(anthy_priv_dic_add_entry, int (*)(const char *yomi, const char *word, const char *wt, int freq));
#undef RESOLV
    }
    qimsysDebugOut();
}

LibAnthyDic::Private::~Private()
{
    qimsysDebugIn();
    libanthy.unload();
    qimsysDebugOut();
}

LibAnthyDic::LibAnthyDic()
{
    d = new Private(this);
}

LibAnthyDic::~LibAnthyDic()
{
    delete d;
}

bool LibAnthyDic::isAvailable() const
{
    return d->libanthy.isLoaded();
}
