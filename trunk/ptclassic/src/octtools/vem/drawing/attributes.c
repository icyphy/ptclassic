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
 * VEM Attribute Manager
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1988
 *
 * This module maintains all display attributes associated with
 * drawing items on in VEM Oct display windows.  These attributes 
 * include color, fill patterns,  line styles,  and brush sizes.
 */

#include "attributes.h"		/* Self-declaration          */
#include <X11/Xutil.h>		/* X11 utility functions     */
#include "list.h"		/* List management           */
#include "message.h"		/* Message output            */
#include "vemUtil.h"		/* General utilities         */
#include "xvals.h"		/* Sets critical constants   */
#include "defaults.h"		/* For critical values       */

char *atr_pkg_name = "atr";

#define MAXINTENSITY	65535

/*
 * The global context used for all display primitives is exported
 * to the world in this module.  AGC stands for Attribute Graphics Context.
 */

atrContext _AGC;

/*
 * Connection information - available through atrQuery().
 */
static Visual *atr_vis = (Visual *) 0;	/* Selected visual             */
static Colormap atr_cmap;	/* Selected colormap           */
static int atr_depth;		/* Depth of selected visual    */
static Drawable atr_drawable;	/* Drawable used in some cases */
static XFontStruct *atr_font;	/* Font selected by atrFont()  */
static XRectangle atr_clip;	/* Current clipping rectangle  */

/* Forward declarations */

static int full_alloc();
static void find_visual();
static int vis_eval();
static void set_colors();
static void do_fill();
static void do_pat();
static Pixmap get_bitmap();
static void rel_color();
static void rel_fill();
static void rel_pat();
static void changeColor();
static int norm_resolve();
static int repl_resolve();
static int sel_resolve();
static int hi_resolve();
static int low_resolve();


/*
 * Internal attribute structure
 *
 * This structure stores all of the information found in the
 * exported structure atrContext in a form suitable for
 * regeneration on demand.
 *
 * The color is represented as either a direct pixel value
 * or an offset into a table of pixel values maintained
 * by this module (see below for details).
 *
 * Some devices have severe limitations on Pixmap storage
 * space.  This module maintains a LRU cache of pixmaps
 * to attempt to get around this problem.
 */

typedef struct atr_bitmap {
    flPattern BM_pattern;	/* Regenerable form of bitmap         */
    Pixmap BM;			/* Depth one pixmap (0 if not active) */
    lsHandle BM_spot;		/* Location in list of bitmaps        */
    lsHandle BM_actspot;	/* Location in active bitmap list     */
    int ref_count;		/* Reference count for deallocation   */
} atrBitmap;

typedef struct atr_line_style {
    int d_offset;		/* Phase of pattern                  */
    int d_length;		/* Length of pattern (in characters) */
    char *d_list;		/* Actual dash list pattern          */
} atrLineStyle;

#define ATR_SELECT	0x01	/* Attribute is on selection plane */
#define ATR_PLANE	0x02	/* Pixel value is from its plane   */
#define ATR_SOLIDFILL	0x04	/* Always solid fill               */
#define ATR_SOLIDLINE	0x08	/* Always draw solid lines         */

typedef struct atr_internal {
    int atrFlags;		/* One of the bit values given above   */
    unsigned long pix_or_offset; /* Either the pixel value or the offset */
    unsigned long pl_mask;	/* Plane mask for drawing              */
    int disp_func;		/* Display function                    */
    atrBitmap *fill_style;	/* Bitmap used for fill (zero if none) */
    atrLineStyle *l_style;	/* Line style for outline (zero if none) */
    int l_width;		/* Brush size (normally used for outline) */
} atrInternal;


/*
 * Internal data structures for resource allocation.
 *
 * If possible,  this module attempts to allocate a certain number
 * of colors and planes based on defaults read at start-up time.
 * If successful,  all color allocation is done from this pool
 * of colors and planes.  If it can't allocate this pool,  it tries
 * to allocate a new color table and tries again.  If the color
 * table allocation fails,  or the system has a read-only color
 * table,  it falls back to allocating colors using XAllocColor.
 * If resources become exhausted using XAllocColor,  the package
 * gives up.  Its up to the caller to recover at that point.
 */

#define IS_BW		1	/* Display is black and white */
#define IS_LOWCOL	2	/* Low capability color       */
#define IS_HICOL	3	/* Allocate from local table  */

static int col_state;		/* See bits defined above        */
static int max_planes;		/* Successfully allocated planes */
static int max_colors;		/* Maximum colors in color table */

/* Main array of pixel values returned from XGetColorCells */
static XColor *attr_pixvalues = (XColor *) 0;

/* Parallel reference count array for pixel values */
static int *attr_pixcount = (int *) 0;

/* 
 * Array containing allocated selection values where the pixel value
 * is actually the bit indicating the plane.  No reference counting
 * is done here.  Unallocated selection values are indicated
 * by the bit SEL_UNALLOC.
 */
#define SEL_BITSHIFT	30
#define SEL_UNALLOC	(1 << SEL_BITSHIFT)
static unsigned long *attr_planes = (unsigned long *) 0;
static unsigned long attr_allmasks;

/* Resource list of bitmaps - allocated by atrInit */
static int max_bitmaps;		/* Maximum allocated bitmaps       */
static lsList BMResources;	/* Containts pointers to atrBitmap */
static lsList BMActive;		/* List of active bitmaps          */


