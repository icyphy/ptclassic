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
 * VEM Oct Window Mangement Routines
 * Window option handling
 *
 * David Harrison
 * University of Calfornia, Berkeley
 * 1988
 *
 * This file contains routines for modification and query of
 * the many options supported by VEM graphics windows.
 */

#include "wininternal.h"	/* Internal view of windows */
#include "xvals.h"		/* X Values  */



vemStatus wnGetInfo(win, opts, mainfct, altfct, altbind, outExtent)
Window win;			/* VEM graphics window       */
wnOpts *opts;			/* Returned window info      */
octId *mainfct;			/* Returned main facet id    */
octId *altfct;			/* Returned alternate facet id */
bdTable *altbind;		/* Alternate buffer bindings */
struct octBox *outExtent;	/* Current window extent     */
/*
 * This routine returns information about the VEM graphics window
 * `win'.  It is safe to pass zero on arguments the caller
 * does not care about. Note that the layer list and interface in `opts' are
 * internal representations and should not be modified without
 * copying them first.  May return VEM_CANTFIND if the window is
 * not owned by VEM.
 */
{
    internalWin *theWin;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	if (opts) *opts = theWin->opts;
	if (mainfct) *mainfct = theWin->fcts[W_PRI].winFctId;
	if (altfct && (theWin->num_fcts > 1)) *altfct =
	  theWin->fcts[W_MALT].winFctId;
	if (altbind) *altbind = theWin->altTable;
	if (outExtent) {
	    outExtent->lowerLeft.x = theWin->fcts[W_PRI].x_off;
	    outExtent->lowerLeft.y = theWin->fcts[W_PRI].y_off;
	    outExtent->upperRight.x =
	      theWin->fcts[W_PRI].x_off +
		OCTSCALE(theWin->width, theWin->scaleFactor);
	    outExtent->upperRight.y =
	      theWin->fcts[W_PRI].y_off +
		OCTSCALE(theWin->height, theWin->scaleFactor);
	}
	return VEM_OK;
    } else return VEM_CANTFIND;
}


