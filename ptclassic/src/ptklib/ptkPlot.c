/*
 * C interface for creating a plotting utility
 *
 * Author: Wei-Jen Huang and E. A. Lee, using some code from
 * David Harrison's xgraph program.
 * Version: $Id$
 */
/*
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/

#include "ptkPlot_defs.h"
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include "compat.h"		/* Pick up decl for sscanf() under sunos4.1.3*/
#include "ieee.h"

/* Number of colors defined in the ptkOptions database */
#define NUMBER_OF_COLORS 12

/* String sizes for single Tcl commands and single integer */
/* in ASCII form, respectively. */
#define SCRATCH_STRING_SIZE 512
#define INT_STRING_SIZE 32

/* Radius of points plotted on the screen */
#define RADIUS 2

/*
 * The calling sequence for plotting should be as follows:
 *     ptkInitPlot
 *         call once for each ptkPlotWin object before calling anything else
 *     ptkCreatePlot
 *         call any number of times to make a new plot.  The old is destroyed.
 *     ptkPlotPoint
 *         call any number of times to add points to the plot.
 *     ptkFreePlot
 *         call once to free allocated memory.
 *
 * Error handling is as follows:
 * The routines ptkCreatePlot and ptkPlotPoint will return 0 if an error occured,
 * and one otherwise.  The error message is then obtained by calling ptkPlotErrorMsg.
 *
 * For each plot, a set of Tcl routines is created:
 *    ptkXYPlotRedrawXXX
 *    ptkXYPlotZoomFitXXX
 *    ptkXYPlotZoomOriginalXXX
 *    ptkXYPlotResizeXXX x1 y1 x2 y2
 *    ptkXYPlotZoomXXX factor
 * where "XXX" is the name of the plot.  Error handling for these routines is through
 * the normal Tcl management.  The arguments to the resize command are the X and Y
 * coordinates (in pixels) of the lower left and upper right of the desired plot area.
 * The argument to the Zoom command is the zoom-out factor.  A factor smaller than one
 * will zoom in.
 */

/* Global scract buffer used for constructing Tcl commands */
static char strTmp[SCRATCH_STRING_SIZE];

static char *errmsg = "";

/* ptkPlotErrorMsg
 * return a pointer to the latest error message.
 */
/* Note that non-ansi C does not have const defined, so we use a macro
 * here, included from somewhere.  Otherwise this file won't compile under
 * sunOS4.1.3 cc
 */
CONST char *ptkPlotErrorMsg() {
  return errmsg;
}

/* roundUp
 * This routine rounds up the given positive number such that
 * it is some power of ten times either 1, 2, or 5.  It is
 * used to find increments for grid lines.  This is a slightly
 * modified version of D. Harrison's roundDown() routine.
 * For zero or negative numbers, return 1e-15
 * For NaN or Infinity, return 1e15.
 */
static double roundUp(val)
double val;
{
    int exponent, idx;
    if (IsNANorINF(val)) return 1e15;
    if (val <= 0) return 1e-15;
    exponent = (int) floor(LOG10(val) + 1e-1);
    if (exponent < 0) {
      for (idx = exponent;  idx < 0; idx++) {
	val *= 10.0;
      }
    } else {
      for (idx = 0;  idx < exponent; idx++) {
	val /= 10.0;
      }
    }
    if (val > 5.0) val = 10.0;
    else if (val > 2.0) val = 5.0;
    else if (val > 1.0) val = 2.0;
    else val = 1.0;
    if (exponent < 0) {
	for (idx = exponent;  idx < 0;  idx++) {
	    val /= 10.0;
	}
    } else {
	for (idx = 0;  idx < exponent;  idx++) {
	    val *= 10.0;
	}
    }
    return val;
}

/* drawAxes
 * Create the axes labeling the plot, with carefully selected tick marks.
 */
