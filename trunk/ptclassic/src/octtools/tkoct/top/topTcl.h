#if !defined(_TOPTCL_H_)
#define _TOPTCL_H_ 1
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

/*
 * tclRet.c
 */
extern void	topTclRet( Tcl_Interp *ip, char *msg);
extern void	topTclRetVa ARGS_VA((Tcl_Interp *ip, char *fmt, va_list args));
extern void	topTclRetFmt( Tcl_Interp *ip, char *fmt, ...);
extern void	topTclRetApnd( Tcl_Interp *ip, char *msg);
extern void	topTclRetApndVa ARGS_VA((Tcl_Interp *ip, char *fmt, va_list args));
extern void	topTclRetApndFmt( Tcl_Interp *ip, char *fmt, ...);

/*
 * tclEval.c
 */
extern void	topTclCaptureMsgsBegin( Tcl_Interp *ip);
extern int	topTclCaptureMsgsEnd( Tcl_Interp *ip);

extern char*	topTclGetRetMsg ARGS((Tcl_Interp *ip, int r));
extern void	topTclBgError ARGS((Tcl_Interp *ip, char *errmsg));
extern int	topTclBgEval ARGS((Tcl_Interp *ip, char *pkg, char *cmd));
extern int	topTclBgEvalVa ARGS_VA((Tcl_Interp *ip, char *pkg,
		  char *fmt, va_list args));
extern int	topTclBgEvalFmt ARGS((Tcl_Interp *ip, char *pkg, 
		  char *fmt, ...));

extern int	topTclEval ARGS((Tcl_Interp *ip, char *pkg, char *cmd));
extern int	topTclEvalVa ARGS_VA((Tcl_Interp *ip, char *pkg, 
		  char *fmt, va_list args));
extern int	topTclEvalFmt ARGS((Tcl_Interp *ip, char *pkg, char *fmt, ...));

extern int	topTclMsgEval ARGS((Tcl_Interp *ip, char *source, char *cmd));
extern int	topTclMsgEvalFmt ARGS((Tcl_Interp *ip, char *source, 
		  char *fmt, ...));

/* 
 * tclMain.c
 */
extern void	topTclInitVars ARGS((Tcl_Interp *ip, char *fileName,
		  int argc, char **argv));
extern int	topTclSourceInitFile ARGS((Tcl_Interp *ip,
		  char *appname, char *defpath));
extern void	topTclPrompt0 ARGS((Tcl_Interp *ip));
extern void	topTclPrompt ARGS((Tcl_Interp *ip, TOPLogical partialB,
		  char *defPrompt));
extern int	topTclMainLoop ARGS((Tcl_Interp *ip, 
		  char *prompt, char *appname, TOPLogical initB));


/* 
 * tclDh.c
 */
extern void	xpRegisterDataHandler ARGS((Tcl_Interp *ip, char *name, 
		  char *class, TOPPtr pDH));
extern TOPPtr	xpGetDataHandler ARGS((Tcl_Interp *ip, char *name, 
		  char *class, TOPPtr defVal));

/*
 * tclMisc.c
 */
extern int	topTclGetInt2( Tcl_Interp *ip, char *xStr, char *yStr,
		  int *pX, int *pY);
extern int	topTclPrsInt( Tcl_Interp *ip, char **pStr, int *pX);
extern int	topTclPrsInt2( Tcl_Interp *ip, char **pStr, int *pX, int *pY);
extern int	topTclPrsInt4( Tcl_Interp *ip, char **pStr,
  		  int *pX1,int *pY1, int *pX2,int *pY2);

typedef char* TOPTclUid;
extern TOPTclUid	topTclGetUid(char *string);

/*
 * tkMain.c
 */
extern int	 topTkInitialize1 ARGS_TK((Tcl_Interp *ip,
		  int *argc, char **argv, Tk_Window *pWin));
extern int	topTkInitialize2 ARGS_TK((Tcl_Interp *ip, Tk_Window win,
		  int argc, char **argv));

/*
 * tkMisc.c
 */
extern int	topTkMapCmd ARGS_TK((ClientData cld, Tcl_Interp *ip, 
		  int aC, char **aV));
extern int	topTkUnmapCmd ARGS_TK((ClientData cld, Tcl_Interp *ip,
		  int aC, char **aV));
extern int	topTkMiscRegisterCmds ARGS_TK((Tcl_Interp *ip,
		  Tk_Window appwin));

#endif /* _TOPTCL_H_ */