int atrInit(disp, planes, colors)
Display *disp;			/* Display connection           */
unsigned int planes, colors;	/* Initial color allocation     */
/*
 * This routine initializes the attributes package by quering the
 * underlying device for critical display information.  This
 * routine should be called once after establishing a connection
 * to the X server.  Uses errtrap to signal error conditions.
 * Returns a non-zero value if the display has color capabilities.
 */
{
    XSetWindowAttributes wattr;
    XVisualInfo info;

    /* Not sure this will work on B/W displays */
    _AGC.disp = disp;
    atr_vis = DefaultVisual(disp, DefaultScreen(disp));
    atr_cmap = DefaultColormap(disp, DefaultScreen(disp));
    atr_depth = DefaultDepth(disp, DefaultScreen(disp));
    if (full_alloc(disp, atr_cmap, planes, colors)) {
	vemMsg(MSG_L, "Full color in default map: %d planes, %d colors.\n",
	       max_planes, max_colors);
	col_state = IS_HICOL;
    } else {
	/* Attempt to find another suitable visual */
	find_visual(disp, &info);
	atr_vis = info.visual;
	atr_cmap = XCreateColormap(disp,
				   RootWindow(disp, info.screen),
				   info.visual, AllocNone);
	atr_depth = info.depth;
	/* Try to allocate in new colormap */
	if (full_alloc(disp, atr_cmap, planes, colors)) {
	    vemMsg(MSG_L, "Full color in alternate map: %d planes, %d colors.\n",
		   max_planes, max_colors);
	    col_state = IS_HICOL;
	} else {
	    /* Destroy alternate colormap */
	    XFreeColormap(disp, atr_cmap);
	    atr_vis = DefaultVisual(disp, DefaultScreen(disp));
	    atr_cmap = DefaultColormap(disp, DefaultScreen(disp));
	    atr_depth = DefaultDepth(disp, DefaultScreen(disp));
	    /* See if we can use some color */
	    if (DefaultDepth(disp, DefaultScreen(disp)) > 1) {
		vemMsg(MSG_L, "Partial color in default map: using XAllocColor.\n");
		col_state = IS_LOWCOL;
	    } else {
		/* No color capability */
		vemMsg(MSG_L, "No color: using black and white.\n");
		col_state = IS_BW;
	    }
	}
    }
    /* Set these constants */
    xv_set_vis(atr_vis);
    xv_set_cmap(atr_cmap);
    xv_set_depth(atr_depth);

    /* Allocate graphics contexts */
    wattr.colormap = atr_cmap;
#ifdef OLD
    atr_drawable = XCreateWindow(_AGC.disp, DefaultRootWindow(disp),
				 0, 0, 10, 10, 0, atr_depth,
				 CopyFromParent, atr_vis,
				 CWColormap, &wattr);
#else
    atr_drawable = XCreatePixmap(_AGC.disp, DefaultRootWindow(disp),
				 10, 10, atr_depth);
#endif
    xv_set_drw(atr_drawable);
    _AGC.solid_gc = XCreateGC(_AGC.disp, atr_drawable, 0, (XGCValues *) 0);
    _AGC.line_gc = XCreateGC(_AGC.disp, atr_drawable, 0, (XGCValues *) 0);
    _AGC.stipple_gc = XCreateGC(_AGC.disp, atr_drawable, 0, (XGCValues *) 0);
    _AGC.clear_gc = XCreateGC(_AGC.disp, atr_drawable, 0, (XGCValues *) 0);

    _AGC.fill_thres = ATR_DONT;
    _AGC.line_thres = ATR_DONT;

    atr_font = (XFontStruct *) 0;
    atr_clip.width = atr_clip.height = 0;

    /* Allocate active Pixmap resource lists */
    max_bitmaps = VEMMAXINT;
    BMResources = lsCreate();
    BMActive = lsCreate();
    atrSet((atrHandle) 0);
    return col_state != IS_BW;
}


static int full_alloc(disp, cmap, planes, colors)
Display *disp;			/* X Connection            */
Colormap cmap;			/* Colormap for allocation */
unsigned int planes;		/* Number of planes        */
unsigned int colors;		/* Number of colors        */
/*
 * Attempts to allocate `planes' planes and `colors' colors in
 * colormap `cmap'.  If successful,  it sets up the pixels
 * appropriately and returns a non-zero status.  If not,
 * it returns zero.  Places plane masks in global `attr_planes'.
 * All masks are plane masks are stored in `attr_allmasks'.
 * If successful,  sets globals max_planes and max_colors.
 */
{
    unsigned long *r_masks;
    unsigned long *r_pixels;

    r_masks = VEMARRAYALLOC(unsigned long, planes);
    r_pixels = VEMARRAYALLOC(unsigned long, colors);
    if (XAllocColorCells(disp, cmap, False, r_masks, planes, r_pixels, colors)) {
	max_planes = planes;
	max_colors = colors;
	set_colors(r_masks, r_pixels);
	VEMFREE(r_masks);
	VEMFREE(r_pixels);
	return 1;
    } else {
	max_planes = 0;
	max_colors = 0;
	VEMFREE(r_masks);
	VEMFREE(r_pixels);
	return 0;
    }
}



static void find_visual(disp, info)
Display *disp;			/* X Connection    */
XVisualInfo *info;		/* Returned visual */
/*
 * Finds an appropriate alternate visual that (hopefully) supports
 * color better than the default visual.  Examines all visuals
 * returned by XGetVisualInfo().  The one with the highest goodness
 * index is selected.
 */
{
    XVisualInfo *vlist;
    int num_vis, idx, max_cost, cost;
    XVisualInfo *chosen;

    vlist = XGetVisualInfo(disp, VisualNoMask, (XVisualInfo *) 0, &num_vis);
    if (!vlist || !num_vis)
      errRaise(atr_pkg_name, VEM_CORRUPT, "in find_visual: no visuals");

    max_cost = vis_eval(&(vlist[0]));
    chosen = vlist;
    for (idx = 1;  idx < num_vis;  idx++) {
	if ((cost = vis_eval(&(vlist[idx]))) > max_cost) {
	    max_cost = cost;
	    chosen = &(vlist[idx]);
	}
    }
    *info = *chosen;
    XFree((char *) vlist);
}

