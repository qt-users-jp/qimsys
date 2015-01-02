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

#ifndef FRAMEMGR_H
#define FRAMEMGR_H

#include <X11/Xmd.h>
#include <X11/Xlib.h>
#include <stdio.h>

#if defined(VAXC) && !defined(__DECC)
#define xim_externalref globalref
#define xim_externaldef globaldef
#else
#define xim_externalref extern
#define xim_externaldef
#endif

/* Definitions for FrameMgr */

#define COUNTER_MASK 0x10

typedef enum {
    BIT8     = 0x1,       /* {CARD8* | INT8*}   */
    BIT16    = 0x2,       /* {CARD16* | INT16*} */
    BIT32    = 0x3,       /* {CARD32* | INT32*} */
    BIT64    = 0x4,   /* {CARD64* | INT64*} */
    BARRAY   = 0x5,       /* int*, void*        */
    ITER     = 0x6,       /* int*               */
    POINTER  = 0x7,       /* specifies next item is a PTR_ITEM */
    PTR_ITEM = 0x8,       /* specifies the item has a pointer */
    /* BOGUS - POINTER and PTR_ITEM
     *   In the current implementation, PTR_ITEM should be lead by
     *   POINTER.  But actually, it's just redundant logically.  Someone
     *   may remove this redundancy and POINTER from the enum member but he
     *   should also modify the logic in FrameMgr program.
     */
    PADDING  = 0x9,       /* specifies that a padding is needed.
             * This requires extra data in data field.
             */
    EOL      = 0xA,       /* specifies the end of list */

    COUNTER_BIT8  = COUNTER_MASK | 0x1,
    COUNTER_BIT16 = COUNTER_MASK | 0x2,
    COUNTER_BIT32 = COUNTER_MASK | 0x3,
    COUNTER_BIT64 = COUNTER_MASK | 0x4
} XimFrameType;

/* Convenient macro */
#define _FRAME(a) {a, NULL}
#define _PTR(p)   {PTR_ITEM, (void *)p}
/* PADDING's usage of data field
 * B15-B8  : Shows the number of effective items.
 * B7-B0   : Shows padding unit.  ex) 04 shows 4 unit padding.
 */
#define _PAD2(n)   {PADDING, (void*)((n)<<8|2)}
#define _PAD4(n)   {PADDING, (void*)((n)<<8|4)}

#define FmCounterByte 0
#define FmCounterNumber 1

#define _BYTE_COUNTER(type, offset) \
    {(COUNTER_MASK|type), (void*)((offset)<<8|FmCounterByte)}

#define _NUMBER_COUNTER(type, offset) \
    {(COUNTER_MASK|type), (void*)((offset)<<8|FmCounterNumber)}

typedef struct _XimFrame {
    XimFrameType type;
    void* data;       /* For PTR_ITEM and PADDING */
} XimFrameRec, *XimFrame;

typedef enum {
    FmSuccess,
    FmEOD,
    FmInvalidCall,
    FmBufExist,
    FmCannotCalc,
    FmNoMoreData
} FmStatus;

typedef struct _FrameMgr *FrameMgr;

FrameMgr FrameMgrInit(XimFrame frame, char* area, Bool byte_swap);
void FrameMgrInitWithData(FrameMgr fm, XimFrame frame, void* area,
                          Bool byte_swap);
void FrameMgrFree(FrameMgr fm);
FmStatus FrameMgrSetBuffer(FrameMgr, void*);
FmStatus _FrameMgrPutToken(FrameMgr, void*, int);
FmStatus _FrameMgrGetToken(FrameMgr, void*, int);
FmStatus FrameMgrSetSize(FrameMgr, int);
FmStatus FrameMgrSetIterCount(FrameMgr, int);
FmStatus FrameMgrSetTotalSize(FrameMgr, int);
int FrameMgrGetTotalSize(FrameMgr);
int FrameMgrGetSize(FrameMgr);
FmStatus FrameMgrSkipToken(FrameMgr, int);
void FrameMgrReset(FrameMgr);
Bool FrameMgrIsIterLoopEnd(FrameMgr, FmStatus*);

#define FrameMgrPutToken(fm, obj) _FrameMgrPutToken((fm), &(obj), sizeof(obj))
#define FrameMgrGetToken(fm, obj) _FrameMgrGetToken((fm), &(obj), sizeof(obj))

#endif /* FRAMEMGR_H */
