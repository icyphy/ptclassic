
/*
 * C interface for creating a plotting utility
 *
 * Author: Wei-Jen Huang, E. A. Lee, and D. Niehaus
 * Version: $Id$
 *
 * Many of the ideas come from David Harrison's XGraph plotting tool
 *  and some of his source code.
 *
 * Note that this code uses sprintf into local strings of fixed size.
 * This would be better changed to use Tcl's command buffer, which
 * would be more robust.
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
#include "math.h"
#include "ieee.h"

/* Global scract buffer used for constructing Tcl commands */
static char strTmp[400];

static double roundUp(val)
double val;			/* Value */
/*
 * This routine rounds up the given positive number such that
 * it is some power of ten times either 1, 2, or 5.  It is
 * used to find increments for grid lines.  This is a slightly
 * modified version of D. Harrison's roundDown() routine.
 * For zero or negative numbers, return 1e-15
 * For NaN or Infinity, return 1e15.
 */
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

static int drawAxes(interp,plotPtr)
    Tcl_Interp *interp;
    plotWin *plotPtr;
{
    /* FIXME: extra declarations here */
    static XFontStruct *fontPtr;
    Tk_Window *win, canvWin;
    char *identifier;
    char *xTitle, *yTitle;
    static XCharStruct bbox;
    int intTmp;
    int canvWidth, canvHeight;
    char *name;
    int xExp, yExp; /* the exponent factor of the x, y-axes scales, resp. */
    char *xExpStr, *yExpStr; /* the exponent factors as strings */
    int nw, nh; /* # of pixels/char width, height, resp. */
    int nx, ny; /* # of ticks between first and last in x, y-dir, resp. */
    int nhTitle; /* # of titlefont pixels/char height */
    double xStep, yStep, xStart, yStart, doubleTmp, doubleNum;
    double xCoord1, xCoord2, yCoord1, yCoord2, tickLength;
    double xLabelYCoord, yLabelXCoord;

    name = plotPtr->name;
    win = plotPtr->win;
    identifier = plotPtr->identifier;
    xTitle = plotPtr->xTitle;
    yTitle = plotPtr->yTitle;

    sprintf(strTmp,"%s.pf.c",name);
    canvWin = Tk_NameToWindow(interp,strTmp,*win);
    canvWidth = Tk_Width(canvWin);
    canvHeight = Tk_Height(canvWin);

    if ((fontPtr = Tk_GetFontStruct(interp,*win,Tk_GetUid(STD_FONT))) == NULL) {
        Tcl_AddErrorInfo(interp, "Cannot retrieve font");
	return 0;
    }
    XTextExtents(fontPtr, "8", strlen("8"),&intTmp,&intTmp,&intTmp,&bbox);
    nw = (bbox.rbearing - bbox.lbearing);
    nh = (bbox.ascent + bbox.descent);
    Tk_FreeFontStruct(fontPtr);

    if ((fontPtr = Tk_GetFontStruct(interp,*win,Tk_GetUid(TITLE_FONT))) == NULL) {
        Tcl_AddErrorInfo(interp, "Cannot retrieve font");
	return 0;
    }
    XTextExtents(fontPtr, "8", strlen("8"),&intTmp,&intTmp,&intTmp,&bbox);
    nhTitle = (bbox.ascent + bbox.descent);
    Tk_FreeFontStruct(fontPtr);

    doubleNum = MAX(fabs(plotPtr->yMax),fabs(plotPtr->yMin));
    /* From David Harrison's Xgraph source */
    yExp = (int) floor(doubleNum == 0.0 ? 0.0 : log10(doubleNum) + 1e-15);
    sprintf(strTmp,"%d",yExp);
    yExpStr = STRDUP(strTmp);

    doubleNum = MAX(fabs(plotPtr->xMax),fabs(plotPtr->xMin));
    xExp = (int) floor(doubleNum == 0.0 ? 0.0 : log10(doubleNum) + 1e-15);
    sprintf(strTmp,"%d",xExp);
    xExpStr = STRDUP(strTmp);

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
     * Start generating objects in the canvas.
     * Begin by removing everything with the tag nonData.
     * Note that the following should probably be done in a Tcl procedure,
     * not in C code as done here.
     */
    sprintf(strTmp,"%s.pf.c delete nonData",name);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot delete existing plot markers");
	return 0;
    }

    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData",name,
	    plotPtr->llx,plotPtr->lly,plotPtr->urx,plotPtr->lly);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot create plot borders");
	return 0;
    }
    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData",name,
	    plotPtr->llx,plotPtr->lly,plotPtr->llx,plotPtr->ury);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot create plot borders");
	return 0;
    }
    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData",name,
	    plotPtr->llx,plotPtr->ury,plotPtr->urx,plotPtr->ury);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot create plot borders");
	return 0;
    }
    sprintf(strTmp,"%s.pf.c create line %d %d %d %d -tags nonData",name,
	    plotPtr->urx,plotPtr->lly,plotPtr->urx,plotPtr->ury);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot create plot borders");
	return 0;
    }
    sprintf(strTmp,
   "%s.pf.c create text %d %d -text \"%s\" -font %s -anchor n -tags nonData",
	    name, (int)(plotPtr->urx+plotPtr->llx)/2,
	    (int)(nh*0.75),identifier,TITLE_FONT);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
    }
    sprintf(strTmp,
   "%s.pf.c create text %d %d -text \"%s\" -font %s -anchor w -tags nonData",
	    name, (int)(nw*1.5),(int)(nh*2.25+nhTitle),yTitle,STD_FONT);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
    }
    if(yExp!=0) {
      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text x10 -font %s -anchor e -tags nonData",
	      name, (int)(plotPtr->llx-nw*strlen(yExpStr)),(int)plotPtr->ury,
	      STD_FONT);
      if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
      }
      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text %d -font %s -anchor e -tags nonData",
	      name, (int)(plotPtr->llx-nw/6),
	      (int)(plotPtr->ury-nh*5/6),yExp,STD_FONT);
      if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
      }
    }

    sprintf(strTmp,
  "%s.pf.c create text %d %d -text \"%s\" -font %s -anchor se -tags nonData",
	    name, (int)(canvWidth-nw*1.5),
	    (int)(canvHeight-0.5*nh),xTitle,STD_FONT);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
    }
    if (xExp != 0) {
      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text x10 -font %s -anchor w -tags nonData",
	      name, plotPtr->urx+nw,plotPtr->lly,STD_FONT);
      if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
      }
      sprintf(strTmp,
	      "%s.pf.c create text %d %d -text %d -font %s -anchor w -tags nonData",
	      name, plotPtr->urx+(4+5/6)*nw,plotPtr->lly-nh*5/6,xExp,STD_FONT);
      if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "Cannot label titles");
	return 0;
      }
    }

    nx=(plotPtr->urx-plotPtr->llx)/12/nw;
    ny = (plotPtr->lly-plotPtr->ury)/12/nh;

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
	sprintf(strTmp,"%s.pf.c create line %lf %d %lf %lf -tags nonData", name,
		xCoord1, plotPtr->lly, xCoord1, yCoord1);
	if (Tcl_Eval(interp,strTmp) != TCL_OK) {
		Tcl_AddErrorInfo(interp, "Cannot create ticks");
		return 0;
	}
	sprintf(strTmp,"%s.pf.c create line %lf %d %lf %lf -tags nonData", name,
		xCoord1, plotPtr->ury, xCoord1, yCoord2);
	if (Tcl_Eval(interp,strTmp) != TCL_OK) {
		Tcl_AddErrorInfo(interp, "Cannot create ticks");
		return 0;
	}
	sprintf(strTmp,
"%s.pf.c create text %lf %lf -anchor n -font %s -text %1.2lf -tags nonData",
		name, xCoord1, xLabelYCoord, STD_FONT,
		doubleTmp/pow(10.0,(double)xExp));
	if (Tcl_Eval(interp,strTmp) != TCL_OK) {
		Tcl_AddErrorInfo(interp, "Cannot create ticks");
		return 0;
	}
    }
    /* y-ticks */

