#if !defined(_XPXFA_H_)
#define _XPXFA_H_ 1
/* 
    RCS: $Header$
    xfa.h (Transform Axis) :: XPole

    Package Prefix: xpXfa

    This packages performs float->integer translation.
    The transform can be initialized to be either linear or logrithmic.

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

typedef int			XPXfaDstType;
typedef double			XPXfaSrcType;
typedef struct _XPXfaCalls	XPXfaCalls;
typedef struct _XPXfa		XPXfa;

typedef void		(XPXfaRecalcFunc) ARGS((XPXfa *xfa));
typedef TOPLogical	(XPXfaGetWinFunc) ARGS((XPXfa *xfa, 
			  XPXfaSrcType *lo, XPXfaSrcType *hi));
typedef TOPLogical	(XPXfaGetViewFunc) ARGS((XPXfa *xfa, 
			  XPXfaDstType *lo, XPXfaDstType *hi));
typedef XPXfaDstType	(XPXfaToViewFunc) ARGS((XPXfa *xfa, XPXfaSrcType val));
typedef XPXfaSrcType	(XPXfaToWinFunc) ARGS((XPXfa *xfa, XPXfaDstType val));

enum _XPXfaMode { 
  XPXfaMode_Null, XPXfaMode_Lin, XPXfaMode_Log, XPXfaMode_DSLog
};
typedef enum _XPXfaMode		XPXfaMode;


struct _XPXfaCalls {
    XPXfaMode		Mode;
    XPXfaRecalcFunc*	ReCalc;
    XPXfaGetWinFunc*	GetWin;
    XPXfaGetViewFunc*	GetView;
    XPXfaToViewFunc*	ToView;
    XPXfaToWinFunc*	ToWin;
};

struct _XPXfa {
    TOPLogical		valid;
    int			serial;
    XPXfaDstType	loView;
    XPXfaDstType	hiView;
    XPXfaSrcType	loWin;
    XPXfaSrcType	hiWin;
    XPXfaDstType	threshView;
    XPXfaSrcType	threshWin;

    double		linFactor;
    double		logFactor;
    double		logLoWin;
    int			centerView;
    int			vClipSgn;
    double		logThreshWin;

    double		fNegFraction;
    double		numDecades;	/* used by xpAxis */

    XPXfaCalls	*Calls;
};

#define XPXFA_Mode(xf)	((xf)->Calls->Mode)

/* must call xpXfaSet before anything else  to establish mode */
extern XPXfaMode	_xpXfaScaleStrToMode ARGS((char *scale));
extern void		xpXfaSet ARGS((XPXfa *xf, XPXfaMode m, 
			  XPXfaDstType lView, XPXfaDstType hView,
			  XPXfaSrcType lWin,XPXfaSrcType hWin));
extern void		xpXfaSetMode ARGS((XPXfa *xf, XPXfaMode m));
extern void		xpXfaSetThresh ARGS((XPXfa *xf,
			  XPXfaDstType tv, XPXfaSrcType tw));
extern TOPStatus	xpXfaConfig ARGS((XPXfa *xf, XPXfaMode m,
			  XPXfaDstType vLo, XPXfaDstType vHi,
			  XPXfaSrcType wLo, XPXfaSrcType wHi,
    			  XPXfaDstType vClip, XPXfaSrcType wClip));

extern void		xpXfaSetView ARGS((XPXfa *xf,
			  XPXfaDstType l,XPXfaDstType h));
extern TOPLogical	xpXfaGetView ARGS(( XPXfa *xf,
			  XPXfaDstType *lo, XPXfaDstType *hi));
extern XPXfaDstType	xpXfaToView ARGS((XPXfa *xf, XPXfaSrcType w));
extern XPXfaSrcType	xpXfaScaleToView ARGS((XPXfa *xf, XPXfaSrcType val));
extern XPXfaDstType	xpXfaRangeToView( XPXfa *xf,
			  XPXfaSrcType wLo, XPXfaSrcType wHi);

extern void		xpXfaSetWin ARGS((XPXfa *xf,
			  XPXfaSrcType l, XPXfaSrcType h));
extern TOPLogical	xpXfaGetWin ARGS((XPXfa *xf,
			  XPXfaSrcType *lo, XPXfaSrcType *hi));
extern XPXfaSrcType	xpXfaToWin ARGS((XPXfa *xf, XPXfaDstType v));
extern XPXfaSrcType	xpXfaScaleToWin ARGS((XPXfa *xf, XPXfaSrcType val));
extern XPXfaSrcType	xpXfaRangeToWin( XPXfa *xf, 
			  XPXfaDstType vLo, XPXfaDstType vHi);

extern void		xpXfaDump ARGS((XPXfa *xf));
extern TOPLogical	xpXfaDSLogGetPosWin ARGS((XPXfa *xf,
			  XPXfaSrcType *lo, XPXfaSrcType *hi));
extern TOPLogical	xpXfaDSLogGetNegWin ARGS(( XPXfa *xf,
			  XPXfaSrcType *lo, XPXfaSrcType *hi));


extern void	xpXfaDrawCircle2 ARGS_X((Display *dpy, Window win, GC gc,
		  XPXfa *xmap, XPXfa *ymap,
		  XPXfaSrcType x, XPXfaSrcType y, XPXfaSrcType r));
extern void	xpXfaDrawCircle ARGS_X((Display *dpy, Window win, GC gc,
		  XPXfa *xmap, XPXfa *ymap,
		  XPXfaSrcType x, XPXfaSrcType y, XPXfaSrcType r));
#endif /* _XPXFA_H_ */
