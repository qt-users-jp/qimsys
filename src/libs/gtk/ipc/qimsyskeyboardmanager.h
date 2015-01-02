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

#ifndef QIMSYSKEYBOARDMANAGER_H
#define QIMSYSKEYBOARDMANAGER_H

#include "qimsysabstractipcobject.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSKEYBOARDMANAGER_TYPE            (qimsys_keyboard_manager_get_type())
#define QIMSYSKEYBOARDMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSKEYBOARDMANAGER_TYPE, QimsysKeyboardManager))
#define QIMSYSKEYBOARDMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSKEYBOARDMANAGER_TYPE, QimsysKeyboardManagerClass))
#define IS_QIMSYSKEYBOARDMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSKEYBOARDMANAGER_TYPE))
#define IS_QIMSYSKEYBOARDMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSKEYBOARDMANAGER_TYPE))
#define QIMSYSKEYBOARDMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSKEYBOARDMANAGER, QimsysKeyboardManagerClass))

typedef struct _QimsysKeyboardManager       QimsysKeyboardManager;
typedef struct _QimsysKeyboardManagerClass  QimsysKeyboardManagerClass;

struct _QimsysKeyboardManager
{
    QimsysAbstractIpcObject object;
};

struct _QimsysKeyboardManagerClass
{
    QimsysAbstractIpcObjectClass parent_class;

    void (* qimsys_keyboard_manager) (QimsysKeyboardManager *qkm);
};

QIMSYS_EXPORT GType qimsys_keyboard_manager_get_type();
QIMSYS_EXPORT QimsysKeyboardManager *qimsys_keyboard_manager_new();

QIMSYS_EXPORT gboolean qimsys_application_manager_get_visible(QimsysKeyboardManager *qam, gboolean *value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_visible(QimsysKeyboardManager *qam, gboolean value);
// signal: "visible-changed"

QIMSYS_EXPORT gboolean qimsys_keyboard_manager_get_shared_data(QimsysKeyboardManager *qkm, char **h);
QIMSYS_EXPORT gboolean qimsys_keyboard_manager_set_shared_data(QimsysKeyboardManager *qkm, const char *h);
// signal: "shared-data-changed"

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // QIMSYSKEYBOARDMANAGER_H
