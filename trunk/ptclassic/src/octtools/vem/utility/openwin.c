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
 * VEM utility routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1989, 1990
 *
 * This file contains various utility routines for commonly
 * used VEM operations.  There could be some small core
 * leaks here.  Some provision for setting the icon would
 * be nice.
 */

#include "general.h"		/* General definitions       */
#include <X11/Xlib.h>		/* X prototype definitions   */
#include <X11/Xutil.h>		/* X utilities               */
#include "xvals.h"		/* X Values                  */
#include "oct.h"		/* OCT data manager          */
#include "defaults.h"		/* Defaults package          */
#include "buffer.h"		/* Buffer management         */
#include "windows.h"		/* Graphics Windows          */
#include "bindings.h"		/* Command bindings          */
#include "message.h"		/* Output message handling   */
#include "vemUtil.h"		/* Self Declaration          */
#include "attributes.h"		/* Attribute handling        */
#include "list.h"		/* List management           */
#include "cursors.h"		/* Cursor management         */

/* From vem.c */
extern int vemSyncEvents();

/* From remote procedure call system */
extern vemStatus RPCExecApplication();

/* Window name size */
#define NAME_LEN	150

/* Size of larger dimension of new window */
#define NORMSIZE	600

/* 
 * The following is used for the default position of windows.  See
 * the vuOpenWindow and vuCopyWindow for details.
 */

static int defaultSpot = 50;



void vuSetWMProps(disp, win, w_name, i_name, u_pos, u_size, x, y, w, h,
		       mw, mh, in_model, state, leader, r_name, r_class )
Display *disp;			/* X Connection                */
Window win;			/* Window to set properties on */
char *w_name;			/* Window name (disp in title) */
char *i_name;			/* Icon name (disp in icon)    */
int u_pos;			/* If non-zero, user position  */
int u_size;			/* If non-zero, user size      */
int x, y;			/* Window position             */
unsigned int w, h;		/* Window size                 */
int mw, mh;			/* Minimum window size         */
int in_model;			/* Input model (True or False) */
int state;			/* Initial state               */
Window leader;			/* Group window leader         */
char *r_name;			/* Name for resources          */
char *r_class;			/* General class of window     */
/*
 * This routine fills in and sets all of the above window manager
 * attributes via window properties.  All fields should be
 * specified.  This should now be ICCCM compliant.
 */
{
    static XSizeHints *s_hints = (XSizeHints *) 0;
    static XWMHints *w_hints = (XWMHints *) 0;
    static XClassHint *c_hints = (XClassHint *) 0;
    XTextProperty win_name, icon_name;

    /* Future compatibility */
    if (!s_hints) s_hints = XAllocSizeHints();
    if (!w_hints) w_hints = XAllocWMHints();
    if (!c_hints) c_hints = XAllocClassHint();

    if (XStringListToTextProperty(&w_name, 1, &win_name) &&
	XStringListToTextProperty(&i_name, 1, &icon_name)) {
	/* Size hints */
	s_hints->flags = PMinSize;
	if (u_pos) s_hints->flags |= USPosition;
	else s_hints->flags |= PPosition;
	if (u_size) s_hints->flags |= USSize;
	else s_hints->flags |= PSize;
	s_hints->x = x;
	s_hints->y = y;
	s_hints->width = w;
	s_hints->height = h;
	s_hints->min_width = mw;
	s_hints->min_height = mh;
	/* Window manager hints */
	w_hints->flags = InputHint | StateHint | WindowGroupHint;
	w_hints->input = in_model;
	w_hints->initial_state = state;
	w_hints->window_group = leader;
	/* Class hints */
	c_hints->res_name = r_name;
	c_hints->res_class = r_class;
	XSetWMProperties(disp, win, &win_name, &icon_name, (char **) 0, 0,
			 s_hints, w_hints, c_hints);

    }
}


