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
/*LINTLIBRARY*/
/*
 * Title Bar Maintenence
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989
 *
 * This module exports routines for maintaining a title bar in a VEM
 * graphics window.  The title bar displays five pieces of information:
 *   1.  The edit cell name
 *   2.  The context cell name
 *   3.  Status icons
 *   4.  The current cursor location
 *   5.  The difference between the current location and the last point entered
 *
 * Title bars are implemented as an independent window which is `grabbed'
 * by this module and controlled by it.  The width of the title bar is
 * unconstrained.  The height of the bar must be at least large enough
 * to show the text.  This parameter is returned by tbGrab().
 *
 * Title bars can be turned off entirely by unmapping the title bar
 * window (remapping them turns them back on).  Alternatively,  the
 * both of the cursor tracking features can be turned on or off independently.
 * Turning off these features may speed response on displays with long
 * network latency.
 *
 * It is important to note that this module only maintains the output
 * properties of the window.  Event handling must be done outside this
 * module.
 */

#include "titlebar.h"		/* Self declaration        */
#include <X11/Xutil.h>		/* Xlib utilities          */
#include "vemUtil.h"		/* VEM utilities           */
#include "defaults.h"		/* Default handling        */
#include "errtrap.h"		/* Error trapping          */


#include "read_only.bitmap"	/* Bitmap used for read_only icon */
#include "rpc.bitmap"		/* Bitmap used for rpc icon       */
#include "layer_missing.bitmap" /* Bitmap used for missing layers */
#include "interrupt.bitmap"	/* Window has been interrupted    */

char *tb_pkg_name = "titlebar";



#define MAX_TEMP_STR	1024
#define MAX_COORD	128
#define MID_COORD	64
#define ALL_INFO	(TB_EDIT_CELL | TB_CONTEXT_CELL | TB_STAT_ICONS | \
			TB_DIFF_COORDS | TB_ABS_COORDS | TB_RPC_NAME)
/*
 * tbHandle is a hidden pointer to this structure:
 */

#define TB_INACTIVE	0x0100	/* Title bar is inactive  */

typedef struct tb_info {
    Window tb_win;		/* Actual titlebar window   */
    int cur_width;		/* Current tb width         */
    int state_bits;		/* Current tb state (above) */
    int control_bits;		/* Current control bits     */
    int lambda;			/* Oct units per lambda     */
    /* Center editing string */
    STR edit_str;		/* Edited cell string       */
    STR con_str;		/* Context cell string      */
    STR rpc_str;                /* RPC application name     */
    int center_start;		/* Position for edit string */
    int center_length;		/* Size of edit string      */
    /* Coordinate display */
    int diff_len;		/* Difference length        */
    int abs_len;		/* Absolute length          */
    struct octPoint cur_pnt;	/* Current point            */
    struct octPoint diff_pnt;	/* Relative point           */
    struct octPoint diff_dist;	/* Relative distance        */
    /* icon display */
    int on_icons;		/* Which icons are on       */
} tbInfo;

static Display *tbDisp = (Display *) 0;
static XFontStruct *tbFont = (XFontStruct *) 0;
				/* Titlebar display font     */
static XColor *fg_color;	/* Titlebar foreground color */
static XColor *bg_color;	/* Titlebar background color */
static int space_size;		/* Size of space in font     */
static int req_height;		/* Required titlebar height  */

/* Pre-set parameters */
#define TB_PADDING	1

/* Forward declarations */
static void draw_icons();
static void draw_center();
static void new_parameters();
static void update_cen();
static void tbUpdate();

static int big_bitmap();
static int all_size();
static int icon_size();
static int final_str();



