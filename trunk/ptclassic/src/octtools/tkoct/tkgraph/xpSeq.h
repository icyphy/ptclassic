#if !defined(_XPSEQ_H_)
#define _XPSEQ_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

#if !defined(_TOPVEC_H_) && __TOP_AUTOINCLUDE__
#include "topVec.h"
#endif
#if !defined(_XPXFA_H_) && __TOP_AUTOINCLUDE__
#include "xpXfa.h"
#endif

/**
	XPSeqDataExt :: Sequence Data Extension :: Sequence Widget

	This extension information is used by the sequence widget core
	to communicate with various data handlers.  Not all information
	used is used by all handlers.  The first group of fields are
	filled in by the core for use by the handler.  The second group
	of fields are filled in by the core and used by the core.  The
	third group of fields (userData) are exclusively for the
	data handler: it may store any value there.

	All core-supplied data is always valid: it doesn't go away during
	after a callback.  Handlers may assume this.

	The {vPtsVec} is a vector of XPoint structures, see TOPVector(3top).
	It is allocated by the core, and set (filled in) by the handler.
	The core uses this vector to do *all* of its drawing; the core
	never writes it.  It is the handlers responsibility to correctly
	set the length of the vector and to set the {x} fields appropriately;
	in particular, {vWidth} should be honored.

	The {minVal} and {maxVal} fields are used for auto-zoom within
	the graph widget.  The data handler should compute them even if
	auto-zoom is not currently turned on.  The values are the
	min and max of the source data that is actually displayed;
	that is, the min and max of the source data used to fill in
	the {vPtsVec} vector.
**/
typedef struct _XPSeqDataExt XPSeqDataExt;
struct _XPSeqDataExt {
    /* core -> handler */
    char*		graphTag;	/* Tag to uniquely ID this graph */
    char*		dataSpec;	/* resource -dataspec */
    char*		axisNameX;	/* name of axisWdg for data */
    char*		axisNameY;	/* name of axisWdg for data */
    XPXfa*		xfaX;		/* mapping func for x-axis */
    XPXfa*		xfaY;		/* mapping func for y-axis */
    double		wDomainLo;
    double		wDomainHi;
    int			vWidth;		/* width in pixels of draw area */
    int			vResol;		/* horizontal pixel resolution */

    /* handler --> core */
    TOPLogical		configReqB;	/* req to later config and map */
    TOPLogical		mapReqB;	/* req to later map */
    TOPVector		vPtsVec;	/* of XPoint */
    int			vZero;		/* mapped loc of y=zero */
    double		minVal;		/* min range of source data */
    double		maxVal;		/* max range of source data */
    double		minDom;		/* min domain of source data */
    double		maxDom;		/* max domain of source data */

    /* handler-only data */
    TOPPtr		userData;
    TOPPtr		userData2;
};

/**
	XPSeqDataHandler :: Sequence Data Handler :: Sequence Widget
	
	A sequence data handler is an interface between the sequence 
	widget core and the application.  The handler is defined by
	a set of callback functions.  These functions are called by
	the widget core to perform specific functions and to notify
	the application of relevant state changes within the widget.

	The primary function of the data handler is to map an application
	provided data sequence (normally doubles) into an XPoint vector
	of pixel locations.  The widget core will then use that vector
	to draw the graph.

	The XPSeqDataExt structure encapsulates all data that can be
	communicated between the data handler and widget core.  It is
	passed to all callback functions.  The widget core and the
	XPSeqDataExt structure provide builtin support for axis widgets
	and the XPXfa mapping library.  However, the data handler can
	ignore this information if it has an alternative method for
	performing the mapping.

	Depending on the source of the application data, the data handler
	may need to respond to changes in the size and/or scaling of the
	widget.
**/

typedef struct _XPSeqDataHandler XPSeqDataHandler;
struct _XPSeqDataHandler {
    TOPLogical		(*constructor) ARGS((XPSeqDataExt *e));
    TOPLogical		(*destructor) ARGS((XPSeqDataExt *e));
    TOPLogical		(*config) ARGS((XPSeqDataExt *e));
    TOPLogical		(*map) ARGS((XPSeqDataExt *e));
    TOPLogical		(*insertPts) ARGS((XPSeqDataExt *e, 
			  TOPDblPoint2 *pts, unsigned n));
    TOPLogical		(*deletePts) ARGS((XPSeqDataExt *e, 
			  double l, double h));
    int			(*getIndex) ARGS((XPSeqDataExt *e, double val));
    TOPLogical		(*getPts) ARGS((XPSeqDataExt *e,
			  int idx, TOPDblPoint2 *pts, unsigned num));
    TOPLogical		(*setPts) ARGS((XPSeqDataExt *e,
			  int idx, TOPDblPoint2 *pts, unsigned num));
};

/*
 * seqTk.c
 */
extern void	xpSeqDataUpdate ARGS((XPSeqDataExt *pExt));
extern void	xpSeqWdgRegisterCmds ARGS_TK((Tcl_Interp *ip,Tk_Window appwin));

/*
 * seqSdh.c
 */
extern XPSeqDataHandler XpSeqSimpleDataHandler;
#endif /* _XPSEQ_H_ */
