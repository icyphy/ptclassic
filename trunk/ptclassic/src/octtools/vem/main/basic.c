#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/*LINTLIBRARY*/
/*
 * Basic VEM Commands
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file contains the implementation of the basic VEM commands
 * for creating and deleting windows, panning, zooming, and saving
 * buffers.
 *
 * See COMMANDS.c for how these functions are called.
 */

#include "general.h"		/* General VEM definitions      */
#include <X11/Xlib.h>		/* C language interface to X    */
#include "xvals.h"		/* X constants                  */
#include "list.h"		/* List handling package        */
#include "message.h"		/* Message handling system      */
#include "oct.h"		/* OCT data base definitions    */
#include "buffer.h"		/* Buffer management            */
#include "argsel.h"		/* Argument selection primitives */
#include "windows.h"		/* Window manipulation          */
#include "bindings.h"		/* Command bindings             */
#include "defaults.h"		/* Default handling             */
#include "commands.h"		/* Definitions for VEM commands */
#include "vemUtil.h"		/* Basic utilities              */
#include "region.h"		/* Region package               */
#include "tap.h"		/* To find menu palatte         */
#include "oh.h"			/* Standard Oct helper routines */
#include "vemDM.h"		/* High-level dialogs           */
#include "drs.h"		/* For undo operations          */

static vemStatus doRevert();
static vemStatus recurRevert();
static vemStatus revInsts();



/*
 * Some argument list editing commands.
 */

/*ARGSUSED*/
vemStatus delOneItemCmd(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * This command deletes the last item found on the argument list.
 * Thus,  if the last argument is a points argument with 5 points,
 * it will become a points argument with 4 points.  If there was
 * only one item in the argument,  the argument is deleted.
 * It handles whatever argument list it is passed.
 */
{
    vemOneArg *lastArg;
    int len;

    len = lsLength(cmdList);
    if (len < 1)
      return VEM_ARGMOD;

    /* First,  we find the last argument */
    lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH);

    /* Now,  we determine what to do based on type */
    switch (lastArg->argType) {
    case VEM_POINT_ARG:
	/* First,  remove last point */
	argDelPoint(lastArg, lastArg->length-1);
	/* If zero,  eliminate the argument */
	if (lastArg->length == 0) {
	    lsDelEnd(cmdList, (lsGeneric *) &lastArg);
	    vuOffSelect(lastArg);
	}
	break;
    case VEM_LINE_ARG:
	/* First,  remove last point */
	argDelPoint(lastArg, lastArg->length-1);
	/* If zero,  eliminate the argument */
	if (lastArg->length == 1) {
	    /* Downgrade to a point */
	    argType(lastArg, VEM_POINT_ARG);
	} else if (lastArg->length == 0) {
	    lsDelEnd(cmdList, (lsGeneric *) &lastArg);
	    vuOffSelect(lastArg);
	}
	break;
    case VEM_BOX_ARG:
	/* First,  remove last point */
	argDelPoint(lastArg, lastArg->length*2-1);
	/* If zero,  eliminate the argument */
	if (lastArg->length == 0) {
	    lsDelEnd(cmdList, (lsGeneric *) &lastArg);
	    argOff(lastArg);
	}
	break;
    case VEM_TEXT_ARG:
	/* We must put the command loop into text input */
	return VEM_ARGBREAK;
    default:
	/* We don't handle strings or objects */
	XBell(xv_disp(), 0);
	break;
    }
    return VEM_ARGMOD;
}

/*ARGSUSED*/
vemStatus delOneArgCmd(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * This routine is similar to delOneItemCmd.  However,  this command
 * deletes the last *argument* on the argument list (regardless of
 * how large the argument may be).
 */
{
    vemOneArg *lastArg;

    /* First,  we find the last argument */
    if (lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH) == LS_OK) {
	/* Take it out of the list */
	lsDelEnd(cmdList, (lsGeneric *) &lastArg);

	/* Now,  we unselect and free the argument */
	vuOffSelect(lastArg);
    }
    return VEM_ARGMOD;
}



