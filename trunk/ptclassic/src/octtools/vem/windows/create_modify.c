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
 * VEM Oct Window Management Routines
 * Basic Window Creation and Modification
 *
 * David Harrison
 * University of California, Berkeley, 1988
 *
 * This module contains routines for creating and deleting X windows
 * showing a selectable portion of an Oct facet.  
 *
 * Windows in this module are implemented using X windows.  Thus,
 * clipping, moving, resizing, and iconification of windows is done
 * using the X system.  Information associated with windows is
 * kept in a global hash table (implemented using the "st" package
 * written by Peter Moore).
 *
 * OCT windows are not really allocated or deallocated in this module.
 * Creating and destroying windows is the function of X.  After creating
 * a window,  the caller transfers control of the window to this package
 * using wnGrab.  Control is released by calling wnRelease.  The operations
 * offered by this module can be used on a window as long as it has
 * been "grabbed" using wnGrab.  Note any X window can be passed to
 * the routines but only those which have been grabbed will work.
 */

#include "wininternal.h"	/* Internal window definition */
#include "defaults.h"		/* Default handling           */
#include "vemDM.h"		/* Dialog stuff               */

/* Structure used for passing both the buffer and deletion func to relFunc */

typedef struct relRecord_defn {
    octId theFct;		/* Facet to search for                    */
    vemStatus (*delFunc)();	/* Function to call when releasing window */
} relRecord;

#define INIT_LAYERS	8

/* Forward declarations for lint */
static void wnGetOptions();
static st_table *wnCopyLyrs();
static void wnCopyOptions();



vemStatus wnGrab(win, winFacet, winExtent, brdr, bkgd)
Window win;			/* X Window to grab                  */
octObject *winFacet;		/* OCT facet associated with window  */
struct octBox *winExtent;	/* Window extent in OCT coordinates  */
unsigned long brdr, bkgd;	/* Border and Background pixels      */
/*
 * This routine "grabs" a previously created X window allowing OCT
 * window operations to be performed on it.  The non-essential window
 * attributes like abstraction threshold and window options are found
 * using the defaults system.  The alternate binding table for the window
 * is set to zero.  These can be queried and changed using
 * wnGetInfo and wnSetInfo. Returns VEM_OK if the window was sucessfully grabbed,
 * VEM_CORRUPT if not.  Under the new system,  an Oct graphics window
 * actually consists of two subwindows: a titlebar and the graphics area.
 * These windows are created and managed by this module.
 */
{
    XWindowAttributes info;
    /* Add back in window information */
    internalWin *newWin;
    Window tbWin, grWin;
    XColor *tb_bg;

    /* Do some error checking */
    VEM_CKRVAL(winFacet, "No window facet", VEM_CORRUPT);
    VEM_CKRVAL(winExtent, "No Oct Extent", VEM_CORRUPT);
    VEM_CKRVAL(XGetWindowAttributes(_AGC.disp, win, &info),
	       "Can't get window attributes", VEM_CORRUPT);

    /* Create subwindows */
    dfGetPixel("title.backgr", &tb_bg);
    tbWin = XCreateSimpleWindow(_AGC.disp, win, -1, -1, 1, 1,
				1, brdr, tb_bg->pixel);
    XSelectInput(_AGC.disp, tbWin, ExposureMask);
    grWin = XCreateSimpleWindow(_AGC.disp, win, 0, 10, 1, 1, 0, 0L, bkgd);
    XSelectInput(_AGC.disp, grWin,
		 ButtonPressMask|ButtonReleaseMask|ExposureMask|
		 KeyPressMask|PointerMotionMask|PointerMotionHintMask|
		 OwnerGrabButtonMask);


    /* Assign the normal information */
    newWin = VEMALLOC(internalWin);
    newWin->mainWin = win;
    newWin->XWin = grWin;
    newWin->TitleBar = tbWin;
    wnGetOptions(&(newWin->opts), bufStyle(winFacet->objectId));

    /* Initialize titlebar and arrange the two windows */
    tbGrab(_AGC.disp, tbWin, newWin->opts.lambda, &(newWin->tb), &_wnTBHeight);

    /* Arrange windows only if top level is mapped */
    if (info.map_state != IsUnmapped) {
	_wnArrange(newWin, info.width, info.height, 0);
    } else {
	newWin->tw = newWin->th = -1;
	newWin->width = info.width;
	newWin->height = info.height - _wnTBHeight;
    }
    XMapWindow(_AGC.disp, grWin);
    if (newWin->opts.disp_options & VEM_TITLEDISP) {
	XMapWindow(_AGC.disp, tbWin);
    } else {
	tbDeactivate(newWin->tb);
    }
    _wnDownBits(newWin->tb, newWin->opts.disp_options);
    if (bufWritable(winFacet->objectId) != VEM_OK) {
	tbBits(newWin->tb, TB_READ_ONLY, 0, 0, (int *) 0);
    }
    if (newWin->opts.off_layers && ST_EXIST(newWin->opts.off_layers)) {
	tbBits(newWin->tb, TB_MIS_LAYERS, 0, 0, (int *) 0);
    }

    /* Fill in other information */
    newWin->num_fcts = 1;
    newWin->fcts[W_PRI].winFctId = winFacet->objectId;
    newWin->fcts[W_PRI].layers = daAlloc(tkLayer, INIT_LAYERS);
    newWin->fcts[W_PRI].fctbits = F_ULYR;
    newWin->altTable = (bdTable) 0;

    /* Set the name */
    tbEditCell(newWin->tb, winFacet);

    /* Add it into the hash table */
    _wnEnter(newWin, WN_ACTIVE);

    /* Compute the necessary scale information */
    newWin->fcts[W_PRI].x_off = winExtent->lowerLeft.x;
    newWin->fcts[W_PRI].y_off = winExtent->lowerLeft.y;
    return wnZoom(win, winExtent);
}


