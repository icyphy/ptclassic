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
/* $XConsortium: LImage.c,v 1.2 88/10/25 17:40:25 swick Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#include "port.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "LImageP.h"
#include "utility.h"

/* Actually PixDataPtr */
#define XtRPixdata	"pixData"

typedef struct _PixData {
    unsigned int width, height;
    char *data;
} PixData, *PixDataPtr;

#include "default.bm"
static PixData defaultData = { default_width, default_height, default_bits };
static PixDataPtr defaultDataPtr = &defaultData;

static XtResource resources[] = {
#define offset(field) XtOffset(LImageWidget, lImage.field)
    /* {name, class, type, size, offset, default_type, default_addr}, */
    { XtNforeground, XtCColor, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, XtDefaultForeground },
    { XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(font), XtRString, "XtDefaultFont" },
    { XtNlabel, XtCLabel, XtRString, sizeof(String),
	offset(label), XtRString, NULL },
    { XtNimage, XtCPixmap, XtRSqPixmap, sizeof(SqPixmap),
	offset(image), XtRPixdata, (caddr_t) &defaultDataPtr },
    { XtNselect, XtCselect, XtRBoolean, sizeof(Boolean),
	offset(select), XtRImmediate, (caddr_t) False },
    { XtNpadding, XtCWidth, XtRDimension, sizeof(Dimension),
	offset(padding), XtRImmediate, (caddr_t) 2 },
    { XtNclickThres, XtCWidth, XtRInt, sizeof(int),
	offset(click_thres), XtRImmediate, (caddr_t) 500 },
    { XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	offset(callbacks), XtRCallback, (caddr_t)NULL }
#undef offset
};

static void LiPushAction(/* Widget, XEvent*, String*, Cardinal* */);

static XtActionsRec actions[] =
{
  /* {name, procedure}, */
    {"pushAction",	LiPushAction},
};

static char translations[] =
"<BtnDown>:		pushAction()	\
";

static void LiClassInitialize();
static void LiInitialize();
static void LiRedisplay();
static Boolean LiSetValues();

LImageClassRec lImageClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"LImage",
    /* widget_size		*/	sizeof(LImageRec),
    /* class_initialize		*/	LiClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	LiInitialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	NULL,
    /* expose			*/	LiRedisplay,
    /* set_values		*/	LiSetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* lImage fields */
    /* empty			*/	0
  }
};

WidgetClass lImageWidgetClass = (WidgetClass)&lImageClassRec;



/*
 * Resource converters (stolen from toggle widget)
 */

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
    static SqPixmap result;
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
	to->size = sizeof(SqPixmap);
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
    static SqPixmap result;
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
	to->size = sizeof(SqPixmap);
	to->addr = (caddr_t) &result;
    } else {
	num_params = 0;
	XtWarningMsg("cvtDataToPixmap", "badData", "XtToolkitError",
		     "Cannot translate data to Pixmap", params, &num_params);
    }
}


static void LiClassInitialize()
/*
 * Adds appropriate bitmap converters and sets gray bitmap
 * in class record.
 */
{
    XtAddConverter(XtRString, XtRSqPixmap, cvtStrToPixmap,
		   pmConvertArgs, XtNumber(pmConvertArgs));
    XtAddConverter(XtRPixdata, XtRSqPixmap, cvtDataToPixmap,
		   pmConvertArgs, XtNumber(pmConvertArgs));
}

static GC LiGC(w, fg, bg, image, x, y, font)
Widget w;			/* For creating new widget          */
Pixel fg, bg;			/* Foreground and background pixels */
Pixmap image;			/* Image to display                 */
int x, y;			/* Offset into window               */
XFontStruct *font;		/* Font to use for text             */
/*
 * Arranges to return a statically allocated graphics context
 * with the appropriate fields set.  This of course has problems
 * across multiple displays.
 */
{
    static GC gc = (GC) 0;
    XGCValues values;
    unsigned long mask;

    mask = GCForeground | GCBackground | GCClipXOrigin | GCClipYOrigin |
      GCClipMask;
    values.foreground	 = fg;
    values.background	 = bg;
    values.clip_x_origin = x;
    values.clip_y_origin = y;
    values.clip_mask     = image;
    if (font) {
	mask |= GCFont;
	values.font	 = font->fid;
    }
    if (!gc) {
	gc = XCreateGC(XtDisplay(w), XtWindow(w), mask, &values);
    } else {
	XChangeGC(XtDisplay(w), gc, mask, &values);
    }
    return gc;
}

