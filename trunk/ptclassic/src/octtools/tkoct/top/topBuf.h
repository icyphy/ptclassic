#if !defined(_TOPBUF_H_)
#define _TOPBUF_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

extern void		topBufInit();
extern void		topBufUninit();
extern char*		topBufGet();	/* get an unlocked buf */
extern char*		topBufLGet();	/* get a locked buf */
extern void		topBufUnlock ARGS((char* buf));

extern char*		_topBufStrcpy ARGS((char *buf, int buflen, char *str));
extern char*		topBufStrcpy ARGS((char *str));
extern char*		topBufLStrcpy ARGS((char *str));

extern char*		_topBufStrcat ARGS((char *buf, int buflen, char *str));
extern char*		topBufStrcat ARGS((char *buf, char *str));

extern char*		_topBufFmtVa ARGS_VA((char *buf, int buflen, 
			  char *fmt, va_list args));
extern char*		_topBufFmt ARGS(( char *buf, int buflen,
			  char *fmt, ...));
extern char*		topBufFmtVa ARGS_VA((char *fmt, va_list args));
extern char*		topBufFmt ARGS(( char *fmt, ...));
extern char*		topBufLFmtVa ARGS_VA((char *fmt, va_list args));
extern char*		topBufAppendFmt ARGS(( char *buf,
			  char *fmt, ...));
extern char*		topBufAppendFmtVa ARGS_VA((char *buf,
			  char *fmt, va_list args));
extern char*		topBufLFmt ARGS(( char *fmt, ...));
#endif /* _TOPBUF_H_ */