static void wnGetOptions(opts, style)
wnOpts *opts;			/* Option structure to fill in */
int style;			/* Editing style               */
/*
 * This routine downloads the standard defaults read from the defaults
 * package into the local options structure for a window.  This routine
 * will set certain options based on editing style.
 */
{
    dfGetInt("window.options", &(opts->disp_options));
    dfGetInt("lambda", &(opts->lambda));
    dfGetInt("snap", &(opts->snap));

    dfGetInt("grid.base", &(opts->grid_base));
    dfGetInt("grid.minbase", &(opts->grid_minbase));
    dfGetInt("grid.major.units", &(opts->grid_majorunits));
    dfGetInt("grid.minor.units", &(opts->grid_minorunits));
    dfGetInt("grid.pixels", &(opts->grid_diff));
    
    opts->off_layers = (st_table *) 0;
    
    dfGetDouble("boundbox.thres", &opts->bb_thres);
    dfGetInt("boundbox.min", &(opts->bb_min));
    dfGetInt("abstraction", &(opts->con_min));
    dfGetInt("solid.threshold", &(opts->solid_thres));
    dfGetString("interface", &(opts->interface));
    opts->interface = VEMSTRCOPY(opts->interface);
    dfGetInt("inst.priority", &(opts->inst_prio));

    /* Edit style specific switches */
    switch (style) {
    case BUF_PHYSICAL:
	/* No settings */
	break;
    case BUF_SYMBOLIC:
	opts->disp_options |= VEM_MANLINE;
	break;
    case BUF_SCHEMATIC:
	opts->disp_options |= (VEM_DOTGRID|VEM_MANLINE|VEM_GRAVITY);
	opts->disp_options &= ~(VEM_EXPAND);
	opts->grid_diff += 8;
	break;
    }
}