void tbGrab(disp, win, lambda, rtn_handle, rtn_height)
Display *disp;			/* Connection to X           */
Window win;			/* Window to control         */
int lambda;			/* Oct units per lambda      */
tbHandle *rtn_handle;		/* Returned handle           */
int *rtn_height;		/* Recommended window height */
/*
 * This routine takes control of an existing window and makes
 * it a title bar window.  The routine returns a handle which
 * should be passed to other routines for modifying the window.
 * Only the routines in this module should be used for modifying
 * the window.  The routine forces the height of the window to
 * be the size required to show the information.  This height is
 * returned in `rtn_height'.
 */
{
    tbInfo *new_info;
    Window root_w;
    int wx, wy;
    unsigned int ww, wh, wbw, wd;

    if (!tbDisp) {
	tbDisp = disp;
    } else if (tbDisp != disp) {
	errRaise(tb_pkg_name, VEM_CORRUPT, "Incompatible displays");
    }
	
    /* Get font and foreground and background colors (if required) */
    if (!tbFont) {
	dfGetFont("title.font", &tbFont);
	dfGetPixel("title.color", &fg_color);
	dfGetPixel("title.backgr", &bg_color);
    }

    new_info = VEMALLOC(tbInfo);
    new_info->tb_win = win;
    new_info->control_bits = TB_EDIT_CELL | TB_CONTEXT_CELL |
      TB_STAT_ICONS | TB_DIFF_COORDS | TB_ABS_COORDS | TB_RPC_NAME;
    new_info->state_bits = 0;
    new_info->lambda = lambda;
    new_info->edit_str = new_info->con_str = new_info->rpc_str = (STR) 0;
    new_info->center_start = new_info->center_length = 0;
    new_info->diff_len = new_info->abs_len = 0;
    new_info->cur_pnt.x = new_info->cur_pnt.y = 0;
    new_info->diff_pnt.x = new_info->diff_pnt.y = 0;
    new_info->diff_dist.x = new_info->diff_dist.y = 0;
    new_info->on_icons = 0;
    space_size = XTextWidth(tbFont, " ", 1);

    if (XGetGeometry(tbDisp, win, &root_w, &wx, &wy, &ww, &wh, &wbw, &wd)) {
	new_info->cur_width = ww;
	req_height = VEMMAX(tbFont->ascent+tbFont->descent, big_bitmap()) +
	  (2 * TB_PADDING);
	if (wh != req_height) {
	    XResizeWindow(tbDisp, win, ww, req_height);
	}
	if (rtn_height) *rtn_height = req_height;
    }

    if (rtn_handle) *rtn_handle = (tbHandle) new_info;
}


static int big_bitmap()
/* Returns the height of the largest bitmap icon used in title bar */
{
    int result;

    result = VEMMININT;
    result = VEMMAX(result, read_only_height);
    result = VEMMAX(result, rpc_height);
    result = VEMMAX(result, layer_missing_height);
    result = VEMMAX(result, interrupt_height);
    return result;
}


void tbRelease(tb)
tbHandle tb;			/* title bar information */
/*
 * Releases control of the titlebar window and frees all resources
 * associated with it.  The window can be freed normally after
 * this function returns.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    if (real_tb->edit_str) VEMFREE(real_tb->edit_str);
    if (real_tb->con_str) VEMFREE(real_tb->con_str);
    if (real_tb->rpc_str) VEMFREE(real_tb->rpc_str);
    VEMFREE(real_tb);
}



void tbRedraw(tb)
tbHandle tb;			/* Window to redraw     */
/*
 * This routine redraws the specified title bar window.  It is expected
 * that this routine will be used to service exposure events on title
 * bar windows.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    real_tb->state_bits &= (~TB_INACTIVE);

    /* Redraw all primitives */
    tbUpdate(real_tb, 0);
    if ((real_tb->control_bits & TB_STAT_ICONS) &&
	!(real_tb->state_bits & TB_STAT_ICONS))
      draw_icons(real_tb);
    if (real_tb->center_length > 0)
      draw_center(real_tb);
}