static int drawAxes(interp,plotPtr)
    Tcl_Interp *interp;
    ptkPlotWin *plotPtr;
{
    static XFontStruct *fontPtr;
    Tk_Window *win, canvWin;
    char *identifier;
    char *xTitle, *yTitle;
    static XCharStruct bbox;
    int intTmp;
    int canvWidth, canvHeight;
    char *name;
    int xExp, yExp;          /* the exponent factor of the x, y-axes scales, resp. */
    /* Space for a pair of integers in ASCII form */
    static char xExpStr[INT_STRING_SIZE];
    static char yExpStr[INT_STRING_SIZE];
    int nw, nh;              /* # of pixels/char width, height, resp. */
    int nx, ny;              /* # of ticks between first and last in x, y-dir, resp. */
    int nhTitle;             /* # of titlefont pixels/char height */
    double xStep, yStep, xStart, yStart = 0.0, doubleTmp, doubleNum;
    double xCoord1, xCoord2, yCoord1, yCoord2, tickLength;
    double xLabelYCoord, yLabelXCoord;
    static Tcl_DString ds;   /* dynamic string used to build Tcl commands */

    name = plotPtr->name;
    win = plotPtr->win;
    identifier = plotPtr->identifier;
    xTitle = plotPtr->xTitle;
    yTitle = plotPtr->yTitle;

    /* Get the width and height of the canvas */
    sprintf(strTmp,"%s.pf.c",name);
    canvWin = Tk_NameToWindow(interp,strTmp,*win);
    canvWidth = Tk_Width(canvWin);
    canvHeight = Tk_Height(canvWin);

    /* Get the font size information for the standard font */
    if ((fontPtr = Tk_GetFontStruct(interp,*win,Tk_GetUid(STD_FONT))) == NULL) {
        errmsg = "Cannot retrieve font";
	return 0;
    }
    XTextExtents(fontPtr, "8", strlen("8"),&intTmp,&intTmp,&intTmp,&bbox);
    nw = (bbox.rbearing - bbox.lbearing);
    nh = (bbox.ascent + bbox.descent);
    Tk_FreeFontStruct(fontPtr);

    /* Get the font size information for the title font */
    if ((fontPtr = Tk_GetFontStruct(interp,*win,Tk_GetUid(TITLE_FONT))) == NULL) {
        errmsg = "Cannot retrieve font";
	return 0;
    }
    XTextExtents(fontPtr, "8", strlen("8"),&intTmp,&intTmp,&intTmp,&bbox);
    nhTitle = (bbox.ascent + bbox.descent);
    Tk_FreeFontStruct(fontPtr);

    doubleNum = MAX(fabs(plotPtr->yMax),fabs(plotPtr->yMin));

    /* From David Harrison's Xgraph source, modified to remove memory leaks */
    yExp = (int) floor(doubleNum == 0.0 ? 0.0 : log10(doubleNum) + 1e-15);
    sprintf(yExpStr,"%d",yExp);

    doubleNum = MAX(fabs(plotPtr->xMax),fabs(plotPtr->xMin));
    xExp = (int) floor(doubleNum == 0.0 ? 0.0 : log10(doubleNum) + 1e-15);
    sprintf(xExpStr,"%d",xExp);

    /* 5+1 = 6 characters of padding */

    /* 2 characters of padding */
    /* "x10" translates to 3 characters */
    /* "x.xx" of the y-axis tick labels translates to 4 characters */

    plotPtr->llx = (MAX(strlen(yExpStr)+3,4)+2)*nw;

    /* some characters of padding */
    /* Title & X-axis tick labels ==> 2 char high */
    /* Names are indicative of position: "ur" means "upper right" */
    plotPtr->urx = canvWidth-nw*(strlen(xExpStr)+3+3);
    plotPtr->lly = canvHeight-nh*(1+1+1+1);

    /* 
     * ury padding: title + y label + between_padding
     * + yexp + between_padding + padding
     */
    plotPtr->ury = nhTitle+nh*(1+1+1+.75+1.25);

    /*
     * Compute the scale factor to be used in plotting each point
     */
    plotPtr->scalex = (plotPtr->urx - plotPtr->llx)/(plotPtr->xMax - plotPtr->xMin);
    plotPtr->scaley = (plotPtr->lly - plotPtr->ury)/(plotPtr->yMax - plotPtr->yMin);

    /*
     * Start generating objects in the canvas.
     * Begin by removing everything with the tag nonData.
     * Note that the following should probably be done in a Tcl procedure,
     * not in C code as done here.
     */
    Tcl_DStringInit(&ds);
    sprintf(strTmp,"%s.pf.c delete nonData; ",name);
    Tcl_DStringAppend(&ds,strTmp,-1);

    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData; ",name,
	    plotPtr->llx,plotPtr->lly,plotPtr->urx,plotPtr->lly);
    Tcl_DStringAppend(&ds,strTmp,-1);

    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData; ",name,
	    plotPtr->llx,plotPtr->lly,plotPtr->llx,plotPtr->ury);
    Tcl_DStringAppend(&ds,strTmp,-1);

    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData; ",name,
	    plotPtr->llx,plotPtr->ury,plotPtr->urx,plotPtr->ury);
    Tcl_DStringAppend(&ds,strTmp,-1);

    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData; ",name,
	    plotPtr->urx,plotPtr->lly,plotPtr->urx,plotPtr->ury);
    Tcl_DStringAppend(&ds,strTmp,-1);

    sprintf(strTmp,
   "%s.pf.c create text %d %d -text \"%s\" -font %s -anchor n -tags nonData; ",
	    name, (int)(plotPtr->urx+plotPtr->llx)/2,
	    (int)(nh*0.75),identifier,TITLE_FONT);
    Tcl_DStringAppend(&ds,strTmp,-1);

    sprintf(strTmp,
   "%s.pf.c create text %d %d -text \"%s\" -font %s -anchor w -tags nonData; ",
	    name, (int)(nw*1.5),(int)(nh*2.25+nhTitle),yTitle,STD_FONT);
    Tcl_DStringAppend(&ds,strTmp,-1);

    if(yExp!=0) {
      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text x10 -font %s -anchor e -tags nonData; ",
	      name, (int)(plotPtr->llx-nw*strlen(yExpStr)),(int)plotPtr->ury,
	      STD_FONT);
      Tcl_DStringAppend(&ds,strTmp,-1);

      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text %d -font %s -anchor e -tags nonData; ",
	      name, (int)(plotPtr->llx-nw/6),
	      (int)(plotPtr->ury-nh*5/6),yExp,STD_FONT);
      Tcl_DStringAppend(&ds,strTmp,-1);
    }

    sprintf(strTmp,
	    "%s.pf.c create text %d %d -text \"%s\" -font %s -anchor se -tags nonData",
	    name, (int)(canvWidth-nw*1.5),
	    (int)(canvHeight-0.5*nh),xTitle,STD_FONT);
    Tcl_DStringAppend(&ds,strTmp,-1);

    if (xExp != 0) {
      sprintf(strTmp,
	      "; %s.pf.c create text %d %d -text x10 -font %s -anchor w -tags nonData; ",
	      name, plotPtr->urx+nw,plotPtr->lly,STD_FONT);
      Tcl_DStringAppend(&ds,strTmp,-1);

      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text %d -font %s -anchor w -tags nonData",
	      name, plotPtr->urx+(4+5/6)*nw,plotPtr->lly-nh*5/6,xExp,STD_FONT);
      Tcl_DStringAppend(&ds,strTmp,-1);
    }

    nx=(plotPtr->urx-plotPtr->llx)/12/nw;
    ny = (plotPtr->lly-plotPtr->ury)/8/nh;

    xStep=roundUp((plotPtr->xMax-plotPtr->xMin)/(double)(1+nx));
    yStep=roundUp((plotPtr->yMax-plotPtr->yMin)/(double)(1+ny));
    xStart=xStep*ceil(plotPtr->xMin/xStep);
    if (xStart == plotPtr->xMin) { xStart+=xStep; }
    if (yStart == plotPtr->yMin) { yStart+=yStep; }
    yStart=yStep*ceil(plotPtr->yMin/yStep);

    /* Create the ticks and the labels */
    /* x-axis */
    tickLength = MIN((plotPtr->lly-plotPtr->ury)/50,
	(plotPtr->urx-plotPtr->llx)/50);
    yCoord1 = plotPtr->lly-tickLength;
    yCoord2 = plotPtr->ury+tickLength;
    xLabelYCoord = plotPtr->lly+nh/2;