static st_table *wnCopyLyrs(old_tbl)
st_table *old_tbl;		/* Old layer table */
/*
 * This routine copies a layer list from one table to a
 * newly created one.
 */
{
    st_table *new_tbl;
    st_generator *gen;
    char *key;

    if (!old_tbl) return (st_table *) 0;
    new_tbl = st_init_table(strcmp, st_strhash);
    gen = st_init_gen(old_tbl);
    while (st_gen(gen, &key, (char **) 0)) {
	st_insert(new_tbl, VEMSTRCOPY(key), (Pointer) 0);
    }
    st_free_gen(gen);
    return new_tbl;
}




void _wnDownBits(tb, dspBits)
tbHandle tb;			/* Titlebar handle     */
int dspBits;			/* Display option bits */
/*
 * This routine checks the state of `dspBits' and downloads
 * the appropriate bits down into the title bar.
 */
{
    int on_bits = 0, off_bits = 0;

    if (dspBits & VEM_SHOWDIFF) on_bits |= TB_DIFF_COORDS;
    else off_bits |= TB_DIFF_COORDS;
    if (dspBits & VEM_SHOWABS) on_bits |= TB_ABS_COORDS;
    else off_bits |= TB_ABS_COORDS;
    if (dspBits & VEM_SHOWBITS) on_bits |= TB_STAT_ICONS;
    else off_bits |= TB_STAT_ICONS;
    if (dspBits & VEM_SHOWEDIT) on_bits |= TB_EDIT_CELL;
    else off_bits |= TB_EDIT_CELL;
    if (dspBits & VEM_SHOWCON) on_bits |= TB_CONTEXT_CELL;
    else off_bits |= TB_CONTEXT_CELL;
    if (dspBits & VEM_SHOWRPC) on_bits |= TB_RPC_NAME;
    else off_bits |= TB_RPC_NAME;
    tbControl(tb, on_bits, off_bits, 0, (int *) 0);
}