static int vis_eval(vis)
XVisualInfo *vis;		/* Visual to examine */
/*
 * Returns a desirability index for the passed visual.
 * This functions preference list is:
 *   PsuedoColor
 *   DirectColor
 *   StaticColor
 *   TrueColor
 *   GrayScale
 *   StaticGray
 */
{
    switch (vis->class) {
    case PseudoColor:
	return vis->colormap_size * 100;
    case DirectColor:
	return vis->depth * 1000;
    case StaticColor:
	return vis->colormap_size * 50;
    case TrueColor:
	return vis->depth * 500;
    case GrayScale:
	return vis->colormap_size * 25;
    case StaticGray:
	return vis->depth * 250;
    default:
	errRaise(atr_pkg_name, VEM_CORRUPT,
		 "in vis_eval: unknown visual type: %d\n", vis->class);
    }
    return 0;
}



static void set_colors(masks, pixels)
unsigned long *masks;		/* Array of plane masks  */
unsigned long *pixels;		/* Array of pixel values */
/*
 * This routine fills the static color allocation tables with
 * the appropriate initial values.  See the routine col_resolve
 * for details about the operation of this scheme.
 */
{
    int i;

    /*
     * Allocate and fill normal and reference count arrays
     */
    attr_pixvalues = VEMARRAYALLOC(XColor, max_colors);
    attr_pixcount = VEMARRAYALLOC(int, max_colors);
    for (i = 0;  i < max_colors;  i++) {
	attr_pixvalues[i].pixel = pixels[i];
	attr_pixcount[i] = 0;
    }
    /*
     * The new XAllocColorCells returns an array of masks.
     * We simply copy this array.
     */
    attr_planes = VEMARRAYALLOC(unsigned long, max_planes);
    attr_allmasks = 0;
    for (i = 0;  i < max_planes;  i++) {
	attr_planes[i] = masks[i] | SEL_UNALLOC;
	attr_allmasks |= masks[i];
    }
}



vemStatus atrNew(newattr, col, fill, lstyle, l_width)
atrHandle *newattr;		/* Returned attribute handle */
XColor *col;			/* Pixel value for color     */
flPattern fill;			/* Fill pattern to use       */
flPattern lstyle;		/* Line style pattern        */
int l_width;			/* Brush size for outlines   */
/*
 * This routine creates attributes assuming the programmer has
 * knowledge of the X parameters he wishes to use for the display.
 * For example,  this can be used to create attributes derived
 * from values returned by the defaults package.  This routine
 * may return VEM_RESOURCE if it can't allocate the appropriate
 * resources (only possibility - color allocation failure).
 */
{
    atrInternal *new_internal;

    new_internal = VEMALLOC(atrInternal);
    new_internal->atrFlags = 0;
    if (!norm_resolve(col, &(new_internal->pix_or_offset)))
      return VEM_RESOURCE;

    new_internal->pl_mask = ((col_state == IS_HICOL) ?
			     AllPlanes ^ attr_allmasks :
			     AllPlanes);

    new_internal->disp_func = GXcopy; 
    if (fill) {
	if (flIsSolid(fill) == VEM_OK) {
	    /* Solid fill all the time */
	    new_internal->atrFlags |= ATR_SOLIDFILL;
	    new_internal->fill_style = (atrBitmap *) 0;
	} else {
	    /* Stipple fill */
	    do_fill(fill, &(new_internal->fill_style));
	}
    } else {
	/* Not filled */
	new_internal->fill_style = (atrBitmap *) 0;
    }

    if (lstyle) {
	if (flIsSolid(lstyle) == VEM_OK) {
	    /* Solid lines all the time */
	    new_internal->atrFlags |= ATR_SOLIDLINE;
	    new_internal->l_style = (atrLineStyle *) 0;
	} else {
	    /* Patterned line */
	    do_pat(lstyle, &(new_internal->l_style));
	}
    } else {
	/* No outline */
	new_internal->l_style = (atrLineStyle *) 0;
    }

    new_internal->l_width = l_width;

    *newattr = (atrHandle) new_internal;
    return VEM_OK;
}



vemStatus atrReplace(oldattr, col, fill, l_pat, l_width)
atrHandle oldattr;		/* Attribute to modify       */
XColor *col;			/* Pixel value for color     */
flPattern fill;			/* Fill pattern to use       */
flPattern l_pat;		/* Line style pattern        */
int l_width;			/* Brush size for outlines   */
/*
 * This routine replaces the display attributes of `oldattr'
 * with those specified in this call.  Anyone else having
 * a handle to this attribute see the change the next time
 * the attribute is referenced.  In the case of devices
 * rich in color,  the change may be reflected immediately
 * on the screen (but this is NOT guaranteed).  The routine
 * may return VEM_RESOURCE if there was a resource failure.
 * Only normal attributes can be replaced: it is an error
 * to attempt to replace a selection attribute.
 */
{
    atrInternal *internal = (atrInternal *) oldattr;

    if (internal->atrFlags & ATR_SELECT) {
	errRaise(atr_pkg_name, VEM_BADARG,
		 "in atrReplace: passed attribute is selection");
    }

    if (!repl_resolve(col, &(internal->pix_or_offset), internal->atrFlags))
      return VEM_RESOURCE;

    /*
     * atrFlags, pl_mask, and disp_func are unaffected
     * Fill pattern is deallocated and reallocated.
     */
    if (internal->fill_style) {
	rel_fill(internal->fill_style);
    }
    if (fill) {
	if (flIsSolid(fill) == VEM_OK) {
	    /* Solid fill all the time */
	    internal->atrFlags |= ATR_SOLIDFILL;
	    internal->fill_style = (atrBitmap *) 0;
	} else {
	    /* Stipple fill */
	    do_fill(fill, &(internal->fill_style));
	}
    } else {
	/* Not filled */
	internal->fill_style = (atrBitmap *) 0;
    }

    if (internal->l_style) {
	rel_pat(internal->l_style);
    }
    if (l_pat) {
	if (flIsSolid(l_pat) == VEM_OK) {
	    /* Solid lines all the time */
	    internal->atrFlags |= ATR_SOLIDLINE;
	    internal->l_style = (atrLineStyle *) 0;
	} else {
	    /* Patterned line */
	    do_pat(l_pat, &(internal->l_style));
	}
    } else {
	/* No outline */
	internal->l_style = (atrLineStyle *) 0;
    }

    internal->l_width = l_width;
    return VEM_OK;
}