static String getword(spot, count)
String *spot;
int *count;
/*
 * Moves forward through a string looking for word delimeters.
 */
{
    String ret = *spot;

    *count = 0;
    if (**spot) {
	while (**spot) {
	    switch (**spot) {
	    case ' ':
	    case '\n':
	    case '\t':
	    case '/':
	    case '-':
	    case '_':
	    case ':':
		/* Word delims */
		(*spot)++;
		(*count)++;
		return ret;
	    default:
		(*spot)++;
		(*count)++;
		break;
	    }
	}
	return ret;
    } else {
	return (String) 0;
    }
}


static void LiComputeLabelSize(liw)
LImageWidget liw;
/*
 * Determines the status of the label.  May cause word breaks.
 * Determines size of text.
 */
{
    String idx, word;
    int alloc_lines = 5;
    int cur_width, word_size, count, i;

    if (liw->lImage.line_array) XtFree((char *)liw->lImage.line_array);
    liw->lImage.num_lines = 0;
    liw->lImage.max_line_width = 0;
    liw->lImage.all_height = 0;

    liw->lImage.line_array = (LILabelLine *)
      XtCalloc(alloc_lines, sizeof(LILabelLine));
    idx = liw->lImage.label;
    word = getword(&idx, &count);
    cur_width = XTextWidth(liw->lImage.font, word, count);
    liw->lImage.line_array[0].line = word;
    liw->lImage.line_array[0].count = count;
    liw->lImage.num_lines = 1;
    while ( (word = getword(&idx, &count)) ) {
	word_size = XTextWidth(liw->lImage.font, word, count);
	if (cur_width + word_size > 2*liw->lImage.image.width) {
	    /* Too large for this line */
	    if (liw->lImage.num_lines >= alloc_lines) {
		/* Make more lines */
		alloc_lines *= 2;
		liw->lImage.line_array = (LILabelLine *)
		  XtRealloc((char *)liw->lImage.line_array,
			    alloc_lines * sizeof(LILabelLine));
	    }
	    liw->lImage.line_array[liw->lImage.num_lines-1].width = cur_width;
	    liw->lImage.line_array[liw->lImage.num_lines].line = word;
	    liw->lImage.line_array[liw->lImage.num_lines].count = count;
	    liw->lImage.num_lines++;
	    cur_width = word_size;
	} else {
	    /* Fits on this line */
	    liw->lImage.line_array[liw->lImage.num_lines-1].count += count;
	    cur_width += word_size;
	}
    }
    liw->lImage.line_array[liw->lImage.num_lines-1].width = cur_width;

    liw->lImage.max_line_width = 0;
    for (i = 0;  i < liw->lImage.num_lines;  i++) {
	if (liw->lImage.line_array[i].width > liw->lImage.max_line_width) {
	    liw->lImage.max_line_width = liw->lImage.line_array[i].width;
	}
    }
    liw->lImage.all_height = liw->lImage.num_lines *
      (liw->lImage.font->ascent + liw->lImage.font->descent) +
	liw->lImage.num_lines-1;
}

#define BIGGER(a,b)	((a) > (b) ? (a) : (b))

static Dimension LiWidth(liw)
LImageWidget liw;
{
    return BIGGER(liw->lImage.image.width, liw->lImage.max_line_width) +
      2*liw->lImage.padding;
}

static Dimension LiHeight(liw)
LImageWidget liw;
{
    return liw->lImage.image.height + liw->lImage.all_height +
      3*liw->lImage.padding;
}

static void LiInitialize(request, new)
Widget request, new;
{
    LImageWidget liw = (LImageWidget) new;

    if (liw->lImage.label == NULL) {
	liw->lImage.label = XtNewString(liw->core.name);
    } else {
	liw->lImage.label = XtNewString(liw->lImage.label);
    }
    liw->lImage.num_lines = 0;
    liw->lImage.line_array = 0;
    liw->lImage.max_line_width = 0;
    liw->lImage.all_height = 0;
    liw->lImage.last_ms = 0;
    liw->lImage.click_count = 0;

    LiComputeLabelSize(liw);
    if (liw->core.width == 0) {
	liw->core.width = LiWidth(liw);
    }
    if (liw->core.height == 0) {
	liw->core.height = LiHeight(liw);
    }
}

