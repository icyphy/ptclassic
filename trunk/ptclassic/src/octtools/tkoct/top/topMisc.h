#if !defined(_TOPMISC_H_)
#define _TOPMISC_H_ 1
/* 
    RCS: $Header$
    topMisc.h :: TOP Library
    Misc functions provided by the TOP library.

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

/* 
 * topFmt.c
 */
extern TOPLogical	topFmtDbl ARGS((char *buf, char *fmt, double f));
extern int		topFmtDblCalcRange ARGS((char *fmt,
			  double lo, double hi,
			  int sameDigs, char *realFmt));
extern TOPLogical	topFmtVerifyB ARGS((char *fmt));

extern TOPLogical	topPrsDbl ARGS((char *str, double *pDbl));
extern int		topPrsDblTcl ARGS_TCL((Tcl_Interp *ip, 
			  char *str, double *pDbl));
extern int		topFmtGetType( char *fmt);	/* rets char */
extern int		topFmtGetScale( char *fmt, double *pScale);	/* rets char */
extern TOPLogical	topFmtChangeScale( char *newFmt, char *fmt,char newScl);

extern TOPLogical	topFmtChangeType( char *newFmt, char *fmt,char newType);



extern void		topFmtRegisterCmds ARGS_TCL((Tcl_Interp *ip));

/*
 * topMisc.c
 */
extern TOPLogical	TOPXServerFixB;

extern char*		topStrError ARGS((int error));
extern char*		topBasename ARGS((char *path));
extern void		topSetEnv ARGS((char *name, char *value));
extern char*		topFindSubPath ARGS((char *path, char *subpath));

#endif /* _TOPMISC_H_ */
