#if !defined(_TOPMSG_H_)
#define _TOPMSG_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
/**
    topMsg.h
**/

#define TOP_MSG_OS	TOP_BIT(0)
#define TOP_MSG_DBG	TOP_BIT(1)
#define TOP_MSG_INFO	TOP_BIT(2)
#define TOP_MSG_WARN	TOP_BIT(3)
#define TOP_MSG_ERR	TOP_BIT(4)
#define TOP_MSG_DIE	TOP_BIT(5)
#define TOP_MSG_PROG	TOP_BIT(6)

extern char*	TOPAppName;		/* read-only */
extern int	TOPDebugMode;		/* read-only */

extern void	topSetAppName ARGS((char *name));
extern char*	topGetAppName();
extern void	topMsgSetDbgMode ARGS((int d));

/* msg.c */
typedef void (TOPMsgHandlerFunc) ARGS((TOPMask flags, char* msg));
extern TOPMsgHandlerFunc* topMsgSetHandler ARGS((TOPMsgHandlerFunc *pNewFunc));


extern void topMsg ARGS((TOPMask flags, char* msg));
extern void topMsgVa ARGS_VA((TOPMask flags, char* fmt, va_list args));
extern void topMsgFmt ARGS((TOPMask flags, char* fmt, ...));
extern void topDbgMsg ARGS((char* fmt, ...));
extern void topInfoMsg ARGS((char* fmt, ...));
extern void topWarnMsg ARGS((char* fmt, ...));
extern void topWarnSysMsg ARGS((char *fmt, ...));
extern void topErrMsg ARGS((char* fmt, ...));
extern void topProgMsg ARGS((char* fmt, ...));
extern void topNullMsg ARGS((char* fmt, ...));
extern void topErrSysMsg ARGS((char* fmt, ...));
extern void topAbortMsg ARGS((char *fmt, ...));
extern void topAssertMsg ARGS((char *pkg, char *file, int line));

#if __TOP_DEBUG__
#define TOP_DBG(x) { topDbgMsg x; }
#else
#define TOP_DBG(x) /* */
#endif

#define TOP_MSG_BUFSIZE 4000
#define TOP_MSG_BUFPADSIZE 200

/***
    Stuff from topMsgPkg.c
****/

extern void		topMsgPkgSet ARGS((char *pkgName, int flags));

extern void		topMsgPkgBuf ARGS((char *pkgName, char *msg));
extern void		topMsgPkgVa ARGS_VA((char *pkgName, char *fmt, va_list args));
extern void		topMsgPkgFmt ARGS((char *pkgName, char *fmt, ...));
extern TOPLogical	topMsgPkgDbgOnB ARGS((char *pkgName));


#ifdef __TOP_DEBUG__
#define TOP_PDBG(x) { topMsgPkgFmt x; }
#else
#define TOP_PDBG(x) /* */
#endif

#define TOP_ABORT(pkg) topAssertMsg(pkg, __FILE__, __LINE__)

#if __TOP_ASSERT__ || __TOP_DEBUG__
#define TOP_ASSERT(pkg,ex) \
   ( (ex) ? (void)0 : topAssertMsg(pkg, __FILE__, __LINE__) )
#else
#define TOP_ASSERT(pkg,ex) /* */
#endif

	

/*
 * topMsgTcl.c
 */
extern int		topMsgRegisterCmds ARGS_TCL((Tcl_Interp *ip));
#endif /* _TOPMSG_H_ */
