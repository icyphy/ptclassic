/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#define X11

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h> 
#include <X11/cursorfont.h>
#include <X11/Xresource.h>	/* Pick up XrmUniqueQuark() */
#include "xpa.h"
#else
#include <X/Xlib.h>
#include <X/XMenu.h>
#endif
#include "utility.h"
#include "xg.h"
#include "x10-x11.h"

#ifndef MAXFLOAT
#define MAXFLOAT	HUGE
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#define GRIDPOWER 	10
#define INITSIZE 	128
#define PADDING 	2
#define SPACE 		10
#define TICKLENGTH	5

#define XGI_NOCOLOR	-1

#define XGI_ERROR	-1
#define XGI_NEWSET	1
#define XGI_FOUND	2

#define XGI_DASHLEN	6

#ifdef X11
/* Basic transformation stuff */
#define SCREENX(ws, userX) \
	((int) (((userX) - ws->UsrOrgX)/ws->XUnitsPerPixel + 0.5) + ws->XOrgX)
#define SCREENY(ws, userY) \
	(ws->XOppY - ((int) (((userY) - ws->UsrOrgY)/ws->YUnitsPerPixel + 0.5)))
#else
/* Basic transformation stuff */
#define SCREENX(ws, userX) \
	((int) (((userX) - ws->UsrOrgX)/ws->XUnitsPerPixel + 0.5))
#define SCREENY(ws, userY) \
	(ws->XOppY - ((int) (((userY) - ws->UsrOrgY)/ws->YUnitsPerPixel + 0.5))\
		- ws->XOrgY)
#endif

/**********************************************************************
 * RDFLG
 * Reads a flag value (basically whether or not the default is present
 */
#define RDFLG(name,flag)\
	if (XGETDEFAULT(display,progname, name)) propertyFlags |= flag

/***********************************************************************
 * RDCLR
 * Reads a color default - uses char *value, int temp, and char *progname
 */
#define RDCLR(name, pix) \
	if ((value = XGETDEFAULT(display,progname, (name))) ) \
        if ((temp = xgiGetColor(display,value)) != XGI_NOCOLOR) (pix) = temp

/***********************************************************************
 * RDINT
 * Reads an integer default - uses char *value, and char *progname
 */
#define RDINT(name, newval) \
	if ( (value = XGETDEFAULT(display,progname, (name))) ) \
		newval = atoi(value)

/**********************************************************************
 * RDFNT
 * Reads a font - uses temporary FontInfo *tempFont - must be fixed width
 */
#ifdef X11
#define RDFNT(name, fnt) \
	if ( (value = XGETDEFAULT(display,progname, name)) ) \
		if ( (tempFont = XLOADQUERYFONT(display,value)) ) \
			(fnt) = tempFont
#else
#define RDFNT(name, fnt) \
	if (value = XGETDEFAULT(display,progname, name)) \
		if (tempFont = XLOADQUERYFONT(display,value)) \
            if (tempFont->fixedwidth) {\
				(fnt) = tempFont; \
			} else { \
				(void) fprintf(stderr, "Font %s is not fixed width\n", value); \
				exit(1); \
			}
#endif

typedef struct attr_set {
#ifdef X11
	int     lineStyle;
	char	*dashList;
	PIXEL   pixelValue;
	Pixmap  markStyle;
#else
	Pattern lineStyle;
	PIXEL   pixelValue;
	Bitmap  markStyle;
#endif
} AttrSet;

typedef struct data_set {
	char	*setName;		/* Name of set     */
	int		numPoints;		/* How many points */
	int		allocSize;		/* Allocated size  */
	double	*xvec;			/* X values        */
	double	*yvec;			/* Y values        */
} DataSet;

typedef struct local_win {
	int		width, height;		/* Current size of window               */
	double	loX, loY, hiX, hiY;	/* Local bounding box of window         */
	int		XOrgX, XOrgY;		/* Origin of bounding box on screen     */
	int		XOppX, XOppY;		/* Other point defining bounding box    */
	double	UsrOrgX, UsrOrgY;	/* Origin of bounding box in user space */
	double	UsrOppX, UsrOppY;	/* Other point of bounding box          */
	double	XUnitsPerPixel;		/* X Axis scale factor                  */
	double	YUnitsPerPixel;		/* Y Axis scale factor                  */
} LocalWin;

