/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                    *
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

#ifndef _XTRANS_H_
#define _XTRANS_H_

#include <X11/Xfuncproto.h>
#include <X11/Xos.h>


/*
 * Set the functions names according to where this code is being compiled.
 */

#ifdef X11_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _X11Trans##func
#else
#define TRANS(func) _X11Trans/**/func
#endif
#endif /* X11_t */

#ifdef XSERV_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _XSERVTrans##func
#else
#define TRANS(func) _XSERVTrans/**/func
#endif
#define X11_t
#endif /* X11_t */

#ifdef XIM_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _XimdXTrans##func
#else
#define TRANS(func) _XimdXTrans/**/func
#endif
#endif /* XIM_t */

#ifdef FS_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _FSTrans##func
#else
#define TRANS(func) _FSTrans/**/func
#endif
#endif /* FS_t */

#ifdef FONT_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _FontTrans##func
#else
#define TRANS(func) _FontTrans/**/func
#endif
#endif /* FONT_t */

#ifdef ICE_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _IceTrans##func
#else
#define TRANS(func) _IceTrans/**/func
#endif
#endif /* ICE_t */

#ifdef TEST_t
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _TESTTrans##func
#else
#define TRANS(func) _TESTTrans/**/func
#endif
#endif /* TEST_t */

#if !defined(TRANS)
#if (__STDC__ && !defined(UNIXCPP)) || defined(ANSICPP)
#define TRANS(func) _XTrans##func
#else
#define TRANS(func) _XTrans/**/func
#endif
#endif /* !TRANS */


/*
 * Create a single address structure that can be used wherever
 * an address structure is needed. struct sockaddr is not big enough
 * to hold a sockadd_un, so we create this definition to have a single
 * structure that is big enough for all the structures we might need.
 *
 * This structure needs to be independent of the socket/TLI interface used.
 */

#define XTRANS_MAX_ADDR_LEN 128 /* large enough to hold sun_path */

typedef struct {
    unsigned char addr[XTRANS_MAX_ADDR_LEN];
} Xtransaddr;


#ifdef LONG64
typedef int BytesReadable_t;
#else
typedef long BytesReadable_t;
#endif


#if defined(WIN32) || (defined(USG) && !defined(CRAY) && !defined(umips) && !defined(MOTOROLA) && !defined(uniosu) && !defined(__sxg__))

/*
 *      TRANS(Readv) and TRANS(Writev) use struct iovec, normally found
 *      in Berkeley systems in <sys/uio.h>.  See the readv(2) and writev(2)
 *      manual pages for details.
 */

struct iovec {
    caddr_t iov_base;
    int iov_len;
};

#else
#include <sys/uio.h>
#endif

typedef struct _XtransConnInfo *XtransConnInfo;


/*
 * Transport Option definitions
 */

#define TRANS_NONBLOCKING 1
#define TRANS_CLOSEONEXEC 2


/*
 * Return values of Connect (0 is success)
 */

#define TRANS_CONNECT_FAILED  -1
#define TRANS_TRY_CONNECT_AGAIN -2


/*
 * Return values of Accept (0 is success)
 */

#define TRANS_ACCEPT_BAD_MALLOC   -1
#define TRANS_ACCEPT_FAILED    -2
#define TRANS_ACCEPT_MISC_ERROR   -3


/*
 * ResetListener return values
 */

#define TRANS_RESET_NOOP 1
#define TRANS_RESET_NEW_FD 2
#define TRANS_RESET_FAILURE 3


/*
 * Function prototypes for the exposed interface
 */

#ifdef TRANS_CLIENT

XtransConnInfo TRANS(OpenCOTSClient)(
#if NeedFunctionPrototypes
    char *  /* address */
#endif
);

#endif /* TRANS_CLIENT */

#ifdef TRANS_SERVER

XtransConnInfo TRANS(OpenCOTSServer)(
#if NeedFunctionPrototypes
    char *  /* address */
#endif
);

