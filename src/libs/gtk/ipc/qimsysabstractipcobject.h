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

#ifndef QIMSYSABSTRACTIPCOBJECT_H
#define QIMSYSABSTRACTIPCOBJECT_H

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib.h>

#include "qimsysglobal.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSABSTRACTIPCOBJECT_TYPE            (qimsys_abstract_ipc_object_get_type())
#define QIMSYSABSTRACTIPCOBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSABSTRACTIPCOBJECT_TYPE, QimsysAbstractIpcObject))
#define QIMSYSABSTRACTIPCOBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSABSTRACTIPCOBJECT_TYPE, QimsysAbstractIpcObjectClass))
#define IS_QIMSYSABSTRACTIPCOBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSABSTRACTIPCOBJECT_TYPE))
#define IS_QIMSYSABSTRACTIPCOBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSABSTRACTIPCOBJECT_TYPE))
#define QIMSYSABSTRACTIPCOBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSABSTRACTIPCOBJECT, QimsysAbstractIpcObjectClass))

typedef struct _QimsysAbstractIpcObject       QimsysAbstractIpcObject;
typedef struct _QimsysAbstractIpcObjectClass  QimsysAbstractIpcObjectClass;

struct _QimsysAbstractIpcObject
{
    GObject parent_instance;

    DBusGProxy *proxy;
    gboolean is_connected;
};

struct _QimsysAbstractIpcObjectClass
{
    GObjectClass parent_class;

    void (* qimsys_abstract_ipc_object) (QimsysAbstractIpcObject *qaio);
};

QIMSYS_EXPORT GType qimsys_abstract_ipc_object_get_type();

void qimsys_abstract_ipc_object_connect(QimsysAbstractIpcObject *qaio, const char *name, const char *path, const char *interface);
gboolean qimsys_abstract_ipc_object_is_connected(QimsysAbstractIpcObject *qaio);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif//QIMSYSABSTRACTIPCOBJECT_H
