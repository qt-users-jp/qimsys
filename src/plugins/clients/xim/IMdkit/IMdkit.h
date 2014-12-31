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

#ifndef _IMdkit_h
#define _IMdkit_h

#include <X11/Xmd.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* IM Attributes Name */
#define IMModifiers  "modifiers"
#define IMServerWindow  "serverWindow"
#define IMServerName  "serverName"
#define IMServerTransport "serverTransport"
#define IMLocale  "locale"
#define IMInputStyles  "inputStyles"
#define IMProtocolHandler "protocolHandler"
#define IMOnKeysList  "onKeysList"
#define IMOffKeysList  "offKeysList"
#define IMEncodingList  "encodingList"
#define IMFilterEventMask "filterEventMask"
#define IMProtocolDepend "protocolDepend"

    /* Masks for IM Attributes Name */
#define I18N_IMSERVER_WIN 0x0001 /* IMServerWindow */
#define I18N_IM_NAME  0x0002 /* IMServerName */
#define I18N_IM_LOCALE  0x0004 /* IMLocale */
#define I18N_IM_ADDRESS  0x0008 /* IMServerTransport */
#define I18N_INPUT_STYLES 0x0010 /* IMInputStyles */
#define I18N_ON_KEYS  0x0020 /* IMOnKeysList */
#define I18N_OFF_KEYS  0x0040 /* IMOffKeysList */
#define I18N_IM_HANDLER  0x0080 /* IMProtocolHander */
#define I18N_ENCODINGS  0x0100 /* IMEncodingList */
#define I18N_FILTERMASK  0x0200 /* IMFilterEventMask */
#define I18N_PROTO_DEPEND 0x0400 /* IMProtoDepend */

    typedef struct {
        char *name;
        XPointer value;
    } XIMArg;

    typedef struct {
        CARD32 keysym;
        CARD32 modifier;
        CARD32 modifier_mask;
    } XIMTriggerKey;

    typedef struct {
        unsigned short count_keys;
        XIMTriggerKey *keylist;
    } XIMTriggerKeys;

    typedef const char *XIMEncoding;

    typedef struct {
        unsigned short count_encodings;
        XIMEncoding *supported_encodings;
    } XIMEncodings;

    typedef struct _XIMS *XIMS;

    typedef struct {
        void* (*setup)(Display *, XIMArg *);
        Status(*openIM)(XIMS);
        Status(*closeIM)(XIMS);
        char*(*setIMValues)(XIMS, XIMArg *);
        char*(*getIMValues)(XIMS, XIMArg *);
        Status(*forwardEvent)(XIMS, XPointer);
        Status(*commitString)(XIMS, XPointer);
        int (*callCallback)(XIMS, XPointer);
        int (*preeditStart)(XIMS, XPointer);
        int (*preeditEnd)(XIMS, XPointer);
        int (*syncXlib)(XIMS, XPointer);
    } IMMethodsRec, *IMMethods;

    typedef struct {
        Display *display;
        int  screen;
    } IMCoreRec, *IMCore;

    typedef struct _XIMS {
        IMMethods methods;
        IMCoreRec core;
        Bool sync;
        void *protocol;
    } XIMProtocolRec;

    /*
     * X function declarations.
     */
    extern XIMS IMOpenIM(Display *, ...);
    extern Status IMCloseIM(XIMS);
    extern char *IMSetIMValues(XIMS, ...);
    extern char *IMGetIMValues(XIMS, ...);
    void IMForwardEvent(XIMS, XPointer);
    void IMCommitString(XIMS, XPointer);
    int IMCallCallback(XIMS, XPointer);
    int IMPreeditStart(XIMS, XPointer);
    int IMPreeditEnd(XIMS, XPointer);
    int IMSyncXlib(XIMS, XPointer);

#ifdef __cplusplus
}
#endif

#endif /* IMdkit_h */