vemStatus wnSetInfo(win, newopts, altbind)
Window win;			/* VEM graphics window       */
wnOpts *newopts;		/* Returned window info      */
bdTable *altbind;		/* Alternate buffer bindings */
/*
 * This routine sets information about the VEM window `win'.
 * If an argument is non-zero,  its corresponding parameter
 * is set.  Otherwise,  it is unchanged.  Note that the
 * layer list and interface of `opts' are NOT copied.  However,
 * they WILL be freed when the window is destroyed.  To change
 * the main or alternate buffer,  use wnNewBuf or wnAltBuf.  The
 * routine will automatically redraw the window if certain key
 * parameters are changed.
 */
{
    internalWin *theWin;
    int redraw_flag = 0, inactive_flag = 0, tb_bits, dr_bits;
    int winstate;

    if (!_wnFind(win, &theWin, &winstate, (int *) 0)) {
	return VEM_CANTFIND;
    }
    inactive_flag = (winstate != WN_ACTIVE);

    /* Check for certain changes */
    if (newopts) {
	/* Check certain bits for redraw */
	dr_bits = VEM_GRIDLINES|VEM_DOTGRID|VEM_SHOWBB|VEM_SHOWAT;
	if ((theWin->opts.disp_options & dr_bits) !=
	    (newopts->disp_options & dr_bits)) {
	    /* Mark window for redraw */
	    redraw_flag = 1;
	}
	/* Changes in expand should also mark for layer recomp */
	if ((theWin->opts.disp_options & VEM_EXPAND) !=
	    (newopts->disp_options & VEM_EXPAND)) {
	    int i;

	    redraw_flag = 1;
	    for (i = 0;  i < theWin->num_fcts;  i++) {
		theWin->fcts[i].fctbits |= F_ULYR;
	    }
	}
	/* Check certain variables for redraw */
	if ((theWin->opts.grid_base != newopts->grid_base) ||
	    (theWin->opts.grid_minbase != newopts->grid_minbase) ||
	    (theWin->opts.grid_majorunits != newopts->grid_majorunits) ||
	    (theWin->opts.grid_minorunits != newopts->grid_minorunits) ||
	    (theWin->opts.inst_prio != newopts->inst_prio) ||
	    STRCOMP(theWin->opts.interface, newopts->interface)) {
	    redraw_flag = 1;
	}
	
	/*
	 * Compare two hash tables.  
	 */
	if (ST_EXIST(theWin->opts.off_layers)==ST_EXIST(newopts->off_layers))
	  {
	      if (theWin->opts.off_layers) {
		  st_generator *gen = st_init_gen(theWin->opts.off_layers);
		  char *key;
	      
		  while (st_gen(gen, &key, (char **) 0)) {
		      if (!st_is_member(newopts->off_layers, key)) {
			  redraw_flag = 1;
		      }
		  }
		  st_free_gen(gen);
	      } else if (newopts->off_layers) {
		  redraw_flag = 1;
	      }
	  } else {
	      redraw_flag = 1;
	  }
	if (newopts->off_layers && ST_EXIST(newopts->off_layers)) {
	    tbBits(theWin->tb, TB_MIS_LAYERS, 0, 0, (int *) 0);
	} else {
	    tbBits(theWin->tb, 0, TB_MIS_LAYERS, 0, (int *) 0);
	}
	
	/* Check for title bar inactivation */
	if ((theWin->opts.disp_options & VEM_TITLEDISP) !=
	    (newopts->disp_options & VEM_TITLEDISP)) {
	    /* Title bar state has changed */
	    if (newopts->disp_options & VEM_TITLEDISP) {
		/* Title bar is now active */
		theWin->opts.disp_options |= VEM_TITLEDISP;
		XMapWindow(xv_disp(), theWin->TitleBar);
	    } else {
		/* Title bar is inactive */
		theWin->opts.disp_options &= ~(VEM_TITLEDISP);
		tbDeactivate(theWin->tb);
		XUnmapWindow(xv_disp(), theWin->TitleBar);
	    }
	    _wnArrange(theWin, theWin->tw, theWin->th, 1);
	    redraw_flag = 1;
	}
	tb_bits = VEM_SHOWDIFF|VEM_SHOWABS|VEM_SHOWBITS|
	  VEM_SHOWEDIT|VEM_SHOWCON|VEM_SHOWRPC;
	if ((newopts->disp_options & tb_bits) !=
	    (theWin->opts.disp_options & tb_bits)) {
	    /* Some of its bits have changed */
	    _wnDownBits(theWin->tb, newopts->disp_options);
	}
	if ((newopts->disp_options & VEM_REMOTERUN) !=
	    (theWin->opts.disp_options & VEM_REMOTERUN)){
	    if (newopts->disp_options & VEM_REMOTERUN) {
		tbBits(theWin->tb, TB_RPC_APPL, 0, 0, (int *) 0);
	    } else {
		tbBits(theWin->tb, 0, TB_RPC_APPL, 0, (int *) 0);
	    }
	}
	if ((newopts->disp_options & VEM_NODRAW) !=
	    (theWin->opts.disp_options & VEM_NODRAW)) {
	    if (newopts->disp_options & VEM_NODRAW) {
		(void) wnMakeInactive(theWin->XWin);
	    } else {
		(void) _wnMakeActive(theWin->XWin);
	    }
	}
	if (newopts->lambda != theWin->opts.lambda) {
	    tbLambda(theWin->tb, newopts->lambda);
	}
    }
    
    if (altbind) theWin->altTable = *altbind;
    if (redraw_flag && !inactive_flag) (void) wnQWindow(win);
    if (newopts) theWin->opts = *newopts;
    return VEM_OK;
}


typedef struct set_cursor_info_defn {
    Display *disp;
    Cursor cursor;
} set_cursor_info;

/*ARGSUSED*/
static enum st_retval setWinCursor(key, value, arg)
char *key;			/* Key of data item          */
char *value;			/* Value associated with key */
char *arg;			/* Standard argument         */
/*
 * This routine is used with the st_foreach routine to set
 * the cursor of all active windows.  The arg is the cursor
 * all windows should now have.
 */
{
    set_cursor_info *info = (set_cursor_info *) arg;

    XDefineCursor(info->disp, ((internalWin *) value)->XWin, info->cursor);
    return ST_CONTINUE;
}


vemStatus wnCursor(theCursor)
Cursor theCursor;
/*
 * This routine sets all of the cursors of active windows
 * to 'theCursor'.
 */
{
    set_cursor_info info;

    info.disp = xv_disp();
    info.cursor = theCursor;
    _wnForeach(WN_ACTIVE | WN_INTR, setWinCursor, (char *) &info);
    return VEM_OK;
}


