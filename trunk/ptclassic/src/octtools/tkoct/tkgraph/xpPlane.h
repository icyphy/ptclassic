#if !defined(_XPPLANE_H_)
#define _XPPLANE_H_ 1
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

#if !defined(_TOPVEC_H_) && __TOP_AUTOINCLUDE__
#include "topVec.h"
#endif
#if !defined(_EEMSILAR_H_) && __TOP_AUTOINCLUDE__
#include "eemSilar.h"
#endif
#if !defined(_XPXFA_H_) && __TOP_AUTOINCLUDE__
#include "xpXfa.h"
#endif

typedef struct _XPPlaneLocus XPPlaneLocus;
struct _XPPlaneLocus {
    TOPMask		flags;
    TOPVector		vPts;			/* of XPoint */
};
#define XPPLANE_LocusPtNum(pLocus)	TOPVEC_Num(&(pLocus)->vPts)
#define XPPLANE_LocusPtGet(pLocus,i) \
    TOPVEC_Get(&(pLocus)->vPts,(i),XPoint)
#define XPPLANE_LocusPtArray(pLocus)	TOPVEC_Array(&(pLocus)->vPts,XPoint)

/**
	XPPlaneDataExt :: Plane Data Extension :: Plane Graph Widget

	This extension information is used by the plane widget core
	to communicate with various data handlers.  Not all information
	used is used by all handlers.  The first group of fields are
	filled in by the core for use by the handler.  The second group
	of fields are filled in by the core and used by the core.  The
	third group of fields (userData) are exclusively for the
	data handler: it may store any value there.

	All core-supplied data is always valid: it doesn't go away
	after a callback.  Handlers may assume this.

	The {wSilars} is a vector of EEMSilar structures.
	It is allocated by the core, and set (filled in) by the handler.
	The core uses this vector to do *all* of its drawing.
**/
typedef struct _XPPlaneDataExt XPPlaneDataExt;
struct _XPPlaneDataExt {
    /* core -> handler */
    char*		dataSpec;
    XPXfa*		xfaX;
    XPXfa*		xfaY;

    /* handler <--> core */
    EEMSilarGrp		wSilars;	/* gain is ignored */
    TOPVector		vLoci;		/* vec of vec of XPoints for loci */
    EEMSilarGrp		wLociMarks;	/* gain is ignored */

    /* handler -> core */
    EEMComplex		bbLo;
    EEMComplex		bbHi;

    /* handler-only data */
    TOPPtr		userData;
    TOPPtr		userData2;
};

#define XPPLANE_LociNum(pExt)	TOPVEC_Num(&(pExt)->vLoci)
#define XPPLANE_LociGet(pExt,i)	TOPVEC_Get(&(pExt)->vLoci,(i),XPPlaneLocus)
#define XPPLANE_LociAppend(pExt) \
    TOPVEC_Append(&(pExt)->vLoci,XPPlaneLocus)

/**
	XPPlaneDataHandler :: Plane Data Handler :: Plane Graph Widget
	
	A sequence data handler is an interface between the sequence 
	widget core and the application.  The handler is defined by
	a set of callback functions.  These functions are called by
	the widget core to perform specific functions and to notify
	the application of relevant state changes within the widget.

	The primary function of the data handler is to map an application
	provided data sequence (normally doubles) into an XPoint vector
	of pixel locations.  The widget core will then use that vector
	to draw the graph.

	The XPPlaneDataExt structure encapsulates all data that can be
	communicated between the data handler and widget core.  It is
	passed to all callback functions.  The widget core and the
	XPPlaneDataExt structure provide builtin support for axis widgets
	and the XPXfa mapping library.  However, the data handler can
	ignore this information if it has an alternative method for
	performing the mapping.

	Depending on the source of the application data, the data handler
	may need to respond to changes in the size and/or scaling of the
	widget.
**/

typedef enum _XPPlaneChangeType XPPlaneChangeType;
enum _XPPlaneChangeType {
    XPPlaneCT_Null, XPPlaneCT_Insert, XPPlaneCT_Modify, XPPlaneCT_Delete,
};

#define XPPlaneLF_NegInf	TOP_BIT(0)
#define XPPlaneLF_PosInf	TOP_BIT(1)
#define XPPlaneLF_Mark		TOP_BIT(2)

typedef struct _XPPlaneDataHandler XPPlaneDataHandler;
struct _XPPlaneDataHandler {
    TOPLogical		(*constructor) ARGS((XPPlaneDataExt *e));
    TOPLogical		(*destructor) ARGS((XPPlaneDataExt *e));
    TOPLogical		(*changeSilar) ARGS((XPPlaneDataExt *e, 
			  XPPlaneChangeType ct, int idx));
    TOPLogical		(*calcLoci) ARGS((XPPlaneDataExt *e));
    TOPLogical		(*mapLoci) ARGS((XPPlaneDataExt *e));
    TOPLogical		(*drawK) ARGS((XPPlaneDataExt *e,
			  TOPMask flags, double klo, double khi));
    TOPLogical		(*markK) ARGS((XPPlaneDataExt *e,
			  TOPMask flags, double k));
};

/**
    Update requests: xpPlaneDataUpdateOne() & xpPlaneDataUpdateAll()

    When the data handler changes the silar data (e.g. location, power)
    , it must notify the plane widget so that the window can be redrawn
    correctly.  There are two mechanisms available.

    For quick changes, as in response to mouse events, the data handler
    may update {XPPlaneDataExt.wSilars}, and then call UpdateOne()
    for each silar effected.  The silar will be redrawn immediately.

    For bulk changes, the data handler may update {XPPlaneDataExt.wSilars},
    and then call UpdateAll().  This will cause, at some later time,
    the entire screen to be cleared and all silar locations remapped and
    drawn.
**/

/*
 * planeTk.c
 */
extern int xpPlaneWdgRegisterCmds ARGS_TK(( Tcl_Interp *ip, Tk_Window appwin));
extern void xpPlaneDataUpdateOne ARGS(( XPPlaneDataExt *pExt, int idx));
extern void xpPlaneDataUpdateAll ARGS(( XPPlaneDataExt *pExt));
extern void xpPlaneDataUpdateMarks ARGS(( XPPlaneDataExt *pExt));
extern void xpPlaneDataDestroy ARGS((XPPlaneDataExt *pExt));


/*
 * planeSdh.c
 */
extern XPPlaneDataHandler XpPlaneSimpleDataHandler;
#endif /* _XPPLANE_H_ */