vemStatus wnDup(win, oldWin, newExtent, brdr, bkgd)
Window win;			/* X Window to grab                 */
Window oldWin;			/* Previously grabbed window        */
struct octBox *newExtent;	/* Window extent in OCT coordinates */
unsigned long brdr, bkgd;	/* Border and background pixels     */
/*
 * This routine is similar to wnGrab except the attributes of the
 * window are copied from 'oldWin'.  The attributes inherited are:
 * the window buffer,  the alternate binding table,  the window
 * display options,  and the abstraction threshold.  The old window
 * must be active (displayed).  If 'newExtent' is null,  the extent
 * of the old window is used.
 */
{
    octObject winFacet;
    XWindowAttributes info;
    internalWin *oldWinInfo, *newWinInfo;
    struct octBox localExtent;
    Window tbWin, grWin;
    XColor *tb_bg;
    int deact_bit, i;

    /* Look up the old window */
    if (!_wnFind(oldWin, &oldWinInfo, (int *) 0, (int *) 0)) {
	return VEM_CANTFIND;
    }

    /* Find out how big the new window is */
    VEM_CKRVAL(XGetWindowAttributes(_AGC.disp, win, &info),
	       "Can't query window", VEM_CORRUPT);

    deact_bit = (oldWinInfo->opts.disp_options & VEM_NODRAW) &&
      (newExtent == NIL(struct octBox));
    if (newExtent == NIL(struct octBox)) {
	newExtent = &localExtent;
	newExtent->lowerLeft.x = oldWinInfo->fcts[W_PRI].x_off;
	newExtent->lowerLeft.y = oldWinInfo->fcts[W_PRI].y_off;
	newExtent->upperRight.x =
	  oldWinInfo->fcts[W_PRI].x_off +
	    OCTSCALE(oldWinInfo->width, oldWinInfo->scaleFactor);
	newExtent->upperRight.y =
	  oldWinInfo->fcts[W_PRI].y_off +
	    OCTSCALE(oldWinInfo->height, oldWinInfo->scaleFactor);
    }

    /* Create sub-windows of main window */
    dfGetPixel("title.backgr", &tb_bg);
    tbWin = XCreateSimpleWindow(_AGC.disp, win, -1, -1, info.width,
				1, 1, brdr, tb_bg->pixel);
    XSelectInput(_AGC.disp, tbWin, ExposureMask);
    grWin = XCreateSimpleWindow(_AGC.disp, win, 0, 1, info.width, info.height-1,
				0, 0L, bkgd);
    XSelectInput(_AGC.disp, grWin,
		 ButtonPressMask|ButtonReleaseMask|ExposureMask|
		 KeyPressMask|PointerMotionMask|PointerMotionHintMask|
		 OwnerGrabButtonMask);

    /* Assign the normal information */
    newWinInfo = VEMALLOC(internalWin);
    newWinInfo->mainWin = win;
    newWinInfo->XWin = grWin;
    newWinInfo->TitleBar = tbWin;
    wnCopyOptions(&(newWinInfo->opts), &(oldWinInfo->opts));

    /* Initialize titlebar and arrange the two windows */
    tbGrab(_AGC.disp, tbWin, newWinInfo->opts.lambda, &(newWinInfo->tb), &_wnTBHeight);
    _wnArrange(newWinInfo, info.width, info.height, 0);
    XMapWindow(_AGC.disp, grWin);
    if (newWinInfo->opts.disp_options & VEM_TITLEDISP) {
	XMapWindow(_AGC.disp, tbWin);
    } else {
	tbDeactivate(newWinInfo->tb);
    }
    _wnDownBits(newWinInfo->tb, newWinInfo->opts.disp_options);

    /* Fill in other information */
    newWinInfo->num_fcts = oldWinInfo->num_fcts;
    for (i = 0;  i < oldWinInfo->num_fcts;  i++) {
	newWinInfo->fcts[i] = oldWinInfo->fcts[i];
	newWinInfo->fcts[i].layers = daDup(oldWinInfo->fcts[i].layers);
    }
    if (bufWritable(newWinInfo->fcts[W_PRI].winFctId) != VEM_OK) {
	tbBits(newWinInfo->tb, TB_READ_ONLY, 0, 0, (int *) 0);
    }
    if (newWinInfo->opts.disp_options & VEM_REMOTERUN) {
	tbBits(newWinInfo->tb, TB_RPC_APPL, 0, 0, (int *) 0);
	/* set the RPC name while you're here */
        tbRPCName(newWinInfo->tb, tbGetRPCName(oldWinInfo->tb));
    }
    if (newWinInfo->opts.off_layers && ST_EXIST(newWinInfo->opts.off_layers)) {
	tbBits(newWinInfo->tb, TB_MIS_LAYERS, 0, 0, (int *) 0);
    }
    newWinInfo->altTable = oldWinInfo->altTable;

    /* Set the name(s) */
    winFacet.objectId = newWinInfo->fcts[W_PRI].winFctId;
    if (octGetById(&winFacet) == OCT_OK) {
	tbEditCell(newWinInfo->tb, &winFacet);
    }
    if (newWinInfo->num_fcts > 1) {
	winFacet.objectId = newWinInfo->fcts[W_MALT].winFctId;
	if (octGetById(&winFacet) == OCT_OK) {
	    tbContextCell(newWinInfo->tb, &winFacet);
	}
    }

    /* Add it into the hash table */
    if (deact_bit) {
	newWinInfo->opts.disp_options |= VEM_NODRAW;
	tbBits(newWinInfo->tb, TB_INTERRUPT, 0, 0, (int *) 0);
	_wnEnter(newWinInfo, WN_INTR);
    } else {
	newWinInfo->opts.disp_options &= (~VEM_NODRAW);
	tbBits(newWinInfo->tb, 0, TB_INTERRUPT, 0, (int *) 0);
	_wnEnter(newWinInfo, WN_ACTIVE);
    }

    /* Compute the necessary extent */
    return wnZoom(win, newExtent);
}


static void wnCopyOptions(newopts, oldopts)
wnOpts *newopts, *oldopts;	/* Destination and Source */
/*
 * Copies `oldopts' into `newopts'.
 */
{
    *newopts = *oldopts;
    newopts->off_layers = wnCopyLyrs(oldopts->off_layers);
    newopts->interface = VEMSTRCOPY(oldopts->interface);
}