static GC tbGC(drbl, cx, cy, cmask)
Drawable drbl;			/* Drawable    */
int cx, cy;			/* Clip origin */
Pixmap cmask;			/* Clip mask   */
/*
 * Returns a suitable graphics context with the specified
 * fields set.  Reuses one graphics context.
 */
{
    XGCValues values;
    static GC gc = (GC) 0;

    values.clip_x_origin = cx;
    values.clip_y_origin = cy;
    values.clip_mask = cmask;
    if (gc) {
	XChangeGC(tbDisp, gc, GCClipXOrigin | GCClipYOrigin | GCClipMask,
		  &values);
    } else {
	values.foreground = fg_color->pixel;
	values.background = bg_color->pixel;
	values.font = tbFont->fid;
	gc = XCreateGC(tbDisp, drbl,
		       GCForeground | GCBackground | GCFont |
		       GCClipXOrigin | GCClipYOrigin | GCClipMask,
		       &values);
    }
    return gc;
}

/*
 * Bitmap generators
 */

static Pixmap read_only_map(drbl)
Drawable drbl;
{
    static Pixmap map = (Pixmap) 0;

    if (!map) {
	map = XCreateBitmapFromData(tbDisp, drbl, read_only_bits,
				    read_only_width, read_only_height);
    }
    return map;
}

static Pixmap rpc_map(drbl)
Drawable drbl;
{
    static Pixmap map = (Pixmap) 0;

    if (!map) {
	map = XCreateBitmapFromData(tbDisp, drbl, rpc_bits,
				    rpc_width, rpc_height);
    }
    return map;
}

static Pixmap layer_missing_map(drbl)
Drawable drbl;
{
    static Pixmap map = (Pixmap) 0;

    if (!map) {
	map = XCreateBitmapFromData(tbDisp, drbl, layer_missing_bits,
				    layer_missing_width, layer_missing_height);
    }
    return map;
}
    
static Pixmap interrupt_map(drbl)
Drawable drbl;
{
    static Pixmap map = (Pixmap) 0;

    if (!map) {
	map = XCreateBitmapFromData(tbDisp, drbl, interrupt_bits,
				    interrupt_width, interrupt_height);
    }
    return map;
}


static void draw_icons(real_tb)
tbInfo *real_tb;		/* Real titlebar information */
/*
 * Draws all of the icons that are currently turned on.
 */
{
    int start_pos;
    int height;

    if (real_tb->center_length > 0) {
	start_pos = real_tb->center_start + real_tb->center_length + TB_PADDING;
    } else {
	start_pos = (real_tb->cur_width - icon_size(real_tb))/2;
    }
    /* Begin drawing */
    if (real_tb->on_icons & TB_READ_ONLY) {
	height = (req_height - read_only_height)/2;
	XFillRectangle(tbDisp, real_tb->tb_win,
		       tbGC(real_tb->tb_win, start_pos, height,
			    read_only_map(real_tb->tb_win)),
		       start_pos, height,
		       read_only_width, read_only_height);
	start_pos += read_only_width + TB_PADDING;
    }
    if (real_tb->on_icons & TB_RPC_APPL) {
	height = (req_height - rpc_height)/2;
	XFillRectangle(tbDisp, real_tb->tb_win,
		       tbGC(real_tb->tb_win, start_pos, height,
			    rpc_map(real_tb->tb_win)),
		       start_pos, height,
		       rpc_width, rpc_height);
	start_pos += rpc_width + TB_PADDING;
    }
    if (real_tb->on_icons & TB_MIS_LAYERS) {
	height = (req_height - layer_missing_height)/2;
	XFillRectangle(tbDisp, real_tb->tb_win,
		       tbGC(real_tb->tb_win, start_pos, height,
			    layer_missing_map(real_tb->tb_win)),
		       start_pos, height,
		       layer_missing_width, layer_missing_height);
	start_pos += layer_missing_width + TB_PADDING;
    }
    if (real_tb->on_icons & TB_INTERRUPT) {
	height = (req_height - interrupt_height)/2;
	XFillRectangle(tbDisp, real_tb->tb_win,
		       tbGC(real_tb->tb_win, start_pos, height,
			    interrupt_map(real_tb->tb_win)),
		       start_pos, height,
		       interrupt_width, interrupt_height);
	start_pos += interrupt_width + TB_PADDING;
    }
    /* More can be added */
}


