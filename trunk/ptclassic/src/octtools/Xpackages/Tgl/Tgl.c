#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
 * Tgl - Subclass of Command supporting toggle buttons
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains the implementation for the Toggle widget.
 */

#include "port.h"
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Xmu.h>
#include "TglP.h"
#include "utility.h"

#include "btonhi.bm"
#include "btoffhi.bm"
#include "bton.bm"
#include "btoff.bm"

#define TGL_CLASS_NAME	"Tgl"

static PixData btonhiData =
   { btonhi_width, btonhi_height, btonhi_bits };
static PixData btoffhiData =
   { btoffhi_width, btoffhi_height, btoffhi_bits };
static PixData btonData =
   { bton_width, bton_height, bton_bits };
static PixData btoffData =
   { btoff_width, btoff_height, btoff_bits };
static Widget defLeader = (Widget) 0;
static Widget defCurrent = (Widget) 0;

static PixDataPtr hisetMDPtr = &btonhiData;
static PixDataPtr hiunsetMDPtr = &btoffhiData;
static PixDataPtr setMDPtr = &btonData;
static PixDataPtr unsetMDPtr = &btoffData;

static char defaultTranslations[] =
    "<EnterWindow>:	highlight()		\n\
     <LeaveWindow>:	reset()			\n\
     <Btn1Down>:	toggle()		\n\
     <Btn1Up>:		notify()  		";

#define offset(field) XtOffset(TglWidget, field)
static XtResource resources[] = { 
   { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
       offset(tgl.foreground), XtRString, "XtDefaultForeground" },
   { XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t), 
       offset(tgl.callbacks), XtRCallback, (caddr_t) NULL },
   { XtNstate, XtCState, XtRBoolean, sizeof(Boolean),
       offset(tgl.state), XtRImmediate, (caddr_t) False },
   { XtNhisetPixmap, XtCPixmap, XtRTgPixmap, sizeof(TgPixmap),
       offset(tgl.hisetPixmap), XtRPixdata, (caddr_t) &hisetMDPtr },
   { XtNhiunsetPixmap, XtCPixmap, XtRTgPixmap, sizeof(TgPixmap),
       offset(tgl.hiunsetPixmap), XtRPixdata, (caddr_t) &hiunsetMDPtr },
   { XtNsetPixmap, XtCPixmap, XtRTgPixmap, sizeof(TgPixmap),
       offset(tgl.setPixmap), XtRPixdata, (caddr_t) &setMDPtr },
   { XtNunsetPixmap, XtCPixmap, XtRTgPixmap, sizeof(TgPixmap),
       offset(tgl.unsetPixmap), XtRPixdata, (caddr_t) &unsetMDPtr },
   { XtNleader, XtCLeader, XtRWidget, sizeof(Widget),
       offset(tgl.leader), XtRWidget, (caddr_t) &defLeader },
   { XtNcurrent, XtCCurrent, XtRWidget, sizeof(Widget),
       offset(tgl.current), XtRWidget, (caddr_t) &defCurrent },
   { XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
       offset(tgl.justify), XtRImmediate, (caddr_t) XtJustifyCenter }
};
#undef offset


/*
 * Forward declarations of actions
 */

static void Toggle();
static void Set();
static void Unset();
static void Notify();
static void Highlight();
static void Unhighlight();
static void Reset();

static XtActionsRec actionsList[] = {
    { "toggle",		Toggle		},
    { "set",		Set		},
    { "notify",		Notify		},
    { "highlight",	Highlight	},
    { "reset",		Reset		},
    { "unset",		Unset		},
    { "unhighlight",	Unhighlight	}
};

static void TgClassInitialize();
static void TgInitialize();
static void TgDestroy();
static void TgRedisplay();
static Boolean TgSetValues();
static XtGeometryResult TgQueryGeometry();

#define NO_CHANGE	0
#define FORCE_ON	1
#define FORCE_OFF	2
#define FORCE_TOGGLE	3
#define FORCE_OLD	4

#define LOCK		0x10

static void OtherOff();
static void ChangeState();