/*ARGSUSED*/
static enum st_retval clrAltTbl(key, value, arg)
char *key;			/* Key of data item          */
char *value;			/* Value associated with key */
char *arg;			/* Standard argument         */
/*
 * This routine is used with the st_foreach routine to clear
 * the alternate binding table of all windows.  The arg
 * is a binding table which is compared against the window's
 * binding table.  If there is a match,  the window's binding
 * table is cleared.
 */
{
    internalWin *theWin = (internalWin *) value;

    if (theWin->altTable == (bdTable) arg) {
	theWin->opts.disp_options &= (~VEM_REMOTERUN);
	tbBits(theWin->tb, 0, TB_RPC_APPL, 0, (int *) 0);
	theWin->altTable = (bdTable) 0;
	tbRPCName(theWin->tb, (STR) 0);
    }
    return ST_CONTINUE;
}

vemStatus wnZeroAltTbl(theTbl)
bdTable theTbl;			/* Binding table to look for */
/*
 * This routine sets the binding table of all windows with
 * alternate binding table 'theTbl'.  It is used by the
 * remote procedure support routine vemCloseApplication
 * to de-register an application's bindings.
 */
{
    _wnForeach(WN_ACTIVE | WN_INTR | WN_INACTIVE, clrAltTbl, (char *) theTbl);
    return VEM_OK;
}



/*
 * Overall window state changing functions
 */

vemStatus _wnUnmap(win)
Window win;			/* Window to unmap */
/*
 * This routine marks the specified window as unmapped.  It is removed
 * from the hash table of active windows and placed on the hash table
 * of unmapped windows.  Windows become active again when the window
 * is mapped and X sends a ExposeWindow event for the window.  The
 * caller is then expected to call wnQRedraw with the CHECKWIN flag.
 * This makes the window active again.
 */
{
    internalWin *theWin;
    int winstate;

    if (_wnFind(win, &theWin, &winstate, (int *) 0)) {
	if (winstate != WN_INACTIVE) {
	    _wnChange(theWin, WN_INACTIVE);
	}
    } else {
	return VEM_CANTFIND;
    }
    return VEM_OK;
}



vemStatus wnMakeInactive(win)
Window win;			/* Window to make inactive */
/*
 * This routine makes the specified window inactive.  An inactive
 * window remains mapped,  but is not updated (exposure events
 * have no effect).  Windows become active again when a call
 * to wnQWindow is made.
 */
{
    internalWin *theWin;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	/* Turn on flag */
	theWin->opts.disp_options |= VEM_NODRAW;
	/* Turn on icon in window */
	tbBits(theWin->tb, TB_INTERRUPT, 0, 0, (int *) 0);
	/* Place in inactive table */
	_wnChange(theWin, WN_INTR);
	return VEM_OK;
    } else return VEM_CANTFIND;
}

vemStatus _wnMakeActive(win)
Window win;			/* Window to make active */
/*
 * This routine reverses the effects of wnMakeInactive.  It is called
 * by wnQWindow when it encounters a deactivated window.
 */
{
    internalWin *theWin;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	/* Turn off flag */
	theWin->opts.disp_options &= (~VEM_NODRAW);
	/* Turn off icon in window */
	tbBits(theWin->tb, 0, TB_INTERRUPT, 0, (int *) 0);
	/* Place in active table */
	_wnChange(theWin, WN_ACTIVE);
	return VEM_OK;
    } else return VEM_CANTFIND;
}



vemStatus wnSetRPCName(win, name)
Window win;                    /* window running the application */
STR name;                      /* name of the RPC application    */
/*
 * Changes the remote application name in title bar.  This should
 * actually be part of wnSetInfo and wnGetInfo but RLS refused to
 * do it that way.
 */
{
    internalWin *theWin;

    /* Get the current information about the window */
    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	tbRPCName(theWin->tb, name);
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}



Window wnGrabWindow(win)
Window win;			/* Some window */
/*
 * Under X11, events can occur in any of the three windows that
 * constitute a VEM graphics window:  title bar, graphics, or
 * main.  This routine takes one of those windows and always
 * maps it to the top (main) window that was initially grabbed
 * by wnGrab.  Returns NULL if the window is not a VEM graphics
 * window.
 */
{
    internalWin *theWin;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	return theWin->mainWin;
    } else {
	return (Window) 0;
    }
}
