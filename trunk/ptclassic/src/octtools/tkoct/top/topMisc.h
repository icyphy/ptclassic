#if !defined(_TOPMISC_H_)
#define _TOPMISC_H_ 1
/* 
    RCS: $Header$
    topMisc.h :: TOP Library
    Misc functions provided by the TOP library.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
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