TglClassRec tglClassRec = {
    {   /* Core fields */
	(WidgetClass) &simpleClassRec,		/* superclass		  */	
	TGL_CLASS_NAME,				/* class_name		  */
	sizeof(TglRec),				/* size			  */
	TgClassInitialize,			/* class_initialize	  */
	NULL,					/* class_part_initialize  */
	FALSE,					/* class_inited		  */
	TgInitialize,				/* initialize		  */
	NULL,					/* initialize_hook	  */
	XtInheritRealize,			/* realize		  */
	actionsList,				/* actions		  */
	XtNumber(actionsList),			/* num_actions		  */
	resources,				/* resources		  */
	XtNumber(resources),			/* resource_count	  */
	NULLQUARK,				/* xrm_class		  */
	FALSE,					/* compress_motion	  */
	TRUE,					/* compress_exposure	  */
	TRUE,					/* compress_enterleave    */
	FALSE,					/* visible_interest	  */
	TgDestroy,				/* destroy		  */
	XtInheritResize,			/* resize		  */
	TgRedisplay,				/* expose		  */
	TgSetValues,				/* set_values		  */
	NULL,					/* set_values_hook	  */
	XtInheritSetValuesAlmost,		/* set_values_almost	  */
	NULL,					/* get_values_hook	  */
	NULL,					/* accept_focus		  */
	XtVersion,				/* version		  */
	NULL,					/* callback_private	  */
	defaultTranslations,			/* tm_table		  */
    	TgQueryGeometry,			/* query_geometry	  */
    	XtInheritDisplayAccelerator,		/* display_accelerator	  */
	NULL					/* extension		  */
    },
    {   /* simple class fields */
	XtInheritChangeSensitive		/* change sensitive       */
    },
    {   /* toggle class fields */
	0					/* not used */
    }
};

WidgetClass tglWidgetClass = (WidgetClass) &tglClassRec;


static GC Get_GC(tw, image, cx, cy, free_flag)
TglWidget tw;		/* Toggle widget     */
Pixmap image;			/* Bitmap to display */
int cx, cy;			/* Offset of bitmap  */
Boolean free_flag;		/* True if deleting  */
/*
 * Since the GCs used by this widget change often,  one GC is
 * cached in this routine.
 */
{
    static GC gc = (GC) 0;
    XGCValues values;

    if (!free_flag) {
	values.foreground   = tw->tgl.foreground;
	values.background   = tw->core.background_pixel;
	values.clip_x_origin  = cx;
	values.clip_y_origin  = cy;
	values.clip_mask    = image;
	if (!gc) {
	    gc = XCreateGC(DisplayOfScreen(tw->core.screen), tw->core.window,
			   GCForeground  | GCBackground  |
			   GCClipXOrigin | GCClipYOrigin | GCClipMask,
			   &values);
	} else {
	    XChangeGC(DisplayOfScreen(tw->core.screen), gc,
		      GCForeground  | GCBackground  |
		      GCClipXOrigin | GCClipYOrigin | GCClipMask,
		      &values);
	}
    } else {
	/* Free GC */
	if (gc) XFreeGC(DisplayOfScreen(tw->core.screen), gc);
	gc = (GC) 0;
    }
    return gc;
}


#define BIG(current, newcomer) \
if ((newcomer) > (current)) (current) = (newcomer);

static void EvalSize(tg, width, height)
TglWidget tg;		/* Widget itself */
Dimension *width, *height;	/* Returned size */
/*
 * Determines smallest possible size for widget.
 */
{
    *width = 0;
    BIG(*width, tg->tgl.hisetPixmap.width);
    BIG(*width, tg->tgl.hiunsetPixmap.width);
    BIG(*width, tg->tgl.setPixmap.width);
    BIG(*width, tg->tgl.unsetPixmap.width);
    *height = 0;
    BIG(*height, tg->tgl.hisetPixmap.height);
    BIG(*height, tg->tgl.hiunsetPixmap.height);
    BIG(*height, tg->tgl.setPixmap.height);
    BIG(*height, tg->tgl.unsetPixmap.height);
}



static XtConvertArgRec pmConvertArgs[] = {
    { XtBaseOffset, (caddr_t) XtOffset(Widget, core.screen), sizeof(Screen *) }
};

