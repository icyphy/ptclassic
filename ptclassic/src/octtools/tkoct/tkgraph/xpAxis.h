#if !defined(_XPAXIS_H_)
#define _XPAXIS_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

typedef struct _XPAxisWdg XPAxisWdg, *XPAxisHandle;

/* zoom flags for xpAxisInformRange() */
#define XPAxisZM_In	TOP_BIT(0)
#define XPAxisZM_Out	TOP_BIT(1)

extern int		xpAxisCmd ARGS_TCL((ClientData cld, Tcl_Interp *ip, 
			  int aC, char **aV));
extern int		xpAxisRegisterCmds ARGS_TCL((Tcl_Interp *ip, 
			  Tk_Window w));
extern int		xpAxisGetHandle ARGS_TCL((Tcl_Interp *ip, char *name,
			  XPAxisWdg **ppAxis));
extern XPXfa*		xpAxisGetXfa ARGS((XPAxisWdg *pAxis));
extern TOPLogical	xpAxisGetDomain ARGS((XPAxisWdg *pAxis,
			  double *pwLo, double *pwHi));
extern void		xpAxisInformRange ARGS((XPAxisWdg *pAxis, 
			  double wLo, double wHi, TOPMask zoomFlags));
extern void		xpAxisZoomFull ARGS((XPAxisWdg *pAxis));

extern TOPLogical	xpAxisXformPendingB ARGS((XPAxisWdg *pAxis));
void			xpAxisExposeDrawWin ARGS((XPAxisWdg *pAxis));

#endif /* _XPAXIS_H_ */