static void draw_center(real_tb)
tbInfo *real_tb;		/* Real titlebar information */
/*
 * Draws the center string in the titlebar.
 */
{
    int len;
    char outstr[MAX_TEMP_STR];

    len = final_str(real_tb, outstr);
    if (len > 0) {
	XDrawString(tbDisp, real_tb->tb_win, tbGC(real_tb->tb_win, 0, 0, None),
		    real_tb->center_start,
		    (req_height + tbFont->ascent + tbFont->descent)/2,
		    outstr, len);
    }
}




/*ARGSUSED*/
void tbResize(tb, w, h)
tbHandle tb;			/* Window to resize */
int w, h;			/* New window size  */
/*
 * This routine allows the caller to change the width
 * of a title bar window. The height of a title bar window
 * is fixed by tbGrab.  As the window gets smaller,  certain
 * information may be omitted so that more important information
 * is visible.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    real_tb->state_bits &= (~TB_INACTIVE);
    real_tb->cur_width = w;
    XResizeWindow(tbDisp, real_tb->tb_win, w, req_height);

    /* Turn off all space too small flags */
    real_tb->state_bits &= ~(ALL_INFO);

    /* Recomputes parameters here */
    new_parameters(real_tb);

    tbRedraw(tb);
}


static void new_parameters(real_tb)
tbInfo *real_tb;		/* Real titlebar information */
/*
 * This routine determines what components can be displayed
 * in the titlebar `real_tb'.  It will incrementally turn
 * off components until the information fits.  Those turned
 * off are indicated by a bit turned on the state_bits field
 * of `real_tb'.
 */
{
    int cen_len, icon_len, diff_len, abs_len, tot_size;

    /* Find all of the lengths */
    update_cen(real_tb);
    cen_len = real_tb->center_length;

    icon_len = icon_size(real_tb);

    if ((real_tb->control_bits & TB_DIFF_COORDS) &&
	!(real_tb->state_bits & TB_DIFF_COORDS))
      {
	  diff_len = real_tb->diff_len;
      } else {
	  diff_len = 0;
      }
    if ((real_tb->control_bits & TB_ABS_COORDS) &&
	!(real_tb->state_bits & TB_ABS_COORDS))
      {
	  abs_len = real_tb->abs_len;
      } else {
	  abs_len = 0;
      }

    /* Now compute total size */
    tot_size = all_size(cen_len, icon_len, diff_len, abs_len);

    /* Now see if this is big enough */
    if (real_tb->cur_width < tot_size) {
	/* Not enough room - try to turn off some stuff */
	if (real_tb->state_bits & TB_ABS_COORDS) {
	    if (real_tb->state_bits & TB_DIFF_COORDS) {
		if (real_tb->state_bits & TB_RPC_NAME) {
		    if (real_tb->state_bits & TB_STAT_ICONS) {
			if (real_tb->state_bits & TB_CONTEXT_CELL) {
			    /* Nothing left to turn off */
			} else {
			    real_tb->state_bits |= TB_CONTEXT_CELL;
			    new_parameters(real_tb);
			}
		    } else {
			real_tb->state_bits |= TB_STAT_ICONS;
			new_parameters(real_tb);
		    }
		} else {
		    real_tb->state_bits |= TB_RPC_NAME;
		    new_parameters(real_tb);
		}
	    } else {
		real_tb->state_bits |= TB_DIFF_COORDS;
		new_parameters(real_tb);
	    }
	} else {
	    real_tb->state_bits |= TB_ABS_COORDS;
	    new_parameters(real_tb);
	}
    }
}