static void cvtStrToPixmap(args, num_args, from, to)
XrmValue *args;			/* Arguments to converter */
Cardinal *num_args;		/* Number of arguments    */
XrmValue *from;			/* From type              */
XrmValue *to;			/* To type                */
/*
 * Converts a string representation into a pixmap.  This routine
 * interprets the string as a file name of a file containing
 * data in bitmap(1) form.  Two arguments are expected: the
 * screen and the window.
 */
{
    Screen *screen;
    String filename, params[1];
    static TgPixmap result;
    int hx_rtn, hy_rtn;
    int stat, num_params;

    if (*num_args != 1) {
	XtErrorMsg("cvtStrToPixmap", "wrongParameters", "XtToolkitError",
		   "String to pixmap needs screen argument",
		   (String *) NULL, (Cardinal *) NULL);
    }
    screen = *((Screen **) args[0].addr);
    filename = (String) from->addr;

    stat = XReadBitmapFile(DisplayOfScreen(screen),
			   RootWindowOfScreen(screen),
			   util_file_expand(filename),
			   &result.width, &result.height, &result.map,
			   &hx_rtn, &hy_rtn);
    switch (stat) {
    case BitmapSuccess:
	to->size = sizeof(TgPixmap);
	to->addr = (caddr_t) &result;
	break;
    case BitmapOpenFailed:
	num_params = 1;
	params[0] = filename;
	XtWarningMsg("cvtStrToPixmap", "noFile", "XtToolkitError",
		     "Cannot open file `%s'", params, &num_params);
	break;
    case BitmapFileInvalid:
	num_params = 1;
	params[0] = filename;
	XtWarningMsg("cvtStrToPixmap", "badFile", "XtToolkitError",
		     "Invalid file format in `%s'", params, &num_params);
	break;
    default:
	num_params = 1;
	params[0] = filename;
	XtWarningMsg("cvtStrToPixmap", "noFile", "XtToolkitError",
		     "Unknown error parsing `%s'", params, &num_params);
	break;
    }
}
	


static void cvtDataToPixmap(args, num_args, from, to)
XrmValue *args;			/* Arguments to converter */
Cardinal *num_args;		/* Number of arguments    */
XrmValue *from;			/* From type              */
XrmValue *to;			/* To type                */
/*
 * Converts a PixData structure into a pixmap.  Two arguments are 
 * expected: the screen and the window.
 */
{
    Screen *screen;
    PixDataPtr pix_data;
    String params[1];
    static TgPixmap result;
    int num_params;

    if (*num_args != 1) {
	XtErrorMsg("cvtStrToPixmap", "wrongParameters", "XtToolkitError",
		   "String to pixmap needs screen and window arguments",
		   (String *) NULL, (Cardinal *) NULL);
    }
    screen = *((Screen **) args[0].addr);
    pix_data = *((PixDataPtr *) from->addr);
    result.map = XCreateBitmapFromData(DisplayOfScreen(screen),
				       RootWindowOfScreen(screen),
				       pix_data->data,
				       pix_data->width, pix_data->height);
    if (result.map) {
	result.width = pix_data->width;
	result.height = pix_data->height;
	to->size = sizeof(TgPixmap);
	to->addr = (caddr_t) &result;
    } else {
	num_params = 0;
	XtWarningMsg("cvtDataToPixmap", "badData", "XtToolkitError",
		     "Cannot translate data to Pixmap", params, &num_params);
    }
}




static void TgClassInitialize()
/*
 * Adds appropriate bitmap converters.
 */
{
    XtAddConverter(XtRString, XtRTgPixmap, cvtStrToPixmap,
		   pmConvertArgs, XtNumber(pmConvertArgs));
    XtAddConverter(XtRPixdata, XtRTgPixmap, cvtDataToPixmap,
		   pmConvertArgs, XtNumber(pmConvertArgs));
    XtAddConverter(XtRString, XtRJustify, XmuCvtStringToJustify,
		   (XtConvertArgList) 0, 0);
}