static vemStatus wnRemove(theWin)
internalWin *theWin;		/* Real information */
/*
 * Disposes of `theWin' in a politically correct fashion.
 */
{
    int idx;

    /* Remove from hash tables */
    _wnOut(theWin);

    /* The buffer could be freed but no ref counts are done as yet */
    /* bufClose(theWin->winFct) */

    /* Remove outstanding requests from redraw queue */
    _wnQRemove(theWin);

    /* Should also eliminate events from selection queue */

    /* Destroy subwindows */
    XDestroyWindow(_AGC.disp, theWin->XWin);
    tbRelease(theWin->tb);
    XDestroyWindow(_AGC.disp, theWin->TitleBar);
    
    /* Destroy the record */

    if (theWin->opts.off_layers) {
	st_generator *gen = st_init_gen(theWin->opts.off_layers);
	char *key;

	while (st_gen(gen, &key, (char **) 0)) {
	    VEMFREE(key);
	}
	st_free_gen(gen);
	st_free_table(theWin->opts.off_layers);
    }
    for (idx = 0;  idx < theWin->num_fcts;  idx++) {
	daFree(theWin->fcts[idx].layers, (void (*)()) 0);
    }
    VEMFREE(theWin->opts.interface);
    VEMFREE(theWin);
    return VEM_OK;
}

vemStatus wnRelease(win)
Window win;			/* VEM window */
/*
 * Releases control of the specified OCT window.  If the window is
 * not an OCT window,  it returns VEM_CANTFIND with no ill side effects.
 * The routine attempts to close the associated facet and technology
 * (if they are still open somewhere else,  they remain open).  Important:
 * OCT window should be reclaimed using this routine before it is
 * destroyed using XDestroyWindow (or a similar routine).
 */
{
    internalWin *theWin;

    /* Get the real information about the window */
    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	return wnRemove(theWin);
    } else {
	return VEM_CANTFIND;
    }
}



/*ARGSUSED*/
static enum st_retval relFunc(key, value, arg)
char *key;			/* Key of window (XId)               */
char *value;			/* Internal window structure pointer */
char *arg;			/* User argument (relRecord *) */
/*
 * This routine is called for each window under the control of the
 * this module.  If the buffer passed matches either the alternate
 * or primary buffer of the window,  the routine carries out the necessary
 * operations to wipe out that buffer.
 */
{
    Window saveWin;
    internalWin *theWin = (internalWin *) value;
    relRecord *info = (relRecord *) arg;
    int idx, j;

    idx = 0;
    while (idx < theWin->num_fcts) {
	if (theWin->fcts[idx].winFctId == info->theFct) {
	    /* Move down ones above it */
	    for (j = idx;  j < theWin->num_fcts-1;  j++) {
		theWin->fcts[j] = theWin->fcts[j+1];
	    }
	    theWin->num_fcts -= 1;
	} else {
	    idx++;
	}
    }
    if (theWin->num_fcts == 0) {
	/* No more buffers left - remove window */
	saveWin = theWin->mainWin;
	(void) wnRemove(theWin);
	info->delFunc(saveWin);
    } else {
	/* Just redraw it */
	(void) wnQWindow(theWin->mainWin);
    }
    return ST_CONTINUE;
}

vemStatus wnBufRelease(fctId, delFunc)
octId fctId;			/* Facet  to release */
vemStatus (*delFunc)();		/* Deletion function */
/*
 * This routine removes all traces of `fctId' from the current
 * list of Oct windows.  If the buffer is the only buffer
 * associated with the window,  the window itself is released
 * and the routine calls 'delFunc' passing the top-level window as an 
 * argument.  'delFunc' should be of the following form:
 *     vemStatus delFunc(win)
 *     Window win;
 * If the buffer is the alternate buffer of a window,  the
 * alternate buffer will be cleared and a redraw of the window
 * will take place.  If the buffer is the primary buffer,  a
 * buffer swap will take place and the alternate buffer will
 * be wiped out.
 */
{
    relRecord passedArg;
    enum st_retval relFunc();

    passedArg.theFct = fctId;
    passedArg.delFunc = delFunc;
    _wnForeach(WN_ACTIVE | WN_INACTIVE, relFunc, (Pointer) &passedArg);
    return VEM_OK;
}


