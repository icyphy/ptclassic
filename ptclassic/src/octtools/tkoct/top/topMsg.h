#if !defined(_TOPMSG_H_)
#define _TOPMSG_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
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