/*ARGSUSED*/
static void TgInitialize(request, new)
Widget request;			/* Values from resources */
Widget new;			/* Actual widget         */
{
    TglWidget tw = (TglWidget) new;
    Dimension w, h;

    EvalSize(tw, &w, &h);
    if (tw->core.width == 0) {
	tw->core.width = w;
    }
    if (tw->core.height == 0) {
	tw->core.height = h;
    }
    tw->tgl.highlight = False;
    tw->tgl.prev_state = tw->tgl.state;
    tw->tgl.prev_current = (Widget) 0;
}



/*ARGSUSED*/
static void TgDestroy(w)
Widget w;
/*
 * Releases locally cached GC.
 */
{
    (void) Get_GC((TglWidget) w, (Pixmap) 0, 0, 0, True);
}




/*ARGSUSED*/
static void TgRedisplay(w, event, region)
Widget w;			/* Widget         */
XEvent *event;			/* Exposure event */
Region region;			/* Merged regions */
/*
 * The toggle expose routine simply draws one of the four pixmaps
 * depending on `state' and `highlight'.  All pixmaps are drawn
 * in the foreground color.  No support for insensitive yet.
 * The image is always centered.
 */
{
   TglWidget tw = (TglWidget) w;
   TgPixmap themap;
   XtExposeProc proc;
   int x = 0, y;

   /* XXX - This is a patch: */
   if (!XtWindow(w)) return;

   if (event->type != Expose) {
       XClearWindow(XtDisplay(w), XtWindow(w));
   }
   if (tw->tgl.highlight) {
       themap = ((tw->tgl.state) ?
		 tw->tgl.hisetPixmap : tw->tgl.hiunsetPixmap);
   } else {
       themap = ((tw->tgl.state) ?
		 tw->tgl.setPixmap : tw->tgl.unsetPixmap);
   }
   switch (tw->tgl.justify) {
   case XtJustifyLeft:
       x = 0;
       break;
   case XtJustifyCenter:
       x = (tw->core.width - themap.width)/2;
       break;
   case XtJustifyRight:
       x = tw->core.width - themap.width;
       break;
   }
   y = (tw->core.height - themap.height)/2;
   XFillRectangle(XtDisplay(w), XtWindow(w),
		  Get_GC(tw, themap.map, x, y, False),
		  x, y, themap.width, themap.height);
   proc = tw->core.widget_class->core_class.superclass->core_class.expose;
   if (proc) {
       (*proc)(w, event, region);
   }
}


/*ARGSUSED*/
static Boolean TgSetValues (current, request, new)
Widget current;			/* Before call to XtSetValues */
Widget request;			/* After call to XtSetValues  */
Widget new;			/* Final version of widget    */
/*
 * Checks for changes.  If some are detected,  causes a redisplay
 * to occur.
 */
{
    TglWidget oldtg = (TglWidget) current;
    TglWidget tg = (TglWidget) new;
    Boolean redisplay = False;
    XEvent evt;

#ifdef NOTDEF
/* sensitive not supported yet */
    if (XtCField(oldcbw,sensitive) != ComWsensitive && !ComWsensitive) {
	/* about to become insensitive */
	ComWset = FALSE;
	ComWhighlighted = FALSE;
    }
#endif

    /* Change to current widget */
    evt.type = NoExpose;
    if (oldtg->tgl.current != tg->tgl.current) {
	ChangeState((TglWidget) tg->tgl.current, &evt, LOCK | FORCE_ON,
		    NO_CHANGE);
    }
    redisplay =
      (oldtg->core.background_pixel != tg->core.background_pixel) ||
      (oldtg->tgl.justify != tg->tgl.justify) ||
      (oldtg->tgl.foreground != tg->tgl.foreground) ||
      (oldtg->tgl.state != tg->tgl.state) ||
      (memcmp((char *) &(oldtg->tgl.hisetPixmap),
	      (char *) &(tg->tgl.hisetPixmap),
	      sizeof(TgPixmap)) != 0) ||
      (memcmp((char *) &(oldtg->tgl.hiunsetPixmap),
	      (char *) &(tg->tgl.hiunsetPixmap),
	      sizeof(TgPixmap)) != 0) ||
      (memcmp((char *) &(oldtg->tgl.setPixmap),
	      (char *) &(tg->tgl.setPixmap),
	      sizeof(TgPixmap)) != 0) ||
      (memcmp((char *) &(oldtg->tgl.unsetPixmap),
	      (char *) &(tg->tgl.unsetPixmap),
	      sizeof(TgPixmap)) != 0);
    /* Change in state may require turning off other widget */
    if (tg->tgl.state && tg->tgl.leader) {
	OtherOff(&evt, tg, (TglWidget) tg->tgl.leader);
    }
    /* Hard set if state is changed */
    if (oldtg->tgl.state != tg->tgl.state) {
	tg->tgl.prev_state = tg->tgl.state;
    }
		      
    return redisplay;
}