static char *vuTp2Str(tp)
XTextProperty *tp;		/* Text Property */
/*
 * Returns a string from the given text property.  This string
 * is allocated and should be freed when no longer needed.
 */
{
    char **str_list, *result;
    int count, len, i;

    if (XTextPropertyToStringList(tp, &str_list, &count)) {
	len = 0;
	for (i = 0;  i < count;  i++) {
	    len += strlen(str_list[i]);
	}
	result = VEMARRAYALLOC(char, len+1);
	result[0] = '\0';
	for (i = 0;  i < count;  i++) {
	    STRCONCAT(result, str_list[i]);
	}
	XFreeStringList(str_list);
	return result;
    } else {
	return VEMSTRCOPY("Failed");
    }
}

static char *vuWindowName(disp, win)
Display *disp;			/* X Connection */
Window win;			/* VEM Window   */
/*
 * Returns the window manager name for `win'.
 */
{
    XTextProperty t_prop;

    if (XGetWMName(disp, win, &t_prop)) {
	return vuTp2Str(&t_prop);
    } else {
	return VEMSTRCOPY("No Window Name");
    }
}

static char *vuIconName(disp, win)
Display *disp;			/* X Connection */
Window win;			/* VEM Window   */
/*
 * Returns the icon name used by the window manager name for `win'.
 */
{
    XTextProperty t_prop;

    if (XGetWMIconName(disp, win, &t_prop)) {
	return vuTp2Str(&t_prop);
    } else {
	return VEMSTRCOPY("No Icon Name");
    }
}



#define USER_POS	0x01	/* User specified position */
#define USER_SIZE	0x02	/* User specified size     */

#define MIN_SIZE	50

/*ARGSUSED*/
static vemStatus vuCoreWindow(newXWindow, oldWin, newFacet, oldInst,
			      ext, geoSpec)
