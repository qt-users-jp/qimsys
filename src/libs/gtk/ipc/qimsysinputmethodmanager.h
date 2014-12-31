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

#ifndef QIMSYSINPUTMETHODMANAGER_H
#define QIMSYSINPUTMETHODMANAGER_H

#include "qimsysabstractipcobject.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSINPUTMETHODMANAGER_TYPE            (qimsys_inputmethod_manager_get_type())
#define QIMSYSINPUTMETHODMANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSINPUTMETHODMANAGER_TYPE, QimsysInputMethodManager))
#define QIMSYSINPUTMETHODMANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSINPUTMETHODMANAGER_TYPE, QimsysInputMethodManagerClass))
#define IS_QIMSYSINPUTMETHODMANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSINPUTMETHODMANAGER_TYPE))
#define IS_QIMSYSINPUTMETHODMANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSINPUTMETHODMANAGER_TYPE))
#define QIMSYSINPUTMETHODMANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSINPUTMETHODMANAGER, QimsysInputMethodManagerClass))

typedef struct _QimsysInputMethodManager       QimsysInputMethodManager;
typedef struct _QimsysInputMethodManagerClass  QimsysInputMethodManagerClass;

struct _QimsysInputMethodManager
{
    QimsysAbstractIpcObject instance_class;
};

struct _QimsysInputMethodManagerClass
{
    QimsysAbstractIpcObjectClass parent_class;

    void (* qimsys_inputmethod_manager) (QimsysInputMethodManager *qimm);
};

QIMSYS_EXPORT GType qimsys_inputmethod_manager_get_type();
QIMSYS_EXPORT QimsysInputMethodManager *qimsys_inputmethod_manager_new();

QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_get_locale(QimsysInputMethodManager *qimm, char **value);
QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_set_locale(QimsysInputMethodManager *qimm, char *value);
// signal: "locale-changed"

QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_get_identifier(QimsysInputMethodManager *qimm, char **value);
QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_set_identifier(QimsysInputMethodManager *qimm, char *value);
// signal: "identifier-changed"

QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_get_converter(QimsysInputMethodManager *qimm, char **value);
QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_set_converter(QimsysInputMethodManager *qimm, char *value);
// signal: "converter-changed"

QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_get_interpreter(QimsysInputMethodManager *qimm, char **value);
QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_set_interpreter(QimsysInputMethodManager *qimm, char *value);
// signal: "interpreter-changed"

QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_get_engine(QimsysInputMethodManager *qimm, char **value);
QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_set_engine(QimsysInputMethodManager *qimm, char *value);
// signal: "engine-changed"

QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_get_state(QimsysInputMethodManager *qimm, guint *value);
QIMSYS_EXPORT gboolean qimsys_inputmethod_manager_set_state(QimsysInputMethodManager *qimm, guint value);
// signal: "state-changed"

gboolean qimsys_inputmethod_manager_execute(QimsysInputMethodManager *qimm, char *value);
// signal: executed

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // QIMSYSINPUTMETHODMANAGER_H
