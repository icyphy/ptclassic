/**
    rubber.c :: XP Rubber Object :: XPole

    Tk rubber object (not widget)

    This implements rubber objects: a rectangle or circle that change
    size as the mouse is moved.  They are typically used for sweeping
    out a region.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
**/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
#endif

#include "topFixup.h"
#include <tk.h>

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"

#include "xpRubber.h"

static char _XpRubberSubPackage[] = "xp.rubber";

#define SPKG _XpRubberSubPackage

/**
	Notes:
	-	We handle the expose event.  We assue that the owner
		of the draw widget will have already erased and redrawn
		its portion of the window.  This works because Tk will
		call the event handlers in the order created.
**/


#define XPRubF_Active		TOP_BIT(0)
#define XPRubF_Drawn		TOP_BIT(1)
#define XPRubF_Event		TOP_BIT(2)

typedef struct _XPRubber XPRubber;
struct _XPRubber {
    int			flags;
    Tcl_Interp*		ip;
    char*		name;		/* our own name: from create */
    Tk_Window		drawWin;	/* to draw on: from create */
    Display*		dpy;		/* used after destroy */
    char*		command;	/* resource -command */

    char*		shapeUid;	/* resource -shape */
    int			shape;		/* from shapeUid; XPRubShape_* */
    TOPLogical		autoEventB;	/* resource -auto */

    /* from mouse events */
    XPoint		pt1;
    XPoint		pt2;
    XRectangle		drawRect;
    XArc		drawArc;

    XColor*		foreground;	/* resource -foreground */
    GC			drawGC;
};

/* for XPRubber.shape */
#define XPRubShape_Null		(0)
#define XPRubShape_Rect		(1)
#define XPRubShape_Circle	(2)

#define DEF_RUB_FG		"#000"
#define DEF_RUB_SHAPE		"rect"
#define DEF_RUB_COMMAND		NULL
#define DEF_RUB_AUTO		"true"

