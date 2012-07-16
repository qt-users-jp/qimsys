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

#include "qimsysinputmethodmanager.h"
#include "dbus.h"
#include "qimsysdebug.h"
#include "qimsysmarshalers.h"

enum {
    QIMSYSINPUTMETHODMANAGER_LOCALE_CHANGED,
    QIMSYSINPUTMETHODMANAGER_IDENTIFIER_CHANGED,
    QIMSYSINPUTMETHODMANAGER_CONVERTER_CHANGED,
    QIMSYSINPUTMETHODMANAGER_INTERPRETER_CHANGED,
    QIMSYSINPUTMETHODMANAGER_ENGINE_CHANGED,
    QIMSYSINPUTMETHODMANAGER_STATE_CHANGED,
    QIMSYSINPUTMETHODMANAGER_EXECUTED,
    QIMSYSINPUTMETHODMANAGER_LAST_SIGNAL
};

static guint qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_LAST_SIGNAL] = { 0, 0 };

G_DEFINE_TYPE(QimsysInputMethodManager, qimsys_inputmethod_manager, QIMSYSABSTRACTIPCOBJECT_TYPE)

static void qimsys_inputmethod_manager_locale_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void qimsys_inputmethod_manager_identifier_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void qimsys_inputmethod_manager_converter_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void qimsys_inputmethod_manager_interpreter_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void qimsys_inputmethod_manager_engine_changed(DBusGProxy *proxy, char *value, gpointer user_data);
static void qimsys_inputmethod_manager_state_changed(DBusGProxy *proxy, guint value, gpointer user_data);
static void qimsys_inputmethod_manager_executed(DBusGProxy *proxy, char *value, gpointer user_data);

static void qimsys_inputmethod_manager_class_init(QimsysInputMethodManagerClass *klass)
{
    qimsys_debug_in();
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_LOCALE_CHANGED]
            = g_signal_new("locale-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_IDENTIFIER_CHANGED]
            = g_signal_new("identifier-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_CONVERTER_CHANGED]
            = g_signal_new("converter-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_INTERPRETER_CHANGED]
            = g_signal_new("interpreter-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_ENGINE_CHANGED]
            = g_signal_new("engine-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_STATE_CHANGED]
            = g_signal_new("state-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__UINT,
                           G_TYPE_NONE, 1, G_TYPE_UINT);
    qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_EXECUTED]
            = g_signal_new("executed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysInputMethodManagerClass, qimsys_inputmethod_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
    qimsys_debug_out();
}

static void qimsys_inputmethod_manager_init(QimsysInputMethodManager *qimm)
{
    QimsysAbstractIpcObject *qaio;
    qimsys_debug_in();

    qaio = QIMSYSABSTRACTIPCOBJECT(qimm);

    /* Create a proxy object */
    qimsys_abstract_ipc_object_connect(qaio, QIMSYS_DBUS_SERVICE, "/QimsysInputMethodManager", "local.InputMethodManager");

    dbus_g_proxy_add_signal(qaio->proxy, "localeChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "localeChanged", G_CALLBACK(qimsys_inputmethod_manager_locale_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "identifierChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "identifierChanged", G_CALLBACK(qimsys_inputmethod_manager_identifier_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "converterChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "converterChanged", G_CALLBACK(qimsys_inputmethod_manager_converter_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "interpreterChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "interpreterChanged", G_CALLBACK(qimsys_inputmethod_manager_interpreter_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "engineChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "engineChanged", G_CALLBACK(qimsys_inputmethod_manager_engine_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "stateChanged", G_TYPE_UINT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "stateChanged", G_CALLBACK(qimsys_inputmethod_manager_state_changed), qimm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "executed", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "executed", G_CALLBACK(qimsys_inputmethod_manager_executed), qimm, NULL);

    qimsys_debug_out();
}

QimsysInputMethodManager *qimsys_inputmethod_manager_new()
{
    QimsysInputMethodManager *ret = QIMSYSINPUTMETHODMANAGER(g_object_new(QIMSYSINPUTMETHODMANAGER_TYPE, NULL));
    return ret;
}

gboolean qimsys_inputmethod_manager_get_locale(QimsysInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "locale", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean qimsys_inputmethod_manager_set_locale(QimsysInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "setLocale", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_locale_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_LOCALE_CHANGED], 0, value);
}

gboolean qimsys_inputmethod_manager_get_identifier(QimsysInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "identifier", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean qimsys_inputmethod_manager_set_identifier(QimsysInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "setIdentifier", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_identifier_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_IDENTIFIER_CHANGED], 0, value);
}

gboolean qimsys_inputmethod_manager_get_converter(QimsysInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "converter", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean qimsys_inputmethod_manager_set_converter(QimsysInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "setConverter", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_converter_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_CONVERTER_CHANGED], 0, value);
}

gboolean qimsys_inputmethod_manager_get_interpreter(QimsysInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "interpreter", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean qimsys_inputmethod_manager_set_interpreter(QimsysInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "setInterpreter", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_interpreter_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_INTERPRETER_CHANGED], 0, value);
}

gboolean qimsys_inputmethod_manager_get_engine(QimsysInputMethodManager *qimm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "engine", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean qimsys_inputmethod_manager_set_engine(QimsysInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "setEngine", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_engine_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_ENGINE_CHANGED], 0, value);
}

gboolean qimsys_inputmethod_manager_get_state(QimsysInputMethodManager *qimm, guint *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "state", &error, G_TYPE_INVALID, G_TYPE_STRING, value, G_TYPE_INVALID))
    return ret;
}

gboolean qimsys_inputmethod_manager_set_state(QimsysInputMethodManager *qimm, guint value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "setState", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_state_changed(DBusGProxy *proxy, guint value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_STATE_CHANGED], 0, value);
}

gboolean qimsys_inputmethod_manager_execte(QimsysInputMethodManager *qimm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qimm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qimm)->proxy, "execute", &error, G_TYPE_STRING, value, G_TYPE_INVALID, G_TYPE_INVALID))
    return ret;
}

void qimsys_inputmethod_manager_executed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSINPUTMETHODMANAGER(user_data), qimsys_inputmethod_manager_signals[QIMSYSINPUTMETHODMANAGER_EXECUTED], 0, value);
}
