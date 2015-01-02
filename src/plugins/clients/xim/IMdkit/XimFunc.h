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

#ifndef _XimFunc_h
#define _XimFunc_h

/* i18nAttr.c */
void _Xi18nInitAttrList(Xi18n i18n_core);
void _Xi18nInitExtension(Xi18n i18n_core);

/* i18nClbk.c */
int _Xi18nGeometryCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nPreeditStartCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nPreeditDrawCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nPreeditCaretCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nPreeditDoneCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nStatusStartCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nStatusDrawCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nStatusDoneCallback(XIMS ims, IMProtocol *call_data);
int _Xi18nStringConversionCallback(XIMS ims, IMProtocol *call_data);

/* i18nIc.c */
void _Xi18nChangeIC(XIMS ims, IMProtocol *call_data, unsigned char *p,
                    int create_flag);
void _Xi18nGetIC(XIMS ims, IMProtocol *call_data, unsigned char *p);

/* i18nUtil.c */
int _Xi18nNeedSwap(Xi18n i18n_core, CARD16 connect_id);
Xi18nClient *_Xi18nNewClient(Xi18n i18n_core);
Xi18nClient *_Xi18nFindClient(Xi18n i18n_core, CARD16 connect_id);
void _Xi18nDeleteClient(Xi18n i18n_core, CARD16 connect_id);
void _Xi18nSendMessage(XIMS ims, CARD16 connect_id, CARD8 major_opcode,
                       CARD8 minor_opcode, unsigned char *data, long length);
void _Xi18nSendTriggerKey(XIMS ims, CARD16 connect_id);
void _Xi18nSetEventMask(XIMS ims, CARD16 connect_id, CARD16 im_id,
                        CARD16 ic_id, CARD32 forward_mask, CARD32 sync_mask);

/* Xlib internal */
void _XRegisterFilterByType(Display*, Window, int, int,
                            Bool(*filter)(Display*, Window, XEvent*, XPointer), XPointer);
void _XUnregisterFilter(Display*, Window,
                        Bool(*filter)(Display*, Window, XEvent*, XPointer), XPointer);

#endif