Window *newXWindow;		/* New window (returned)   */
Window oldWin;			/* Old window for inheritance */
octObject *newFacet;		/* Facet for new window    */
octObject *oldInst;		/* For opening masters     */
struct octBox *ext;		/* Optional extent         */
STR geoSpec;			/* Optional geometry spec  */
/*
 * This routine opens a new VEM graphics window that displays
 * the facet `newFacet'.  If `oldInst' is non-zero,  the master
 * of the instance will be opened and returned in `newFacet'.
 * If `ext' is non-zero,  only that portion
 * of the window will be displayed.  If `geospec' is non-zero,
 * the window will be created with the size and location given
 * in the specification (WxH+X+Y).  Otherwise,  the user will
 * be prompted interactively for the window size and location.
 * If `oldWin' is non-zero,  all window attributes will be copied from it.
 * Otherwise,  the routine determines the proper technology and 
 * command bindings for the facet along with its window options (see 
 * wnGrab for details).  If `ext' is non-zero,  the window will
 * be zoomed to the indicated extent.  This routine is
 * used to implement the routines vuOpenWindow, vuRPCWindow, vuMasterWindow,
 * and vuCopyWindow.
 */
{
    Display *disp;
    Window root;
    int ox, oy;			/* Old window location          */
    unsigned int ow, oh;	/* Old window size              */
    unsigned int ob, od;	/* Old window width, border     */
    int nx, ny;			/* New window position          */
    unsigned int nw, nh;	/* New window size              */
    XSetWindowAttributes wattr;	/* New window attributes        */
    int user_flags;		/* USER_POS and/or USER_SIZE    */
    int geo_mask;		/* Geometry specification mask  */
    struct octBox bb;		/* Facet bounding box       */
    struct octBox newExtent;	/* Initial extent of window */
    int gridSize;		/* Default major grid size  */
    int bdrWidth;		/* Window border            */
    char *winName;		/* Window name              */
    char *icoName;		/* Iconified name           */
    XColor *tempColor;		/* Border color             */
    int extraWidth, extraHeight;
    int extWidth, extHeight;
    
    disp = xv_disp();
    dfGetInt("lambda", &gridSize);
    user_flags = 0;
    if ( (oldWin = wnGrabWindow(oldWin)) ) {
	/* Get information about old window */
	XGetGeometry(disp, oldWin, &root, &ox, &oy, &ow, &oh, &ob, &od);
	if (ext) {
	    int extWidth, extHeight;
	    
	    /* Modify based on aspect ratio of new extent */
	    extWidth=VEMMAX(VEMABS(ext->upperRight.x - ext->lowerLeft.x),
			    gridSize);
	    extHeight=VEMMAX(VEMABS(ext->upperRight.y - ext->lowerLeft.y),
			     gridSize);
	    if (extWidth > extHeight) {
		nw = ow;
		nh = VEMMAX((ow * extHeight) / extWidth, MIN_SIZE);
	    } else {
		nh = oh;
		nw = VEMMAX((oh * extWidth) / extHeight, MIN_SIZE);
	    }
	} else {
	    /* Same width and height */
	    nw = ow;
	    nh = oh;
	}
    } else {
	/* Open a new buffer */
	if (oldInst) {
	    /* Open master of instance */
	    if (bufOpenMaster(oldInst, newFacet, 0) != VEM_OK) {
		vemMsg(MSG_A, "Cannot find master of instance `%s:%s':\n  %s\n",
		       oldInst->contents.instance.master,
		       oldInst->contents.instance.view,
		       octErrorString());
		return VEM_CORRUPT;
	    }
	} else {
	    /* Open facet itself */
	    if (bufOpen(newFacet, 0) != VEM_OK) {
		/* Writes its own error messages */
		return VEM_CORRUPT;
	    }
	}
	if (!ext) {
	    if (octBB(newFacet, &bb) != OCT_OK) {
		vemMsg(MSG_L, "Warning: Unable to get bounding box of cell:\n  %s\n",
		       octErrorString());
		/* New computation required here */
		newExtent.lowerLeft.x = -(gridSize * 20);
		newExtent.lowerLeft.y = -(gridSize * 20);
		newExtent.upperRight.x = gridSize * 20;
		newExtent.upperRight.y = gridSize * 20;
	    } else {
		newExtent = bb;
	    }
	    /* Adds 5% slack */
	    extraWidth = VEMMAX((newExtent.upperRight.x-newExtent.lowerLeft.x) / 20,
				gridSize);
	    extraHeight = VEMMAX((newExtent.upperRight.y-newExtent.lowerLeft.y) / 20,
				 gridSize);
	    newExtent.lowerLeft.x  =  newExtent.lowerLeft.x - extraWidth;
	    newExtent.lowerLeft.y  =  newExtent.lowerLeft.y - extraHeight;
	    newExtent.upperRight.x =  newExtent.upperRight.x + extraWidth;
	    newExtent.upperRight.y =  newExtent.upperRight.y + extraHeight;
	} else {
	    newExtent = *ext;
	}

	/* Determine default size */
	extWidth = newExtent.upperRight.x - newExtent.lowerLeft.x;
	extHeight = newExtent.upperRight.y - newExtent.lowerLeft.y;
	if (extWidth > extHeight) {
	    nw = NORMSIZE;
	    nh = VEMMAX((NORMSIZE * extHeight) / extWidth, MIN_SIZE);
	} else {
	    nh = NORMSIZE;
	    nw = VEMMAX((NORMSIZE * extWidth) / extHeight, MIN_SIZE);
	}
    }
    nx = ny = defaultSpot;
    defaultSpot = (defaultSpot + 50) % 500;

    /* Handle geo specification */
    geo_mask = 0;
    if (geoSpec) {
	geo_mask = XParseGeometry(geoSpec, &nx, &ny, &nw, &nh);
	if ((geo_mask & XNegative) && (nx < 0)) {
	    nx += WidthOfScreen(DefaultScreenOfDisplay(disp));
	}
	if ((geo_mask & YNegative) && (ny < 0)) {
	    ny += HeightOfScreen(DefaultScreenOfDisplay(disp));
	}
	if (geo_mask & (XValue|YValue)) user_flags |= USER_POS;
	if (geo_mask & (WidthValue|HeightValue)) user_flags |= USER_SIZE;
    }

    /* Make sure all events have been handled */
    vemSyncEvents();

    /* set attributes */
    wattr.background_pixel = atrQBg();
    dfGetPixel("border.color", &tempColor);
    wattr.border_pixel = tempColor->pixel;
    wattr.bit_gravity = CenterGravity;
    wattr.event_mask = StructureNotifyMask;
    wattr.colormap = xv_cmap();
    wattr.cursor = vemCursor(CROSS_CURSOR);
    dfGetInt("border.width", &bdrWidth);

    /* Make the window */
    *newXWindow = XCreateWindow(disp, DefaultRootWindow(disp), nx, ny, nw, nh,
				bdrWidth, xv_depth(), InputOutput, xv_vis(),
				CWBackPixel|CWBorderPixel|CWBitGravity|
				CWEventMask|CWColormap|CWCursor, &wattr);
    if (*newXWindow == 0) {
	vemMsg(MSG_A, "Window creation aborted\n");
	return VEM_CORRUPT;
    }
    if (oldWin) {
	if (wnDup(*newXWindow, oldWin, ext, wattr.border_pixel,
		  wattr.background_pixel) != VEM_OK)
	  {
	      vemMsg(MSG_A, "Can't grab hold of duplicated window\n");
	      XDestroyWindow(disp, *newXWindow);
	      return VEM_CORRUPT;
	  }
	winName = vuWindowName(disp, oldWin);
	icoName = vuIconName(disp, oldWin);
	vuSetWMProps(disp, *newXWindow, winName, icoName, user_flags & USER_POS,
		     user_flags & USER_SIZE, nx, ny, nw, nh, MIN_SIZE, MIN_SIZE,
		     True, NormalState, xv_drw(), "vem", "Vem");
    } else {
	if (wnGrab(*newXWindow, newFacet, &newExtent, wattr.border_pixel,
		   wattr.background_pixel) != VEM_OK)
	  {
	      vemMsg(MSG_A, "Can't grab hold of new window\n");
	      XDestroyWindow(disp, *newXWindow);
	      return VEM_CORRUPT;
	  }
	icoName = VEMARRAYALLOC(char, NAME_LEN);
	if (STRCOMP(newFacet->contents.facet.facet, "contents") == 0) {
	    (void) sprintf(icoName, " %s:%s ", 
			   newFacet->contents.facet.cell,
			   newFacet->contents.facet.view);
	} else {
	    (void) sprintf(icoName, " %s:%s:%s ", 
			   newFacet->contents.facet.cell,
			   newFacet->contents.facet.view,
			   newFacet->contents.facet.facet);
	}
	winName = VEMARRAYALLOC(char, NAME_LEN);
	(void) sprintf(winName, " VEM cell%s", icoName);
	vuSetWMProps(disp, *newXWindow, winName, icoName, user_flags & USER_POS,
		     user_flags & USER_SIZE, nx, ny, nw, nh, 100, 100,
		     True, NormalState, xv_drw(), "vem", "Vem");
    }
    XMapWindow(disp, *newXWindow);
    VEMFREE(icoName);
    VEMFREE(winName);
    return VEM_OK;
}