/* FIXME: Case -0.00 (eg., likely, -0.0000343) unresolved */
    for (doubleTmp=xStart; doubleTmp < plotPtr->xMax; doubleTmp+=xStep) {
	xCoord1 = MAPX(doubleTmp);
	sprintf(strTmp,"; %s.pf.c create line %f %d %f %f -tags nonData", name,
		xCoord1, plotPtr->lly, xCoord1, yCoord1);
	Tcl_DStringAppend(&ds,strTmp,-1);
	sprintf(strTmp,"; %s.pf.c create line %f %d %f %f -tags nonData", name,
		xCoord1, plotPtr->ury, xCoord1, yCoord2);
	Tcl_DStringAppend(&ds,strTmp,-1);
	sprintf(strTmp,
"; %s.pf.c create text %f %f -anchor n -font %s -text %1.2f -tags nonData",
		name, xCoord1, xLabelYCoord, STD_FONT,
		doubleTmp/pow(10.0,(double)xExp));
	Tcl_DStringAppend(&ds,strTmp,-1);
    }
    /* y-ticks */

/* FIXME: Case -0.00 (eg., likely, -0.0000343) unresolved */
    xCoord1 = plotPtr->llx+tickLength;
    xCoord2 = plotPtr->urx-tickLength;
    yLabelXCoord = plotPtr->llx-nw/2;
    for (doubleTmp=yStart; doubleTmp < plotPtr->yMax; doubleTmp+=yStep) {
	yCoord1 = MAPY(doubleTmp);
	sprintf(strTmp,"; %s.pf.c create line %d %f %f %f -tags nonData", name,
		plotPtr->llx, yCoord1, xCoord1, yCoord1);
	Tcl_DStringAppend(&ds,strTmp,-1);
	sprintf(strTmp,"; %s.pf.c create line %d %f %f %f -tags nonData", name,
		plotPtr->urx, yCoord1, xCoord2, yCoord1);
	Tcl_DStringAppend(&ds,strTmp,-1);
	sprintf(strTmp,
"; %s.pf.c create text %f %f -anchor e -font %s -text %1.2f -tags nonData",
		name, yLabelXCoord, yCoord1, STD_FONT,
		doubleTmp/pow(10.0,(double)yExp));
	Tcl_DStringAppend(&ds,strTmp,-1);
    }

    /* Finally, evaluate the tcl script we have just built */
    if (Tcl_Eval(interp,Tcl_DStringValue(&ds)) != TCL_OK) {
	errmsg = "Failed to build plot labels";
	return 0;
    }

    return 1;
}

