#if !defined(_TOPBUF_H_)
#define _TOPBUF_H_ 1
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