static XtGeometryResult TgQueryGeometry(w, request, geo_return)
Widget w;			/* Toggle widget        */
XtWidgetGeometry *request;	/* Parent intended size */
XtWidgetGeometry *geo_return;	/* This widget's size   */
/*
 * This routine is called by a parent that wants a preferred
 * size for the widget.  The `request' is the size (and/or position) 
 * the parent intends to make the widget.  The `geo_return' is the
 * preferred size of the widget.  The preferred size of the
 * toggle widget is the largest size of any of its pixmaps.
 * It tries to make its border zero.
 */
{
    TglWidget tg = (TglWidget) w;

    geo_return->request_mode = CWWidth | CWHeight | CWBorderWidth;
    geo_return->border_width = 0;
    EvalSize(tg, &(geo_return->width), &(geo_return->height));

    /* Now determine return code */
    if (((geo_return->request_mode & request->request_mode) !=
	 geo_return->request_mode) ||
	(request->width < geo_return->width) ||
	(request->height < geo_return->height) ||
	(request->border_width < geo_return->border_width)) {
	return XtGeometryAlmost;
    } else if ((request->width == geo_return->width) &&
	       (request->height == geo_return->height) &&
	       (request->border_width == geo_return->border_width)) {
	return XtGeometryNo;
    } else {
	return XtGeometryYes;
    }
    /*NOTREACHED*/
}




/*
 * Action Routines
 */

static void OtherOff(evt, new_on, leader)
XEvent *evt;			/* Causal event       */
TglWidget new_on;		/* New one to turn on */
TglWidget leader;		/* Leader widget      */
/*
 * This routine changes the current widget in `leader' to
 * `new_on'.  It also turns off the old current widget
 * in `leader' if different from `new_on'.
 */
{
    TglWidget old = (TglWidget) leader->tgl.current;

    if (old && (old != new_on)) {
	ChangeState(old, evt, LOCK | FORCE_OFF, NO_CHANGE);
    }
    leader->tgl.prev_current = leader->tgl.current;
    leader->tgl.current = (Widget) new_on;
}

static void ChangeState(tg, evt, st_opt, hi_opt)
TglWidget tg;		/* Tgl widget    */
XEvent *evt;			/* Trigger event    */
Cardinal st_opt;		/* State option     */
Cardinal hi_opt;		/* Highlight option */
/*
 * This routine forms the basis of all toggle action routines.
 * Both `st_opt' 'and `hi_opt' can have any of the values
 * listed above.  The routine also causes an automatic
 * redisplay of the widget.
 */
{
    TglWidget leader = (TglWidget) tg->tgl.leader;
    Boolean lock_flag;

    lock_flag = st_opt & LOCK;
    if (lock_flag) st_opt &= (~LOCK);
    if ((st_opt != NO_CHANGE) && (st_opt != FORCE_OLD)) {
	tg->tgl.prev_state = tg->tgl.state;
    }
    switch (st_opt) {
    case FORCE_ON:
	tg->tgl.state = True;
	break;
    case FORCE_OFF:
	tg->tgl.state = False;
	break;
    case FORCE_TOGGLE:
	tg->tgl.state = (tg->tgl.state == False);
	break;
    case FORCE_OLD:
	if (leader && tg->tgl.state && leader->tgl.prev_current &&
	    (((TglWidget) leader->tgl.prev_current) != tg)) {
	    ChangeState((TglWidget) leader->tgl.prev_current, evt,
			LOCK | FORCE_ON, NO_CHANGE);
	} else {
	    tg->tgl.state = tg->tgl.prev_state;
	}
	break;
    default:
	break;
    }
    /* Leader processing */
    if (!tg->tgl.prev_state && tg->tgl.state && leader) {
	OtherOff(evt, tg, leader);
    }
    if (lock_flag) tg->tgl.prev_state = tg->tgl.state;
    switch (hi_opt) {
    case FORCE_ON:
	tg->tgl.highlight = True;
	break;
    case FORCE_OFF:
	tg->tgl.highlight = False;
	break;
    default:
	break;
    }
    TgRedisplay((Widget) tg, evt, (Region) 0);
}