/*
 * Display a single point in a dataset.
 * Seems like there is too much complexity in this.
 */
static int displayPoint(interp,plotPtr,setPtr, point)
     Tcl_Interp *interp;
     ptkPlotWin *plotPtr;
     ptkPlotDataset *setPtr;
     int point;
{
  int showpoint, showsomething, prevInside;
  double xpoint, ypoint, canvX, canvY, pct, canvPrevX, canvPrevY;

  if (point >= plotPtr->persistence || point < 0) {
    errmsg = "displayPoint: point number is out of range";
    return 0;
  }
  /* Compute the canvas position of the point */
  xpoint = setPtr->xvec[point];
  ypoint = setPtr->yvec[point];
  canvPrevX = canvX = MAPX(xpoint);
  canvPrevY = canvY = MAPY(ypoint);
  if (plotPtr->style == 1) {
    canvPrevX = MAPX(setPtr->prevX);
    canvPrevY = MAPY(setPtr->prevY);
  }

  /* Compute an indicator as to whether the point should actually show up on the screen */
  showsomething = showpoint = (xpoint <= plotPtr->xMax) && (xpoint >= plotPtr->xMin) &&
    (ypoint <= plotPtr->yMax) && (ypoint >= plotPtr->yMin);

  /* If we are in line drawing mode, and the previous point was out of range, */
  /* recompute the previous point position to lie on the boundary of the plot */
  prevInside = 1;
  if (plotPtr->style == 1) {
    if (canvPrevX < plotPtr->llx) {
      pct = (plotPtr->llx - canvPrevX)/(canvX - canvPrevX);
      canvPrevX = plotPtr->llx;
      canvPrevY = canvPrevY + (canvY - canvPrevY) * pct;
      prevInside = 0;
    } else if (canvPrevX > plotPtr->urx) {
      pct = (plotPtr->urx - canvPrevX)/(canvX - canvPrevX);
      canvPrevX = plotPtr->urx;
      canvPrevY = canvPrevY + (canvY - canvPrevY) * pct;
      prevInside = 0;
    }

    /* Note that y increases downward */
    if (canvPrevY < plotPtr->ury) {
      pct = (plotPtr->ury - canvPrevY)/(canvY - canvPrevY);
      canvPrevY = plotPtr->ury;
      canvPrevX = canvPrevX + (canvX - canvPrevX) * pct;
      prevInside = 0;
    } else if (canvPrevY > plotPtr->lly) {
      pct = (plotPtr->lly - canvPrevY)/(canvY - canvPrevY);
      canvPrevY = plotPtr->lly;
      canvPrevX = canvPrevX + (canvX - canvPrevX) * pct;
      prevInside = 0;
    }

    /* If we are in line drawing mode, and the previous point was in range, */
    /* but the current point is out of range, recompute the current point   */
    /* position to lie on the boundary of the plot */
    if (!showpoint && prevInside) {
      showsomething = 1;
      if (canvX < plotPtr->llx) {
	pct = (plotPtr->llx - canvX)/(canvPrevX - canvX);
	canvX = plotPtr->llx;
	canvY = canvY + (canvPrevY - canvY) * pct;
      } else if (canvX > plotPtr->urx) {
	pct = (plotPtr->urx - canvX)/(canvPrevX - canvX);
	canvX = plotPtr->urx;
	canvY = canvY + (canvPrevY - canvY) * pct;
      }

      /* Note that y increases downward */
      if (canvY < plotPtr->ury) {
	pct = (plotPtr->ury - canvY)/(canvPrevY - canvY);
	canvY = plotPtr->ury;
	canvX = canvX + (canvPrevX - canvX) * pct;
      } else if (canvY > plotPtr->lly) {
	pct = (plotPtr->lly - canvY)/(canvPrevY - canvY);
	canvY = plotPtr->lly;
	canvX = canvX + (canvPrevX - canvX) * pct;
      }
    }

    /* Suppress the point if we are in line mode and it is the next point to be updated */
    /* or the previous point was not valid */
    if ((point == setPtr->idx) || (!setPtr->connect[point])) showsomething = 0;

    /* If the beenOnceThrough flag is not set, then we need */
    /* to create a new point. */
    if (!setPtr->beenOnceThrough) {
      if (showsomething && point > 0) {
	sprintf(strTmp,
		"%s.pf.c create line %f %f %f %f -fill [option get . plotColor%d PlotColor%d] -width 2.0",
		plotPtr->name, canvX, canvY, canvPrevX, canvPrevY,
		setPtr->color, setPtr->color);
      } else {
	/* create a zero length line so that it exists and can be configured later */
	sprintf(strTmp,
		"%s.pf.c create line 0.0 0.0 0.0 0.0 -fill [option get . plotColor%d PlotColor%d] -width 2.0",
		plotPtr->name, setPtr->color, setPtr->color);
      }
    } else {
      /* If we have already been once through, then we just need to reconfigure */
      if (showsomething) {
	sprintf(strTmp,"%s.pf.c coords %d %f %f %f %f",
		plotPtr->name, setPtr->id[point],
		canvX, canvY,canvPrevX,canvPrevY);
      } else {
	/* Hide the point by configuring to a zero-length line */
	sprintf(strTmp,"%s.pf.c coords %d 0.0 0.0 0.0 0.0",
		plotPtr->name, setPtr->id[point]);
      }
    }
  } else {
    /* Not drawing lines.  Create points */
    if (!setPtr->beenOnceThrough) {
      if (showsomething) {
	sprintf(strTmp,
		"%s.pf.c create oval %f %f %f %f -fill [option get . plotColor%d PlotColor%d]",
		plotPtr->name, canvX-RADIUS, canvY-RADIUS, canvX+RADIUS, canvY+RADIUS,
		setPtr->color, setPtr->color);
      } else {
	/* create a zero-sized circle so it exists and can be configured later */
	sprintf(strTmp,
		"%s.pf.c create oval 0.0 0.0 0.0 0.0 -fill [option get . plotColor%d PlotColor%d]",
		plotPtr->name, setPtr->color, setPtr->color);
      }
    } else {
      /* If we have already been once through, then we just need to reconfigure */
      if (showsomething) {
	sprintf(strTmp,"%s.pf.c coords %d %f %f %f %f",
		plotPtr->name, setPtr->id[point],
		canvX-RADIUS, canvY-RADIUS,canvX+RADIUS,canvY+RADIUS);
      } else {
	sprintf(strTmp,"%s.pf.c coords %d 0.0 0.0 0.0 0.0",
		plotPtr->name, setPtr->id[point]);
      }
    }
  }
  /* For either points or lines, evaluate the Tcl command */
  if (Tcl_Eval(interp,strTmp) != TCL_OK) {
    errmsg = "ptkPlotPoint: Failed to plot point";
    return 0;
  }
  /* If this is the first time through, set the id array */
  if (!setPtr->beenOnceThrough) {
    sscanf(interp->result,"%d",setPtr->id+point);
  }
    
  setPtr->prevX = xpoint;
  setPtr->prevY = ypoint;
  return 1;
}