/*
 * Return codes from sel_resolve
 */
#define SELFAIL	0
#define SELBW	1
#define SELHARD	2
#define SELPOOL	3
#define SELPLANE 4

vemStatus atrSelect(newattr, mask, col, fill, l_pat, l_width)
atrHandle *newattr;		/* Returned attribute handle    */
unsigned long *mask;		/* Returned plane mask (if any) */
XColor *col;			/* Pixel value for color        */
flPattern fill;			/* Fill pattern to use (if any) */
flPattern l_pat;		/* Line style pattern (if any)  */
int l_width;			/* Brush size for outlines      */
/*
 * This routine creates an attribute suitable for selection purposes.
 * It tries to allocate the colors such that selection is done on
 * its own plane.  If it is successful,  the routine will return
 * VEM_SELPLANE and `mask' will be set to the plane mask for the
 * selection attribute.  If it can't allocate a plane for the attribute,
 * it will allocate a normal selection attribute and return VEM_OK.
 * Normally,  the display function for selection attributes is
 * GXcopy like all other attributes.  However,  if the programmer
 * uses atrSetXor() to select the attribute,  the effects
 * of drawing can be reversed by drawing an item again.  This
 * is used for argument list primitive display.
 */
{
    atrInternal *new_internal;
    int selreturn;

    new_internal = VEMALLOC(atrInternal);
    new_internal->atrFlags = ATR_SELECT;
    switch (selreturn = sel_resolve(col, &(new_internal->pix_or_offset))) {
    case SELFAIL:
	/* Color allocation totally failed */
	return VEM_RESOURCE;
    case SELBW:
    case SELHARD:
	/* Black and white and hardware colors */
	new_internal->pl_mask = AllPlanes;
	break;
    case SELPOOL:
	/* Allocated from pool */
	new_internal->pl_mask = AllPlanes ^ attr_allmasks;
	break;
    case SELPLANE:
	/* Managed to get it on its own plane */
	new_internal->atrFlags |= ATR_PLANE;
	new_internal->pl_mask = attr_planes[new_internal->pix_or_offset];
	*mask = new_internal->pl_mask;
	break;
    }

    new_internal->disp_func = GXcopy;

    if (fill) {
	if (flIsSolid(fill) == VEM_OK) {
	    /* Solid fill all the time */
	    new_internal->atrFlags |= ATR_SOLIDFILL;
	    new_internal->fill_style = (atrBitmap *) 0;
	} else {
	    /* Stipple fill */
	    do_fill(fill, &(new_internal->fill_style));
	}
    } else {
	/* Not filled */
	new_internal->fill_style = (atrBitmap *) 0;
    }
    if (l_pat) {
	if (flIsSolid(l_pat) == VEM_OK) {
	    /* Solid lines all the time */
	    new_internal->atrFlags |= ATR_SOLIDLINE;
	    new_internal->l_style = (atrLineStyle *) 0;
	} else {
	    /* Patterned line */
	    do_pat(l_pat, &(new_internal->l_style));
	}
    } else {
	/* No outline */
	new_internal->l_style = (atrLineStyle *) 0;
    }
    new_internal->l_width = l_width;
    *newattr = (atrHandle) new_internal;
    if (selreturn == SELPLANE) {
	return VEM_SELPLANE;
    } else {
	return VEM_OK;
    }
}


unsigned long atrQBg()
/*
 * Returns the pixel value of the background used for all windows.  This
 * is given by the attribute `Background' provided by the defaults module.
 */
{
    static unsigned long *bg_pixel = (unsigned long *) 0;
    atrInternal *bg_intern;
    atrHandle norm_bg;

    if (!bg_pixel) {
	bg_pixel = VEMALLOC(unsigned long);
	dfGetAttr("Background", &norm_bg);
	bg_intern = (atrInternal *) norm_bg;
	if (col_state == IS_HICOL) {
	    /* Pixel is an offset */
	    *bg_pixel = attr_pixvalues[bg_intern->pix_or_offset].pixel;
	} else {
	    /* Pixel is direct */
	    *bg_pixel = bg_intern->pix_or_offset;
	}
    }
    return *bg_pixel;
}




/*
 * Allocating colors.
 *
 * If the display is classed IS_HICOL,  all colors are allocated
 * from the tables initialized by atrInit.  There are two parts
 * to this scheme:  normal allocation and separate plane allocation.
 * Normal colors are allocated from attr_colors array.  Selection
 * plane colors are allocated from the attr_planes array.  In
 * both cases,  the allocation scheme is a simple linear lookup
 * where unallocated colors are marked with a special bit.
 *
 * If the display is classed IS_LOWCOL,  all colors are allocated
 * using XAllocColor.  If the display is classed IS_BW,
 * all colors except "white" are mapped into BlackPixel.
 */

static int norm_resolve(col, result)
XColor *col;			/* Color to allocate  */
unsigned long *result;		/* Returned value     */
/*
 * This routine allocates normal colors.  If the display is
 * IS_HICOL,  the color will be allocated from attr_pixvalues.
 * The returned result is interpreted as an offset into this array.
 * Otherwise,  XAllocColor is used unless it is a
 * black and white display where all colors except white
 * are mapped to black.
 */
{
    if (col_state == IS_HICOL) {
	return hi_resolve(col, result);
    } else {
	return low_resolve(col, result);
    }
}


