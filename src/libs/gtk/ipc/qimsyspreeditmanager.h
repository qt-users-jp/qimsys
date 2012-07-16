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

#ifndef QIMSYSPREEDITMANAGER_H
#define QIMSYSPREEDITMANAGER_H

#include "qimsysabstractipcobject.h"
#include "qimsyspreedititem.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSPREEDITMANAGER_TYPE            (qimsys_preedit_manager_get_type())
#define QIMSYSPREEDITMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSPREEDITMANAGER_TYPE, QimsysPreeditManager))
#define QIMSYSPREEDITMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSPREEDITMANAGER_TYPE, QimsysPreeditManagerClass))
#define IS_QIMSYSPREEDITMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSPREEDITMANAGER_TYPE))
#define IS_QIMSYSPREEDITMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSPREEDITMANAGER_TYPE))
#define QIMSYSPREEDITMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSPREEDITMANAGER, QimsysPreeditManagerClass))

typedef struct _QimsysPreeditManager       QimsysPreeditManager;
typedef struct _QimsysPreeditManagerClass  QimsysPreeditManagerClass;

struct _QimsysPreeditManager
{
    QimsysAbstractIpcObject object;
};

struct _QimsysPreeditManagerClass
{
    QimsysAbstractIpcObjectClass parent_class;

    void (* qimsys_preedit_manager) (QimsysPreeditManager *qam);
};

QIMSYS_EXPORT GType qimsys_preedit_manager_get_type();
QIMSYS_EXPORT QimsysPreeditManager *qimsys_preedit_manager_new();

QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_item(QimsysPreeditManager *qam, QimsysPreeditItem **value);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_item(QimsysPreeditManager *qam, QimsysPreeditItem *value);
// signal: "item-changed"

QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_rect(QimsysPreeditManager *qam, int *x, int *y, int *w, int *h);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_rect(QimsysPreeditManager *qam, int x, int y, int w, int h);
// signal: "rect-changed"

#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_font(QimsysPreeditManager *qam, char **value);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_font(QimsysPreeditManager *qam, char *value);
// signal: "font-changed"
#endif

QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_cursor_position(QimsysPreeditManager *qam, int *cursor_position);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_cursor_position(QimsysPreeditManager *qam, int cursor_position);
// signal: "cursor-position-changed"

QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_surrounding_text(QimsysPreeditManager *qam, char **surrounding_text);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_surrounding_text(QimsysPreeditManager *qam, const char *surrounding_text);
// signal: "surrounding-text-changed"

QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_current_selection(QimsysPreeditManager *qam, char **current_selection);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_current_selection(QimsysPreeditManager *qam, const char *current_selection);
// signal: "current-selection-changed"

QIMSYS_EXPORT gboolean qimsys_preedit_manager_get_maximum_text_length(QimsysPreeditManager *qam, int *maximum_text_length);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_set_maximum_text_length(QimsysPreeditManager *qam, int maximum_text_length);
// signal: "maximum-text-length-changed"

QIMSYS_EXPORT gboolean qimsys_preedit_manager_commit(QimsysPreeditManager *qam, gulonglong target);
QIMSYS_EXPORT gboolean qimsys_preedit_manager_commit_string(QimsysPreeditManager *qam, char *string, gulonglong target);
// signal: "committed"
G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // QIMSYSPREEDITMANAGER_H