static int drawAllPoints(interp,plotPtr)
    Tcl_Interp *interp;
    ptkPlotWin *plotPtr;
{
  ptkPlotDataset* curSetPtr;
  int numPoints, i, j, saveBeenOnce;
  double lastx, lasty, savex, savey;

  curSetPtr = NULL;   /* silence warnings */
  for (j=0; j < plotPtr->numsets; j++) {
    curSetPtr = &((plotPtr->sets)[j]);

    /* figure out how many points need to be plotted. */
    /* this depends on whether we've been once through all the points. */
    if (!curSetPtr->beenOnceThrough) {
      numPoints = curSetPtr->idx;
    } else {
      numPoints = plotPtr->persistence;
    }

    savex = curSetPtr->prevX;
    savey = curSetPtr->prevY;
    saveBeenOnce = curSetPtr->beenOnceThrough;

    /* Initialize these */
    if (curSetPtr->beenOnceThrough) {
      lastx = curSetPtr->xvec[plotPtr->persistence - 1];
      lasty = curSetPtr->yvec[plotPtr->persistence - 1];
      curSetPtr->prevX = lastx;
      curSetPtr->prevY = lasty;
    } else {
      /* Avoid creating new objects */
      curSetPtr->beenOnceThrough = 1;
    }

    for (i=0; i<numPoints; i++) {
      if (!displayPoint(interp,plotPtr,curSetPtr,i)) return 0;
    }

    /* Suppress spurious first point after resize or redraw */
    curSetPtr->prevX = savex;
    curSetPtr->prevY = savey;
    curSetPtr->beenOnceThrough = saveBeenOnce;
  }
  return 1;
}

/* Redraw (in response to resized window) */
static int
ptkXYPlotRedraw(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  ptkPlotWin* plotPtr;
  plotPtr = (ptkPlotWin *)clientData;
  if (!drawAxes(interp,plotPtr)) return TCL_ERROR;
  if (!drawAllPoints(interp,plotPtr)) return TCL_ERROR;
  return TCL_OK;
}

