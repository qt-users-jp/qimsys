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

#ifndef QIMSYSKEYMANAGER_H
#define QIMSYSKEYMANAGER_H

#include "qimsysabstractipcobject.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSKEYMANAGER_TYPE            (qimsys_key_manager_get_type())
#define QIMSYSKEYMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSKEYMANAGER_TYPE, QimsysKeyManager))
#define QIMSYSKEYMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSKEYMANAGER_TYPE, QimsysKeyManagerClass))
#define IS_QIMSYSKEYMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSKEYMANAGER_TYPE))
#define IS_QIMSYSKEYMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSKEYMANAGER_TYPE))
#define QIMSYSKEYMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSKEYMANAGER, QimsysKeyManagerClass))

typedef struct _QimsysKeyManager       QimsysKeyManager;
typedef struct _QimsysKeyManagerClass  QimsysKeyManagerClass;

struct _QimsysKeyManager
{
    QimsysAbstractIpcObject object;
};

struct _QimsysKeyManagerClass
{
    QimsysAbstractIpcObjectClass parent_class;

    void (* qimsys_key_manager) (QimsysKeyManager *qam);
};

QIMSYS_EXPORT GType qimsys_key_manager_get_type();
QIMSYS_EXPORT QimsysKeyManager *qimsys_key_manager_new();

QIMSYS_EXPORT gboolean qimsys_key_manager_process(QimsysKeyManager *qam, char *text, int keycode, int modifiers, gboolean isPress, gboolean autoRepeat);

QIMSYS_EXPORT gboolean qimsys_key_manager_key_press(QimsysKeyManager *qam, char *text, int keycode, int modifiers, gboolean autoRepeat, gboolean *is_accepted);
QIMSYS_EXPORT gboolean qimsys_key_manager_key_release(QimsysKeyManager *qam, char *text, int keycode, int modifiers, gboolean autoRepeat, gboolean *is_accepted);

QIMSYS_EXPORT gboolean qimsys_key_manager_get_accepted(QimsysKeyManager *qam, gboolean *value);
QIMSYS_EXPORT gboolean qimsys_key_manager_accept(QimsysKeyManager *qam);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // QIMSYSKEYMANAGER_H