/* FIXME: Case -0.00 (eg., likely, -0.0000343) unresolved */
    xCoord1 = plotPtr->llx+tickLength;
    xCoord2 = plotPtr->urx-tickLength;
    yLabelXCoord = plotPtr->llx-nw/2;
    for (doubleTmp=yStart; doubleTmp < plotPtr->yMax; doubleTmp+=yStep) {
	yCoord1 = MAPY(doubleTmp);
	sprintf(strTmp,"%s.pf.c create line %d %lf %lf %lf -tags nonData", name,
		plotPtr->llx, yCoord1, xCoord1, yCoord1);
	if (Tcl_Eval(interp,strTmp) != TCL_OK) {
		Tcl_AddErrorInfo(interp, "Cannot create ticks");
		return 0;
	}
	sprintf(strTmp,"%s.pf.c create line %d %lf %lf %lf -tags nonData", name,
		plotPtr->urx, yCoord1, xCoord2, yCoord1);
	if (Tcl_Eval(interp,strTmp) != TCL_OK) {
		Tcl_AddErrorInfo(interp, "Cannot create ticks");
		return 0;
	}
	sprintf(strTmp,
"%s.pf.c create text %lf %lf -anchor e -font %s -text %1.2lf -tags nonData",
		name, yLabelXCoord, yCoord1, STD_FONT,
		doubleTmp/pow(10.0,(double)yExp));
	if (Tcl_Eval(interp,strTmp) != TCL_OK) {
		Tcl_AddErrorInfo(interp, "Cannot create ticks");
		return 0;
	}
    }

    /* During zoom, need to check root window coordinates.. */
    free(xExpStr); free(yExpStr);
    return 1;
}