/* Resize to a particular set of coordinates */
/* The given coordinates are screen coordinates */
static int
ptkXYPlotResize(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  ptkPlotWin* plotPtr;
  int x[4];
  double coords[4];
  int i;

  plotPtr = (ptkPlotWin *)clientData;

  if (argc != 5) {
    Tcl_AddErrorInfo(interp, "usage: ptkXYPlotResize x1 y1 x2 y2");
    return TCL_ERROR;
  }
  
  for (i = 0; i < 4; i++) {
    if (sscanf(argv[i+1], "%d", &(x[i])) != 1) {
      Tcl_AddErrorInfo(interp, "ptkXYPlotResize: invalid coordinates");
      return TCL_ERROR;
    }
  }
  coords[0] = INVMAPX(x[0]);
  coords[1] = INVMAPY(x[1]);
  coords[2] = INVMAPX(x[2]);
  coords[3] = INVMAPY(x[3]);

  plotPtr->xMin = coords[0];
  plotPtr->xMax = coords[2];
  plotPtr->yMax = coords[1];
  plotPtr->yMin = coords[3];
  if (!drawAxes(interp,plotPtr)) return TCL_ERROR;
  if (!drawAllPoints(interp,plotPtr)) return TCL_ERROR;
  
  return TCL_OK;
}

/* Zoom out by a specified factor */
static int
ptkXYPlotZoom(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  ptkPlotWin* plotPtr;
  double factor, midpoint, distToMid;

  plotPtr = (ptkPlotWin *)clientData;

  if (argc != 2) {
    Tcl_AddErrorInfo(interp, "usage: ptkXYPlotZoom factor");
    return TCL_ERROR;
  }
  if (sscanf(argv[1], "%lf", &factor) != 1) {
    Tcl_AddErrorInfo(interp, "ptkXYPlotZoom: invalid factor");
    return TCL_ERROR;
  }

  /* Zooms are centered */
  midpoint = (plotPtr->xMin + plotPtr->xMax)*0.5;
  distToMid = (midpoint - plotPtr->xMin)*factor;
  plotPtr->xMin = midpoint - distToMid;
  plotPtr->xMax = midpoint + distToMid;

  /* Now do the y axis */
  midpoint = (plotPtr->yMin + plotPtr->yMax)*0.5;
  distToMid = (midpoint - plotPtr->yMin)*factor;
  plotPtr->yMin = midpoint - distToMid;
  plotPtr->yMax = midpoint + distToMid;

  if (!drawAxes(interp,plotPtr)) return TCL_ERROR;
  if (!drawAllPoints(interp,plotPtr)) return TCL_ERROR;
  
  return TCL_OK;
}

/* Use data from canvas to autoscale; add one unit extra to avoid cut-off */
static int
ptkXYPlotZoomFit(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  ptkPlotWin* plotPtr;
  ptkPlotDataset *curSetPtr;
  int i,j, numPoints;
  double xval, yval, xMinPoint, xMaxPoint, yMinPoint, yMaxPoint;

  plotPtr = (ptkPlotWin *)clientData;

  if (plotPtr->sets == NULL) return TCL_OK;   /* ignore */

  xMinPoint=LARGE_ENOUGH;
  xMaxPoint=-LARGE_ENOUGH;
  yMinPoint=LARGE_ENOUGH;
  yMaxPoint=-LARGE_ENOUGH;

  /* Figure out what the max and min are */
  for (j=0; j < plotPtr->numsets; j++) {
    curSetPtr = &((plotPtr->sets)[j]);

    /* figure out how many points need to be plotted. */
    /* this depends on whether we've been once through all the points. */
    if (!curSetPtr->beenOnceThrough) {
      numPoints = curSetPtr->idx;
    } else {
      numPoints = plotPtr->persistence;
    }

    for (i=0; i<numPoints; i++) {
      xval = curSetPtr->xvec[i];
      yval = curSetPtr->yvec[i];
      xMaxPoint = (xval > xMaxPoint) ? xval : xMaxPoint;
      xMinPoint = (xval < xMinPoint) ? xval : xMinPoint;
      yMaxPoint = (yval > yMaxPoint) ? yval : yMaxPoint;
      yMinPoint = (yval < yMinPoint) ? yval : yMinPoint;
    }
  }

  plotPtr->xMin = xMinPoint;
  plotPtr->xMax = xMaxPoint;
  plotPtr->yMin = yMinPoint;
  plotPtr->yMax = yMaxPoint;

  if (!drawAxes(interp,plotPtr)) return TCL_ERROR;
  if (!drawAllPoints(interp,plotPtr)) return TCL_ERROR;
  
  return TCL_OK;
}