#endif /* TRANS_SERVER */

#ifdef TRANS_CLIENT

XtransConnInfo TRANS(OpenCLTSClient)(
#if NeedFunctionPrototypes
    char *  /* address */
#endif
);

#endif /* TRANS_CLIENT */

#ifdef TRANS_SERVER

XtransConnInfo TRANS(OpenCLTSServer)(
#if NeedFunctionPrototypes
    char *  /* address */
#endif
);

#endif /* TRANS_SERVER */

#ifdef TRANS_REOPEN

XtransConnInfo TRANS(ReopenCOTSServer)(
#if NeedFunctionPrototypes
    int,  /* trans_id */
    int,  /* fd */
    char *  /* port */
#endif
);

XtransConnInfo TRANS(ReopenCLTSServer)(
#if NeedFunctionPrototypes
    int,  /* trans_id */
    int,  /* fd */
    char *  /* port */
#endif
);

int TRANS(GetReopenInfo)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    int *,  /* trans_id */
    int *,  /* fd */
    char **  /* port */
#endif
);

#endif /* TRANS_REOPEN */


int TRANS(SetOption)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    int,  /* option */
    int   /* arg */
#endif
);

#ifdef TRANS_SERVER

int TRANS(CreateListener)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    char *  /* port */
#endif
);

int TRANS(ResetListener)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

XtransConnInfo TRANS(Accept)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    int *  /* status */
#endif
);

#endif /* TRANS_SERVER */

#ifdef TRANS_CLIENT

int TRANS(Connect)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    char *  /* address */
#endif
);

#endif /* TRANS_CLIENT */

int TRANS(BytesReadable)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    BytesReadable_t * /* pend */
#endif
);

int TRANS(Read)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    char *,  /* buf */
    int   /* size */
#endif
);

int TRANS(Write)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    char *,  /* buf */
    int   /* size */
#endif
);

int TRANS(Readv)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    struct iovec *, /* buf */
    int   /* size */
#endif
);

int TRANS(Writev)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    struct iovec *, /* buf */
    int   /* size */
#endif
);

int TRANS(Disconnect)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

int TRANS(Close)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

int TRANS(CloseForCloning)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

int TRANS(IsLocal)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

int TRANS(GetMyAddr)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    int *,  /* familyp */
    int *,  /* addrlenp */
    Xtransaddr ** /* addrp */
#endif
);

int TRANS(GetPeerAddr)(
#if NeedFunctionPrototypes
    XtransConnInfo, /* ciptr */
    int *,  /* familyp */
    int *,  /* addrlenp */
    Xtransaddr ** /* addrp */
#endif
);

int TRANS(GetConnectionNumber)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

#ifdef TRANS_SERVER

int TRANS(MakeAllCOTSServerListeners)(
#if NeedFunctionPrototypes
    char *,  /* port */
    int *,  /* partial */
    int *,  /* count_ret */
    XtransConnInfo ** /* ciptrs_ret */
#endif
);

int TRANS(MakeAllCLTSServerListeners)(
#if NeedFunctionPrototypes
    char *,  /* port */
    int *,  /* partial */
    int *,  /* count_ret */
    XtransConnInfo ** /* ciptrs_ret */
#endif
);

#endif /* TRANS_SERVER */


/*
 * Function Prototypes for Utility Functions.
 */

#ifdef X11_t

int TRANS(ConvertAddress)(
#if NeedFunctionPrototypes
    int *,  /* familyp */
    int *,  /* addrlenp */
    Xtransaddr * /* addrp */
#endif
);

#endif /* X11_t */

#ifdef ICE_t

char *
TRANS(GetMyNetworkId)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

char *
TRANS(GetPeerNetworkId)(
#if NeedFunctionPrototypes
    XtransConnInfo /* ciptr */
#endif
);

#endif /* ICE_t */

#endif /* _XTRANS_H_ */