typedef struct switch_fct_info {
    octId oldFctId;
    octId newFctId;
} switch_fct;

/*ARGSUSED*/
static enum st_retval switch_it(key, value, arg)
char *key, *value, *arg;
/*
 * The `arg' field is actually a pointer to switch_fct.
 * The `value' field is actually a pointer to internalWin.  This
 * routine compares the facet value of the main and alternate
 * facets with the oldFctId field.  If they match,  it is replaced
 * with `newFctId'.  Currently does not handle edit cell/context
 * cell switch.
 */
{
    internalWin *theWin = (internalWin *) value;
    switch_fct *info = (switch_fct *) arg;
    int idx;

    for (idx = 0;  idx < theWin->num_fcts;  idx++) {
	if (theWin->fcts[idx].winFctId == info->oldFctId) {
	    theWin->fcts[idx].winFctId = info->newFctId;
	}
    }
    return ST_CONTINUE;
}

vemStatus wnSwitchBuf(oldFctId, newFctId)
octId oldFctId;			/* Old buffer */
octId newFctId;			/* New buffer */
/*
 * This routine finds all occurances of `oldFctId' in all windows
 * and replaces it with `newFctId'.  This is normally used to
 * keep things consistent across a revert of a facet.  Also
 * invalidates the layer table for the window.
 */
{
    switch_fct info;

    info.oldFctId = oldFctId;
    info.newFctId = newFctId;
    _wnForeach(WN_ACTIVE | WN_INACTIVE | WN_INTR, switch_it, (char *) &info);
    wnLyrChange(newFctId);
    return VEM_OK;
}



vemStatus wnNewBuf(win, fctId, extent)
Window win;			/* Window identifier */
octId fctId;			/* New primary facet */
struct octBox *extent;		/* Extent for window */
/*
 * This routine replaces the contents of the specified window with
 * the specified buffer.  All other features of the window except
 * its extent are left untouched.
 */
{
    internalWin *theWin;
    octObject winFacet;

    if (!_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	return VEM_CANTFIND;
    }

    if (fctId == oct_null_id) return VEM_NOTSUFF;

    /* Replace, zoom, and return */
    winFacet.objectId = fctId;
    if (octGetById(&winFacet) == OCT_OK) {
	theWin->fcts[W_PRI].winFctId = fctId;
	tbEditCell(theWin->tb, &winFacet);
	_wnLyrUpdate(theWin, 0);
	return wnZoom(win, extent);
    } else {
	return VEM_CORRUPT;
    }
}


vemStatus wnAltBuf(win, altFct, altPnt, mainPnt)
Window win;			/* Window identifier  */
octId altFct;			/* Alternate buffer   */
struct octPoint *altPnt;	/* Point in alternate */
struct octPoint *mainPnt;	/* Point in primary   */
/*
 * This routine adds an alternate facet to the list of
 * facets for window `win'.  Alternate facets are displayed
 * before the primary facet. This can be used for editing in
 * context or as a means of writing annotations on a buffer.
 * All facets share the same scale but may differ in
 * offsets.  The 'altPnt' and 'mainPnt' parameters specify
 * points of alignment,  i.e. the two points will always be
 * on top of each other.  This routine does not redraw the
 * window.   NOTE: the routine will return VEM_RESOURCE
 * if the maximum number of facets has been exceeded.
 */
{
    internalWin *theWin;
    octObject winFacet;

    if (!_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	return VEM_CANTFIND;
    }

    if (altFct == oct_null_id) return VEM_NOTSUFF;

    if (theWin->num_fcts >= MAX_FACETS) return VEM_RESOURCE;

    winFacet.objectId = altFct;
    if (octGetById(&winFacet) == OCT_OK) {
	theWin->fcts[theWin->num_fcts].winFctId = altFct;
	theWin->fcts[theWin->num_fcts].x_off =
	  altPnt->x - (mainPnt->x - theWin->fcts[W_PRI].x_off);
	theWin->fcts[theWin->num_fcts].y_off =
	  altPnt->y - (mainPnt->y - theWin->fcts[W_PRI].y_off);
	theWin->num_fcts += 1;
	tbContextCell(theWin->tb, &winFacet);
	return VEM_OK;
    } else {
	return VEM_CORRUPT;
    }
}