/* Use original scale */
static int
ptkXYPlotZoomOriginal(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  ptkPlotWin* plotPtr;

  plotPtr = (ptkPlotWin *)clientData;

  if (plotPtr->sets == NULL) return TCL_OK;   /* ignore */

  plotPtr->xMin = plotPtr->origXmin;
  plotPtr->xMax = plotPtr->origXmax;
  plotPtr->yMin = plotPtr->origYmin;
  plotPtr->yMax = plotPtr->origYmax;

  if (!drawAxes(interp,plotPtr)) return TCL_ERROR;
  if (!drawAllPoints(interp,plotPtr)) return TCL_ERROR;
  
  return TCL_OK;
}

/* ptkInitPlot
 * This procedure must be called before ptkCreatePlot.
 * Not doing so will risk a core dump.  It initializes pointers
 * that indicate that no dynamic memory has been allocated, so none should be
 * freed.  Think of it as a constructor, but since this is C, not C++,
 * it must be called explicitly.
 */
void ptkInitPlot(plotPtr)
     ptkPlotWin *plotPtr;
{
  plotPtr->sets = NULL;
}

/*
 * Free the arrays associated with the dataset structure.
 * Note that this does not free the structure itself.  The
 * caller is responsible for this, if appropriate.
 */
static void ptkFreeDataset(setPtr)
     ptkPlotDataset *setPtr;
{
  free(setPtr->xvec);
  free(setPtr->yvec);
  free(setPtr->id);
  free(setPtr->connect);
}

/*
 * Free the string copies associated with the plot structure.
 * Note that this does not free the structure itself.  The
 * caller is responsible for this, if appropriate.
 */
