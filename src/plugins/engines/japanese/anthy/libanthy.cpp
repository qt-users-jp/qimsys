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

#include "libanthy.h"

#include <qimsysdebug.h>

#include <QLibrary>

struct LibAnthy::Private {
public:
    Private(LibAnthy *parent);
    ~Private();

private:
    LibAnthy *q;

public:
    QLibrary libanthy;
};

LibAnthy::Private::Private(LibAnthy *parent)
    : q(parent)
    , libanthy("libanthy", 0)
{
    qimsysDebugIn() << parent;
//    libanthy.setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint | QLibrary::LoadArchiveMemberHint);
    if (!libanthy.load()) {
        qimsysWarning() << libanthy.errorString();
        libanthy.unload();
    } else {
        qimsysDebug() << "libanthy loaded.";
#define RESOLVE( func, ... ) \
        q->func = (__VA_ARGS__)libanthy.resolve( #func );
        RESOLVE(anthy_init, int(*)(void));
        RESOLVE(anthy_quit, void(*)(void));
        RESOLVE(anthy_create_context, anthy_context_t(*)(void));
        RESOLVE(anthy_reset_context, void (*)(anthy_context_t));
        RESOLVE(anthy_release_context, void (*)(anthy_context_t));
        RESOLVE(anthy_set_string, int (*)(anthy_context_t, const char*));
        RESOLVE(anthy_resize_segment, void (*)(anthy_context_t, int, int));
        RESOLVE(anthy_get_stat, int (*)(anthy_context_t, struct anthy_conv_stat*));
        RESOLVE(anthy_get_segment_stat, int (*)(anthy_context_t, int, struct anthy_segment_stat*));
        RESOLVE(anthy_get_segment, int (*)(anthy_context_t, int, int, char*, int));
        RESOLVE(anthy_commit_segment, int (*)(anthy_context_t, int, int));
        RESOLVE(anthy_set_prediction_string, int (*)(anthy_context_t, const char*));
        RESOLVE(anthy_get_prediction_stat, int (*)(anthy_context_t, struct anthy_prediction_stat*));
        RESOLVE(anthy_get_prediction, int (*)(anthy_context_t, int, char*, int));
        RESOLVE(anthy_commit_prediction, int (*)(anthy_context_t, int));
#undef RESOLVE
    }
    qimsysDebugOut();
}

LibAnthy::Private::~Private()
{
    qimsysDebugIn();
    libanthy.unload();
    qimsysDebugOut();
}

LibAnthy::LibAnthy()
{
    d = new Private(this);
}

LibAnthy::~LibAnthy()
{
    delete d;
}

bool LibAnthy::isAvailable() const
{
    return d->libanthy.isLoaded();
}