static int all_size(cl, il, dl, al)
int cl, il, dl, al;		/* Lengths of components */
/*
 * Computes the overall length needed to display all of the
 * titlebar components in the most compact form.  Note that
 * the center string is always displayed in the center.
 */
{
    int left_amount, right_amount;

    left_amount = dl;
    right_amount = il + al;
    return 2 * ((cl/2 + 1) + VEMMAX(left_amount, right_amount));
}

static int icon_size(real_tb)
tbInfo *real_tb;		/* Real titlebar information */
/*
 * This routine computes the width required to display
 * all of the icons currently turned on.  It will return
 * zero if icons are turned off.  It uses brute force
 * methods to determine this size.
 */
{
    int ret_length = 0;

    if ((real_tb->control_bits & TB_STAT_ICONS) &&
	!(real_tb->state_bits & TB_STAT_ICONS))
      {
	  if (real_tb->on_icons & TB_READ_ONLY) {
	      ret_length += read_only_width;
	  }
	  if (real_tb->on_icons & TB_RPC_APPL) {
	      ret_length += rpc_width + (ret_length ? TB_PADDING : 0);
	  }
	  if (real_tb->on_icons & TB_MIS_LAYERS) {
	      ret_length += layer_missing_width + (ret_length ? TB_PADDING : 0);
	  }
	  if (real_tb->on_icons & TB_INTERRUPT) {
	      ret_length += interrupt_width + (ret_length ? TB_PADDING : 0);
	  }
      }
    return ret_length;
}


void tbDeactivate(tb)
tbHandle tb;			/* Title bar to deactivate */
/*
 * This routine marks the specified title bar as inactive.  No display
 * updates to the title bar are done while it is marked as inactive.
 * It is expected this routine will be called when a title bar is
 * unmapped.  A title bar is made active again with the first call
 * to tbRedraw() or tbResize().
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    real_tb->state_bits |= TB_INACTIVE;
}



void tbControl(tb, on_bits, off_bits, tog_bits, rtn_bits)
tbHandle tb;			/* Title bar handle  */
int on_bits;			/* Those to turn on  */
int off_bits;			/* Those to turn off */
int tog_bits;			/* Those to toggle   */
int *rtn_bits;			/* Current state     */
/*
 * This routine allows the caller to change the amount of
 * information currently displayed in the title bar.
 * Each piece of information is identified by a unique bit.
 * Pieces of information in `on_bits' are turned on,  those
 * in `off_bits' are turned off,  and those in `tog_bits'
 * are toggled.  The state after the change is returned
 * in `rtn_bits'.  All of the bits are described in titlebar.h.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    /* Make sure bits are ok */
    if (on_bits & ~ALL_INFO)
      errRaise(tb_pkg_name, VEM_BADARG, "in tbControl: bad on_bits argument");
    if (off_bits & ~ALL_INFO)
      errRaise(tb_pkg_name, VEM_BADARG, "in tbControl: bad off_bits argument");
    if (tog_bits & ~ALL_INFO)
      errRaise(tb_pkg_name, VEM_BADARG, "in tbControl: bad tog_bits argument");

    /* Set them as appropriate */
    real_tb->control_bits |= on_bits;
    real_tb->control_bits &= (~off_bits);
    real_tb->control_bits ^= tog_bits;
    if (rtn_bits) *rtn_bits = real_tb->control_bits;

    XClearWindow(tbDisp, real_tb->tb_win);
    update_cen(real_tb);
    tbRedraw(tb);
}



/*
 * Display updates
 *
 * The following routines update the information displayed in a title
 * bar.  These changes are reflected in the title bar immediately (unless
 * the title bar is inactive).
 */