/*ARGSUSED*/
vemStatus intrCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine interrupts (deactivates) the window containing
 * the cursor.  No drawing will be done in the window until a
 * full redraw of the window (using pan or zoom) is done.  This
 * is the counterpart to interrupting the window while it is drawing.
 */
{
    if (spot) {
	wnMakeInactive(spot->theWin);
	return VEM_ARGMOD;
    } else {
	vemMessage("Cannot interrupt console window\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
}


/*ARGSUSED*/
vemStatus panCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * Basic window panning.  The routine examines the last argument
 * on the argument list.  If it is a point,  it will the window
 * containing the mouse such that it is centered around that point.
 * The point will be removed from the argument list.
 * If there is no point,  it centers the window containing the
 * mouse to the spot where the command was issued.  It does
 * not effect the argument list.
 */
{
    vemOneArg *oneArg;

    if (spot == (vemPoint *) 0) {
	vemMessage("You cannot pan non-graphic windows\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    if ((lsLastItem(cmdList, (lsGeneric *) &oneArg, LS_NH) == LS_OK) &&
	(oneArg->argType == VEM_POINT_ARG))
      {
	  /*
	   * The first form has been chosen.  
	   * We need to make sure they are the same facet.
	   */
	  if (oneArg->argData.points[oneArg->length-1].theFct
	      != spot->theFct)
	    {
		vemMessage("\nWindows to PAN must display the same facet.\n",
			   MSG_NOLOG|MSG_DISP);
		return VEM_ARGRESP;
	    }
	  /* Do the pan */
	  wnPan(spot->theWin,
		&(oneArg->argData.points[oneArg->length-1].thePoint));
	  wnQWindow(spot->theWin);
	  /* Eat the point */
	  delOneItemCmd(spot, cmdList);
      } else {
	  /* No arguments.  Simply pan to the point indicated by 'spot' */

	  wnPan(spot->theWin, &(spot->thePoint));
	  wnQWindow(spot->theWin);
      }
    return VEM_ARGMOD;
}



/* A short sorting macro for swapping x and y if x > y. */

#define SORT2(x, y, t) \
if (x > y) {  t = x;  x = y;  y = t; }


vemStatus zoomInCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the zoom command.  If the last argument is a box,
 * it zooms the window containing spot to the extent of the box.
 * If the list has no box,  the window is zoomed in by a factor
 * of two.  If provided,  the extent box is removed from the
 * argument list.  Other arguments are left untouched.
 */
{
    vemOneArg *oneArg;
    struct octBox zoomExtent;
    octCoord temp;
    int idx;

    if (spot == (vemPoint *) 0) {
	vemMessage("You cannot zoom non-graphics windows\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    lsLastItem(cmdList, (lsGeneric *) &oneArg, LS_NH);
    if (oneArg && (oneArg->argType == VEM_BOX_ARG)) {
	/* The last box must match */
	for (idx = oneArg->length*2-2;  idx < oneArg->length*2;  idx++) {
	    if (oneArg->argData.points[idx].theFct != spot->theFct)
	      {
		  vemMessage("All arguments to 'zoom' must point at the same facet\n",
			     MSG_NOLOG|MSG_DISP);
		  return VEM_ARGRESP;
	      }
	}
	/*
	 * It has passed all the tests
	 */
	zoomExtent.lowerLeft =
	  oneArg->argData.points[oneArg->length*2-2].thePoint;
	zoomExtent.upperRight =
	  oneArg->argData.points[oneArg->length*2-1].thePoint;
	/* Order the points so that lowerLeft and upperRight are correct */
	SORT2(zoomExtent.lowerLeft.x,  zoomExtent.upperRight.x, temp);
	SORT2(zoomExtent.lowerLeft.y,  zoomExtent.upperRight.y, temp);
	/* Do the zoom */
	wnZoom(spot->theWin, &zoomExtent);
	wnQWindow(spot->theWin);
	/* Eat the argument */
	delOneItemCmd(spot, cmdList);
    } else {
	/* Zoom in by a factor of two */
	wnGetInfo(spot->theWin, (wnOpts *) 0, (octId *) 0,
		  (octId *) 0, (bdTable *) 0, &zoomExtent);
	/* Compute quarter width */
	temp = (zoomExtent.upperRight.x - zoomExtent.lowerLeft.x) >> 2;
	zoomExtent.lowerLeft.x += temp;
	zoomExtent.upperRight.x -= temp;
	/* Compute quarter height */
	temp = (zoomExtent.upperRight.y - zoomExtent.lowerLeft.y) >> 2;
	zoomExtent.lowerLeft.y += temp;
	zoomExtent.upperRight.y -= temp;
	/* Do the zoom */
	wnZoom(spot->theWin, &zoomExtent);
	wnQWindow(spot->theWin);
    }
    return VEM_ARGMOD;
}



vemStatus zoomOutCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the zoom out command (also known as the expand command).
 * If the command list is empty,  the routine zooms the window
 * containing 'spot' out by a factor of two.  The command takes
 * a box as its only argument.  If there is one box,  it will zoom out
 * the window containing 'spot' such that the contents of the window
 * will be squeezed into the extent formed by the box.  Again, if
 * spot is not supplied,  the routine fails.
 */
{
    vemOneArg *oneArg;
    struct octBox zoomExtent, winExtent;
    struct octPoint newCenter;
    double halfWidth, halfHeight; /* Half of current window size */
    octCoord widthSpace, heightSpace;  /* Amount to add to outer edges */
    octCoord zoomWidth, zoomHeight, temp;
    int idx;

    if (spot == (vemPoint *) 0) {
	vemMessage("You cannot expand non-graphics windows\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    wnGetInfo(spot->theWin, (wnOpts *) 0, (octId *) 0, (octId *) 0,
	      (bdTable *) 0, &winExtent);
    halfWidth = ((double)(winExtent.upperRight.x-winExtent.lowerLeft.x))/2.0;
    halfHeight = ((double)(winExtent.upperRight.y-winExtent.lowerLeft.y))/2.0;
    lsLastItem(cmdList, (lsGeneric *) &oneArg, LS_NH);
    if (oneArg && (oneArg->argType == VEM_BOX_ARG)) {
	/* All of the facets must match */
	for (idx = oneArg->length*2-2;  idx < oneArg->length*2;  idx++) {
	    if (oneArg->argData.points[idx].theFct != spot->theFct)
	      {
		  vemMessage("All points to 'zoomOut' must point at the same facet",
			     MSG_NOLOG|MSG_DISP);
		  return VEM_ARGRESP;
	      }
	}
	zoomExtent.lowerLeft =
	  oneArg->argData.points[oneArg->length*2-2].thePoint;
	zoomExtent.upperRight =
	  oneArg->argData.points[oneArg->length*2-1].thePoint;
	/* Kill the argument */
	delOneItemCmd(spot, cmdList);
    } else {
	/* No arguments:  zoom out by a factor of two */
	zoomExtent = winExtent;
	zoomExtent.lowerLeft.x += (int) (halfWidth / 2.0);
	zoomExtent.lowerLeft.y += (int) (halfHeight / 2.0);
	zoomExtent.upperRight.x -= (int) (halfWidth / 2.0);
	zoomExtent.upperRight.y -= (int) (halfHeight / 2.0);
    }
    /*
     * It has passed all the tests.  The input region is in zoomExtent.
     * First,  we pan to the center of the given extent.
     */

    /* Order the points so that lowerLeft and upperRight are correct */
    SORT2(zoomExtent.lowerLeft.x,  zoomExtent.upperRight.x, temp);
    SORT2(zoomExtent.lowerLeft.y,  zoomExtent.upperRight.y, temp);

    newCenter.x = (zoomExtent.upperRight.x + zoomExtent.lowerLeft.x) / 2;
    newCenter.y = (zoomExtent.upperRight.y + zoomExtent.lowerLeft.y) / 2;
    wnPan(spot->theWin, &newCenter);
    
    /* Step 2:  Determine multiplication factor for width and height */
    
    zoomWidth  = zoomExtent.upperRight.x - zoomExtent.lowerLeft.x;
    zoomHeight = zoomExtent.upperRight.y - zoomExtent.lowerLeft.y;
    if (zoomWidth <= 0) zoomWidth = 1;
    if (zoomHeight <= 0) zoomHeight = 1;

    widthSpace = (int)
      (0.5 + halfWidth * ((halfWidth+halfWidth)/((double) zoomWidth)-1.0));
    heightSpace = (int)
      (0.5 + halfHeight * ((halfHeight+halfHeight)/((double) zoomHeight)-1.0));

    /* Add the necessary offsets to the current extent */

    winExtent.lowerLeft.x -=widthSpace;  winExtent.upperRight.x +=widthSpace;
    winExtent.lowerLeft.y -=heightSpace; winExtent.upperRight.y +=heightSpace;

    /* Do the zoom */

    wnZoom(spot->theWin, &winExtent);
    wnQWindow(spot->theWin);
    return VEM_ARGMOD;
}



vemStatus eqWinCmd(spot, cmdList)
vemPoint *spot;			/* Where the command was issued */
lsList cmdList;			/* Argument list                */
/*
 * This command takes one point argument and the spot.  The window containing
 * 'spot' will be made to have the same scale as the window containing
 * the one point on the argument list.
 */
{
    vemOneArg *oneArg;

    if (spot == (vemPoint *) 0) {
	vemMessage("You cannot scale non-graphic windows\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    if (lsLength(cmdList) != 1) {
	vemMessage("format:  [point] same-scale (in another window)\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    lsFirstItem(cmdList, (lsGeneric *) &oneArg, LS_NH);
    if ((oneArg->argType != VEM_POINT_ARG) || (oneArg->length != 1)) {
	vemMessage("same-scale takes one point argument only\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    /* We have managed to pass all the tests */
    wnAssignScale(spot->theWin, oneArg->argData.points[0].theWin);
    wnQWindow(spot->theWin);
    return VEM_OK;
}



vemStatus winMenuCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command opens a new window onto a cell which is assumed
 * to contain a set of items for either layer selection or
 * instance selection.  The command takes one text argument:
 * the name of the palette.  If omitted,  the "layer" palette
 * is assumed.  The name of the cell is determined by concatenating
 * the technology path onto the menu name. The view name is determined 
 * by the value of the property EDITSTYLE.  If there is no such 
 * property,  it uses the view name of the facet.
 */
{
    Window newXWindow;
    wnOpts opts;
    vemOneArg *theArg;
    struct octObject newFacet, curFacet;
    char paletteName[VEMMAXSTR];

    if (spot == (vemPoint *) 0) {
	vemMessage("Cannot spawn item menu from console window\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    /* Get the buffer */
    curFacet.objectId = spot->theFct;
    if (octGetById(&curFacet) != OCT_OK) {
	vemMsg(MSG_C, "Cannot get facet of window:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    /* Now the palette name */
    if ((lsLastItem(cmdList, (lsGeneric *) &theArg, LS_NH) == VEM_OK) &&
	(theArg->argType == VEM_TEXT_ARG))
      {
	  STRMOVE(paletteName, theArg->argData.vemTextArg);
	  STRDOWNCASE(paletteName);
	  /* Wipe out last argument */
	  delOneArgCmd(spot, cmdList);
      } else {
	  /* By default - the layer palette */
	  STRMOVE(paletteName, TAP_LAYER_PALETTE_NAME);
      }
    if (tapOpenPalette(&curFacet, paletteName, &newFacet, "r")) {
	if (vuOpenWindow(&newXWindow, "Palette", &newFacet, (STR) 0) == VEM_OK) {
	    VEM_CKRVAL(wnGetInfo(newXWindow, &opts, (octId *) 0,
				 (octId *) 0, (bdTable *) 0,
				 (struct octBox *) 0) == VEM_OK,
		       "Failed to get options", VEM_CORRUPT);
	    opts.disp_options |= VEM_TITLEDISP|VEM_EXPAND|VEM_SHOWBB;
	    opts.disp_options &= ~(VEM_GRIDLINES|VEM_SHOWDIFF|VEM_SHOWABS);
	    VEM_CKRVAL(wnSetInfo(newXWindow, &opts, (bdTable *) 0) == VEM_OK,
		       "Failed to set options", VEM_CORRUPT);
	    return VEM_ARGMOD;
	} else {
	    vemMessage("Could not open palette window\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
    } else {
	vemMsg(MSG_C, "Palette `%s' is not in technology.\nUse list-palettes to get a list of possible palettes.\n", paletteName);
	return VEM_ARGRESP;
    }
}


/*ARGSUSED*/
vemStatus listPalettes(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * Lists the available palettes for the cell in `spot'.
 */
{
    octObject facet;
    int i, np;
    char **nameArray;

    if (spot == (vemPoint *) 0) {
	vemMsg(MSG_C, "No palettes available in console window\n");
	return VEM_ARGRESP;
    }
    facet.objectId = spot->theFct;
    if (octGetById(&facet) != OCT_OK) {
	vemMsg(MSG_A, "Cannot get facet of window:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    /* Now get list of palettes */
    tapListPalettes(&facet, &np, &nameArray);
    if (np > 0) {
	vemMsg(MSG_C, "Palettes: %s", nameArray[0]);
	for (i = 1;  i < np;  i++) {
	    vemMsg(MSG_C, " %s", nameArray[i]);
	}
	vemMsg(MSG_C, "\n");
	VEMFREE(nameArray);
    } else {
	vemMsg(MSG_C, "No palettes found\n");
    }
    return VEM_ARGRESP;
}



vemStatus newWinCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the new window command.  If the last argument 
 * on the argument list is a text string,  it will interpret
 * that string as a facet name (cell view facet version separated
 * by colons) for the new window.  If the last argument is
 * a box,  it will duplicate the current window,  an zoom
 * the result to the extent of the box.  If the last argument
 * is neither,  it will duplicate the window containing the mouse.
 * In the first two cases,  it will eat the last argument.  The
 * last case does not effect the argument list.
 */
{
    Window newXWindow;
    vemOneArg *oneArg;
    char promptStr[VEMMAXSTR], defStr[VEMMAXSTR], *viewtype;
    struct octObject newFacet, spotFacet;
    struct octBox newExtent;

    lsLastItem(cmdList, (lsGeneric *) &oneArg, LS_NH);

    if (oneArg && (oneArg->argType == VEM_TEXT_ARG)) {
	/* Get the facet name out */
	if (spot) {
	    spotFacet.objectId = spot->theFct;
	}
	if (spot && (octGetById(&spotFacet) == OCT_OK)) {
	    newFacet = spotFacet;
	} else {
	    dfGetString("viewtype", &viewtype);
	    (void) sprintf(defStr, ":%s:contents", viewtype);
	    ohUnpackDefaults(&newFacet, "a", defStr);
	}
	if (ohUnpackFacetName(&newFacet, oneArg->argData.vemTextArg) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot parse cell specification: `%s'.\n",
		   oneArg->argData.vemTextArg);
	    return VEM_CORRUPT;
	}
	sprintf(promptStr, "OCT Window Displaying %s %s",
		newFacet.contents.facet.cell,
		newFacet.contents.facet.view);
	vuOpenWindow(&newXWindow, promptStr, &newFacet, (STR) 0);
	/* Eat the argument */
	delOneArgCmd(spot, cmdList);
	return VEM_ARGRESP;
    } else {
	/* Duplicate the window he is pointing at */

	if (spot == (vemPoint *) 0) {
	    vemMessage("You cannot duplicate non-graphics windows\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}

	if (oneArg && (oneArg->argType == VEM_BOX_ARG)) {
	    /* He has asked to open to a new extent */
	    newExtent.lowerLeft =
	      oneArg->argData.points[oneArg->length*2-2].thePoint;
	    newExtent.upperRight =
	      oneArg->argData.points[oneArg->length*2-1].thePoint;

	    vuCopyWindow(&newXWindow, "Open to a Region",
			 spot->theWin, &newExtent);
	    /* Eat the argument */
	    delOneItemCmd(spot, cmdList);
	    return VEM_ARGRESP;
	} else {
	    /* Normal duplication */
	    vuCopyWindow(&newXWindow, "Duplicate Window",
			 spot->theWin, (struct octBox *) 0);
	    return VEM_ARGRESP;
	}
    }
}



vemStatus delWinCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine deletes the window containing 'spot'.  If the
 * window is the console window,  the routine returns the termination
 * directive.  The command must have no arguments passed to it.  If
 * spot is not provided,  it returns VEM_ABORT.
 */
{
    Window win;
    vemStatus stat;
    wnOpts opts;
    static char *warn_msg =
"There is a remote application running in this window.\n\
\n\
Do you really want to close the window without stopping\n\
the application?";

    if (lsLength(cmdList) > 0) {
	vemMessage("close-window takes no arguments\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    if (spot == (vemPoint *) 0) {
	stat = bufSaveAll();
	if (stat == VEM_NOSELECT) return VEM_OK;
	if ( stat != VEM_OK ) {
	    errMsgArea[0] = '\0';
	    STRCONCAT(errMsgArea, "Closing the console window\n");
	    STRCONCAT(errMsgArea, "will terminate the program.\n");
	    STRCONCAT(errMsgArea, "\nDo you really want to exit?\n\n");
	    if (dmConfirm("Warning", errMsgArea, "No", "Yes") == VEM_OK) {
		return VEM_OK;
	    }
	}
	return VEM_ABORT;
    } else {
	/* Normal delete window */
	win = wnGrabWindow(spot->theWin);
	if (wnGetInfo(win, &opts, (octId *) 0, (octId *) 0, (bdTable *) 0,
		      (struct octBox *) 0) == VEM_OK) {
	    if ( opts.disp_options & VEM_REMOTERUN ) {
		if (dmConfirm("Warning", warn_msg, "Yes", "No") != VEM_OK) {
		    return VEM_OK;
		}
	    }
	    if (wnRelease(spot->theWin) == VEM_OK) {
		XDestroyWindow(xv_disp(), win);
	    }
	}
	return VEM_OK;
    }
}




/*ARGSUSED*/
vemStatus cancelCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine clears the argument list unconditionally.
 */
{
    return VEM_OK;
}

/*
 * Helpers for the where command
 */

static void show_facet(mainFct)
octId mainFct;			/* Window facet */
/* Shows the full name of the facet */
{
    octObject theFacet;

    theFacet.objectId = mainFct;
    if (octGetById(&theFacet) == OCT_OK) {
	vemMsg(MSG_C, "Edit cell: %s:%s:%s",
	       theFacet.contents.facet.cell,
	       theFacet.contents.facet.view,
	       theFacet.contents.facet.facet);
    } else {
	vemMsg(MSG_A, "Cannot retrieve edit cell:\n  %s\n", octErrorString());
    }
}

static void show_alt(win)
Window win;			/* Incoming window */
/* If there is an alternate buffer, it will be output to the console */
{
    octObject theFacet;

    theFacet.objectId = oct_null_id;
    VEM_CKRET(wnGetInfo(win, (wnOpts *) 0, (octId *) 0,
			&(theFacet.objectId), (bdTable *) 0,
			(struct octBox *) 0) == VEM_OK,
	      "where: can't get alternate buffer information");
    if (theFacet.objectId != oct_null_id) {
	if (octGetById(&theFacet) == OCT_OK) {
	    vemMsg(MSG_C, " Context cell: %s:%s:%s",
		   theFacet.contents.facet.cell,
		   theFacet.contents.facet.view,
		   theFacet.contents.facet.facet);
	} else {
	    vemMsg(MSG_A, " Cannot retrieve context cell:\n  %s\n",
		   octErrorString());
	}
    }
}

static void show_location(spot)
vemPoint *spot;			/* Where command was issued */
/* Shows `spot' in Oct units and lambda */
{
    octId locFctId;
    wnOpts opts;
    struct octPoint thePoint, spotPoint;
    int unitsPerLambda;

    VEM_CKRET(((wnPoint(spot->theWin, spot->x, spot->y,
		       &locFctId, &thePoint, 0) == VEM_OK) &&
	      (wnPoint(spot->theWin, spot->x, spot->y,
		       &locFctId, &spotPoint, VEM_NOSNAP) == VEM_OK)),
	      "where: can't get information about window\n");
    if (wnGetInfo(spot->theWin, &opts, (octId *) 0,
		  (octId *) 0, (bdTable *) 0,
		  (struct octBox *) 0) == VEM_OK) {
	unitsPerLambda = opts.lambda;
    } else {
	dfGetInt("lambda", &unitsPerLambda);
    }
    sprintf(errMsgArea, "(%ld, %ld) lambda, (%ld, %ld) oct units\n",
	    (long)(thePoint.x / unitsPerLambda),
	    (long)(thePoint.y / unitsPerLambda),
	    (long)spotPoint.x, (long)spotPoint.y);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
}


static void show_obj(spot)
vemPoint *spot;			/* Where command was issued */
/* Locates the object under spot and outputs a textual representation of it */
{
    regObjGen theGen;
    octObject theFacet, theObj;
    struct octBox region;

    theFacet.objectId = spot->theFct;
    VEM_OCTCKRET(octGetById(&theFacet));
    region.lowerLeft = spot->thePoint;
    region.upperRight = spot->thePoint;
    if (regObjStart(&theFacet, &region, OCT_GEO_MASK|OCT_INSTANCE_MASK,
		    &theGen, 0) == REG_OK) {
	while (regObjNext(theGen, &theObj) == REG_OK) {
	    sprintf(errMsgArea, "%s\n", vuDispObject(&theObj));
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	}
	regObjFinish(theGen);
    }
}


/*ARGSUSED*/
vemStatus whereCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine reports information about the position where
 * the command was invoked.  If objects are provided,  the
 * command will enumerate those objects.
 */
{
    vemOneArg *thisArg;
    lsGen lgen;
    octObject bag, theObj;
    octGenerator gen;

    if (spot == (vemPoint *) 0) {
	vemMessage("VEM console window\n", MSG_NOLOG|MSG_DISP);
    } else {
	show_facet(spot->theFct);
	show_alt(spot->theWin);
	vemMessage("\n", MSG_NOLOG|MSG_DISP);
	show_location(spot);
	if (lsLength(cmdList) == 0) {
	    show_obj(spot);
	} else {
	    lgen = lsStart(cmdList);
	    while (lsNext(lgen, (Pointer *) &thisArg, LS_NH) == LS_OK) {
		if (thisArg->argType == VEM_OBJ_ARG) {
		    bag.objectId = thisArg->argData.vemObjArg.theBag;
		    VEM_OCTCKRVAL(octGetById(&bag), VEM_CORRUPT);
		    VEM_OCTCKRVAL(octInitGenContents(&bag, OCT_ALL_MASK, &gen),
				  VEM_CORRUPT);
		    while (octGenerate(&gen, &theObj) == OCT_OK) {
			sprintf(errMsgArea, "%s\n", vuDispObject(&theObj));
			vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
		    }
		} else {
		    show_obj(spot);
		}
	    }
	}
    }
    return VEM_ARGRESP;
}
	


/*ARGSUSED*/
vemStatus saveCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command saves the buffer associated with the window
 * which is indicated by 'spot'.
 */
{
    octObject saveFacet;

    saveFacet.objectId = spot->theFct;
    if (octGetById(&saveFacet) != OCT_OK) {
	vemMsg(MSG_C, "Can't get facet to save it:\n  %s\n", octErrorString());
	return VEM_ARGRESP;
    }
    if (bufSave(spot->theFct) != VEM_OK) {
	vemMessage("Facet was not saved.\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    return VEM_ARGRESP;
}

/*ARGSUSED*/
vemStatus writeCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command write the buffer associated with the window
 * (which is indicated by 'spot') to an alternate facet
 */
{
    octObject saveFacet, writeFacet;
    vemOneArg *oneArg;

    if (lsLength(cmdList) == 0) {
	vemMsg(MSG_A, "format: \"cell:view:facet\" : write-window\n");
	return VEM_CORRUPT;
    }

    lsLastItem(cmdList, (lsGeneric *) &oneArg, LS_NH);
    if (oneArg->argType != VEM_TEXT_ARG) {
	vemMsg(MSG_A, "format: \"cell:view:facet\" : write-window\n");
	return VEM_CORRUPT;
    }

    ohUnpackDefaults(&writeFacet, "w", ":physical:contents");
    if (ohUnpackFacetName(&writeFacet, oneArg->argData.vemTextArg) != OCT_OK) {
	vemMsg(MSG_A, "cannot parse cell specification: `%s'\n",
	       oneArg->argData.vemTextArg);
	vemMsg(MSG_A, "format: \"cell:view:facet\" : write-window\n");
	return VEM_CORRUPT;
    }

    saveFacet.objectId = spot->theFct;

    if (octGetById(&saveFacet) != OCT_OK) {
	vemMsg(MSG_C, "Can't get facet to save it:\n  %s\n", octErrorString());
	return VEM_ARGRESP;
    }

    if (octWriteFacet(&writeFacet, &saveFacet) != OCT_OK) {
	vemMessage("Facet was not saved.\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    return VEM_ARGRESP;
}

/*ARGSUSED*/
vemStatus saveAllCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine saves all currently active buffers.  It takes
 * no arguments and checks to make sure there are none.
 */
{
    if (lsLength(cmdList) != 0) {
	vemMessage("save-all takes no arguments.\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    if (bufSaveAll() != VEM_OK) {
	vemMessage("Could not save all buffers.\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}


vemStatus revertParse(spot, cmdList, cnts, itrf)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
octObject *cnts;		/* Contents facet           */
octObject *itrf;		/* Interface facet (if any) */
/*
 * Parses arguments for revertCmd.  Makes sure there are no
 * arguments in the argument list.  Returns the contents
 * and interface facets associated with `spot'.
 */
{
    wnOpts opts;

    if (lsLength(cmdList) > 0) {
	vemMessage("re-read takes no arguments\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    cnts->objectId = spot->theFct;
    if (octGetById(cnts) != OCT_OK) {
	vemMsg(MSG_A, "Unable to get facet of window:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    /* Get interface facet if it exists */
    itrf->objectId = oct_null_id;
    if (wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	/* Reread the interface too, unless this is already the interface. */
	if ( strcmp(cnts->contents.facet.facet, opts.interface ) ) {
	    *itrf = *cnts;
	    itrf->contents.facet.facet = opts.interface;
	    /* The BUFMUSTEXIST was also set. However, if the 
	     * interface facet did not exist, OCT would be confused.
	     * At least, in this way, we force the creation of
	     * the interface facet, which is the right thing to do any way.
	     */
	    if (bufOpen(itrf, BUFSILENT) != VEM_OK) {
		itrf->objectId = oct_null_id;
	    }
	}
    }
    return VEM_OK;
}

static int revertConf(theFacet)
octObject *theFacet;		/* Facet for re-reading */
/*
 * Presents a confirmation dialog box before reverting a facet.
 * Returns a non-zero value if it is ok to proceed.
 */
{
    errMsgArea[0] = '\0';
    STRCONCAT(errMsgArea, "Reading the facet from disk will wipe\n");
    STRCONCAT(errMsgArea, "out any changes you have made.  Do you\n");
    STRCONCAT(errMsgArea, "really want to re-read\n'");
    STRCONCAT(errMsgArea, theFacet->contents.facet.cell);
    STRCONCAT(errMsgArea, ":");
    STRCONCAT(errMsgArea, theFacet->contents.facet.view);
    STRCONCAT(errMsgArea, "'?");
    if (dmConfirm("Warning", errMsgArea, "No", "Yes") == VEM_OK) {
	vemMessage("re-read aborted\n", MSG_NOLOG|MSG_DISP);
	return 0;
    } else {
	return 1;
    }
}

/*ARGSUSED*/
vemStatus revertCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command reverts the buffer associated with 'spot'.
 */
{
    octObject theFacet, interFacet;
    vemStatus ret;
    int proceed;

    if ( (ret =
	  revertParse(spot, cmdList, &theFacet, &interFacet)) == VEM_OK) {
	/* Ask for confirmation */
	proceed = revertConf(&theFacet);
	if (proceed) {
	    if ((ret = doRevert(&theFacet)) == VEM_OK) {
		if (interFacet.objectId != oct_null_id) {
		    ret = doRevert(&interFacet);
		}
	    }
	}
    }
    return ret;
}


/*ARGSUSED*/
vemStatus
revertNoConf(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command reverts the buffer associated with 'spot'.
 * This is a special version that doesn't ask for confirmation.
 */
{
    octObject theFacet, interFacet;
    vemStatus ret;

    if ( (ret = revertParse(spot, cmdList, &theFacet, &interFacet))
	== VEM_OK) {
	if ((ret = doRevert(&theFacet)) == VEM_OK) {
	    if (interFacet.objectId != oct_null_id) {
		ret = doRevert(&interFacet);
	    }
	}
    }
    return ret;
}



static vemStatus doRevert(fct)
octObject *fct;			/* Corresponding facet */
/*
 * This routine carries out the re-read of the specified buffer.
 * The appropriate redraws are done to insure it is up to date.
 */
{
    struct octBox oldbb, bb;

    if (octBB(fct, &oldbb) != OCT_OK) {
	/* Useless message (just confusing user) **
	 * sprintf(errMsgArea, "Unable to get bounding box of facet:\n  %s\n",
	 * octErrorString());
	 * vemMessage(errMsgArea, MSG_DISP); 
	 */
	oldbb.lowerLeft.x = oldbb.lowerLeft.y = oldbb.upperRight.x =
	  oldbb.upperRight.y = 0;
    }
    /* Revert the buffer */
    if (bufRevert(&(fct->objectId)) != VEM_OK) {
	vemMessage("Unable to re-read facet.\n", MSG_DISP);
	return VEM_CORRUPT;
    }
    if (octGetById(fct) != OCT_OK) {
	vemMsg(MSG_A, "Cannot get new version of cell:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    if (octBB(fct, &bb) != OCT_OK) {
	sprintf(errMsgArea, "Unable to get bounding box of facet:\n  %s\n",
		octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	bb.lowerLeft.x = bb.lowerLeft.y = bb.upperRight.x =
	  bb.upperRight.y = 0;
    }
    /* Redraw all windows with the facet - merge the box */
    bb.lowerLeft.x = VEMMIN(bb.lowerLeft.x, oldbb.lowerLeft.x);
    bb.lowerLeft.y = VEMMIN(bb.lowerLeft.y, oldbb.lowerLeft.y);
    bb.upperRight.x = VEMMAX(bb.upperRight.x, oldbb.upperRight.x);
    bb.upperRight.y = VEMMAX(bb.upperRight.y, oldbb.upperRight.y);
#ifndef OLD_DISPLAY
    wnQRedisplay(fct->objectId, &bb);
#else
    wnQRegion(fct->objectId, &bb);
#endif
    return VEM_OK;
}



/*ARGSUSED*/
static enum st_retval tblRevert(key, value, arg)
char *key, *value, *arg;
/*
 * Passed to st_foreach to re-read all facets stored in
 * a hash table by id.  The key for the item is it's
 * oct id.  The value of the item is its buffer.  `arg'
 * is unused.
 */
{
    octObject fct;

    fct.objectId = (octId) key;
    VEM_OCTCKRVAL(octGetById(&fct), ST_CONTINUE);
    /* Now actually re-read this one */
    vemMsg(MSG_C, "Rereading `%s:%s:%s'.\n",
	   fct.contents.facet.cell,
	   fct.contents.facet.view,
	   fct.contents.facet.facet);
    if (doRevert(&fct) == VEM_OK) {
	return ST_CONTINUE;
    } else {
	return ST_STOP;
    }
}

vemStatus deepRRCmd(spot, cmdList)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
/*
 * This command has two forms:  with no arguments it re-reads
 * the current buffer recursively all the way down to the
 * lowest cell.  With an objects argument,  it will re-read
 * the master of each instance in the set (both interface
 * and contents).  The routine first builds a hash table
 * of unique cells in the heirarchy then re-reads them.
 */
{
    octObject theFacet, bag;
    vemOneArg *arg;
    st_table *tbl;
    vemStatus retval;
    int expert = vemExpert( spot );

    theFacet.objectId = spot->theFct;
    if (octGetById(&theFacet) != OCT_OK) {
	vemMsg(MSG_A, "Unable to get facet of window:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    if (lsLength(cmdList) == 0) {
	/* Full recursive re-read */
	if (!expert) {
	    errMsgArea[0] = '\0';
	    STRCONCAT(errMsgArea, "Recursively re-reading the facet from disk\n");
	    STRCONCAT(errMsgArea, "will wipe out all changes to it and any\n");
	    STRCONCAT(errMsgArea, "changes made to subcells under it.\n\nDo you");
	    STRCONCAT(errMsgArea, "really want to recursively re-read the cell\n`");
	    STRCONCAT(errMsgArea, theFacet.contents.facet.cell);
	    STRCONCAT(errMsgArea, ":");
	    STRCONCAT(errMsgArea, theFacet.contents.facet.view);
	    STRCONCAT(errMsgArea, "'?");
	    if (dmConfirm("Warning", errMsgArea, "No", "Yes") == VEM_OK) {
		vemMessage("recursive re-read aborted\n", MSG_NOLOG|MSG_DISP);
		return VEM_OK;
	    }
	}
	tbl = st_init_table(st_numcmp, st_numhash);
	retval = recurRevert(&theFacet, tbl);
	st_foreach(tbl, tblRevert, (char *) 0);
	st_free_table(tbl);
	return retval;
    } else if ((lsLength(cmdList) == 1) &&
	       (lsFirstItem(cmdList, (Pointer *) &arg, LS_NH) == LS_OK) &&
	       (arg->argType == VEM_OBJ_ARG)) {
	/* Selective re-read */
	if (!expert) {
	    errMsgArea[0] = '\0';
	    STRCONCAT(errMsgArea, "Both the contents and the interface facets\n");
	    STRCONCAT(errMsgArea, "of the instances you have selected will be\n");
	    STRCONCAT(errMsgArea, "re-read.  You will lose any changes made to\n");
	    STRCONCAT(errMsgArea, "these cells.  Are you sure?");
	    if (dmConfirm("Warning", errMsgArea, "No", "Yes") == VEM_OK) {
		vemMessage("recursive re-read aborted\n", MSG_NOLOG|MSG_DISP);
		return VEM_OK;
	    }
	}
	/* Get bag */
	bag.objectId = arg->argData.vemObjArg.theBag;
	VEM_OCTCKRVAL(octGetById(&bag), VEM_CORRUPT);
	return revInsts(&bag);
    } else {
	/* Incorrect format */
	vemMessage("format: [objects] : deep-reread\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
}


static vemStatus revInsts(instbag)
octObject *instbag;		/* Bag containing instance for revert */
/*
 * This routine re-reads the contents and interface facets of all
 * masters of the instances in the bag `instbag'.
 */
{
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    octGenerator gen;
    octObject inst, master;

    VEM_OCTCKRVAL(octInitGenContents(instbag, OCT_INSTANCE_MASK, &gen),
		  VEM_CORRUPT);
    master.type = OCT_FACET;
    master.contents.facet.version = OCT_CURRENT_VERSION;
    while (octGenerate(&gen, &inst) == OCT_OK) {
	/* Open masters and re-read */
	master.contents.facet.facet = "contents";
	if (bufOpenMaster(&inst, &master, 0) == VEM_OK) {
	    if (!st_lookup(tbl, (char *) master.objectId, (char **) 0)) {
		st_insert(tbl, (char *) master.objectId, (char *) 0);
	    }
	}
	master.contents.facet.facet = "interface";
	if (bufOpenMaster(&inst, &master, 0) == VEM_OK) {
	    if (!st_lookup(tbl, (char *) master.objectId, (char **) 0)) {
		st_insert(tbl, (char *) master.objectId, (char *) 0);
	    }
	}
    }
    st_foreach(tbl, tblRevert, (char *) 0);
    st_free_table(tbl);
    return VEM_OK;
}



static vemStatus recurRevert(fct, tbl)
octObject *fct;			/* Corresponding facet           */
st_table *tbl;			/* Table of re-read facets       */
/*
 * This routine generates all instances in `fct' and calls itself
 * recursively to re-read both their contents and their interface
 * facets.  The routine does not actually re-read the cells,  but
 * installs them in `tbl'.  This insures they will be re-read only
 * once.
 */
{
    octGenerator gen;
    octObject inst, master;

    if (!st_lookup(tbl, (char *) fct->objectId, (char **) 0)) {
	/* Add it to the table */
	st_insert(tbl, (char *) fct->objectId, (char *) 0);
	VEM_OCTCKRVAL(octInitGenContents(fct, OCT_INSTANCE_MASK, &gen),
		      VEM_CORRUPT);
	master.type = OCT_FACET;
	master.contents.facet.version = OCT_CURRENT_VERSION;
	while (octGenerate(&gen, &inst) == OCT_OK) {
	    /* Open masters and re-read */
	    master.contents.facet.facet = "contents";
	    if (bufOpenMaster(&inst, &master, 0) == VEM_OK) {
		if (recurRevert(&master, tbl) != VEM_OK) {
		    return VEM_CORRUPT;
		}
	    }
	    master.contents.facet.facet = "interface";
	    if (bufOpenMaster(&inst, &master, 0) == VEM_OK) {
		if (recurRevert(&master, tbl) != VEM_OK) {
		    return VEM_CORRUPT;
		}
	    }
	}
    }
    return VEM_OK;
}


vemStatus
replWinCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command replaces the contents of a window with a new
 * facet.  The command can either take a point or a text argument.
 * If it is a point,  the facet of the window containing the spot
 * becomes the facet of the window containing the point.  If it
 * is a text argument,  it is interpreted as a cell, view, facet
 * specification for the replacing cell.
 */
{
    vemOneArg *firstArg;
    octObject theFacet, spotFacet;
    struct octBox bb, newExtent;
    STR winName;
    int winLen, gridSize, retCode;

    if (lsLength(cmdList) != 1) {
	vemMessage("format: [\"cell view {facet version}\"] switch-facet\n",
		   MSG_NOLOG|MSG_DISP);
	vemMessage("        [point] switch-facet\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    if (spot == (vemPoint *) 0) {
	vemMessage("You can't replace the facet of the console\n", MSG_DISP);
	return VEM_CORRUPT;
    }
    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);

    if (firstArg->argType == VEM_TEXT_ARG) {
	/* Open by text specification */
	spotFacet.objectId = spot->theFct;
	if (octGetById(&spotFacet) != OCT_OK) {
	    ohUnpackDefaults(&theFacet, "a", ":physical:contents");
	} else {
	    theFacet = spotFacet;
	}
	if (ohUnpackFacetName(&theFacet, firstArg->argData.vemTextArg) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot parse cell specification: `%s'\n",
		   firstArg->argData.vemTextArg);
	}
	/* Open it up */
	if (bufOpen(&theFacet, 0) != VEM_OK) return VEM_CORRUPT;
    } else if ((firstArg->argType == VEM_POINT_ARG) && (firstArg->length == 1)) {
	theFacet.objectId = firstArg->argData.points[0].theFct;
	if (octGetById(&theFacet) != VEM_OK) {
	    vemMsg(MSG_A, "Can't get new facet:\n  %s\n", octErrorString());
	    return VEM_CORRUPT;
	}
    } else {
	vemMessage("format: [\"cell view {facet version}\"] switch-facet\n",
		   MSG_NOLOG|MSG_DISP);
	vemMessage("        [point] switch-facet\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    /* Figure out extent of buffer */
    dfGetInt("lambda", &gridSize);
    if ((retCode = octBB(&theFacet, &bb)) != OCT_OK) {
	sprintf(errMsgArea,
		"Can't get BB for switched facet(%d):\n  %s\n",
		retCode, octErrorString());
	vemMessage(errMsgArea, 0);
	/* New computation required here */
	newExtent.lowerLeft.x = -(gridSize * 20);
	newExtent.lowerLeft.y = -(gridSize * 20);
	newExtent.upperRight.x = gridSize * 20;
	newExtent.upperRight.y = gridSize * 20;
    } else {
	int extraWidth, extraHeight;

	/* Adds 5% slack */
	extraWidth = VEMMAX((bb.upperRight.x - bb.lowerLeft.x) / 20, gridSize);
	extraHeight = VEMMAX((bb.upperRight.y - bb.lowerLeft.y) / 20, gridSize);
	newExtent.lowerLeft.x  =  bb.lowerLeft.x - extraWidth;
	newExtent.lowerLeft.y  =  bb.lowerLeft.y - extraHeight;
	newExtent.upperRight.x =  bb.upperRight.x + extraWidth;
	newExtent.upperRight.y =  bb.upperRight.y + extraHeight;
    }

    /* Ready to do the replacement */
    if (wnNewBuf(spot->theWin, theFacet.objectId, &newExtent) != VEM_OK) {
	vemMessage("Can't replace buffer\n", MSG_DISP);
	return VEM_OK;
    }
    /* Replace the name of the window */
    XFetchName(xv_disp(), spot->theWin, &winName);
    XFree(winName);
    winLen = STRLEN(theFacet.contents.facet.cell) +
      STRLEN(theFacet.contents.facet.view) + 4;
    winName = VEMARRAYALLOC(char, winLen);
    if (STRCOMP(theFacet.contents.facet.facet, "contents") == 0) {
	sprintf(winName, " %s:%s ", 
		theFacet.contents.facet.cell, theFacet.contents.facet.view);
    } else {
	sprintf(winName, " %s:%s ",
		theFacet.contents.facet.cell,
		theFacet.contents.facet.view);
	  /* FIXME:  The sprintf() above used to have
	   * 'theFacet.contents.facet.facet' as the last arg, but it
	   * was unused by the format
           */
    }
    XStoreName(xv_disp(), spot->theWin, winName);
    wnQWindow(spot->theWin);
    return VEM_OK;
}
    


static vemStatus des_window(win)
Window win;
/* Destroys window `win' (passed to wnBufRelease */
{
    Display *disp = xv_disp();

    XDestroyWindow(disp, win);
    return VEM_OK;
}

vemStatus
killBufCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command closes the buffer associated with the window
 * passed with 'spot'.  Note this command destroys all windows
 * which look in on the buffer.  It takes one optional
 * string argument which is the name of the facet to kill.
 */
{
    vemOneArg *oneArg;
    octObject theFacet;
    int expert = vemExpert( spot );

    if (lsLength(cmdList) > 1) {
	vemMessage("format:  [\"cell view {facet version}\"] kill-buffer\n",
		   MSG_NOLOG|MSG_DISP);
    }
    if (lsLength(cmdList) == 1) {
	/* A facet name has been supplied */
	lsFirstItem(cmdList, (lsGeneric *) &oneArg, LS_NH);
	ohUnpackDefaults(&theFacet, "a", "::");
	if (ohUnpackFacetName(&theFacet, oneArg->argData.vemTextArg) != OCT_OK) {
	    vemMsg(MSG_A, "Cannot parse cell specification: `%s'\n",
		   oneArg->argData.vemTextArg);
	    return VEM_CORRUPT;
	}
	if (bufOpen(&theFacet, 0) != VEM_OK) return VEM_CORRUPT;
    } else {
	/* No facet name given assume the one in this window */
	if (spot == (vemPoint *) 0) {
	    vemMessage("Cannot kill the console window\n", MSG_DISP);
	    return VEM_CORRUPT;
	}
	theFacet.objectId = spot->theFct;
	if (octGetById(&theFacet) != OCT_OK) {
	    vemMsg(MSG_A, "Unable to get doomed facet:\n  %s\n", octErrorString());
	    return VEM_CORRUPT;
	}
    }
    /* Ask for confirmation */
    if (!expert) {
	sprintf(errMsgArea,
		"Do you really want to kill the facet\n'%s:%s'?",
		theFacet.contents.facet.cell, theFacet.contents.facet.view);
	if (dmConfirm("Warning", errMsgArea, "No", "Yes") == VEM_OK) {
	    vemMessage("kill-buffer aborted\n", MSG_DISP);
	    return VEM_OK;
	}
    }
    /* Ok - we are ready.  Step one:  kill all windows containing the buffer */
    wnBufRelease(theFacet.objectId, des_window);
    /* Flush the buffer out */
    if (bufClose(theFacet.objectId) != VEM_OK) {
	vemMessage("Unable to free facet from memory\n", MSG_DISP);
	return VEM_CORRUPT;
    }
    return VEM_OK;
}




/*ARGSUSED*/
vemStatus
expandCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This routine toggles the expand mode of the window where the
 * command was issued.  It does not effect the argument list.
 */
{
    wnOpts opts;

    VEM_CKRVAL(wnGetInfo(spot->theWin, &opts, (octId *) 0,
			 (octId *) 0, (bdTable *) 0,
			 (struct octBox *) 0) == VEM_OK,
	       "Failed to get options", VEM_CORRUPT);
    opts.disp_options ^= VEM_EXPAND;
    VEM_CKRVAL(wnSetInfo(spot->theWin, &opts, (bdTable *) 0) == VEM_OK,
	       "Failed to set options", VEM_CORRUPT);
    return VEM_ARGMOD;
}




/*ARGSUSED*/
vemStatus
fullCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command zooms the window indicated by 'spot' to an
 * extent which includes all information in the window (plus
 * 5% slack).
 */
{
    octObject theFacet;
    struct octBox bb;
    octCoord extraWidth, extraHeight;

    theFacet.objectId = spot->theFct;
    if (octGetById(&theFacet) != OCT_OK) {
	vemMsg(MSG_A, "Buffer is inaccessible:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    if (octBB(&theFacet, &bb) != OCT_OK) {
	sprintf(errMsgArea, "Can't get bounding box:\n  %s\n",
		octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return VEM_CORRUPT;
    }
    /* Add 5% slack */
    extraWidth = (bb.upperRight.x - bb.lowerLeft.x) / 20;
    extraHeight = (bb.upperRight.y - bb.lowerLeft.y) / 20;
    bb.lowerLeft.x  -= extraWidth;
    bb.lowerLeft.y  -= extraHeight;
    bb.upperRight.x += extraWidth;
    bb.upperRight.y += extraHeight;
    wnZoom(spot->theWin, &bb);
    wnQWindow(spot->theWin);
    return VEM_ARGRESP;
}


vemStatus
altCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command makes the facet under the spot the alternate
 * buffer for the window under the point in the argument list.
 * The origin of the alternate facet will be aligned with this
 * point.  If the last argument is text,  it will be interpreted as
 * the alternate facet instead of the one under the spot.
 */
{
    vemOneArg *oneArg, *lastArg;
    octObject altFacet, spotFacet;
    struct octPoint altPnt;

    if ((lsLength(cmdList) < 1) || (lsLength(cmdList) > 2)) {
	vemMessage("format: [point] [\"facet\"] alt-buffer\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    lsFirstItem(cmdList, (lsGeneric *) &oneArg, LS_NH);
    if ((oneArg->argType != VEM_POINT_ARG) ||
	(oneArg->length != 1))
      {
	  vemMessage("format: [point] [\"facet\"] alt-buffer\n", MSG_NOLOG|MSG_DISP);
	  return VEM_CORRUPT;
      }
    if (lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH) == LS_OK) {
	if (lastArg->argType == VEM_TEXT_ARG) {
	    spotFacet.objectId = spot->theFct;
	    if (octGetById(&spotFacet) != OCT_OK) {
		ohUnpackDefaults(&altFacet, "a", ":physical:contents");
	    } else {
		altFacet = spotFacet;
	    }
	    if (ohUnpackFacetName(&altFacet, lastArg->argData.vemTextArg) != OCT_OK) {
		vemMsg(MSG_A, "Cannot parse cell specification: `%s'\n",
		       lastArg->argData.vemTextArg);
		return VEM_CORRUPT;
	    }
	    if (bufOpen(&altFacet, 0) != VEM_OK) {
		vemMessage("Cannot open alternate buffer\n", MSG_DISP);
		return VEM_CORRUPT;
	    }
	} else {
	    vemMessage("format: [point] [\"facet\"] alt-buffer\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_CORRUPT;
	}
    } else {
	altFacet.objectId = spot->theFct;
    }

    /* Ok,  we are ready */
    altPnt.x = altPnt.y = 0;
    if (wnAltBuf(oneArg->argData.points[0].theWin,
		 altFacet.objectId, &altPnt,
		 &(oneArg->argData.points[0].thePoint)) != VEM_OK)
      {
	  vemMessage("wnAltBuf failed\n", MSG_NOLOG|MSG_DISP);
	  return VEM_CORRUPT;
      } else {
	  /* Success - arrange a redraw of the window */
	  wnQWindow(oneArg->argData.points[0].theWin);
	  return VEM_OK;
      }
}
		     


vemStatus
swapCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command swaps the alternate and current buffers of the
 * window under 'spot'.  It takes no arguments.
 */
{

    if (lsLength(cmdList) != 0) {
	vemMessage("swap-bufs takes no arguments\n", MSG_DISP|MSG_NOLOG);
	return VEM_CORRUPT;
    }
    if (spot == (vemPoint *) 0) {
	vemMessage("Console window has no alternate buffer\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    switch (wnSwapBuf(spot->theWin)) {
    case VEM_CORRUPT:
	vemMessage("Window has no alternate buffer\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    case VEM_OK:
	wnQWindow(spot->theWin);
	return VEM_OK;
    default:
	vemMessage("Unknown error\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
}


#define MAXNAMELEN 255

vemStatus pushInstanceCmd(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * push into the master of the instance under the spot
 *
 * Rick Spickelmier
 */
{
    octObject object, master;
    Window newWindow;
    vemStatus status;
    vemOneArg *arg;
    char *facetType;

    if (spot == (vemPoint *) 0) {
	vemMessage("No instances in a non-graphics window\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList) > 1) {
	vemMessage("[\"facet\"] : push-master\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList) == 1) {
	lsLastItem(cmdList, (lsGeneric *) &arg, LS_NH);
	if (arg->argType != VEM_TEXT_ARG) {
	    vemMessage("[\"facet\"] : push-master\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	lsDelEnd(cmdList, (lsGeneric *) &arg);
	facetType = arg->argData.vemTextArg;
    } else {
	facetType = "contents";
    }

    status = vuFindSpot(spot, &object, OCT_INSTANCE_MASK);

    if (status == VEM_NOSELECT) {
	return VEM_ARGMOD;
    }

    if (status == VEM_CANTFIND) {
	vemMessage("No instance under the cursor\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    /* Open using bufOpenMaster */
    master.type = OCT_FACET;
    master.contents.facet.facet = facetType;
    master.contents.facet.mode = "a";
    master.contents.facet.version = OCT_CURRENT_VERSION;
    if (vuMasterWindow(&newWindow, &master, &object, (STR) 0) != VEM_OK) {
	return VEM_ARGRESP;
    } else {
	return VEM_ARGMOD;
    }
}



static vemStatus widthFormat(msg, retval)
STR msg;			/* Message to display */
vemStatus retval;		/* Returned value     */
/*
 * This routine displays a format message for the path width
 * setting command and returns `retval'.  It also displays `msg'.
 */
{
    (void) sprintf(errMsgArea, "Error: %s\n", msg);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    vemMessage("Format: [points lines or boxes] {\"layer\"} : set-path-width\n",
	       MSG_NOLOG|MSG_DISP);
    return retval;
}


vemStatus pathWidthCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command sets the current path width for a given
 * layer.  It takes one argument which may be points, lines, 
 * boxes, or text.  For points and lines,  the length must be two.
 * The path width is set to the maximum manhattan distance
 * between the points.  For boxes,  only one box is allowed
 * and the distance is the larger of the two dimensions.
 * For text,  the string should contain the path width in
 * lambda.
 */
{
    vemOneArg *theArg;
    wnOpts info;
    octObject spotFacet, targetLayer;
    int len, temp, gridUnits, pathWidth = -1;
    vemStatus stat_code;

    /* Get the facet of spot */
    spotFacet.objectId = spot->theFct;
    if (octGetById(&spotFacet) != OCT_OK)
      return widthFormat("Can't get buffer", VEM_CORRUPT);
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    theArg = (vemOneArg *) 0;
    len = lsLength(cmdList);
    if (len > 2) {
	return widthFormat("Too many arguments", VEM_CORRUPT);
    } else if (len > 0) {
	lsFirstItem(cmdList, (lsGeneric *) &theArg, LS_NH);
    }
    if (len == 1) {
	if (theArg->argType == VEM_TEXT_ARG) {
	    /* Remove it */
	    lsDelBegin(cmdList, (lsGeneric *) &theArg);
	}

	if (theArg->argType == VEM_OBJ_ARG) {
	    return widthFormat("Can't get buffer", VEM_CORRUPT);
	}
    }

    /* Find the layer */
    if (vuParseLayer(spot, cmdList, &spotFacet, &targetLayer) != VEM_OK) 
      return widthFormat("No layer specified", VEM_ARGRESP);
    if (octGetByName(&spotFacet, &targetLayer) < OCT_OK) {
	if (tapIsLayerDefined(&spotFacet, targetLayer.contents.layer.name) != OCT_OK) {
	    (void) sprintf(errMsgArea,
			   "Layer `%s' is not in the technology",
			   targetLayer.contents.layer.name);
	    return widthFormat(errMsgArea, VEM_ARGRESP);
	}
	if (octCreate(&spotFacet, &targetLayer) < OCT_OK) {
	    (void) sprintf(errMsgArea,
			   "Can't create layer `%s'",
			   targetLayer.contents.layer.name);
	    return widthFormat(errMsgArea, VEM_ARGRESP);
	}
	/* Force update of buffer layer information */
	bufChanges( );
    }
    
    if (wnGetInfo(spot->theWin, &info, (octId *) 0, (octId *) 0,
		  (bdTable *) 0, (struct octBox *) 0) == VEM_OK) {
	gridUnits = info.lambda;
    } else {
	dfGetInt("lambda", &gridUnits);
    }

    if (theArg) {
	switch (theArg->argType) {
	case VEM_POINT_ARG:
	case VEM_LINE_ARG:
	case VEM_BOX_ARG:
	    if (((theArg->argType != VEM_BOX_ARG) && (theArg->length != 2)) ||
		((theArg->argType == VEM_BOX_ARG) && (theArg->length != 1)))
	      {
		  vemMessage("Only two points, one line, or one box is allowed\n",
			     MSG_NOLOG|MSG_DISP);
		  return VEM_CORRUPT;
	      }
	    temp = VEMABS(theArg->argData.points[0].thePoint.x -
			  theArg->argData.points[1].thePoint.x);
	    pathWidth = VEMMAX(temp,
			       VEMABS(theArg->argData.points[0].thePoint.y -
				      theArg->argData.points[1].thePoint.y));
	    break;
	case VEM_TEXT_ARG:
	    if (sscanf(theArg->argData.vemTextArg, "%d", &temp) != 1) {
		vemMessage("Text argument must be a path width\n",
			   MSG_NOLOG|MSG_DISP);
		return VEM_CORRUPT;
	    }
	    pathWidth = temp * gridUnits;
	    break;
	default:
	    return widthFormat( "Wrong Argument", VEM_ARGRESP );
	}
    } else {
	/* This will set it back to the minimum */
	pathWidth = -1;
    }

    if (theArg && (theArg->argType != VEM_TEXT_ARG)) {
	stat_code = bufSetWidth(theArg->argData.points[0].theFct,
				&targetLayer, pathWidth);
    } else {
	stat_code = bufSetWidth(spot->theFct, &targetLayer, pathWidth);
    }
    if (stat_code == VEM_CANTFIND) 
      return widthFormat("Can't set path width", VEM_CORRUPT);

    /* Now get the width */
    pathWidth = bufPathWidth(spot->theFct, &targetLayer);

    (void) sprintf(errMsgArea,
		   "New path width for layer %s is %d lambda (%d units)\n",
		   targetLayer.contents.layer.name,
		   pathWidth / gridUnits, pathWidth);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    if (stat_code == VEM_TOOSMALL) {
	vemMessage("WARNING: path width is less than minimum width for layer\n",
		   MSG_NOLOG|MSG_DISP);
    }
    return VEM_OK;
}



static int unq_check(container, obj, new_name, old_name)
octObject *container;		/* Container to look through */
octObject *obj;			/* Old object                */
STR new_name;			/* New object name           */
STR old_name;			/* Old object name           */
/*
 * This routine trys to locate by name `obj' in `container'.
 * If one already exists,  a dialog is posted to confirm that
 * the user wants a non-unique name.
 */
{
    octObject copy;
    char n1[1024], n2[1024], buf[1024];

    copy = *obj;
    if (octGetByName(container, &copy) == OCT_OK) {
	/* Post dialog */
	if (STRLEN(new_name) > 0) {
	    sprintf(n1, "object named `%s'", new_name);
	} else {
	    sprintf(n1, "unnamed object");
	}
	if (STRLEN(old_name) > 0) {
	    sprintf(n2, "object named `%s'", old_name);
	} else {
	    sprintf(n2, "unnamed object");
	}
	sprintf(buf,
		"There is already an\n%s.  Do you\nreally want\
 to rename the\n%s?",
		n1, n2);
	if (dmConfirm("Non-unique name", buf, "Yes", "No") == VEM_OK) {
	    return 1;
	} else {
	    return 0;
	}
    }
    return 1;
}

static vemStatus nameFormat(msg, code)
STR msg;			/* Message to display */
vemStatus code;			/* Code to return     */
/*
 * Outputs format message for naming command to console then
 * returns `code'.
 */
{
    sprintf(errMsgArea, "Error: %s\n", msg);
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    vemMessage("format: [objects][pnts][boxes][lines] \"name\" : name-object\n",
	       MSG_NOLOG|MSG_DISP);
    return code;
}

static vemStatus nameObject(fct, obj, new_name, old_name, obj_type)
octObject *fct;			/* Facet of objects           */
octObject *obj;			/* Object to rename           */
STR new_name;			/* Name to give it            */
STR *old_name;			/* Old object name (returned) */
STR *obj_type;			/* Returned string type       */
/*
 * This routine sets the name field in `obj' to `new_name' and modifies
 * it (puts it into the database).  For convenience,  it returns the
 * old object name and type.  If successful,  it returns VEM_OK.
 * Otherwise,  it returns VEM_FALSE.
 */
{
    static char buf[1024];	/* Old name saving space */
    int is_fct;

    is_fct = (octIsAttached(fct, obj) == OCT_OK);
    switch (obj->type) {
    case OCT_TERM:
	*obj_type = "Terminal";
	STRMOVE(buf, obj->contents.term.name);
	obj->contents.term.name = new_name;
	break;
    case OCT_NET:
	*obj_type = "Net";
	STRMOVE(buf, obj->contents.net.name);
	obj->contents.net.name = new_name;
	break;
    case OCT_INSTANCE:
	*obj_type = "Instance";
	STRMOVE(buf, obj->contents.instance.name);
	obj->contents.instance.name = new_name;
	break;
    case OCT_PROP:
	*obj_type = "Property";
	STRMOVE(buf, obj->contents.prop.name);
	obj->contents.prop.name = new_name;
	break;
    case OCT_BAG:
	*obj_type = "Bag";
	STRMOVE(buf, obj->contents.bag.name);
	obj->contents.bag.name = new_name;
	break;
    default:
	vemFail("Unknown object type", __FILE__, __LINE__);
	break;
    }
    *old_name = buf;
    if (is_fct && !unq_check(fct, obj, new_name, *old_name)) return VEM_FALSE;
    if (octModify(obj) == OCT_OK) return VEM_OK;
    return VEM_FALSE;
}




#define NAMED_OBJECTS (OCT_TERM_MASK | OCT_NET_MASK | OCT_INSTANCE_MASK | \
		       OCT_PROP_MASK | OCT_BAG_MASK )

vemStatus nameObjCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command names all of the objects selected in the
 * first argument to the name given in the last argument.
 * A report about each object renamed is output to the
 * console.  Only objects with names may be named.
 */
{
    octObject nameSetBag, next_obj, fct;
    octGenerator gen;
    octId targetFctId;
    vemOneArg *lastArg;
    STR oldName, objType;
    int len, bad_count, cnt;
    vemStatus retCode;

    len = lsLength(cmdList);
    if (len < 1) return nameFormat("Too few arguments", VEM_CORRUPT);
    if (vuCkStyle(spot, cmdList) != VEM_OK) return VEM_ARGRESP;

    /* Parse argument list */
    if ((retCode = vuObjArgParse(spot, cmdList,
				 &targetFctId, &nameSetBag, 0)) != VEM_OK)
      return nameFormat("Syntax error", retCode);

    /* Discover the name for the objects */
    VEM_CKRVAL(lsLastItem(cmdList, (lsGeneric *) &lastArg, LS_NH) == VEM_OK,
	       "Failed to find object set name", VEM_CORRUPT);
    if (lastArg->argType == VEM_TEXT_ARG) {
	VEM_OCTCKRVAL(octInitGenContents(&nameSetBag, NAMED_OBJECTS, &gen),
		      VEM_CORRUPT);
	octGetFacet(&nameSetBag, &fct);
	bad_count = cnt = 0;
	while (octGenerate(&gen, &next_obj) == OCT_OK) {
	    if (nameObject(&fct, &next_obj, lastArg->argData.vemTextArg,
			   &oldName, &objType)==VEM_OK)
	      {
		  sprintf(errMsgArea, "%s named `%s' renamed to `%s'\n",
			  objType, oldName, lastArg->argData.vemTextArg);
		  vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	      } else {
		  bad_count++;
	      }
	    cnt++;
	}
	if (bad_count > 0) {
	    sprintf(errMsgArea, "%d objects not renamed\n",
		    bad_count);
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	} else if (cnt == 0) {
	    vemMessage("No nameable objects found\n", MSG_DISP);
	}
    } else {
	if (nameSetBag.objectId)
	  VEM_OCTCKRVAL(octDelete(&nameSetBag), VEM_CORRUPT);
	return nameFormat("No name specified", VEM_ARGRESP);
    }
    if (nameSetBag.objectId) VEM_OCTCKRVAL(octDelete(&nameSetBag), VEM_CORRUPT);
    return VEM_OK;
}

/*ARGSUSED*/
vemStatus
recoverCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command attempts to recover the contents of a cell with
 * one of the previously saved alternate versions of the cell.
 * It begins by posting a dialog containing all alternates.
 * The user can then choose one and the recovery is carried
 * out.
 */
{
    octObject fct_obj;
    struct octBox old_bb, new_bb;
    char *versions[BUF_MAX_ALTERNATES];
    dmWhichItem items[BUF_MAX_ALTERNATES];
    int num, i, select = 0;
    static char *help_text =
"There are some alternate versions of the cell that are more recent\n\
then the current version of the cell.  This command replaces the contents\n\
of the current cell with one of these alternate versions.  Select the\n\
appropriate version by clicking the mouse in the check box next to the\n\
version name.  Then click in the `Ok' button to recover the facet.\n\
\n\
Generally, there are two possible alternate versions for a cell.  The\n\
`autosave' version is written by vem automatically after a certain number\n\
of changes are done to a cell.  The `crashsave' version is written when\n\
vem detects a serious error.  Note that the `crashsave' version may itself\n\
be corrupt since a serious error occurred just before it was written.\n\
\n\
Note that this operation is highly destructive (i.e. the current contents\n\
of the cell are lost).  If you don't want to recover the cell, press `Cancel'.";

    fct_obj.objectId = spot->theFct;
    if (octGetById(&fct_obj) != OCT_OK) {
	vemMsg(MSG_C, "Unable to get facet:\n  %s\n", octErrorString());
	return VEM_CORRUPT;
    }
    if (octBB(&fct_obj, &old_bb) != OCT_OK) {
	vemMsg(MSG_C, "Unable to get bounding box of old facet:\n  %s\n",
	       octErrorString());
	old_bb.lowerLeft.x = old_bb.lowerLeft.y = old_bb.upperRight.x =
	  old_bb.upperRight.y = 0;
    }
    num = bufCheckRecover(spot->theFct, BUF_MAX_ALTERNATES, versions);
    num = VEMMIN(num, BUF_MAX_ALTERNATES);
    if (num > 0) {
	for (i = 0;  i < num;  i++) {
	    items[i].itemName = versions[i];
	    items[i].userData = (Pointer) 0;
	    items[i].flag = 0;
	}
	if (dmWhichOne("Recover Facet", num, items, &select,
		       (int (*)()) 0, help_text) == VEM_OK) {
	    if (bufRecover(&(spot->theFct), versions[select]) != VEM_OK) {
		vemMsg(MSG_C, "Unable to recover facet.\n");
		return VEM_CORRUPT;
	    }
	    fct_obj.objectId = spot->theFct;
	    if (octGetById(&fct_obj) != OCT_OK) {
		vemMsg(MSG_C, "Unable to get facet\n  %s\n", octErrorString());
		return VEM_CORRUPT;
	    }
	    if (octBB(&fct_obj, &new_bb) != OCT_OK) {
		vemMsg(MSG_C, "Unable to get bounding box of new facet:\n  %s\n",
		       octErrorString());
		old_bb.lowerLeft.x = old_bb.lowerLeft.y = old_bb.upperRight.x =
		  old_bb.upperRight.y = 0;
	    }
	    /* Merge boxes and redraw */
	    new_bb.lowerLeft.x = VEMMIN(new_bb.lowerLeft.x, old_bb.lowerLeft.x);
	    new_bb.lowerLeft.y = VEMMIN(new_bb.lowerLeft.y, old_bb.lowerLeft.y);
	    new_bb.upperRight.x = VEMMAX(new_bb.upperRight.x, old_bb.upperRight.x);
	    new_bb.upperRight.y = VEMMAX(new_bb.upperRight.y, old_bb.upperRight.y);
#ifndef OLD_DISPLAY
	    wnQRedisplay(fct_obj.objectId, &new_bb);
#else
	    wnQRegion(fct_obj.objectId, &new_bb);
#endif
	} else {
	    vemMsg(MSG_C, "Recover facet cancelled.\n");
	}
    } else {
	vemMsg(MSG_C, "There are no more recent versions to recover from.\n");
    }
    return VEM_OK;
}



vemStatus undoCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This is the undo command.  It takes no arguments
 */
{
    if (spot == (vemPoint *) 0) {
	vemMessage("You cannot undo in the non-graphics window\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList) != 0) {
	  vemMessage("Undo takes no arguments\n", MSG_NOLOG|MSG_DISP);
	  return VEM_ARGRESP;
    }

    drsUndo(spot->theFct);
    /* XXX drsLastCommandWasUndo(); */
    return VEM_ARGRESP;
}