/*ARGSUSED*/
vemStatus vuOpenWindow(newXWindow, prompt, newFacet, geoSpec)
Window *newXWindow;		/* New window (returned)   */
STR prompt;			/* How to prompt the user  */
octObject *newFacet;		/* Facet for new window    */
STR geoSpec;			/* Optional geometry specification */
/*
 * This routine opens a new VEM graphics window which displays
 * `newFacet'.  It sets up the proper technology and command
 * bindings for the facet along with its window options (see
 * wnGrab for details).  `prompt' is displayed in the upper
 * left hand corner of the screen while the user opens the
 * window.  If `geoSpec' is provided,  the window will be
 * opened automatically at the specified size and location.
 * Under X11,  `prompt' is ignored.
 */
{
    return vuCoreWindow(newXWindow, (Window) 0, newFacet, (octObject *) 0,
			(struct octBox *) 0, geoSpec);
}

vemStatus vuMasterWindow(newXWindow, newFacet, oldInst, geoSpec)
Window *newXWindow;		/* New window (returned)           */
octObject *newFacet;		/* Facet for new window            */
octObject *oldInst;		/* Instance for master             */
STR geoSpec;			/* Optional geometry specification */
/*
 * This routine is similar to vuOpenWindow except it opens a new
 * graphics window that displays the master of the instance `oldInst'.
 * If `geoSpec' is provided,  the window will be opened automatically
 * at the specified size and location.
 */
{
    return vuCoreWindow(newXWindow, (Window) 0, newFacet, oldInst,
			(struct octBox *) 0, geoSpec);
}