void tbEditCell(tb, cell)
tbHandle tb;			/* Title bar to change */
octObject *cell;		/* Cell to display     */
/*
 * This routine changes the current edit cell displayed in
 * a title bar.  `cell' should be a fully filled in OCT_FACET
 * object.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    if (real_tb->edit_str) {
	VEMFREE(real_tb->edit_str);
	real_tb->edit_str = (STR) 0;
    }
    real_tb->edit_str = vuCellName(cell);
    new_parameters(real_tb);
    XClearWindow(tbDisp, real_tb->tb_win);
    tbRedraw(tb);
}



void tbContextCell(tb, cell)
tbHandle tb;			/* Title bar to change */
octObject *cell;		/* Context cell spec   */
/*
 * This routine changes the context cell name displayed in
 * a title bar.  No context cell name is displayed until
 * a call to this routine is made.  It is expected this
 * will be done when edit in context is specified.  When
 * edit in context is complete,  the caller should specify
 * (octObject *) 0 for `cell'.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    if (real_tb->con_str) {
	VEMFREE(real_tb->con_str);
	real_tb->con_str = (STR) 0;
    }
    if (cell) {
	real_tb->con_str = vuCellName(cell);
    }
    new_parameters(real_tb);
    XClearWindow(tbDisp, real_tb->tb_win);
    tbRedraw(tb);
}



void tbRPCName(tb, name)
tbHandle tb;			/* Title bar to change */
STR name;                       /* Name of the RPC application */
/*
 * This routine changes the RPC application name displayed in
 * a title bar.  No RPC application name is displayed until
 * a call to this routine is made.   Call with name '(STR) 0'
 * to turn off the name.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    if (real_tb->rpc_str) {
	VEMFREE(real_tb->rpc_str);
	real_tb->rpc_str = (STR) 0;
    }
    if (name) {
	real_tb->rpc_str = VEMSTRCOPY(name);
    }
    new_parameters(real_tb);
    XClearWindow(tbDisp, real_tb->tb_win);
    tbRedraw(tb);
}


STR tbGetRPCName(tb)
tbHandle tb;			/* Title bar to change */
/*
 * This routine gets the RPC application name displayed in
 * a title bar.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    return(real_tb->rpc_str);
}



/*
 * Edit and Context Cell Helper Routines
 */

#define EDITING_LEN	9

static void update_cen(real_tb)
tbInfo *real_tb;		/* Real titlebar info */
/*
 * This routine recomputes the actual size and placement of
 * the center editing string.  The information is placed
 * in the appropriate slots of `real_tb'.
 */
{
    char outstr[MAX_TEMP_STR];

    /* First its length */
    (void) final_str(real_tb, outstr);
    real_tb->center_length = XTextWidth(tbFont, outstr, strlen(outstr));

    /* Now it's position */
    real_tb->center_start = (real_tb->cur_width - real_tb->center_length) / 2;
}

static int final_str(real_tb, outstr)
tbInfo *real_tb;		/* Real titlebar info */
STR outstr;			/* Output string      */
/*
 * Constructs the final string according to current specifications
 * in `real_tb'.  The routine returns the actual number of characters
 * it places in `outstr'.
 */
{
    outstr[0] = '\0';
    if (real_tb->con_str) {
	STRCONCAT(outstr, real_tb->con_str);
	/* There is a context cell name - is edit cell on? */
	if ((real_tb->control_bits & TB_EDIT_CELL) &&
	    !(real_tb->state_bits & TB_EDIT_CELL))
	  {
	      /* Yes - full form */
	      STRCONCAT(outstr, " EDITING ");
	      STRCONCAT(outstr, real_tb->edit_str);
	  }
    } else {
	/* Only edit cell (if on) */
	if ((real_tb->control_bits & TB_EDIT_CELL) &&
	    !(real_tb->state_bits & TB_EDIT_CELL))
	  {
	      /* Display only the edit cell */
	      STRCONCAT(outstr, real_tb->edit_str);
	  }
    }
    
    /* RPC Application */
    if (real_tb->rpc_str) {
	if ((real_tb->control_bits & TB_RPC_NAME) &&
	    !(real_tb->state_bits & TB_RPC_NAME))
	  {
	      /* Display the RPC application name */
	      STRCONCAT(outstr, " (");
	      STRCONCAT(outstr, real_tb->rpc_str);
	      STRCONCAT(outstr, ")");
	  }
    }
    
    return STRLEN(outstr);
}