static int repl_resolve(col, pix_or_offset, flags)
XColor *col;			/* Desired color   */
unsigned long *pix_or_offset;	/* Existing pixel  */
int flags;			/* Attribute flags */
/*
 * This routine reallocates an existing color.  If the display
 * is IS_HICOL,  the color will be physically changed if its
 * not shared.  Otherwise,  the existing pixel will be deallocated
 * and a new one will be allocated.
 */
{
    if ((col_state == IS_HICOL) && (attr_pixcount[*pix_or_offset] == 1)) {
	/* Its non-shared and hard allocated - change in place */
	col->pixel = attr_pixvalues[*pix_or_offset].pixel;
	changeColor(1, col);
	return 1;
    } else {
	/* Deallocate and reallocate */
	rel_color(*pix_or_offset, flags);
	return norm_resolve(col, pix_or_offset);
    }
}



static int sel_resolve(col, result)
XColor *col;			/* Color to allocate  */
unsigned long *result;		/* Returned value     */
/*
 * This routine allocates selection colors.  If the display is
 * IS_HICOL,  the color will be allocated from attr_planes.
 * If this fails,  it will attempt to allocate the color
 * from the normal pixel pool.  If the display is not
 * IS_HICOL,  the routine tries to use XAllocColor.  
 * The following return codes are possible: 
 *    SELFAIL  - selection allocation failed
 *    SELBW    - selection color hard-wired for B/W.
 *    SELHARD  - selection color allocated using XGetHardwareColor
 *    SELPOOL  - selection color allocated from normal pixel pool
 *    SELPLANE - selection color on its own plane
 */
{
    XColor destColor;
    unsigned long i, j;

    if (col_state == IS_HICOL) {
	/* Try to find unallocated color in plane array */
	for (i = 0;  i < max_planes;  i++) {
	    if (attr_planes[i] & SEL_UNALLOC) {
		/* Found one -- allocate it and set it */
		attr_planes[i] &= (~SEL_UNALLOC);

		/* Set all of the normal colors with this mask */
		destColor = *col;
		for (j = 0;  j < max_colors;  j++) {
		    destColor.pixel =
		      attr_pixvalues[j].pixel | attr_planes[i];
		    changeColor(2, &destColor);
		}

		/*
		 * Right now,  we leave the combinations of selection
		 * undefined.  Eventually,  these should also be defined.
		 */
		*result = i;
		return SELPLANE;
	    }
	}
	/* Didn't find one - try to normally allocate */
	if (hi_resolve(col, result)) return SELPOOL;
	else return SELFAIL;
    } else {
	if (col_state == IS_LOWCOL) {
	    if (low_resolve(col, result)) return SELHARD;
	    else return SELFAIL;
	} else {
	    /* Hard allocation of black pixel */
	    *result = vuBlack((XColor *) 0);
	    return SELBW;
	}
    }
}


static int hi_resolve(col, result)
XColor *col;			/* Color to allocate         */
unsigned long *result;		/* Returned value            */
/*
 * This routine is called to allocate a color from the
 * locally allocated table of colors.  It attempts to
 * share colors if possible.
 */
{
    unsigned long i;

    /* Pass one looks for the same color to re-use it */
    for (i = 0;  i < max_colors;  i++) {
	if ((attr_pixcount[i] > 0) &&
	    (attr_pixvalues[i].red == col->red) &&
	    (attr_pixvalues[i].green == col->green) &&
	    (attr_pixvalues[i].blue == col->blue))
	  {
	      /* We can share this color */
	      attr_pixcount[i] += 1;
	      *result = i;
	      return 1;
	  }
    }

    /* Pass two finds the first unallocated pixel */
    for (i = 0;  i < max_colors;  i++) {
	if (attr_pixcount[i] == 0) {
	    /* Found one -- allocate it and set it */
	    attr_pixcount[i] = 1;
	    col->pixel = attr_pixvalues[i].pixel;
	    attr_pixvalues[i] = *col;
	    changeColor(3, col);
	    *result = i;
	    return 1;
	}
    }
    vemMsg(MSG_L, "Out of locally allocated color resources");
    return 0;
}



static int low_resolve(col, result)
XColor *col;			/* Color to allocate */
unsigned long *result;		/* Returned value    */
/*
 * This routine is called to allocate a color assuming that
 * the display is such that local allocation is not possible.
 * It returns one if the allocation is successful,  zero otherwise.
 */
{
    if (col_state == IS_BW) {
	if ((col->red < MAXINTENSITY) ||
	    (col->green < MAXINTENSITY) ||
	    (col->blue < MAXINTENSITY))
	  {
	      /* Set result to black pixel */
	      *result = vuBlack((XColor *) 0);
	  } else {
	      *result = vuWhite((XColor *) 0);
	  }
    } else {
	/* Attempt to allocate using XGetHardwareColor */
	if (!XAllocColor(_AGC.disp, atr_cmap, col)) {
	    vemMsg(MSG_L, "XAllocColor resource failure.\n", 0);
	    return 0;
	}
	*result = col->pixel;
    }
    return 1;
}