/*
 * Display a single point in a dataset
 */
static void displayPoint(interp,setPtr, point)
     Tcl_Interp *interp;
     plotDataset *setPtr;
     int point;
{
  int radius;
  double canvX, canvY;
  plotWin *plotPtr;

  plotPtr=setPtr->curPlot;

  /* Compute the canvas position of the point */
  canvX = MAPX(setPtr->xvec[point]); canvY = MAPY(setPtr->yvec[point]); radius = 2;

  /* If the current point index is less than the persistence, we need */
  /* to create a new point. */
  /* If persistence <= 0, then all points are new. */
  if ((setPtr->persistence > 0 && setPtr->idx <= setPtr->persistence) ||
      (setPtr->persistence <= 0)) {

    sprintf(strTmp,
	    "%s.pf.c create oval %lf %lf %lf %lf -fill black",
	    plotPtr->name, canvX-radius, canvY-radius, canvX+radius, canvY+radius);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
      Tcl_AddErrorInfo(interp, "plotPoint: Cannot plot data");
      return;
    }
    sscanf(interp->result,"%d",setPtr->id+point);
    
  } else {
    sprintf(strTmp,"%s.pf.c coords %d %lf %lf %lf %lf",
	    plotPtr->name, setPtr->id[point],
	    canvX-radius, canvY-radius,canvX+radius,canvY+radius);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
      Tcl_AddErrorInfo(interp, "plotPoint: Cannot move point");
      return;
    }
  }
}

static int drawAllPoints(interp,plotPtr)
    Tcl_Interp *interp;
    plotWin *plotPtr;
{
  plotDataset* curSetPtr;
  int numPoints, i;

  for (curSetPtr=plotPtr->firstSetPtr;
       curSetPtr != NULL;
       curSetPtr=curSetPtr->nextSetPtr) {
    if ((curSetPtr->idx < curSetPtr->persistence) ||
	(curSetPtr->persistence <= 0)) {
      numPoints = curSetPtr->idx;
    } else {
      numPoints = curSetPtr->persistence;
    }
    for (i=0; i<numPoints; i++) {
      displayPoint(interp,curSetPtr,i);
    }
  }
}

/* Redraw (in response to resized window) */
static int
ptkXYPlotRedraw(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  plotWin* plotPtr;
  plotPtr = (plotWin *)clientData;
  drawAxes(interp,plotPtr);
  drawAllPoints(interp,plotPtr);
  return TCL_OK;
}

/* Zoom out by a factor of two */
static int
ptkXYPlotZoomOut(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  plotWin* plotPtr;
  plotPtr = (plotWin *)clientData;

  plotPtr->xMin *= 2;
  plotPtr->xMax *= 2;
  plotPtr->yMin *= 2;
  plotPtr->yMax *= 2;
  drawAxes(interp,plotPtr);
  drawAllPoints(interp,plotPtr);
  
  return TCL_OK;
}