void tbPoint(tb, pnt, a_flag)
tbHandle tb;			/* Title bar to update */
struct octPoint *pnt;		/* New coordinates     */
int a_flag;			/* Set if active       */
/*
 * This routine is the external interface to tbUpdate().  It
 * updates the coordinates displayed in titlebar `tb'.  The
 * routine checks to make sure the coordinate has changed
 * and calls tbUpdate().  See tbUpdate() for details.  If
 * `a_flag' is set,  the difference coordinates will also
 * be updated.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    if ((pnt->x != real_tb->cur_pnt.x) || (pnt->y != real_tb->cur_pnt.y)) {
	real_tb->cur_pnt = *pnt;
	tbUpdate(real_tb, a_flag);
    }
}



/*
 * Efficiency macros for tbUpdate
 */

#define DO_VAL(val) \
    if (val == 0) {    	       	       	  \
	*(spot--) = '0';		  \
    } else {				  \
	if (val < 0) {			  \
	    neg_flag = 1;		  \
	    val = -val;			  \
	} else {			  \
	    neg_flag = 0;		  \
	}				  \
	while (val > 0) {		  \
	    *(spot--) = '0' + (val % 10); \
	    val /= 10;			  \
	}				  \
	if (neg_flag) {			  \
	    *(spot--) = '-';		  \
	}				  \
    }

#define DO_LEN(s) \
    len = XTextWidth(tbFont, (s), num_chars)

#define DO_SPACES(place, dir) \
    while (len <= orig_len) { \
       *place = ' ';	      \
       len += space_size;     \
       place += dir;	      \
       final_chars++;         \
    }


static void tbUpdate(real_tb, a_flag)
tbInfo *real_tb;		/* Real titlebar info   */
int a_flag;			/* If set, update diff  */
/*
 * This routine updates the current difference and absolute
 * coordinates displayed in the title bar.  It should be
 * called whenever the mouse moves in the appropriate window.
 * The difference coordinate is computed with respect to
 * the last coordinate specified to `tbSpot'.  
 */
{
    register char *spot;
    register int x, y, len;
    int neg_flag, num_chars, final_chars, orig_len;
    char *orig;
    char the_str[MAX_COORD];

    x = real_tb->cur_pnt.x / real_tb->lambda;
    y = real_tb->cur_pnt.y / real_tb->lambda;
    /* See if we do the absolute coordinate */
    if (!(real_tb->state_bits & TB_INACTIVE) &&	/* title bar active */
	!(real_tb->state_bits & TB_ABS_COORDS) && /* not too small  */
	(real_tb->control_bits & TB_ABS_COORDS)) /* its turned on */
      {
	  orig = spot = &(the_str[MID_COORD]);
	  *(spot--) = ')';
	  DO_VAL(y);
	  *(spot--) = ' ';
	  *(spot--) = ',';
	  DO_VAL(x);
	  *(spot--) = '(';
	  /* Find size of result */
	  num_chars = orig - spot;
	  DO_LEN(spot+1);
	  /* Add enough spaces to overwrite old string */
	  orig_len = real_tb->diff_len;
	  real_tb->diff_len = len;
	  final_chars = num_chars;
	  /* Ready to write absolute string */
	  DO_SPACES(spot, -1);
	  XDrawImageString(tbDisp, real_tb->tb_win,
			   tbGC(real_tb->tb_win, 0, 0, None),
			   real_tb->cur_width - TB_PADDING - len,
			   (req_height + tbFont->ascent+ tbFont->descent)/2,
			   spot+1, final_chars);
      }
    /* Figure out relative display numbers */
    if (a_flag) {
	real_tb->diff_dist.x = (real_tb->cur_pnt.x - real_tb->diff_pnt.x)
	  / real_tb->lambda;
	real_tb->diff_dist.y = (real_tb->cur_pnt.y - real_tb->diff_pnt.y)
	  / real_tb->lambda;
    }
    x = real_tb->diff_dist.x;
    y = real_tb->diff_dist.y;
    /* See if we display the difference coordinates */
    if (!(real_tb->state_bits & TB_INACTIVE) &&	/* title bar active */
	!(real_tb->state_bits & TB_DIFF_COORDS) && /* not too small  */
	(real_tb->control_bits & TB_DIFF_COORDS)) /* its turned on */
      {
	  orig = spot = &(the_str[MID_COORD]);
	  *(spot--) = ']';
	  DO_VAL(y);
	  *(spot--) = ' ';
	  *(spot--) = ',';
	  DO_VAL(x);
	  *(spot--) = '[';
	  /* Find size of result */
	  num_chars = orig - spot;
	  DO_LEN(spot+1);
	  /* Add enough spaces to overwrite old string */
	  orig_len = real_tb->abs_len;
	  real_tb->abs_len = len;
	  final_chars = num_chars;
	  orig = spot+1;
	  spot = &(the_str[MID_COORD+1]);
	  DO_SPACES(spot, 1);
	  /* Ready to write absolute string */
	  XDrawImageString(tbDisp, real_tb->tb_win,
			   tbGC(real_tb->tb_win, 0, 0, None), TB_PADDING,
			   (req_height + tbFont->ascent + tbFont->descent)/2,
			   orig, final_chars);

      }
}



