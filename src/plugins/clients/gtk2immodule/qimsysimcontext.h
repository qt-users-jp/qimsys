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

#ifndef QIMSYSIMCONTEXT_H
#define QIMSYSIMCONTEXT_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define QIMSYS_IM_CONTEXT_TYPE            (qimsys_im_context_get_type())
#define QIMSYS_IM_CONTEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), QIMSYS_IM_CONTEXT_TYPE, QimsysIMContext))
#define QIMSYS_IM_CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), QIMSYS_TYPE_IM_CONTEXT, QimsysIMContextClass))
#define IS_QIMSYS_IM_CONTEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYS_IM_CONTEXT_TYPE))
#define IS_QIMSYS_IM_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYS_IM_CONTEXT_TYPE))
#define QIMSYS_IM_CONTEXT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYS_IM_CONTEXT, QimsysIMContext))

typedef struct _QimsysIMContext QimsysIMContext;
typedef struct _QimsysIMContextClass QimsysIMContextClass;
typedef struct _QimsysIMContextPrivate QimsysIMContextPrivate;

struct _QimsysIMContext {
    GtkIMContext parent_instance;

    QimsysIMContextPrivate *d;
};

struct _QimsysIMContextClass {
    GtkIMContextClass parent_class;

    GtkIMContext *current_context;
};

void qimsys_im_context_register_type(GTypeModule *module);
GType qimsys_im_context_get_type();

QimsysIMContext *qimsys_im_context_new();

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // QIMSYSIMCONTEXT_H
