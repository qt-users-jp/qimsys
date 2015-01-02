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

#ifndef QIMSYSAPPLICATIONMANAGER_H
#define QIMSYSAPPLICATIONMANAGER_H

#include "qimsysabstractipcobject.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSAPPLICATIONMANAGER_TYPE            (qimsys_application_manager_get_type())
#define QIMSYSAPPLICATIONMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSAPPLICATIONMANAGER_TYPE, QimsysApplicationManager))
#define QIMSYSAPPLICATIONMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSAPPLICATIONMANAGER_TYPE, QimsysApplicationManagerClass))
#define IS_QIMSYSAPPLICATIONMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSAPPLICATIONMANAGER_TYPE))
#define IS_QIMSYSAPPLICATIONMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSAPPLICATIONMANAGER_TYPE))
#define QIMSYSAPPLICATIONMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSAPPLICATIONMANAGER, QimsysApplicationManagerClass))

typedef struct _QimsysApplicationManager       QimsysApplicationManager;
typedef struct _QimsysApplicationManagerClass  QimsysApplicationManagerClass;

enum QimsysApplicationManager_ActionType {
    QimsysApplicationManager_Reset,
    QimsysApplicationManager_ShowDictionary,
    QimsysApplicationManager_ShowSettings,
    QimsysApplicationManager_ShowAboutQimsys,
    QimsysApplicationManager_ShowController,
};

struct _QimsysApplicationManager
{
    QimsysAbstractIpcObject instance_class;
};

struct _QimsysApplicationManagerClass
{
    QimsysAbstractIpcObjectClass parent_class;

    void (* qimsys_application_manager) (QimsysApplicationManager *qam);
};

QIMSYS_EXPORT GType qimsys_application_manager_get_type();
QIMSYS_EXPORT QimsysApplicationManager *qimsys_application_manager_new();

QIMSYS_EXPORT gboolean qimsys_application_manager_get_display_language(QimsysApplicationManager *qam, char **value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_display_language(QimsysApplicationManager *qam, char *value);
// signal: "display-language-changed"

QIMSYS_EXPORT gboolean qimsys_application_manager_get_focus(QimsysApplicationManager *qam, gboolean *value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_focus(QimsysApplicationManager *qam, gboolean value);
// signal: "focus-changed"

QIMSYS_EXPORT gboolean qimsys_application_manager_get_window(QimsysApplicationManager *qam, gulonglong *value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_window(QimsysApplicationManager *qam, gulonglong value);
// signal: "window-changed"

QIMSYS_EXPORT gboolean qimsys_application_manager_get_widget(QimsysApplicationManager *qam, gulonglong *value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_widget(QimsysApplicationManager *qam, gulonglong value);
// signal: "widget-changed"

QIMSYS_EXPORT gboolean qimsys_application_manager_get_composing(QimsysApplicationManager *qam, gboolean *value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_composing(QimsysApplicationManager *qam, gboolean value);
// signal: "composing-changed"

QIMSYS_EXPORT gboolean qimsys_application_manager_get_current_icon(QimsysApplicationManager *qam, GdkPixbuf **value);
QIMSYS_EXPORT gboolean qimsys_application_manager_set_current_icon(QimsysApplicationManager *qam, GdkPixbuf *value);
// signal: "current-icon-changed"

gboolean qimsys_application_manager_exec(QimsysApplicationManager *qam, int value);
// signal: triggered

gboolean qimsys_application_manager_settings_update(QimsysApplicationManager *qam, char *value);
// settings_updated

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // QIMSYSAPPLICATIONMANAGER_H