typedef struct xg_graph {
	Display		*display;
	Window		win;
	LocalWin	*win_info;
	int16		numPlots;
	DataSet		*dataSet;
	XPOINT		*Xpoints;
    int16		allocXpoints;
	char		*titleText;			/* Plot title */
	char		*XUnits;			/* X Unit string */
	char		*YUnits;			/* Y Unit string */
	int32		flags;
	int			(*measuredFunction)();
} XG;

typedef struct {
	double	a,b,c,d;
} COEFF;

typedef struct {
	COEFF	x,y;
} HERMITE;

/* Other defaults */

PIXEL		bgPixel;			/* Background color        */
int			bdrSize;			/* Width of border         */
PIXEL		bdrPixel;			/* Border color            */
#ifdef X11
GC			zeroGC;
#else
int			zeroSize;			/* Size of zero grid line  */
Pattern		zeroStyle;			/* Pattern of zero grid    */
int			zeroPixel;			/* Color of the zero axis  */
#endif
#ifdef X11
GC			normGC;
#else
int			normSize;			/* Size of normal grid     */
Pattern		normStyle;			/* Normal grid style       */
int			normPixel;			/* Foreground (Axis) color */
#endif
PIXEL		cursorPixel;		/* Cursor color 	   */
PIXEL		zoomPixel;			/* Zoom pixel value        */
PIXEL		echoPixel;			/* Echo pixel value        */
XFONTSTRUCT	*axisFont;
XFONTSTRUCT	*titleFont;
int16		bwFlag;				/* Black and white flag    */
int32		propertyFlags;
double		barBase, barWidth;	/* Base and width of bars  */
int			lineWidth;			/* Width of data lines     */
#ifdef X11
#ifdef MAIN_XG
xpa_appearance	menuAttr;
int				menuAttrMask;
#else
extern xpa_appearance	menuAttr;
extern int				menuAttrMask;
#endif
#endif

#define SWAP(type,x,y)  {type t; t = x ; x = y ; y = t;}

#ifdef MAIN_XG 
#ifndef X11
Window		area;

#define XGcursor_width 16
#define XGcursor_height 16
#define XGcursor_x_hot 8
#define XGcursor_y_hot 8
short XGcursor_bits[] = {
   0x0000, 0x0180, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x7ffe,
   0x7ffe, 0x0180, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x0000};

#define zoom_width 16
#define zoom_height 16
#define zoom_x_hot 8
#define zoom_y_hot 7
short zoom_bits[] = {
   0x0000, 0x0000, 0x0000, 0x0000,
   0x0fe0, 0x0820, 0x0820, 0x0820,
   0x0820, 0x0820, 0x0fe0, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000};

extern Bitmap dotMap;
/* For dot marks */
#define dot_width 8
#define dot_height 8
short dot_bits[] = {
	0x0000, 0x003c, 0x007e, 0x007e,
	0x007e, 0x007e, 0x003c, 0x0000};
Bitmap dotMap;

#define wait_width 16
#define wait_height 16
#define wait_x_hot 8
#define wait_y_hot 8
short wait_bits[] = {
	0xffff, 0x7ffe, 0x4002, 0x2004,
	0x17e8, 0x0ff0, 0x07e0, 0x02c0,
	0x0340, 0x04a0, 0x0910, 0x13c8,
	0x27e4, 0x4ff2, 0x7ffe, 0xffff};
#endif

Display	*xgDisplay;
#ifdef X11
GC  xgGC;
#endif

#ifdef X11
Pixmap  zoomCursor;
Pixmap  theCursor = (Pixmap) 0;
Pixmap  waitCursor = (Pixmap) 0;
#else
Cursor zoomCursor;
Cursor theCursor = (Cursor) 0;
Cursor waitCursor = (Cursor) 0;
#endif

int16       suspendRedraw = FALSE;  /* Flag to temporarily suspend the redraw */

#ifdef X11
XContext     win_table = 0;
#else
XAssocTable  *win_table = NIL(XAssocTable);
#endif
#endif

char    *xgiCheckPtr();
#define XG_ALLOC(type,number) \
	(type *) xgiCheckPtr((char *) ALLOC(type,number),\
		(unsigned)(sizeof(type)*number))
#define XG_REALLOC(ptr,type,number)  \
	ptr = (type *) xgiCheckPtr((char *)REALLOC(type,ptr,number), \
		(unsigned)(sizeof(type)*number))

AttrSet AllAttrs[XG_MAXSETS];

#ifndef MAIN_XG
extern void		xgiAddPadding();
#endif
#ifndef MENU_XG
extern int16	xgStartXMenu();
extern void 	xgiMenuHandler();
#endif