static void do_fill(pat, bm)
flPattern pat;			/* Fill pattern to use */
atrBitmap **bm;			/* Returned resource   */
/*
 * This routine allocates and fills an inactive bitmap resource
 * for the fill pattern `pat'.  First,  the routine looks for
 * isomorphic bitmaps in the current list of all bitmaps.  If
 * one is found,  it is returned.  Otherwise,  a new bitmap
 * is allocated and added to the list.
 */
{
    lsGen theGen;
    atrBitmap *oneBM;
    flPattern realpat;
    unsigned int rows, cols;
    unsigned int realrows, realcols;

    /* Search current list of all bitmaps */
    theGen = lsStart(BMResources);
    while (lsNext(theGen, (lsGeneric *) &oneBM, LS_NH) == LS_OK) {
	/* Check it against the current one */
	if (flEqual(pat, oneBM->BM_pattern) == VEM_OK) {
	    /* Found equal pattern -- return this one */
	    oneBM->ref_count += 1;
	    *bm = oneBM;
	    lsFinish(theGen);
	    return;
	}
    }
    lsFinish(theGen);

    /* Allocate a new one */
    oneBM = VEMALLOC(atrBitmap);
    /* Ask for correct size */
    flSize(pat, (int *) &cols, (int *) &rows);
    XQueryBestStipple(_AGC.disp, atr_drawable, cols, rows,
		      &realcols, &realrows);
    if ((rows != realrows) || (cols != realcols)) {
	/* Size it to the right size */
	realpat = flChangeSize(pat, (int) realcols, (int) realrows);
    } else {
	realpat = flCopyPattern(pat);
    }
    oneBM->BM_pattern = realpat;
    oneBM->BM = (Pixmap) 0;
    lsNewBegin(BMResources, (lsGeneric) oneBM, &(oneBM->BM_spot));
    oneBM->BM_actspot = (lsHandle) 0;
    oneBM->ref_count = 1;
    *bm = oneBM;
}



static void do_pat(pat, style)
flPattern pat;			/* Line style pattern  */
atrLineStyle **style;		/* Returned line style */
/*
 * Compiles `pat' into the X11 description given in `style'.  Generally,
 * the pattern should be one row.  If it is not one row,  it is
 * changed into one row.  The fill pattern module is used to actually
 * compile the description into X11 form.
 */
{
    int width, height;

    flSize(pat, (int *) &width, (int *) &height);
    if (height > 1) pat = flChangeSize(pat, width, 1);
    *style = VEMALLOC(atrLineStyle);
    flXLineStyle(pat, &((*style)->d_offset), &((*style)->d_length), &((*style)->d_list));
}


static void atrGCInit()
/*
 * Sets the graphics contexts to a known (very simple) state.  This
 * is done when the package is initialized and when atrSet(0) is
 * called.
 */
{
    XGCValues values;
    unsigned long mask = GCFunction | GCPlaneMask | GCForeground |
      GCBackground | GCLineWidth | GCLineStyle | GCFillStyle;

    values.function = GXcopy;
    values.plane_mask = AllPlanes;
    values.foreground = 0;
    values.background = 0;
    values.line_width = 0;
    values.line_style = LineSolid;
    values.fill_style = FillSolid;
    XChangeGC(_AGC.disp, _AGC.solid_gc, mask, &values);
    XChangeGC(_AGC.disp, _AGC.line_gc, mask, &values);
    XChangeGC(_AGC.disp, _AGC.stipple_gc, mask, &values);
}



void atrSet(attr)
atrHandle attr;			/* Attribute handle */
/*
 * This routine sets the current graphics context to the one specified
 * by the attribute handle `attr'.  The primitives in display.c obey
 * this graphics context.  In effect,  this fills the globally accessible
 * graphics context structure _AGC with the proper information.  Note
 * that fonts are set separately (using atrSetFont).
 */
{
    XGCValues values;
    unsigned long mask = 0;
    unsigned long old_fg;
    static atrHandle last_handle = (atrHandle) 0;
    atrInternal *real_attr = (atrInternal *) attr;
    int i;

    if (attr == (atrHandle) 0) {
	/* Set to initial state */
	atrGCInit();
	return;
    } else if (attr == last_handle) {
	/* No change -- don't need to do anything */
	return;
    } else {
	last_handle = attr;
    }
#ifdef MULTI_TEST
    dspFlushBoxQueue(0);
#endif
    /* First the solid graphics context */
    mask = GCFunction | GCPlaneMask | GCForeground | GCBackground |
      GCLineWidth | GCLineStyle | GCFillStyle;
    values.function = real_attr->disp_func;
    values.plane_mask = real_attr->pl_mask;
    if (col_state == IS_HICOL) {
	/* It is an offset */
	if (real_attr->atrFlags & ATR_PLANE) {
	    values.foreground = attr_planes[real_attr->pix_or_offset];
	} else {
	    values.foreground = attr_pixvalues[real_attr->pix_or_offset].pixel;
	}
    } else {
	/* Its the actual pixel value */
	values.foreground = real_attr->pix_or_offset;
    }
    values.background = atrQBg();
    values.line_width = real_attr->l_width;
    values.line_style = LineSolid;
    values.fill_style = FillSolid;
    XChangeGC(_AGC.disp, _AGC.solid_gc, mask, &values);

    /* Then the clear context */
    old_fg = values.foreground;
    values.foreground = atrQBg();
    XChangeGC(_AGC.disp, _AGC.clear_gc, mask, &values);

    /* Then the line style context */
    values.foreground = old_fg;
    if (real_attr->l_style) {
	values.line_style = LineOnOffDash;
	XSetDashes(_AGC.disp, _AGC.line_gc, real_attr->l_style->d_offset,
		   real_attr->l_style->d_list, real_attr->l_style->d_length);
	_AGC.line_thres = 0;
	for (i = 0;  i < real_attr->l_style->d_length; i++) {
	    _AGC.line_thres += real_attr->l_style->d_list[i];
	}
    } else if (real_attr->atrFlags & ATR_SOLIDLINE) {
	values.line_style = LineSolid;
	_AGC.line_thres = ATR_SOLID;
    } else {
	values.line_style = LineSolid;
	_AGC.line_thres = ATR_DONT;
    }
    values.fill_style = FillSolid;
    _AGC.fill_thres = 0;
    XChangeGC(_AGC.disp, _AGC.line_gc, mask, &values);

    /* Then the stipple context */
    values.foreground = old_fg;
    values.line_style = LineSolid;
    if (real_attr->fill_style) {
	values.fill_style = FillStippled;
	mask |= GCStipple;
	if (max_bitmaps < VEMMAXINT || !real_attr->fill_style->BM) {
	    /* Pixmaps limited or not allocated */
	    values.stipple = get_bitmap(real_attr->fill_style);
	} else if (real_attr->fill_style->BM) {
	    /* Already safely allocated */
	    values.stipple = real_attr->fill_style->BM;
	}
	dfGetInt("solid.threshold", &(_AGC.fill_thres));
    } else if (real_attr->atrFlags & ATR_SOLIDFILL) {
	values.fill_style = FillSolid;
	_AGC.fill_thres = 0;
    } else {
	values.fill_style = FillSolid;
	_AGC.fill_thres = ATR_DONT;
    }
    XChangeGC(_AGC.disp, _AGC.stipple_gc, mask, &values);

    /* Generally exported values */
    _AGC.line_width = real_attr->l_width;
}