void tbSpot(tb, pnt)
tbHandle tb;			/* Title bar         */
struct octPoint *pnt;		/* New relative spot */
/*
 * This routine sets the point for all subsequent difference
 * coordinate displays.  Initially,  this is set to (0,0).
 * The caller should call this routine when the user enters
 * some point in the appropriate window.  This also causes
 * an update to the currently displayed coordinates.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    /* Set the relative point */
    real_tb->diff_pnt = *pnt;
    tbUpdate(real_tb, 1);
}

void tbLambda(tb, lambda)
tbHandle tb;			/* Title bar handle */
int lambda;			/* New lambda value */
/*
 * Resets the number of oct units per lambda for the coordinate
 * displays.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;

    real_tb->lambda = lambda;
    tbUpdate(real_tb, 0);
}



void tbBits(tb, on_bits, off_bits, tog_bits, rtn_bits)
tbHandle tb;			/* Title bar             */
int on_bits;			/* Icons to turn on      */
int off_bits;			/* Icons to turn off     */
int tog_bits;			/* Icons to toggle       */
int *rtn_bits;			/* Current state         */
/*
 * This routine turns on or off status bits for a title bar.
 * Those specified in `on_bits' are turned on,  those in
 * `off_bits' are turned off,  and those in `tog_bits' are
 * toggled.  The resulting state is returned in `rtn_bits'.
 */
{
    tbInfo *real_tb = (tbInfo *) tb;
    int all_mask;

    /* Check to make sure bits are valid */
    all_mask = TB_READ_ONLY | TB_RPC_APPL | TB_MIS_LAYERS | TB_INTERRUPT;
    if ((on_bits & ~all_mask)||(off_bits & ~all_mask)||(tog_bits & ~all_mask))
      errRaise(tb_pkg_name, VEM_BADARG, "in tbBits: bad mask argument");

    /* Change icon mask */
    real_tb->on_icons |= on_bits;
    real_tb->on_icons &= ~off_bits;
    real_tb->on_icons ^= tog_bits;
    if (rtn_bits) *rtn_bits = real_tb->on_icons;

    /* Redraw to effect changes */
    XClearWindow(tbDisp, real_tb->tb_win);
    tbRedraw(tb);
}
