#if !defined(_XPAXIS_H_)
#define _XPAXIS_H_ 1
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