void ptkFreePlot(interp,plotPtr)
     Tcl_Interp *interp;
     ptkPlotWin *plotPtr;
{
  int i;

  if (plotPtr->sets == NULL) return;   /* nothing to free */

  for (i=0; i < plotPtr->numsets; i++) {
    ptkFreeDataset(&((plotPtr->sets)[i]));
  }
  free(plotPtr->sets);
  plotPtr->numsets = 0;
  sprintf(strTmp,"ptkXYPlotRedraw%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotZoomFit%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotZoomOriginal%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotResize%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotZoom%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
}

/* ptkCreatePlot
 * Create a new plot.  Arguments:
 *   interp        Tcl interpreter
 *   plotPtr       pointer to ptkPlotWin data structure
 *   win           Tk window pointer
 *   name          window name
 *   identifier    title to use
 *   geometry      placement and size, in standard X format
 *   xTitle        title for the X axis
 *   yTitle        title for the Y axis
 *   xMin,xMax     range of X axis to plot
 *   yMin,yMax     range of Y axis to plot
 *   numsets       number of datasets to combine in one plot
 *   style         style of plot (0 = points, 1 = lines)
 *   persistence   number of points to keep around at one time in the plot
 */
int ptkCreatePlot(interp,plotPtr,win,name,identifier,
		  geometry,xTitle,yTitle,
		  xMin,xMax,yMin,yMax,numsets,batch,style,persistence)
    Tcl_Interp *interp;
    ptkPlotWin* plotPtr;
    Tk_Window *win;
    char *name, *identifier, *geometry;
    char *xTitle, *yTitle;
    double xMin, xMax, yMin, yMax;
    int numsets, batch, style, persistence;
{
    int i;
    ptkPlotDataset *setPtr;

    /* The following safely does nothing if this is the first call */
    ptkFreePlot(interp,plotPtr);

    /* Error checking */
    if ((xMax <= xMin) || (yMax <= yMin)) {
      errmsg = "ptkCreatePlot: invalid x and y ranges";
      return 0;
    }
    if ((persistence <= 0) || (numsets <= 0)) {
      errmsg = "ptkCreatePlot: persistence and numsets must both be positive";
      return 0;
    }

    plotPtr->interp=interp;
    plotPtr->win=win;
    plotPtr->name=name;
    plotPtr->identifier=identifier;
    /* urx, ury, llx, and lly are uninitialized */
    plotPtr->xTitle=xTitle;
    plotPtr->yTitle=yTitle;
    plotPtr->xMin = plotPtr->origXmin = xMin;
    plotPtr->xMax = plotPtr->origXmax = xMax;
    plotPtr->yMin = plotPtr->origYmin = yMin;
    plotPtr->yMax = plotPtr->origYmax = yMax;
    plotPtr->persistence = persistence;
    plotPtr->refreshBatch = batch;
    plotPtr->style = style;
    plotPtr->sets = 0;
    plotPtr->numsets = numsets;

    /* Create the command for redrawing the window */
    sprintf(strTmp,"ptkXYPlotRedraw%s",name);
    Tcl_CreateCommand(interp,strTmp,ptkXYPlotRedraw,
		      (ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);

    /* Create the commands for resizing the window */
    sprintf(strTmp,"ptkXYPlotZoomFit%s",name);
    Tcl_CreateCommand(interp,strTmp,ptkXYPlotZoomFit,
		      (ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);

    sprintf(strTmp,"ptkXYPlotZoomOriginal%s",name);
    Tcl_CreateCommand(interp,strTmp,ptkXYPlotZoomOriginal,
		      (ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);

    sprintf(strTmp,"ptkXYPlotZoom%s",name);
    Tcl_CreateCommand(interp,strTmp,ptkXYPlotZoom,
		      (ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);

    sprintf(strTmp,"ptkXYPlotResize%s",name);
    Tcl_CreateCommand(interp,strTmp,ptkXYPlotResize,
		      (ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);

    sprintf(strTmp, "ptkCreateXYPlot %s \"%s\" \"%s\" [curuniverse]",
	    name,identifier,geometry);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	errmsg = "createPlot: Cannot create plot window";
        return 0;
    }

    /* Draw in the axes and labels */
    if (!drawAxes(interp,plotPtr)) return 0;

    /* Create and initialize the ptkPlotDataset structures */
    if ((plotPtr->sets = (ptkPlotDataset *)malloc(numsets*(sizeof(ptkPlotDataset)))) == NULL) {
      errmsg = "ptkCreatePlot: Cannot allocate memory for datasets";
      return 0;
    }
    for (i=0; i<numsets; i++) {
      /* initialize the datasets */
      setPtr = &((plotPtr->sets)[i]);
      setPtr->prevX = 0.0;
      setPtr->prevY = 0.0;
      setPtr->idx = 0;
      setPtr->beenOnceThrough = 0;
      setPtr->batchCounter = 0;
      setPtr->color = i+1;
      setPtr->color = (i%NUMBER_OF_COLORS)+1;

      if ((setPtr->xvec = (double *)malloc(persistence*(sizeof(double)))) == NULL ||
	  (setPtr->yvec = (double *)malloc(persistence*(sizeof(double)))) == NULL ||
	  (setPtr->id = (int *)malloc(persistence*(sizeof(int)))) == NULL ||
	  (setPtr->connect = (int *)malloc(persistence*(sizeof(int)))) == NULL) {
	errmsg = "ptkCreateDataset: Cannot allocate memory for dataset";
	return 0;
      }
      setPtr->connect[0] = 0;
    }
    return 1;
}

 
int ptkPlotPoint(interp,plotPtr,setnum,xval,yval)
    Tcl_Interp *interp;
    ptkPlotWin *plotPtr;
    int setnum;
    double xval,yval;
{
    int curPt, lower, upper, point;
    ptkPlotDataset *setPtr;

    if (setnum > 0 && setnum <= plotPtr->numsets && plotPtr->sets != NULL) {
      setPtr = &((plotPtr->sets)[setnum-1]);
    } else {
      setPtr = NULL;   /* silence warnings */
      errmsg = "ptkPlotPoint: setnum out of range";
      return 0;
    }

    curPt = (setPtr->idx++);

    /* Store the new point in the local buffer */
    setPtr->xvec[curPt] = xval;
    setPtr->yvec[curPt] = yval;

    /* The actual drawing of points on the screen */
    /* occurs only every refreshBatch points, but to avoid having */
    /* to wrap around the buffer, there is some complexity in determining */
    /* exactly how many points to plot */
    if (++(setPtr->batchCounter) == plotPtr->refreshBatch) {
      lower = curPt - plotPtr->refreshBatch + 1;
      upper = curPt;
      setPtr->batchCounter = 0;
    } else if (curPt == plotPtr->persistence - 1) {
      lower = plotPtr->persistence - (plotPtr->persistence%plotPtr->refreshBatch);
      upper = plotPtr->persistence - 1;
      setPtr->batchCounter = 0;
    } else {
      lower = 0;
      upper = -1;
    }
    /* It's not clear why it speeds things up to plot in batches, since */
    /* exactly the same number of calls to exactly the same function are made. */
    /* Maybe it has to do with caching? */
    for (point = lower; point <= upper; point++) {
      if (!displayPoint(interp,plotPtr,setPtr,point)) return 0;
    }

    if (setPtr->idx >= plotPtr->persistence) {
      setPtr->beenOnceThrough = 1;
      setPtr->idx = 0;
    }
    /* unless this gets overwritten, the next point will be connected to this one */
    setPtr->connect[setPtr->idx] = 1;

    return 1;
}

/* ptkPlotBreak
 * Break lines in a line drawing plot (i.e., make discontinuities)
 */
int ptkPlotBreak(interp,plotPtr,setnum)
    Tcl_Interp *interp;
    ptkPlotWin *plotPtr;
    int setnum;
{
    ptkPlotDataset *setPtr;

    if (setnum > 0 && setnum <= plotPtr->numsets && plotPtr->sets != NULL) {
      setPtr = &((plotPtr->sets)[setnum-1]);
      setPtr->connect[setPtr->idx] = 0;
      return 1;
    } else {
      errmsg = "ptkPlotPoint: setnum out of range";
      return 0;
    }
}