/*ARGSUSED*/
static void Toggle(w, event, params, num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * This routine is called when the `toggle' action is invoked
 * in the translation table.  It inverts the state of the
 * button and redisplays.  The old state is saved for
 * `reset'.
 */
{
    ChangeState((TglWidget) w, event, FORCE_TOGGLE, NO_CHANGE);
}

/*ARGSUSED*/
static void Set(w,event,params,num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * This routine is similar to Toggle() but forces the value
 * of the button to True.
 */
{
    ChangeState((TglWidget) w, event, FORCE_ON, NO_CHANGE);
}

/*ARGSUSED*/
static void Unset(w,event,params,num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * This routine is similar to Toggle() but forces the value
 * of the button to False.
 */
{
    ChangeState((TglWidget) w, event, FORCE_OFF, NO_CHANGE);
}

/*ARGSUSED*/
static void Highlight(w,event,params,num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * This routine is similar to Toggle() but forces the value
 * of the the highlight flag to true.  Further,  the
 * previous state of highlight is not saved.
 */
{
    ChangeState((TglWidget) w, event, NO_CHANGE, FORCE_ON);
}

/*ARGSUSED*/
static void Unhighlight(w,event,params,num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * This routine is similar to HighLight() but forces the value
 * of the the highlight flag to false.
 */
{
    ChangeState((TglWidget) w, event, NO_CHANGE, FORCE_OFF);
}

/*ARGSUSED*/
static void Reset(w,event,params,num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * This routine is called to reset the state of the widget
 * to its state before its last transition (sort of
 * like undo).  It also turns off all highlighting.
 */
{
    ChangeState((TglWidget) w, event, FORCE_OLD, FORCE_OFF);
}

/*ARGSUSED*/
static void Notify(w,event,params,num_params)
Widget w;			/* Toggle widget */
XEvent *event;			/* Trigger event */
String *params;			/* Arguments     */
Cardinal num_params;		/* Num arguments */
/*
 * The notify action calls the toggle button callback
 * functions.  If the widget has a leader,  a callback
 * to the previously set widget will be made.  In
 * all caese,  no call specific data is passed.
 */
{
    TglWidget tg = (TglWidget)w;
    TglWidget leader = (TglWidget) tg->tgl.leader;
    Widget previous;

    if (tg->tgl.prev_state != tg->tgl.state) {
	if (leader) {
	    previous = leader->tgl.prev_current;
	    leader->tgl.prev_current = leader->tgl.current;
	} else {
	    previous = (Widget) 0;
	}
	tg->tgl.prev_state = tg->tgl.state;
	if (previous) XtCallCallbacks(previous, XtNcallback, (caddr_t) 0);
	XtCallCallbacks(w, XtNcallback, (caddr_t) 0);
    }
}

/**********************************************************************
 *
 * Public routines
 *
 **********************************************************************/

void XtTgResetLeader(w)
Widget w;			/* Widget */
/*
 * When reusing an unmapped toggle widget as a leader widget,
 * the state of the leader must be reset so that it doesn't
 * call an erroneous callback to an old previous widget.
 * This routine resets the state of a leader widget so
 * this doesn't happen.
 */
{
    TglWidget tgl;

    if (strcmp(w->core.widget_class->core_class.class_name,
	       TGL_CLASS_NAME) == 0) {
	tgl = (TglWidget) w;
	tgl->tgl.current = (Widget) 0;
	tgl->tgl.prev_current = (Widget) 0;
    }
}