/* Zoom in by a factor of two */
static int
ptkXYPlotZoomIn(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  plotWin* plotPtr;
  plotPtr = (plotWin *)clientData;

  plotPtr->xMin /= 2;
  plotPtr->xMax /= 2;
  plotPtr->yMin /= 2;
  plotPtr->yMax /= 2;
  drawAxes(interp,plotPtr);
  drawAllPoints(interp,plotPtr);
  
  return TCL_OK;
}

/* Use data from canvas to autoscale; add one unit extra to avoid cut-off */
/* Note that this scales to the largest points *ever* received */
static int
ptkXYPlotZoomFit(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
  plotWin* plotPtr;
  plotPtr = (plotWin *)clientData;

  if (plotPtr->xMinPoint < plotPtr->xMaxPoint) {
    plotPtr->xMin = plotPtr->xMinPoint;
    plotPtr->xMax = plotPtr->xMaxPoint;
  }
  if (plotPtr->yMinPoint < plotPtr->yMaxPoint) {
    plotPtr->yMin = plotPtr->yMinPoint;
    plotPtr->yMax = plotPtr->yMaxPoint;
  }
  drawAxes(interp,plotPtr);
  drawAllPoints(interp,plotPtr);
  
  return TCL_OK;
}

int createPlot(interp,plotPtr,win,name,identifier,
	      geometry,xTitle,yTitle,
	      xMin,xMax,yMin,yMax)
    Tcl_Interp *interp;
    plotWin* plotPtr;
    Tk_Window *win;
    char *name, *identifier, *geometry;
    char *xTitle, *yTitle;
    double xMin, xMax, yMin, yMax;
{
    double doubleNum;
    int width, height, rootx, rooty;
    Tcl_HashEntry *hashEntryPtr;
    int intTmp, heightFlag;
    char *plotName, *geoPtr;
    char *strPtr, *strPtr2;

    if ((xMax <= xMin) || (yMax <= yMin)) {
      Tcl_AddErrorInfo(interp, "createPlot: invalid x and y ranges");
    }

    plotPtr->interp=interp;
    plotPtr->win=win;
    plotPtr->name=STRDUP(name);
    plotPtr->identifier=STRDUP(identifier);
    plotPtr->xTitle=STRDUP(xTitle);
    plotPtr->yTitle=STRDUP(yTitle);
    plotPtr->xMin=xMin;
    plotPtr->xMax=xMax;
    plotPtr->yMin=yMin;
    plotPtr->yMax=yMax;
    plotPtr->xMinPoint=LARGE_ENOUGH;
    plotPtr->xMaxPoint=-LARGE_ENOUGH;
    plotPtr->yMinPoint=LARGE_ENOUGH;
    plotPtr->yMaxPoint=-LARGE_ENOUGH;
    plotPtr->parentPlotPtr=(plotWin *)NULL;
    plotPtr->childPlotPtr=(plotWin *)NULL;
    plotPtr->firstSetPtr=(plotDataset *)NULL;

    /* Create the command for redrawing the window */
    sprintf(strTmp,"info procs ptkXYPlotRedraw%s",name);
    Tcl_Eval(interp,strTmp);
    sprintf(strTmp,"ptkXYPlotRedraw%s",name);
    if (strcmp(interp->result,strTmp) != 0) {
      Tcl_CreateCommand(interp,strTmp,ptkXYPlotRedraw,
			(ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);
    }
    /* Create the commands for resizing the window */
    sprintf(strTmp,"info procs ptkXYPlotZoomFit%s",name);
    Tcl_Eval(interp,strTmp);
    sprintf(strTmp,"ptkXYPlotZoomFit%s",name);
    if (strcmp(interp->result,strTmp) != 0) {
      Tcl_CreateCommand(interp,strTmp,ptkXYPlotZoomFit,
			(ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);
    }
    sprintf(strTmp,"info procs ptkXYPlotZoomOut%s",name);
    Tcl_Eval(interp,strTmp);
    sprintf(strTmp,"ptkXYPlotZoomOut%s",name);
    if (strcmp(interp->result,strTmp) != 0) {
      Tcl_CreateCommand(interp,strTmp,ptkXYPlotZoomOut,
			(ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);
    }
    sprintf(strTmp,"info procs ptkXYPlotZoomIn%s",name);
    Tcl_Eval(interp,strTmp);
    sprintf(strTmp,"ptkXYPlotZoomIn%s",name);
    if (strcmp(interp->result,strTmp) != 0) {
      Tcl_CreateCommand(interp,strTmp,ptkXYPlotZoomIn,
			(ClientData)plotPtr,(Tcl_CmdDeleteProc *)NULL);
    }

    sprintf(strTmp, "ptkCreateXYPlot %s \"%s\" \"%s\"",name,identifier,geometry);
    if (Tcl_Eval(interp,strTmp) != TCL_OK) {
	Tcl_AddErrorInfo(interp, "createPlot: Cannot create plot window");
        return 0;
    }

    /* Draw in the axes and labels */
    return drawAxes(interp,plotPtr);
}

/*
 * Fill in the plotDataset data structure.
 * Note that this allocates memory, so freeDataset should be
 * called when the caller is finished with the dataset.
 */
int createDataset(interp,setPtr,win,persistence,batch)
    Tcl_Interp *interp;
    plotDataset *setPtr;    
    Tk_Window *win;
    int persistence;
    int batch;
{
    setPtr->interp=interp;
    setPtr->win=win;
    setPtr->persistence=persistence;
    setPtr->idx=0;
    setPtr->prevSetPtr=(plotDataset *)NULL;
    setPtr->nextSetPtr=(plotDataset *)NULL;
    setPtr->curPlot=(plotWin *)NULL;
    setPtr->refreshBatch = batch;
    setPtr->batchCounter = 0;

    if ((setPtr->xvec = (double *)malloc(persistence*(sizeof(double)))) == NULL ||
	(setPtr->yvec = (double *)malloc(persistence*(sizeof(double)))) == NULL ||
	(setPtr->id = (int *)malloc(persistence*(sizeof(int)))) == NULL) {
           sprintf(strTmp,"createDataset: Cannot allocate memory for dataset");
           Tcl_AddErrorInfo(interp, strTmp);
           return 0;
    }
    return 1;
}

/*
 * Free the arrays associated with the dataset structure.
 * Note that this does not free the structure itself.  The
 * caller is responsible for this, if appropriate.
 */
void freeDataset(setPtr)
     plotDataset *setPtr;
{
  free(setPtr->xvec);
  free(setPtr->yvec);
  free(setPtr->id);
}

/*
 * Free the string copies associated with the plot structure.
 * Note that this does not free the structure itself.  The
 * caller is responsible for this, if appropriate.
 */
void freePlot(interp,plotPtr)
     Tcl_Interp *interp;
     plotWin *plotPtr;
{
  free(plotPtr->identifier);
  free(plotPtr->xTitle);
  free(plotPtr->yTitle);
  sprintf(strTmp,"ptkXYPlotRedraw%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotZoomFit%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotZoomOut%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  sprintf(strTmp,"ptkXYPlotZoomIn%s",plotPtr->name);
  Tcl_DeleteCommand(interp,strTmp);
  free(plotPtr->name);
}
/* Associates dataset with a plot window */
void assocData(interp,setPtr,plotPtr)
    Tcl_Interp *interp;
    plotDataset *setPtr;
    plotWin *plotPtr;
{
    plotDataset *curSetPtr;

    setPtr->curPlot = plotPtr;
    if (plotPtr->firstSetPtr == NULL) {
      setPtr->prevSetPtr = (plotDataset *)NULL;
      setPtr->nextSetPtr = (plotDataset *)NULL;
      plotPtr->firstSetPtr=setPtr;
    } else {
      for (curSetPtr=plotPtr->firstSetPtr; curSetPtr->nextSetPtr != NULL;
	   curSetPtr = curSetPtr->nextSetPtr) ;
      setPtr->prevSetPtr = curSetPtr;
      curSetPtr->nextSetPtr = setPtr;
      setPtr->nextSetPtr=(plotDataset *)NULL;
    }
    return;
}

/*
 * Remove the association of a dataset and a window.
 * FIXME: This should remove existing points from the window.
 */
void freeAssoc(interp,setPtr,plotPtr)
     Tcl_Interp *interp;
     plotDataset *setPtr;
     plotWin *plotPtr;
{
  setPtr->curPlot = NULL;
  if (setPtr->prevSetPtr == NULL) {
    plotPtr->firstSetPtr=(plotDataset *)NULL;
  } else {
    setPtr->prevSetPtr->nextSetPtr = setPtr->nextSetPtr;
    if (setPtr->nextSetPtr != NULL) {
      setPtr->nextSetPtr->prevSetPtr = setPtr->prevSetPtr;
    }
  }
  return;
}

int plotPoint(interp,setPtr,xval,yval)
    Tcl_Interp *interp;
    plotDataset *setPtr;
    double xval,yval;
{
    plotWin *plotPtr;
    Tk_Window plotwin;
    int intTmp, curPt, lower, upper, point;

    /* If there is no plot window associated with the dataset, return */
    if ((plotPtr=setPtr->curPlot) == NULL) return 1;

    /* For the Zoom Fit to work, we keep track of the largest */
    /* and smallest points to go by. Note that the entire history is used. */
    /* Note also that this is fairly expensive.  Hope it's worth it.  */
    plotPtr->xMaxPoint = (xval > plotPtr->xMaxPoint) ? xval : plotPtr->xMaxPoint;
    plotPtr->xMinPoint = (xval < plotPtr->xMinPoint) ? xval : plotPtr->xMinPoint;
    plotPtr->yMaxPoint = (yval > plotPtr->yMaxPoint) ? yval : plotPtr->yMaxPoint;
    plotPtr->yMinPoint = (yval < plotPtr->yMinPoint) ? yval : plotPtr->yMinPoint;

    /* If the data is out of range, return */
    if ((xval > plotPtr->xMax) ||
	(xval < plotPtr->xMin) ||
	(yval > plotPtr->yMax) ||
	(yval < plotPtr->yMin)) {
      return 1;
    }

    /* Store the new point in the local buffer */
    if (setPtr->persistence > 0) {
      curPt = (setPtr->idx++)%(setPtr->persistence);
    } else {
      curPt = setPtr->idx++;
      /*
       * If persistence <= 0, then we want to plot all points.
       * To do this, we need to add to the memory allocation of the
       * xvec, yvec, and id arrays.  This is expensive.
       */
      if (setPtr->persistence <= 0) {
	if (!(setPtr->idx%INCR_ALLOC_PTS)) {
	  intTmp = (setPtr->idx + INCR_ALLOC_PTS) * sizeof(double);
	  setPtr->xvec=(double *)realloc(setPtr->xvec,intTmp);
	  setPtr->yvec=(double *)realloc(setPtr->yvec,intTmp);
	  setPtr->id=(int *)realloc(setPtr->id,
				    (setPtr->idx+INCR_ALLOC_PTS)*
				    sizeof(int));
	  if (setPtr->xvec == NULL || setPtr->yvec == NULL ||
	      setPtr->id == NULL) {
	    sprintf(strTmp,"plotPoint: Cannot allocate additional memory for dataset");
	    Tcl_AddErrorInfo(interp, strTmp);
	    return 0;
	  }
	}
      }
    }
    setPtr->xvec[curPt] = xval;
    setPtr->yvec[curPt] = yval;

    /* The actual drawing of points on the screen */
    /* occurs only every refreshBatch points, but to avoid having */
    /* to wrap around the buffer, there is some complexity in determining */
    /* exactly how many points to plot */
    lower = 0;
    upper = -1;
    if (++(setPtr->batchCounter) == setPtr->refreshBatch) {
      lower = curPt - setPtr->refreshBatch + 1;
      upper = curPt;
      setPtr->batchCounter = 0;
    } else if (curPt == setPtr->persistence - 1) {
      lower = setPtr->persistence - (setPtr->persistence%setPtr->refreshBatch);
      upper = setPtr->persistence - 1;
      setPtr->batchCounter = 0;
    }
    for (point = lower; point <= upper; point++) {
      displayPoint(interp,setPtr,point);
    }

    return 1;
}