void atrSetXor(attr)
atrHandle attr;
/*
 * This routine sets the current attribute to `attr' but makes
 * the display function become GXxor.  It also changes the pixel
 * value to try to make the color come out right as long as you
 * are xoring information onto the background.
 */
{
    XGCValues values;
    atrInternal *real_attr = (atrInternal *) attr;

    /* First,  do the normal set */
    atrSet(attr);

    /* Then tweek the graphics contexts as appropriate */
    values.function = GXxor;
#ifdef OLD_VEM_BLACK_AND_WHITE_BUG
    if (real_attr->atrFlags & ATR_PLANE) {
	values.foreground = attr_planes[real_attr->pix_or_offset] ^ atrQBg();
    } else if (col_state == IS_LOWCOL) {
	values.foreground = attr_pixvalues[real_attr->pix_or_offset].pixel ^ atrQBg();
    }
#else
    /* 6/95 - Change from Thomas Burger to fix highliting on B&W
     * monitor 
     */
    if( col_state == IS_HICOL ) {
      /* real_attr->pix_or_offset is an offset */
      if (real_attr->atrFlags & ATR_PLANE) {
  	values.foreground = attr_planes[real_attr->pix_or_offset] ^ atrQBg();
      } else {
	values.foreground =
	  attr_pixvalues[real_attr->pix_or_offset].pixel ^ atrQBg(); 
      }
    } else if( col_state == IS_LOWCOL ) {
      /* its the actual pixel value */  
      values.foreground = real_attr->pix_or_offset ^ atrQBg();
    } else {
      /* IS_BW: draw white on black bg and black on white bg */  
      values.foreground = vuBlack((XColor *) 0);
    }
#endif
    XChangeGC(_AGC.disp, _AGC.solid_gc, GCFunction | GCForeground, &values);
    XChangeGC(_AGC.disp, _AGC.line_gc, GCFunction | GCForeground, &values);
    XChangeGC(_AGC.disp, _AGC.stipple_gc, GCFunction | GCForeground, &values);
}



/*
 * Independent Context Parameters
 */

void atrFont(font)
XFontStruct *font;		/* Destination font */
/*
 * This routine sets the font in the current graphics context.  Unlike
 * other parameters,  the displayed font can change independently
 * of the other parameters (e.g.  label display uses a set of
 * possible fonts).  The font is only changed if it is different
 * from the one already set.
 */
{
    if (atr_font && (atr_font->fid == font->fid)) {
	return;
    }
    atr_font = font;
    XSetFont(_AGC.disp, _AGC.solid_gc, atr_font->fid);
    XSetFont(_AGC.disp, _AGC.line_gc, atr_font->fid);
    XSetFont(_AGC.disp, _AGC.stipple_gc, atr_font->fid);
}

void atrClip(x, y, width, height)
int x, y;			/* Origin */
int width, height;		/* Size   */
/*
 * This routine sets the current clipping rectangle for the current
 * graphics context.  Like atrFont(),  this parameter is independent
 * of the parameters set by atrSet() or atrSetXor().  The clip region
 * is reset to null if the width or height is zero.
 */
{
    if ((width <= 0) || (height <= 0)) {
	atr_clip.width = atr_clip.height = 0;
	XSetClipMask(_AGC.disp, _AGC.solid_gc, None);
	XSetClipMask(_AGC.disp, _AGC.line_gc, None);
	XSetClipMask(_AGC.disp, _AGC.stipple_gc, None);
    } else {
	atr_clip.x = x;
	atr_clip.y = y;
	atr_clip.width = width;
	atr_clip.height = height;
	XSetClipRectangles(_AGC.disp, _AGC.solid_gc,
			   0, 0, &atr_clip, 1, YXSorted);
	XSetClipRectangles(_AGC.disp, _AGC.line_gc,
			   0, 0, &atr_clip, 1, YXSorted);
	XSetClipRectangles(_AGC.disp, _AGC.stipple_gc,
			   0, 0, &atr_clip, 1, YXSorted);
    }
}