static Tk_ConfigSpec _XpRubberConfigSpecs[] = {
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_RUB_FG, Tk_Offset(XPRubber, foreground), 0},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},

    {TK_CONFIG_UID, "-shape", "shape", "Shape",
	DEF_RUB_SHAPE, Tk_Offset(XPRubber, shapeUid), 0},
    {TK_CONFIG_STRING, "-command", "command", "Command",
	DEF_RUB_COMMAND, Tk_Offset(XPRubber, command), 0},
    {TK_CONFIG_BOOLEAN, "-auto", "auto", "Auto",
	DEF_RUB_AUTO, Tk_Offset(XPRubber, autoEventB), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

static void _xpRubberEventCB( ClientData cld, XEvent *pEvent);
static void _xpRubberRecvMotion( XPRubber *pRub, TOPLogical getB);

static void
_xpRubberDestroyCB( ClientData cld) {
    XPRubber *pRub = (XPRubber*) cld;

    if ( pRub->drawWin != NULL ) {
        Tk_DeleteEventHandler( pRub->drawWin, ExposureMask|StructureNotifyMask,
	  _xpRubberEventCB, (ClientData) pRub);
	_xpRubberRecvMotion( pRub, TOP_FALSE);
    }

    if (pRub->name != NULL) {
	free(pRub->name);
	pRub->name = NULL;
    }
    if (pRub->command != NULL) {
	free(pRub->command);
	pRub->command = NULL;
    }
    if (pRub->shapeUid != NULL) {
	free(pRub->shapeUid);
	pRub->shapeUid = NULL;
    }
    if (pRub->foreground != NULL) {
	Tk_FreeColor(pRub->foreground);
	pRub->foreground = NULL;
    }
    if (pRub->drawGC != None) {
	Tk_FreeGC( pRub->dpy, pRub->drawGC);
	pRub->drawGC = None;
    }
    MEM_FREE(pRub);
}

static void
_xpRubberDraw( XPRubber *pRub) {
    Tk_Window		tkw = pRub->drawWin;
    Display*		d;
    Window		win;
    GC			gc;

    if ( pRub->shape == XPRubShape_Null || tkw == NULL || !Tk_IsMapped(tkw) )
	return;
    d = Tk_Display( pRub->drawWin);
    win = Tk_WindowId( pRub->drawWin);
    gc = pRub->drawGC;

    switch ( pRub->shape ) {
    case XPRubShape_Rect:
	XDrawRectangles( d, win, gc, &pRub->drawRect, 1);
	break;
    case XPRubShape_Circle:
	XDrawArcs( d, win, gc, &pRub->drawArc, 1);
	break;
    default:
	topProgMsg("%s: Unknown shape code %d\n", SPKG, pRub->shape);
    }
}

static void
_xpRubberPts2Shape( XPRubber *pRub) {
    XPoint		*p1 = &pRub->pt1;
    XPoint		*p2 = &pRub->pt2;
    XRectangle		*pR = &pRub->drawRect;
    XArc		*pA = &pRub->drawArc;
    int			dx, dy, r;

    if ( pRub->shape == XPRubShape_Null )
	return;

    switch ( pRub->shape ) {
    case XPRubShape_Rect:
	if ( p1->x <= p2->x )	{ pR->x = p1->x; pR->width = p2->x - p1->x; }
	else 			{ pR->x = p2->x; pR->width = p1->x - p2->x; }
	if ( p1->y <= p2->y )	{ pR->y = p1->y; pR->height = p2->y - p1->y; }
	else 			{ pR->y = p2->y; pR->height = p1->y - p2->y; }
	break;
    case XPRubShape_Circle:
	dx = p1->x - p2->x;
	dy = p1->y - p2->y;
	r = (int) ceil(sqrt( (double)(dx*dx + dy*dy) ));
	pA->x = p1->x - r;
	pA->y = p1->y - r;
	pA->width = 2 * r;
	pA->height = 2 * r;
	pA->angle1 = 0;
	pA->angle2 = 360*64;
	break;
    default:
	topProgMsg("%s: Unknown shape code %d\n", SPKG, pRub->shape);
    }
}


static void
_xpRubberBegin( XPRubber *pRub, int x, int y) {
    if ( TOP_IsOn(pRub->flags,XPRubF_Active) ) {
	TOP_PDBG((SPKG,"%s: Out of sync: begin when active", pRub->name));
    }
    if ( TOP_IsOn(pRub->flags,XPRubF_Drawn) ) {
	_xpRubberDraw( pRub);	/* get rid of old one */
    }
    pRub->flags |= XPRubF_Drawn | XPRubF_Active;
    pRub->pt1.x = pRub->pt2.x = x;
    pRub->pt1.y = pRub->pt2.y = y;
    _xpRubberPts2Shape( pRub);
    _xpRubberDraw( pRub);
    if ( pRub->autoEventB )
        _xpRubberRecvMotion( pRub, TOP_TRUE);
}

static void
_xpRubberMotion( XPRubber *pRub, int x, int y) {
    if ( pRub->pt2.x == x && pRub->pt2.y == y )
	return;

    if ( TOP_IsOn(pRub->flags,XPRubF_Drawn) )
        _xpRubberDraw( pRub);
    pRub->pt2.x = x;
    pRub->pt2.y = y;
    _xpRubberPts2Shape( pRub);
    _xpRubberDraw( pRub);
    pRub->flags |= XPRubF_Drawn;
}

static void
_xpRubberEnd( XPRubber *pRub, int x, int y) {
    _xpRubberRecvMotion( pRub, TOP_FALSE);
    if ( TOP_IsOff(pRub->flags,XPRubF_Active) ) {
	TOP_PDBG((SPKG,"%s: Out of sync: end when not active", pRub->name));
    }
    if ( TOP_IsOn(pRub->flags,XPRubF_Drawn) ) {
	_xpRubberDraw( pRub);	/* get rid of old one */
    }
    pRub->flags &= ~(XPRubF_Drawn | XPRubF_Active);
    pRub->pt2.x = x;
    pRub->pt2.y = y;
    _xpRubberPts2Shape( pRub);	/* update final shape */
}

static void
_xpRubberInvoke( XPRubber *pRub) {
    char		buf1[100];
    XRectangle		*pR = &pRub->drawRect;
    XArc		*pA = &pRub->drawArc;
    int			rad;

    if ( pRub->command == NULL || pRub->command == '\0' )
	return;

    switch ( pRub->shape ) {
    case XPRubShape_Rect:
	sprintf( buf1, " rect %d %d %d %d",
	  pR->x, pR->y, pR->width, pR->height);
	break;
    case XPRubShape_Circle:
	rad = pA->width / 2;
	sprintf( buf1, " circle %d %d %d",
	  pA->x + rad, pA->y + rad, rad);
	break;
    default:
	return;
    }
    if ( Tcl_VarEval( pRub->ip, pRub->command, buf1, NULL) != TCL_OK )
	topTclBgError( pRub->ip, NULL);
}

static void
_xpRubberEventCB( ClientData cld, XEvent *pEvent) {
    XPRubber *pRub = (XPRubber *) cld;

    /*IF*/ if ((pEvent->type == Expose) && (pEvent->xexpose.count == 0)) {
	if ( TOP_IsOn(pRub->flags,XPRubF_Drawn) )
	    _xpRubberDraw( pRub);
    } else if ( pEvent->type == DestroyNotify ) {
	Tcl_DeleteCommand( pRub->ip, pRub->name);
	pRub->drawWin = NULL;
	Tk_EventuallyFree( (ClientData) pRub, _xpRubberDestroyCB);
    }
}

static void
_xpRubberMotionCB( ClientData cld, XEvent *pEvent) {
    XPRubber *pRub = (XPRubber *) cld;

    if ( pEvent->type == ButtonPress || pEvent->type == ButtonRelease ) {
	if ( TOP_IsOn(pRub->flags,XPRubF_Active) ) {
	    _xpRubberEnd( pRub, pEvent->xbutton.x, pEvent->xbutton.y);
	    _xpRubberInvoke( pRub);
	}
    } else if ( pEvent->type == MotionNotify ) {
	if ( TOP_IsOn(pRub->flags,XPRubF_Active) ) {
	    _xpRubberMotion( pRub, pEvent->xmotion.x, pEvent->xmotion.y);
	}
    }
}

static void
_xpRubberRecvMotion( XPRubber *pRub, TOPLogical getB) {
    if ( getB && TOP_IsOff(pRub->flags,XPRubF_Event) ) {
	pRub->flags |= XPRubF_Event;
        Tk_CreateEventHandler(pRub->drawWin,
	    PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
	    _xpRubberMotionCB, (ClientData) pRub);
    }
    if ( !getB && TOP_IsOn(pRub->flags,XPRubF_Event) ) {
	pRub->flags &= ~XPRubF_Event;
        Tk_DeleteEventHandler(pRub->drawWin,
	    PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
	    _xpRubberMotionCB, (ClientData) pRub);
    }
}

static int
_xpRubberConfig( Tcl_Interp *ip, XPRubber *pRub, int aC, char **aV, int flags) {
    XGCValues		gcValues;
    unsigned long	gcMask;
    unsigned long	bgpix, fgpix;
    TOPLogical		redrawB = TOP_FALSE;
    int			c;
    TOPStrLenType	len;

    if (Tk_ConfigureWidget( ip, pRub->drawWin, _XpRubberConfigSpecs,
	    aC, aV, (char *) pRub, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Take down the image, if current drawn.  We will redraw it
     * after the config is complete.
     */
    if ( TOP_IsOn(pRub->flags, XPRubF_Drawn) ) {
	_xpRubberDraw( pRub);
	pRub->flags &= ~XPRubF_Drawn;
	redrawB = TOP_TRUE;
    }

    /*
     * Track our overall configuration
     */
    c = pRub->shapeUid[0];	len = strlen(pRub->shapeUid);
    /*IF*/ if ( c == 'r' && strncmp(pRub->shapeUid,"rectangle",len)==0 ) {
	pRub->shape = XPRubShape_Rect;
    } else if ( c == 'c' && strncmp(pRub->shapeUid,"circle",len)==0 ) {
	pRub->shape = XPRubShape_Circle;
    } else {
	topTclRetFmt( ip, "%s: invalid shape %s", pRub->name, pRub->shapeUid);
	pRub->shape = XPRubShape_Null;
	return TCL_ERROR;
    }

    /*
     * Set up the gc.
     * For now we use the simplified dash interface
     */
    if (pRub->drawGC != None) {
        Tk_FreeGC( pRub->dpy, pRub->drawGC);
	pRub->drawGC = None;
    }
    gcMask = 0;
    gcValues.function = GXxor;
    bgpix = Tk_Attributes( pRub->drawWin)->background_pixel;
    fgpix = pRub->foreground->pixel;
    gcValues.plane_mask = (unsigned)~0;
    gcValues.background = bgpix;
    gcValues.foreground = bgpix ^ fgpix;
    gcMask |= GCFunction|GCPlaneMask|GCForeground|GCBackground;
    gcValues.line_width = 0;
    gcValues.line_style = LineOnOffDash;
    gcValues.dash_offset = 0;
    gcValues.dashes = 2;
    gcMask |= GCLineWidth|GCLineStyle|GCDashOffset|GCDashList;

    pRub->drawGC = Tk_GetGC(pRub->drawWin, gcMask, &gcValues);

    /*
     * Put image back up there.
     */
    if ( redrawB ) {
	_xpRubberDraw( pRub);
	pRub->flags |= XPRubF_Drawn;
    }

    return TCL_OK;
}

static int
_xpRubberCmds( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    XPRubber	*pRub = (XPRubber*) cld;
    int		result = TCL_OK;
    TOPStrLenType	length;
    char	c;

    if (aC < 2) {
	Tcl_AppendResult(ip, "usage: ", aV[0], " cmd ?arg arg ...?\"", 
	  (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) pRub);
    c = aV[1][0];
    length = strlen(aV[1]);

    /*IF*/ if ( (c == 'm') && (strncmp(aV[1], "motion", length)==0) ) {
	if ( aC != 4 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], " x y", NULL);
	    goto error;
	}
	_xpRubberMotion( pRub, atoi(aV[2]), atoi(aV[3]));
    } else if ( (c == 'b') && (strncmp(aV[1], "begin", length)==0) ) {
	if ( aC != 4 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], " x y", NULL);
	    goto error;
	}
	_xpRubberBegin( pRub, atoi(aV[2]), atoi(aV[3]));
    } else if ( (c == 'e') && (strncmp(aV[1], "end", length)==0) ) {
	if ( aC != 4 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], " x y", NULL);
	    goto error;
	}
	_xpRubberEnd( pRub, atoi(aV[2]), atoi(aV[3]));
    } else if ( (c == 'i') && (strncmp(aV[1], "invoke", length)==0) ) {
	if ( aC != 2 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], NULL);
	    goto error;
	}
	_xpRubberInvoke( pRub);
    } else if ( (c == 'c') && (strncmp(aV[1], "configure", length)==0) ) {
	if (aC == 2) {
	    result = Tk_ConfigureInfo(ip, pRub->drawWin, _XpRubberConfigSpecs,
		    (char *) pRub, (char *) NULL, 0);
	} else if (aC == 3) {
	    result = Tk_ConfigureInfo(ip, pRub->drawWin, _XpRubberConfigSpecs,
		    (char *) pRub, aV[2], 0);
	} else {
	    result = _xpRubberConfig(ip, pRub, aC-2, aV+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else {
	Tcl_AppendResult(ip, aV[0], ": bad command: ", aV[1], NULL);
	goto error;
    }
    Tk_Release((ClientData) pRub);
    return result;

    error:
    Tk_Release((ClientData) pRub);
    return TCL_ERROR;
}



int
xpRubberCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    XPRubber		*pRub;
    Tk_Window		myWin;

    if (aC < 3) {
	Tcl_AppendResult(ip, "usage: ", aV[0], " objName drawPath ?options?",
	  NULL);
	return TCL_ERROR;
    }
    if ( (myWin = Tk_NameToWindow(ip, aV[2], appwin)) == NULL ) {
	return TCL_ERROR;
    }

    pRub = MEM_ALLOC(XPRubber);
    MEM_CLEAR(pRub);
    pRub->name = memStrSave( aV[1]);
    pRub->drawWin = myWin;
    pRub->ip = ip;
    pRub->flags = 0;
    pRub->drawGC = None;
    pRub->dpy = Tk_Display(myWin);

    if ( _xpRubberConfig( ip, pRub, aC-3, aV+3, 0) != TCL_OK ) {
	goto error;
    }

    Tk_CreateEventHandler( pRub->drawWin, ExposureMask|StructureNotifyMask,
	    _xpRubberEventCB, (ClientData) pRub);

    /* we are commited.  create the command */
    Tcl_CreateCommand( ip, pRub->name, _xpRubberCmds,
    	    (ClientData) pRub, (void (*)()) NULL);

    Tcl_SetResult( ip, pRub->name, TCL_STATIC);
    return TCL_OK;

    error:
    _xpRubberDestroyCB( (ClientData)pRub);
    return TCL_ERROR;
}

int
xpRubberRegisterCmds( Tcl_Interp *ip, Tk_Window appwin) {
    Tcl_CreateCommand( ip, "rubber", xpRubberCmd, (ClientData)appwin, NULL);
    return TCL_OK;
}