vemStatus wnSwapBuf(win)
Window win;			/* Window identifier */
/*
 * The primary and first alternate buffers are swapped.  If
 * the window doesn't have a first alternate buffer,  the
 * routine will return VEM_CORRUPT.
 */
{
    internalWin *theWin;
    wnFctSpec temp;
    octObject tf;

    if (!_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	return VEM_CANTFIND;
    }

    if (theWin->num_fcts <= 1) {
	/* No alternate */
	return VEM_CORRUPT;
    }

    /* Swap the two */
    temp = theWin->fcts[W_PRI];
    theWin->fcts[W_PRI] = theWin->fcts[W_MALT];
    theWin->fcts[W_MALT] = temp;

    tf.objectId = theWin->fcts[W_PRI].winFctId;
    if (octGetById(&tf) == OCT_OK) {
	tbEditCell(theWin->tb, &tf);
    }

    tf.objectId = theWin->fcts[W_MALT].winFctId;
    if (octGetById(&tf) == OCT_OK) {
	tbContextCell(theWin->tb, &tf);
    }
    return VEM_OK;
}



void _wnArrange(realWin, width, height, new_offset)
internalWin *realWin;		/* Internal version of window information */
int width, height;		/* Real height of overall window          */
int new_offset;			/* If non-zero,  compute new offset       */
/*
 * This routine arranges the two subwindows that make up a VEM graphics
 * window.  It partitions the space such that the title bar is at the
 * top and consumes a fixed height.  The graphics window is then given
 * the remaining space.  If the title bar is off,  the graphics window
 * is given all the space.  The appropriate size parameters in `realWin'
 * are updated.  Uses the global `_wnTBHeight' set by tbGrab in wnGrab
 * or wnDup.  If the flag `new_offset' is set, the routine will compute
 * a new offset for the graphics window.
 */
{
    int tb_width, tb_height;
    int gr_width, gr_height;
    int add_x, add_y;
    int idx;

    /* Set the overall width and height */
    realWin->tw = width;
    realWin->th = height;

    if (realWin->opts.disp_options & VEM_TITLEDISP) {
	/* Place the titlebar */
	tb_width = width;
	tb_height = _wnTBHeight;
	tbResize(realWin->tb, tb_width, tb_height);
	/* Place the graphics window */
	gr_width = width;
	gr_height = height - (_wnTBHeight + 1);
	XMoveResizeWindow(_AGC.disp, realWin->XWin, 0, _wnTBHeight+1,
			  gr_width, gr_height);
    } else {
	/* Give it all to graphics window */
	gr_width = width;
	gr_height = height;
	XMoveResizeWindow(_AGC.disp, realWin->XWin, 0, 0, gr_width, gr_height);
    }
    if (new_offset) {
	add_x = OCTSCALE(realWin->width - gr_width,
			 realWin->scaleFactor) >> 1;
	add_y = OCTSCALE(realWin->height - gr_height,
			 realWin->scaleFactor) >> 1;
	for (idx = 0;  idx < realWin->num_fcts;  idx++) {
	    realWin->fcts[idx].x_off += add_x;
	    realWin->fcts[idx].y_off += add_y;
	}
    }
    realWin->width = gr_width;
    realWin->height = gr_height;
}