static Pixmap get_bitmap(someBM)
atrBitmap *someBM;		/* Locally defined bitmap */
/*
 * This routine returns the X resource associated with the specified
 * local pixmap.  The routine maintains an LRU cache of the maps
 * in BMActive.  If the bitmap is already made,  it is returned
 * and the map is moved up to the top of the cache.  If it isn't made,
 * the bottom-most element is freed and the requested bitmap replaces
 * it.  The maximum number of bitmaps is determined by when X reports
 * a resource failure.
 */
{
    lsGen theGen;
    atrBitmap *temp;

    if (someBM == (atrBitmap *) 0) return (Pixmap) 0;
    if (someBM->BM != (Pixmap) 0) {
	/* Move item to the top (if no room) */
	if (lsLength(BMActive) >= max_bitmaps) {
	    theGen = lsGenHandle(someBM->BM_actspot,(lsGeneric *) &temp,LS_AFTER);
	    lsDelBefore(theGen, (lsGeneric *) &temp);
	    lsNewBegin(BMActive, (lsGeneric) temp, &(someBM->BM_actspot));
	    lsFinish(theGen);
	}
	return someBM->BM;
    } else {
	/* Replace bottom item if there is no room */
	if (lsLength(BMActive) < max_bitmaps) {
	    int rows, cols;
	    char *data;

	    /* There is room - see if we can allocate */
	    flXBitmap(_AGC.disp, atr_vis, someBM->BM_pattern, &cols, &rows, &data);
	    someBM->BM = XCreateBitmapFromData(_AGC.disp, atr_drawable,
					       data, cols, rows);
	    VEMFREE(data);
	    if (someBM->BM == (Pixmap) 0) {
		/* Oops,  the number of available bitmaps has decreased */
		max_bitmaps = lsLength(BMActive);
		/* Try again by bumping one out */
		return get_bitmap(someBM);
	    } else {
		/* We can allocate.  Insert in list */
		lsNewBegin(BMActive, (lsGeneric) someBM, &(someBM->BM_actspot));
		return someBM->BM;
	    }
	} else {
	    /* There isn't room - get rid of bottom one */
	    lsDelEnd(BMActive, (lsGeneric *) &temp);
	    XFreePixmap(_AGC.disp, temp->BM);
	    temp->BM = (Pixmap) 0;
	    temp->BM_actspot = (lsHandle) 0;
	    /* Now,  try to get it again */
	    return get_bitmap(someBM);
	}
    }
}


vemStatus atrIsPlane(attr)
atrHandle attr;			/* Attribute handle to check */
/*
 * Returns VEM_OK if the attribute is a selection attribute
 * which has been allocated to its own plane.  Returns VEM_FALSE
 * in any other case.
 */
{
    atrInternal *real_attr = (atrInternal *) attr;

    if (real_attr->atrFlags & ATR_PLANE) {
	return VEM_OK;
    } else {
	return VEM_FALSE;
    }
}


void atrRelease(attr)
atrHandle attr;			/* Attribute handle to release */
/*
 * This routine reclaims all of the resources consumed by the
 * attribute `attr'.  It should not be referenced again.
 */
{
    atrInternal *real_attr = (atrInternal *) attr;

    /* Release the serious attributes */
    rel_color(real_attr->pix_or_offset, real_attr->atrFlags);
    if (real_attr->fill_style) {
	rel_fill(real_attr->fill_style);
    }
    if (real_attr->l_style) {
	rel_pat(real_attr->l_style);
    }
    
    /* Remove structure itself */
    VEMFREE(real_attr);
}



static void rel_color(pix_off, flags)
unsigned long pix_off;		/* Pixel value or offset */
int flags;			/* Attribute flags       */
/*
 * This routine frees color resources as it sees fit.  For locally
 * allocated pixels (IS_HICOL),  full deallocation takes place.
 * For XGetHardwareColor,  we have no reference count for the
 * pixels so we take a risk and not free them.
 */
{
    if (col_state == IS_HICOL) {
	/* Pixel is an offset */
	if (flags & ATR_PLANE) {
	    /* Deallocate from attr_planes */
	    attr_planes[pix_off] |= SEL_UNALLOC;
	} else {
	    /* Pool pixel.  Might be xored however */
	    if (attr_pixcount[pix_off] < 0) {
		attr_pixcount[pix_off] = 0;
	    } else {
		attr_pixcount[pix_off] -= 1;
	    }
	}
    } else if (col_state == IS_LOWCOL) {
	/* Allocated using XAllocColor */
	XFreeColors(_AGC.disp, atr_cmap, &pix_off, 1, 0);
    } else {
	/* Black and white -- nothing */
    }
}




static void rel_fill(someBM)
atrBitmap *someBM;		/* Local version of bitmap */
/*
 * Frees all space associated with a local bitmap (including flushing
 * from cache).
 */
{
    lsGen theGen;
    atrBitmap *temp;
    
    /* Reduce reference count */
    if (someBM->ref_count > 0) {
	someBM->ref_count -= 1;
    } else {
	someBM->ref_count = 0;
    }
    if (someBM->ref_count == 0) {
	/* Ready to be released */
	if (someBM->BM != (Pixmap) 0) {
	    /* Its in the cache.  Take it out. */
	    theGen = lsGenHandle(someBM->BM_actspot, (lsGeneric *) &temp,
				 LS_AFTER);
	    lsDelBefore(theGen, (lsGeneric *) &temp);
	    lsFinish(theGen);
	    XFreePixmap(_AGC.disp, someBM->BM);
	}
	/* Remove from overall list */
	theGen = lsGenHandle(someBM->BM_spot, (lsGeneric *) &temp,
			     LS_AFTER);
	lsDelBefore(theGen, (lsGeneric *) &temp);
	lsFinish(theGen);
	VEMFREE(someBM);
    }
}

static void rel_pat(style)
atrLineStyle *style;		/* Style to free */
/*
 * Releases resources consumed by the given line style.  Right
 * now,  this simply requires a free of the structure.
 */
{
    VEMFREE(style);
}



/*ARGSUSED*/
static void changeColor(type, def)
int type;
XColor *def;
/*
 * This routine was written primarily as a debugging aide.
 */
{
#ifdef PRINT_ALLOC
    switch (type) {
    case 1: /* Replace allocation */
	vemMsg(MSG_L, "repl_resolve allocation: ");
	break;
    case 2: /* Selection allocation */
	vemMsg(MSG_L, "sel_resolve allocation: ");
	break;
    case 3: /* Hi resolve allocation */
	vemMsg(MSG_L, "hi_resolve allocation: ");
	break;
    }
#endif
    XStoreColor(_AGC.disp, atr_cmap, def);
}