/*ARGSUSED*/
vemStatus vuCopyWindow(newXWindow, prompt, win, ext)
Window *newXWindow;		/* New window (returned)  */
STR prompt;			/* How to prompt user     */
Window win;			/* Window to duplicate    */
struct octBox *ext;		/* Optional extent        */
/*
 * This routine opens a window in a similar fashion to 'vuOpenWindow'
 * except the new window inherits all its attributes from 'win',
 * a previously created window.  This includes the current extent
 * of the window unless 'ext' is non-zero.  Under X11,  `prompt'
 * is ignored.
 */
{
    return vuCoreWindow(newXWindow, win, (octObject *) 0, (octObject *) 0,
			ext, "");
}


/*ARGSUSED*/
vemStatus vuRPCWindow(newXWindow, prompt, newFacet, rpcSpec, geoSpec)
Window *newXWindow;		/* New window (returned)            */
STR prompt;			/* How to prompt user               */
octObject *newFacet;		/* Facet for new window             */
STR rpcSpec;			/* Remote application specification */
STR geoSpec;			/* Optional geometry specification  */
/*
 * This routine opens a window in a similar fashion to `vuOpenWindow'
 * but also starts an RPC application in that window.  The RPC application
 * is specified by `rpcSpec' that should have the form: {host,}path
 * (like the argument to the command rpcAnyApplication() except a
 * comma is used to separate the host and path).  If the rpc specification
 * is missing,  no remote application is started.  Under X11,  `prompt'
 * is ignored.
 */
{
    wnOpts opts;
    vemStatus stat;
    char host[VEMMAXSTR], path[VEMMAXSTR*4], name[VEMMAXSTR*4];
    char *comma, *prune;
    vemPoint spot;
    lsList args;

    stat = vuCoreWindow(newXWindow, (Window) 0, newFacet, (octObject *) 0,
			(struct octBox *) 0, geoSpec);
    if (stat == VEM_OK) {
	if (rpcSpec) {
	    /* Parse remote specification */
	    STRMOVE(host, rpcSpec);
	    if (( comma = strchr(host, ',')) ) {
		STRMOVE(path, comma+1);
		*comma = '\0';
	    } else {
		STRMOVE(host, "localhost");
		STRMOVE(path, rpcSpec);
	    }

	    /* Start remote application */
	    spot.theWin = *newXWindow;
	    spot.x = spot.y = spot.thePoint.x = spot.thePoint.y = 0;
	    spot.theFct = newFacet->objectId;
	    args = lsCreate();
	    (void) RPCExecApplication(host, path, &spot, args, (long) 0);
	    lsDestroy(args, (void (*)()) 0);

	    /* Set up window options */
	    wnGetInfo(*newXWindow, &opts, (octId *) 0, (octId *) 0,
		      (bdTable *) 0, (struct octBox *) 0);
	    opts.disp_options |= VEM_REMOTERUN;
	    wnSetInfo(*newXWindow, &opts, (bdTable *) 0);

	    /* Set up remote application name */
	    if ((prune = strrchr(path, '/')) == NULL) {
		(void) sprintf(name, "%s@%s", path, host);
	    } else {
		(void) sprintf(name, "%s@%s", ++prune, host);
	    }
	    wnSetRPCName(*newXWindow, name);
	}
	return VEM_OK;
    } else {
	return stat;
    }
}