static void LiRedisplay(w, event, region)
Widget w;			/* Widget         */
XEvent *event;			/* Exposure event */
Region region;			/* Merged regions */
/*
 * The lImage is drawn based on the state of `select'.  If
 * it is off, it is drawn with the foreground on the background.
 * The lImage is drawn according to the four possible states:
 *   SQ_OFF:		area stippled with foreground color
 *   SQ_POSSIBLE:	no drawing at all
 *   SQ_PLAYER_ONE:	piece and lImage drawn appropriately
 *   SQ_PLAYER_TWO:	piece and lImage drawn appropriately
 */
{
    LImageWidget sw = (LImageWidget) w;
    Dimension wi, he;
    Dimension real_wi, real_he;
    Pixel fg, bg;
    GC gc;
    int x, y, i;

    if (!XtWindow(w)) return;

    wi = sw->core.width;
    he = sw->core.height;
    real_wi = LiWidth(sw);
    real_he = LiHeight(sw);

    if (sw->lImage.select == True) {
	fg = sw->core.background_pixel;
	bg = sw->lImage.foreground;
	gc = LiGC(w, bg, fg, (Pixmap) 0, 0, 0, (XFontStruct *) 0);
	XFillRectangle(XtDisplay(w), XtWindow(w), gc, 0, 0, wi, he);
    } else {
	fg = sw->lImage.foreground;
	bg = sw->core.background_pixel;
    }

    /* Draw the image */
    x = (wi - sw->lImage.image.width)/2;
    y = (he - real_he)/2 + sw->lImage.padding;
    gc = LiGC(w, fg, bg, sw->lImage.image.map, x, y, (XFontStruct *) 0);
    XFillRectangle(XtDisplay(w), XtWindow(w), gc, x, y,
		   sw->lImage.image.width, sw->lImage.image.height);

    /* Draw the text */
    gc = LiGC(w, fg, bg, (Pixmap) 0, 0, 0, sw->lImage.font);
    y += (sw->lImage.image.height + sw->lImage.padding + sw->lImage.font->ascent);
    for (i = 0;  i < sw->lImage.num_lines;  i++) {
	XDrawString(XtDisplay(w), XtWindow(w), gc,
		    (wi - sw->lImage.line_array[i].width)/2, y,
		    sw->lImage.line_array[i].line,
		    sw->lImage.line_array[i].count);
	y += sw->lImage.font->ascent + sw->lImage.font->descent + 1;
    }
}

static Boolean PixComp(first, second)
SqPixmapPtr first, second;
{
    return
      (first->width == second->width) &&
	(first->height == second->height) &&
	  (first->map == second->map);
}


static Boolean LiSetValues(current, request, new)
Widget current;			/* Current widget state   */
Widget request;			/* Requested widget state */
Widget new;			/* Actual new values      */
/*
 * Looks for changes in state that should cause a redisplay.
 */
{
    LImageWidget old = (LImageWidget) current;
    LImageWidget now = (LImageWidget) new;
    Boolean new_size = False;
    Boolean redisplay = False;

    if (now->lImage.label == NULL) {
	now->lImage.label = now->core.name;
    }

    if (old->lImage.label != now->lImage.label) {
	if (old->lImage.label) XtFree((char *) old->lImage.label);
	now->lImage.label = XtNewString(now->lImage.label);
	new_size = True;
    }

    if ((old->lImage.font != now->lImage.font) ||
	(PixComp(&(old->lImage.image), &(now->lImage.image)) != 0) ||
	(old->lImage.padding != now->lImage.padding)) {
	new_size = True;
    }

    if (new_size) {
	LiComputeLabelSize((LImageWidget) new);
	redisplay = True;
    }

    if ((old->lImage.select != now->lImage.select) ||
	(old->lImage.foreground != now->lImage.foreground) ||
	(old->core.background_pixel != now->core.background_pixel)) {
	redisplay = True;
    }
    return redisplay;
}

/* Cast to long in case Time is unsigned, thus avoiding
 warning: unsigned value < 0 is always 0
 */
#define li_abs(a)	(((long)a) < 0 ? -((long)a) : ((long)a))

/*ARGSUSED*/
static void LiPushAction(w, evt, params, num_params)
Widget w;
XEvent *evt;
String *params;
Cardinal *num_params;
{
    LImageWidget sw = (LImageWidget) w;
    Time diff;

    if (evt->type == ButtonPress) {
	diff = li_abs((long)evt->xbutton.time -(long)sw->lImage.last_ms);
	if (diff < sw->lImage.click_thres) {
	    sw->lImage.click_count++;
	} else {
	    sw->lImage.click_count = 0;
	}
	sw->lImage.last_ms = evt->xbutton.time;
    }

    XtCallCallbacks(w, XtNcallback, (caddr_t) sw->lImage.click_count);
}
