/**************************************************************************
Version identification:
$Id$

Author: Wei-Jen Huang

Defines and maintains a Tcl ploting utility

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

**************************************************************************/

#include <stdio.h>
#include "ptkPlot.h"

/*
 * This routine rounds up the given positive number such that
 * it is some power of ten times either 1, 2, or 5.  It is
 * used to find increments for grid lines. This is based on
 * source code in xgraph, written by David Harrison.
 */

static double roundUp(val)
double val;			/* Value */
{
    int exponent, idx;
    exponent = (int) floor(val == 0.0 ? 0.0 : LOG10(val) + 1e-15);
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

/*
 * Draw X and Y axes, given a data structure that specifies the
 * configuration of the plot.
 */

static void
drawXYAxes(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;
    if (! pwInfoPtr->showAxes) {
	return;
    }
    if (pwInfoPtr->xMin < 0 && pwInfoPtr->xMax > 0) {
	tmpDouble = MAPX(0.0);
	sprintf(scmd,"mkCanvColorLine %s.%s %lf %lf %lf %lf %s %s",
		pwInfoPtr->toplevel,pwInfoPtr->win,tmpDouble,
		canvasInfoPtr->lly,tmpDouble,canvasInfoPtr->ury,
                             "lemonchiffon","xyaxes tick gridLine nonData");
        if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
                errorReport("Cannot draw Y axis!");
    }

    if (pwInfoPtr->yMin < 0 && pwInfoPtr->yMax > 0) {
	tmpDouble = MAPY(0.0);
	sprintf(scmd,"mkCanvColorLine %s.%s %lf %lf %lf %lf %s %s",
		pwInfoPtr->toplevel,pwInfoPtr->win,
		canvasInfoPtr->llx,tmpDouble,canvasInfoPtr->urx,tmpDouble,
                             "lemonchiffon", "xyaxes tick gridLine nonData");
        if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
                errorReport("Cannot draw X axis!");
    }
}

    
/*
 * Creat tick marks on the graph for both X and Y dimensions.
 * Currently the log scale does not work.
 * Possible improvements:
 * 	combine linear & log scale code
 */

static void
createMarkings(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
    double i;

    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;

    if (Tcl_VarEval(interp,pwInfoPtr->toplevel,".",pwInfoPtr->win,".c",
		    " delete gridLine tick",
                    (char *)NULL) != TCL_OK) {
        errorReport("Error in deleting ticks");
        return;
    }

    switch (pwInfoPtr->xAxisScale) {
	case LINEAR_SCALE:
	    for (i = canvasInfoPtr->xStart; i < canvasInfoPtr->xEnd;
		 i += canvasInfoPtr->xStep) {
		if (pwInfoPtr->marks == GRIDLINE) {
		   tmpDouble = MAPX(i);
		   sprintf(scmd,"mkCanvLine %s.%s %lf %lf %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win,tmpDouble,
			canvasInfoPtr->lly,tmpDouble,canvasInfoPtr->ury,
			"gridLine");
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
			errorReport("Cannot draw vertical grid lines");
		} else if (pwInfoPtr->marks == TICKMARKS) {
		  sprintf(scmd,"drawXTick %s.%s %lf %lf %lf %lf %lf",
			pwInfoPtr->toplevel,pwInfoPtr->win,MAPX(i),
			canvasInfoPtr->lly,canvasInfoPtr->lly-XTICKLENGTH,
			canvasInfoPtr->ury,canvasInfoPtr->ury+XTICKLENGTH);
		  if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
			errorReport("Cannot make XTICKS");
		}
		sprintf(scmd,"labelTick %s.%s %.1lf %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win,i, MAPX(i),
			canvasInfoPtr->xTickLabelYCoord,"n");
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make XTICKS");
		}
	    }
	    break;
	case LOG_SCALE:
	    for (i = canvasInfoPtr->xStart; i < canvasInfoPtr->xEnd;
		 i += canvasInfoPtr->xStep) {
		  sprintf(scmd,"drawXTick %s.%s %lf %lf %lf %lf %lf",
			pwInfoPtr->toplevel,pwInfoPtr->win,MAPX(i),
			canvasInfoPtr->lly,canvasInfoPtr->lly-XTICKLENGTH,
			canvasInfoPtr->ury,canvasInfoPtr->ury+XTICKLENGTH);
		  if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
			errorReport("Cannot make XTICKS");
		  sprintf(scmd,"labelTick %s.%s %.1e %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win,exp10(i),
			MAPX(i),canvasInfoPtr->xTickLabelYCoord,"n");
		  if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make XTICKS");
		  }
	    }
	    break;
    }

    switch (pwInfoPtr->yAxisScale) {
	case LINEAR_SCALE:
	    for (i = canvasInfoPtr->yStart; i < canvasInfoPtr->yEnd;
		 i += canvasInfoPtr->yStep) {
		if (pwInfoPtr->marks == GRIDLINE) {
		  tmpDouble = MAPY(i);
		  sprintf(scmd,"mkCanvLine %s.%s %lf %lf %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win,
			canvasInfoPtr->llx,tmpDouble,canvasInfoPtr->urx,
			tmpDouble, "gridLine");
		  if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make YTICK");
		  }
		} else if (pwInfoPtr->marks == TICKMARKS) {
		  sprintf(scmd,"drawYTick %s.%s %lf %lf %lf %lf %lf",
			pwInfoPtr->toplevel,pwInfoPtr->win,MAPY(i),
			canvasInfoPtr->llx,canvasInfoPtr->llx+YTICKLENGTH,
			canvasInfoPtr->urx,canvasInfoPtr->urx-YTICKLENGTH);
		  if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make YTICK");
		  }
		}
		sprintf(scmd,"labelTick %s.%s %.1lf %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win, i,
			canvasInfoPtr->yTickLabelXCoord,MAPY(i),"e");
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make YTICK");
		}
	    }
	    break;
	case LOG_SCALE:
	    for (i = canvasInfoPtr->yStart; i < canvasInfoPtr->yEnd;
		 i += canvasInfoPtr->yStep) {
		sprintf(scmd,"drawYTick %s.%s %lf %lf %lf %lf %lf",
			pwInfoPtr->toplevel,pwInfoPtr->win,MAPY(i),
			canvasInfoPtr->llx,canvasInfoPtr->llx+YTICKLENGTH,
			canvasInfoPtr->urx,canvasInfoPtr->urx-YTICKLENGTH);
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make YTICK");
		}
		sprintf(scmd,"labelTick %s.%s %.1e %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win, exp10(i),
			canvasInfoPtr->yTickLabelXCoord,MAPY(i),"e");
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot make YTICK");
		}
	    }
	   break;
    }
    drawXYAxes(pwInfoPtr);
}

/*
 * Called by a check button to display the X and Y axes
 */

static int
showXYAxesCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;

    if (argc < 2) {
        Tcl_SetResult(interp,
        "Wrong number of args.  Usage: showXYAxes canvas",TCL_STATIC);
        return TCL_ERROR;
    }
    if (Tcl_VarEval(interp,argv[1]," delete gridLine tick",
		    (char *)NULL) != TCL_OK) {
	Tcl_SetResult(interp,"Error in deleting ticks",TCL_STATIC);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp,
	   Tcl_GetVar(interp,"showAxes",TCL_GLOBAL_ONLY),&tmpInt) != TCL_OK) {
	  Tcl_SetResult(interp,"Error in retrieving gridLine var",TCL_STATIC);
	  return TCL_ERROR;
    }
    pwInfoPtr->showAxes = tmpInt;
    createMarkings(pwInfoPtr);
    return TCL_OK;
}

/*
 * Called by a check button to display the grid.
 */

static int
showGridCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    if (argc < 2) {
        Tcl_SetResult(interp,
        "Wrong number of args.  Usage: showGrid canvas",TCL_STATIC);
        return TCL_ERROR;
    }
    if (Tcl_VarEval(interp,argv[1]," delete gridLine tick",
		    (char *)NULL) != TCL_OK) {
	Tcl_SetResult(interp,"Error in deleting ticks",TCL_STATIC);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp,
	   Tcl_GetVar(interp,"gridLine",TCL_GLOBAL_ONLY),&tmpInt) != TCL_OK) {
	  Tcl_SetResult(interp,"Error in retrieving gridLine var",TCL_STATIC);
	  return TCL_ERROR;
    }
    if (tmpInt) {
	pwInfoPtr->marks = GRIDLINE;
    } else {
	pwInfoPtr->marks = TICKMARKS;
    }
    createMarkings(pwInfoPtr);
    return TCL_OK;
}


/*
 * Figure out what steps, initial value, and final value for axis labels.
 */

static graph_CanvasInfo* computeLabel(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
    graph_CanvasInfo *canvasInfoPtr;
    static XFontStruct *fontPtr;
    static XCharStruct bbox;
    int charWidth, charHeight, dummy;

    if ((fontPtr = Tk_GetFontStruct(interp,w,Tk_GetUid(REG_FONT))) == NULL) {
        errorReport("Cannot retrieve font");
        return (graph_CanvasInfo *)NULL;
    }

    READWH(fontPtr,"8");
    charWidth = WIDTH;
    charHeight = HEIGHT;
    Tk_FreeFontStruct(fontPtr);

    if (pwInfoPtr->canvasInfoPtr == NULL) {
      if ((canvasInfoPtr = (graph_CanvasInfo *)malloc(sizeof(graph_CanvasInfo)))
	== NULL) {
		errorReport("Cannot create canvasInfo data structure");
		return (graph_CanvasInfo *)NULL;
      }
      pwInfoPtr->canvasInfoPtr = canvasInfoPtr;
    } else canvasInfoPtr = pwInfoPtr->canvasInfoPtr;

    switch (pwInfoPtr->xAxisScale) {
	case LINEAR_SCALE:
	case LOG_SCALE:
	  canvasInfoPtr->xStep =
		roundUp((LABEL_NUM_CHAR+LABEL_CHAR_PAD)*charWidth
			* (pwInfoPtr->xMax-pwInfoPtr->xMin)/pwInfoPtr->width);
	  canvasInfoPtr->xStart = canvasInfoPtr->xStep * 
			ceil(pwInfoPtr->xMin/canvasInfoPtr->xStep);
	  if (canvasInfoPtr->xStart == pwInfoPtr->xMin)
		canvasInfoPtr->xStart += canvasInfoPtr->xStep;
	  canvasInfoPtr->xEnd = pwInfoPtr->xMax;
	  break;
    }
    switch (pwInfoPtr->yAxisScale) {
	case LINEAR_SCALE:
	case LOG_SCALE:
	  canvasInfoPtr->yStep =
		roundUp((1+LABEL_CHAR_PAD)*charHeight
			*(pwInfoPtr->yMax-pwInfoPtr->yMin)/pwInfoPtr->height);
	  canvasInfoPtr->yStart = canvasInfoPtr->yStep * 
			ceil(pwInfoPtr->yMin/canvasInfoPtr->yStep);
	  if (canvasInfoPtr->yStart == pwInfoPtr->yMin)
		canvasInfoPtr->yStart += canvasInfoPtr->yStep;
	  canvasInfoPtr->yEnd = pwInfoPtr->yMax;
	  break;
    }
    return canvasInfoPtr;
}

/*
 * Compute locations of the axes within the canvas widget,
 * together with the axis titles.
 */

static void
computeLoc(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;
    canvasInfoPtr->llx =
	       canvasInfoPtr->canvasWidth-pwInfoPtr->width-PAD_RIGHT;
    canvasInfoPtr->ury = canvasInfoPtr->titleHeight+PAD_TOP+PAD_TITLE;
    canvasInfoPtr->urx = canvasInfoPtr->canvasWidth-PAD_RIGHT;
    canvasInfoPtr->lly = canvasInfoPtr->ury + pwInfoPtr->height;

    canvasInfoPtr->xTickLabelYCoord = canvasInfoPtr->lly + PAD_XLABEL_BORDER;
    canvasInfoPtr->yTickLabelXCoord = canvasInfoPtr->llx - PAD_YLABEL_BORDER;

    canvasInfoPtr->titleXCoord = (canvasInfoPtr->llx+canvasInfoPtr->urx)/2.0;
    canvasInfoPtr->titleYCoord = PAD_TOP+canvasInfoPtr->titleHeight/2;
    canvasInfoPtr->xTitleXCoord = canvasInfoPtr->titleXCoord;
    canvasInfoPtr->xTitleYCoord = (canvasInfoPtr->canvasHeight-PAD_BOTTOM+
				   canvasInfoPtr->xTickLabelYCoord+canvasInfoPtr->maxHeight+
				   PAD_YTITLE_YLABEL)/2.0;
    canvasInfoPtr->yTitleYCoord = 
				(canvasInfoPtr->lly+canvasInfoPtr->ury)/2.0; 
    canvasInfoPtr->yTitleXCoord = canvasInfoPtr->yTitleWidth/2+PAD_LEFT;
}


/*
 * Compute locations of the axes within the canvas widget,
 * together with the axis titles.  This is used when the window
 * is resized.
 */

static graph_CanvasInfo* computeGeo(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;
    int totalHeight=0, totalWidth = 0;
    int maxWidth=0,maxHeight=0;
    int titleWidth, titleHeight;
    int xTitleWidth, xTitleHeight;
    int yTitleWidth, yTitleHeight;

    int dummy;
    double i;

    static XFontStruct *fnPtr, *tFnPtr;
    static XCharStruct bbox;

    if ((fnPtr = Tk_GetFontStruct(interp,w,Tk_GetUid(REG_FONT))) == NULL) {
	errorReport("Cannot retrieve font");
	return (graph_CanvasInfo *)NULL;
    }
    if ((tFnPtr = Tk_GetFontStruct(interp,w,Tk_GetUid(REG_FONT))) == NULL) {
	errorReport("Cannot retrieve font");
	return (graph_CanvasInfo *)NULL;
    }

    switch (pwInfoPtr->xAxisScale) {
	case LINEAR_SCALE:
	   for ( i = canvasInfoPtr->xStart; i < canvasInfoPtr->xEnd; 
		i += canvasInfoPtr->xStep) {
		sprintf(tmpStr,"%.1lf",i);
		READWH(fnPtr,tmpStr);
		tmpInt = WIDTH;
		maxWidth = ((maxWidth > tmpInt) ? maxWidth : tmpInt);
		totalHeight += HEIGHT;
	   }
	   break;
	case LOG_SCALE:
	   for ( i = canvasInfoPtr->xStart; i < canvasInfoPtr->xEnd; 
		i += canvasInfoPtr->xStep) {
		sprintf(tmpStr,"%.1e",exp10(i));
		READWH(fnPtr,tmpStr);
		tmpInt = WIDTH;
		maxWidth = ((maxWidth > tmpInt) ? maxWidth : tmpInt);
		totalHeight += HEIGHT;
	   }
	   break;

    }
    switch (pwInfoPtr->yAxisScale) {
	case LINEAR_SCALE:
	   for ( i = canvasInfoPtr->yStart; i < canvasInfoPtr->yEnd; 
		i += canvasInfoPtr->yStep) {
		sprintf(tmpStr,"%.1lf",i);
		READWH(fnPtr,tmpStr);
		tmpInt = HEIGHT;
		maxHeight = ((maxHeight	> tmpInt) ? maxHeight : tmpInt);
		totalWidth += WIDTH;
	   }
	   break;
	case LOG_SCALE:
	   for ( i = canvasInfoPtr->yStart; i < canvasInfoPtr->yEnd; 
		i += canvasInfoPtr->yStep) {
		sprintf(tmpStr,"%.1e",exp10(i));
		READWH(fnPtr,tmpStr);
		tmpInt = HEIGHT;
		maxHeight = ((maxHeight	> tmpInt) ? maxHeight : tmpInt);
		totalWidth += WIDTH;
	   }
	   break;
    }
    READWH(tFnPtr,pwInfoPtr->identifier);
    titleWidth = WIDTH; titleHeight = HEIGHT;
    READWH(fnPtr,pwInfoPtr->xAxisTitle);
    xTitleWidth = WIDTH; xTitleHeight = HEIGHT;
    READWH(fnPtr,pwInfoPtr->yAxisTitle);
    yTitleWidth = WIDTH; yTitleHeight = HEIGHT;

    Tk_FreeFontStruct(fnPtr); Tk_FreeFontStruct(tFnPtr);

    canvasInfoPtr->canvasWidth = 
	pwInfoPtr->width + maxWidth + yTitleWidth +PADX;
    canvasInfoPtr->canvasHeight = pwInfoPtr->height + maxHeight
				+PADY+titleHeight+xTitleHeight;

    canvasInfoPtr->maxHeight = maxHeight;
    canvasInfoPtr->maxWidth = maxWidth;
    canvasInfoPtr->titleHeight = titleHeight;
    canvasInfoPtr->yTitleWidth = yTitleWidth;
    canvasInfoPtr->xTitleHeight = xTitleHeight;
    computeLoc(pwInfoPtr);
    return canvasInfoPtr;

}

/*
 * Figures coordinates of the current mouse position.
 */

static int
revMapCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    double xval,yval, canvasX,canvasY;
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;
    if (argc != 3) {
	Tcl_SetResult(interp,
	"Wrong number of args.  Usage: revMap canvas_x canvas_y",TCL_STATIC);
	return TCL_ERROR;
    }
    if (sscanf(*++argv,"%d",&tmpInt) != 1) {
	sprintf(tmpStr,"Invalid x-coordinate: %s",argv[1]);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
	return TCL_ERROR;
    }
    canvasX = (double)tmpInt;
    if (sscanf(*++argv,"%d",&tmpInt) != 1) {
	sprintf(tmpStr,"Invalid y-coordinate: %s",argv[2]);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
	return TCL_ERROR;
    }
    canvasY = (double)tmpInt;
    xval = REVMAPX(canvasX);
    yval = REVMAPY(canvasY);
    if (xval >= pwInfoPtr->xMin && xval <= pwInfoPtr->xMax &&
	yval >= pwInfoPtr->yMin && yval <= pwInfoPtr->yMax) {
	sprintf(tmpStr,"%g %g",xval,yval);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
    }
    return TCL_OK;
}
  
/*
 * Save data values to a file.
 * FIXME: Only the first data set gets saved.
 */

static int
saveToFileCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    graph_DataSet *setPtr = pwInfoPtr->firstSetPtr;
    FILE *fp;
    int i;

    if (argc != 2) {
	Tcl_SetResult(interp,
	    "Wrong number of args.  Must be \"saveToFile file\"",TCL_STATIC);
	return TCL_ERROR;
    }
    if ((fp = fopen(*++argv,"w")) == NULL) {
	sprintf(tmpStr,"Cannot open %s for writing",*argv);
	errorReport(tmpStr);
    }
    if (setPtr->moveOvalFlag == MOVE) {
	for (i = setPtr->numPoints; i < setPtr->dataMemSize; i++)
	   fprintf(fp,"%lf	%lf\n",setPtr->xvec[i],setPtr->yvec[i]);
    }
    for (i = 0; i < setPtr->numPoints; i++)
	   fprintf(fp,"% lf	% lf\n",setPtr->xvec[i],setPtr->yvec[i]);
    fclose(fp);
}
    

/*
 * Redraw the plot.
 * FIXME: Does not check if points are off the plot
 */

static void
redrawData(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
   graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;
   graph_DataSet *setPtr;
   double canvX, canvY;
   int i;
   double tmpX, tmpY, currX, currY;

   for (setPtr = pwInfoPtr->firstSetPtr; setPtr != NULL;
	setPtr = setPtr->nextSetPtr) {
        if (setPtr->moveOvalFlag == MOVE) {
	    tmpX = MAPX((pwInfoPtr->xAxisScale == LINEAR_SCALE) ? 
			setPtr->discardedX : LOG10(setPtr->discardedX));
	    tmpY = MAPY((pwInfoPtr->yAxisScale == LINEAR_SCALE) ? 
			setPtr->discardedY : LOG10(setPtr->discardedY)); 
	    for (i = setPtr->numPoints; i < setPtr->dataMemSize; i++) {
		switch (pwInfoPtr->xAxisScale) {
			case LOG_SCALE:
				canvX = MAPX(LOG10(setPtr->xvec[i]));
				break;
			case LINEAR_SCALE:
				canvX = MAPX(setPtr->xvec[i]);
				break;
		}
		switch (pwInfoPtr->yAxisScale) {
			case LOG_SCALE:
				canvX = MAPX(LOG10(setPtr->yvec[i]));
				break;
			case LINEAR_SCALE:
				canvY = MAPY(setPtr->yvec[i]);
				break;
		}
		if (setPtr->type == SCATTER) {
			tmpX = canvX-RADIUS; tmpY = canvY-RADIUS;
			currX = canvX+RADIUS, currY = canvY+RADIUS;
		} else if (setPtr->type == LINE) {
			currX = canvX; currY = canvY;
		}
		if (! setPtr->recreate) {
			sprintf(scmd,"moveOval %s.%s %d %lf %lf %lf %lf",
				pwInfoPtr->toplevel,pwInfoPtr->win,
				setPtr->id[i],tmpX,tmpY,currX,currY);
			if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
				errorReport("Cannot move point");
			}
		} else {
			if (setPtr->type == SCATTER) {
			  sprintf(scmd,"createOval %s.%s %lf %lf %lf %lf %s",
				pwInfoPtr->toplevel,pwInfoPtr->win,
				tmpX,tmpY,currX,currY,setPtr->ovalColor);
			} else if (setPtr->type == LINE) {
			  sprintf(scmd,
				  "mkCanvColorLine %s.%s %lf %lf %lf %lf %s",
				  pwInfoPtr->toplevel,pwInfoPtr->win,
				  tmpX,tmpY,currX,currY,setPtr->color);
			}
		        if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
				errorReport("Cannot plot point");
			if (setPtr->type != LINE_SCATTER) 
				sscanf(interp->result,"%d",setPtr->id+i);
		}
		tmpX = currX; tmpY = currY;
	    }
	}
        for (i = 0; i < setPtr->numPoints; i++) {
		switch (pwInfoPtr->xAxisScale) {
			case LOG_SCALE:
				canvX = MAPX(LOG10(setPtr->xvec[i]));
				break;
			case LINEAR_SCALE:
				canvX = MAPX(setPtr->xvec[i]);
				break;
		}
		switch (pwInfoPtr->yAxisScale) {
			case LOG_SCALE:
				canvY = MAPY(LOG10(setPtr->yvec[i]));
			case LINEAR_SCALE:
				canvY = MAPY(setPtr->yvec[i]);
				break;
		}
		if (setPtr->type == SCATTER) {
			tmpX = canvX-RADIUS; tmpY = canvY-RADIUS;
			currX = canvX+RADIUS, currY = canvY+RADIUS;
		} else if (setPtr->type == LINE) {
			currX = canvX; currY = canvY;
		}
		if (! setPtr->recreate) {
			sprintf(scmd,"moveOval %s.%s %d %lf %lf %lf %lf",
				pwInfoPtr->toplevel,pwInfoPtr->win,
				setPtr->id[i],tmpX,tmpY,currX,currY);
			if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
				errorReport("Cannot move point");
			}
		} else {
			if (setPtr->type == SCATTER) {
			  sprintf(scmd,"createOval %s.%s %lf %lf %lf %lf %s",
				pwInfoPtr->toplevel,pwInfoPtr->win,
				tmpX,tmpY,currX,currY,setPtr->ovalColor);
			} else if (setPtr->type == LINE) {
			  sprintf(scmd,
				  "mkCanvColorLine %s.%s %lf %lf %lf %lf %s",
				  pwInfoPtr->toplevel,pwInfoPtr->win,
				  tmpX,tmpY,currX,currY,setPtr->color);
			}
		        if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
				errorReport("Cannot plot point");
			if (setPtr->type != LINE_SCATTER) 
				sscanf(interp->result,"%d",setPtr->id+i);
		}
		tmpX = currX; tmpY = currY;
	}
	setPtr->recreate = 0;
   }
}

/*
 * Tcl routine to invoke the redraw.
 */

static int
redrawDataCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    graph_DataSet *setPtr;

    if (argc < 2 ) {
	Tcl_SetResult(interp,
        "Wrong number of args.  Must be \"redrawData <opt val>\"",TCL_STATIC);
	return TCL_ERROR;
    }
    while (--argc) {
	if (strcmp(*++argv,"-type") == 0) {
	   if (! (--argc)) {
		Tcl_SetResult(interp,
	        "Wrong number of args.  Must be \"redrawData <opt val>\"",
			      TCL_STATIC);
		return TCL_ERROR;
	   }
	   if (sscanf(*++argv,"%d",&tmpInt) != 1) {
		sprintf(tmpStr,"Wrong type argument for redrawData: \"%s\"",
			    *argv);
	        Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
		return TCL_ERROR;
	   }
	   for (setPtr = pwInfoPtr->firstSetPtr; setPtr != NULL;
		setPtr = setPtr->nextSetPtr) {
		setPtr->type = tmpInt;
		setPtr->recreate = 1;
	   }
	}
    }
    redrawData(pwInfoPtr);
    return TCL_OK;
}

/*
 * Tcl routine to rescale the plot.
 */

static int
rescaleCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    graph_CanvasInfo *canvasInfoPtr;

    if (argc != 1) {
	Tcl_SetResult(interp,"Wrong number of args.  Must be \"rescale\"",
		      TCL_STATIC);
	return TCL_ERROR;
    }

    tmpDouble = (pwInfoPtr->curXMax-pwInfoPtr->curXMin)/10.0;
    pwInfoPtr->xMin = pwInfoPtr->curXMin-tmpDouble;
    pwInfoPtr->xMax = pwInfoPtr->curXMax+tmpDouble;
    tmpDouble = (pwInfoPtr->curYMax-pwInfoPtr->curYMin)/10.0;
    pwInfoPtr->yMin = pwInfoPtr->curXMin-tmpDouble;
    pwInfoPtr->yMax = pwInfoPtr->curXMax+tmpDouble;

    if ( (canvasInfoPtr = computeLabel(pwInfoPtr)) == NULL) {
        errorReport("Cannot compute proper x and y tick labels");
	return TCL_ERROR;
    }

   computeLoc(pwInfoPtr);

   createMarkings(pwInfoPtr);

   redrawData(pwInfoPtr);
   return TCL_OK;
}

/*
 * This procedure allocates a dataset for ptkPlotFill()
 * and returns a pointer. The calling routine is responsible
 * for freeing this dataset.
 *
 * If the first set is pointing to NULL, then there are no data sets.
 * If the last set is pointing to NULL, then there is only one data set.
 * pwInfoPtr
 * type		Either LINE, SCATTER, or LINE_SCATTER
 * setName	The name of the set to be displayed in the legend
 * dataMemSize	Specifies the number of points to be kept in memory and on
 *		  the plot for each data set.  When the number of points
 *		  exceeds dataMemSize, the oldest points are moved to the
 *		  new positions.  All points stay in memory and on the plot
 *		  if dataMemSize contains a value of -1.
 */

graph_DataSet* ptkPlotCreateDataset
(pwInfoPtr,type,setName,color,ovalColor,dataMemSize)
    graph_PWInfo *pwInfoPtr;
    int type;
    char *setName;
    char *color;
    char *ovalColor;
    int dataMemSize;
{
    graph_DataSet *setPtr;

    if ((setPtr = (graph_DataSet *)malloc(sizeof(graph_DataSet))) == NULL) {
	sprintf(tmpStr,"Cannot allocate memory for dataset \"%s\"",setName);
	errorReport(tmpStr);
    }

    if (pwInfoPtr->firstSetPtr == NULL) {
	setPtr->prevSetPtr = (graph_DataSet *)NULL;
	setPtr->nextSetPtr = (graph_DataSet *)NULL;
	pwInfoPtr->firstSetPtr = setPtr;
    } else if (pwInfoPtr->lastSetPtr == NULL) {
	setPtr->prevSetPtr = pwInfoPtr->firstSetPtr;
	setPtr->nextSetPtr = (graph_DataSet *)NULL;
	pwInfoPtr->firstSetPtr->nextSetPtr = setPtr;
	pwInfoPtr->lastSetPtr = setPtr;
    } else {
	setPtr->prevSetPtr = pwInfoPtr->lastSetPtr;
	setPtr->nextSetPtr = (graph_DataSet *)NULL;
	pwInfoPtr->lastSetPtr->nextSetPtr = setPtr;
	pwInfoPtr->lastSetPtr = setPtr;
    }
    pwInfoPtr->numSets++;
    setPtr->setName = STRDUP(setName);
    setPtr->type = type;
    sprintf(tmpStr,"%d",type);
    setPtr->dataMemSize = dataMemSize;
    if (dataMemSize > 0)
	tmpInt = dataMemSize;
    else if (dataMemSize == 0 || dataMemSize < -1) {
	sprintf(tmpStr,"No points will be drawn for dataset \"%s\"",setName);
	errorReport(tmpStr);
	return (graph_DataSet *)NULL;
    } else if (dataMemSize == -1)
	tmpInt = INCR_ALLOC_PTS;
    if ((setPtr->xvec = 
	 (double *)malloc(tmpInt*(sizeof(double)))) == NULL) {
	   sprintf(tmpStr,"Cannot allocate memory for dataset \"%s\"",setName);
	   errorReport(tmpStr);
	   return (graph_DataSet *)NULL;
    }
    if ((setPtr->yvec = 
	 (double *)malloc(tmpInt*(sizeof(double)))) == NULL) {
	   sprintf(tmpStr,"Cannot allocate memory for dataset \"%s\"",setName);
	   errorReport(tmpStr);
	   return (graph_DataSet *)NULL;
    }
    if ((setPtr->id = 
	 (int *)malloc(tmpInt*(sizeof(int)))) == NULL) {
	   sprintf(tmpStr,"Cannot allocate memory for dataset \"%s\"",setName);
	   errorReport(tmpStr);
	   return (graph_DataSet *)NULL;
    }
    setPtr->numPoints = 0;
    setPtr->numPointsAlloc = tmpInt;
    setPtr->moveOvalFlag = DONT_MOVE;
    setPtr->recreate = 0;
    setPtr->offScale = 0;
    setPtr->prevX = (double)NULL;
    setPtr->prevY = (double)NULL;
    if (ovalColor == NULL) {
	switch (pwInfoPtr->color) {
	   case COLOR_RED:
		pwInfoPtr->red += COLOR_STEP;
		pwInfoPtr->color = COLOR_GREEN;
		break;
	   case COLOR_GREEN:
		pwInfoPtr->green += COLOR_STEP;
		pwInfoPtr->color = COLOR_BLUE;
		break;
	   case COLOR_BLUE:
		pwInfoPtr->blue += COLOR_STEP;
		pwInfoPtr->color = COLOR_RED;
		break;
	}
	sprintf(tmpStr,"#%04x%04x%04x",pwInfoPtr->red,pwInfoPtr->green,
		pwInfoPtr->blue);
	setPtr->ovalColor = STRDUP(tmpStr);
    } else setPtr->ovalColor = STRDUP(ovalColor);

    if (color == NULL) {
	switch (pwInfoPtr->color) {
	   case COLOR_RED:
		pwInfoPtr->red += COLOR_STEP;
		pwInfoPtr->color = COLOR_GREEN;
		break;
	   case COLOR_GREEN:
		pwInfoPtr->green += COLOR_STEP;
		pwInfoPtr->color = COLOR_BLUE;
		break;
	   case COLOR_BLUE:
		pwInfoPtr->blue += COLOR_STEP;
		pwInfoPtr->color = COLOR_RED;
		break;
	}
	sprintf(tmpStr,"#%04x%04x%04x",pwInfoPtr->red,pwInfoPtr->green,
		pwInfoPtr->blue);
	setPtr->color = STRDUP(tmpStr);
    } else setPtr->color = STRDUP(color);

    return setPtr;
}

/*
 * This procedure fills in the data according to the dataset type
 */

void ptkPlotFill(pwInfoPtr,xval,yval,setPtr)
    graph_PWInfo *pwInfoPtr;
    double xval; double yval;
    graph_DataSet *setPtr;
{
    double canvX,canvY,tmpX,tmpY,currX,currY;
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;

    if (setPtr->dataMemSize > 0) {
	if (setPtr->numPoints == setPtr->dataMemSize) {
		setPtr->numPoints = 0;
		setPtr->moveOvalFlag = MOVE;
	}

    }
    else if (setPtr->dataMemSize == 0 || setPtr->dataMemSize < -1)
	return;

    else if (setPtr->numPoints == setPtr->numPointsAlloc) {
	tmpInt = setPtr->numPointsAlloc + INCR_ALLOC_PTS;
	setPtr->xvec = (double *) realloc(setPtr->xvec,tmpInt*sizeof(double));
	if (setPtr->xvec == NULL) {
		sprintf(tmpStr,"Cannot reallocate dataset %s",setPtr->setName);
		errorReport(tmpStr);
	}
	setPtr->yvec = (double *) realloc(setPtr->yvec,tmpInt*sizeof(double));
	if (setPtr->yvec == NULL) {
		sprintf(tmpStr,"Cannot reallocate dataset %s",setPtr->setName);
		errorReport(tmpStr);
	}
	setPtr->id = (int *) realloc(setPtr->id,tmpInt*sizeof(int));
	if (setPtr->id == NULL) {
		sprintf(tmpStr,"Cannot reallocate dataset %s",setPtr->setName);
		errorReport(tmpStr);
	}
	setPtr->numPointsAlloc = tmpInt;
    }

    if (setPtr->moveOvalFlag == MOVE || setPtr->numPoints > 0) {
	setPtr->discardedX = setPtr->xvec[setPtr->numPoints];
	setPtr->discardedY = setPtr->yvec[setPtr->numPoints];
    }

    setPtr->xvec[setPtr->numPoints] = xval;
    setPtr->yvec[setPtr->numPoints] = yval;

    switch (pwInfoPtr->xAxisScale) {
	case LOG_SCALE:
	    xval = LOG10(xval);
	case LINEAR_SCALE:
	    canvX = MAPX(xval);
	    break;
    }
    switch (pwInfoPtr->yAxisScale) {
	case LOG_SCALE:
	    yval = LOG10(yval);
	case LINEAR_SCALE:
	    canvY = MAPY(yval);
	    break;
    }

    pwInfoPtr->curXMin = MIN(pwInfoPtr->curXMin,xval);
    pwInfoPtr->curXMax = MAX(pwInfoPtr->curXMax,xval);
    pwInfoPtr->curYMin = MIN(pwInfoPtr->curYMin,yval);
    pwInfoPtr->curYMax = MAX(pwInfoPtr->curYMax,yval);

    if (xval >= pwInfoPtr->xMax || xval <= pwInfoPtr->xMin || 
        yval >= pwInfoPtr->yMax || yval <= pwInfoPtr->yMin) {
	   fprintf(stderr,"Coordinates (%g,%g) are off scale\n",xval,yval);
	   setPtr->offScale = 1;
	   if (setPtr->type == SCATTER) {
	      tmpInt=Tcl_VarEval(interp,"createOval ",pwInfoPtr->toplevel,".",
			   pwInfoPtr->win," -1.0"," -1.0"," -1.0"," -1.0 ",
		           setPtr->ovalColor,(char *)NULL);
	   } else if (setPtr->type == LINE) {
	      tmpInt=Tcl_VarEval(interp,"mkCanvColorLine ",
				 pwInfoPtr->toplevel,".",pwInfoPtr->win,
				 " -1.0"," -1.0"," -1.0"," -1.0 ",
				 setPtr->color,(char *)NULL);
	   }
	   if (tmpInt != TCL_OK) 
		errorReport("Cannot plot point");
	   sscanf(interp->result,"%d",setPtr->id+(setPtr->numPoints++));
	   return;
    }

    if (setPtr->moveOvalFlag == MOVE) {
	if (setPtr->type == SCATTER) {
	   tmpX = canvX-RADIUS; tmpY = canvY-RADIUS;
	   currX = canvX+RADIUS; currY = canvY+RADIUS;
	} else if (setPtr->type == LINE) {
	   tmpX = setPtr->prevX; tmpY = setPtr->prevY;
	   currX = canvX; currY = canvY;
	   setPtr->prevX = canvX; setPtr->prevY = canvY;
	}
	if (setPtr->type == SCATTER ||
	    (tmpX != NULL && (!setPtr->offScale))) {
		sprintf(scmd,"moveOval %s.%s %d %lf %lf %lf %lf",
			pwInfoPtr->toplevel, pwInfoPtr->win,
			setPtr->id[(setPtr->numPoints++)],
			tmpX,tmpY,currX,currY);
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot move point");
		}
	   }
           setPtr->offScale = 0;
    } else {
	if (setPtr->type == SCATTER || setPtr->type == LINE_SCATTER) {
		sprintf(scmd,"createOval %s.%s %lf %lf %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win,
			canvX-RADIUS,canvY-RADIUS,
			canvX+RADIUS,canvY+RADIUS,setPtr->ovalColor);
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
			errorReport("Cannot plot point");
		}
		sscanf(interp->result,"%d",
		       setPtr->id+(setPtr->numPoints++));
	} else if (setPtr->type == LINE || setPtr->type == LINE_SCATTER) {
	   if ((!setPtr->offScale) && setPtr->prevX != NULL) {
		sprintf(scmd,"mkCanvColorLine %s.%s %lf %lf %lf %lf %s",
			pwInfoPtr->toplevel,pwInfoPtr->win,
			setPtr->prevX,setPtr->prevY,
			canvX,canvY,setPtr->color);
		if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK)
			errorReport("Cannot graph line");
		if (setPtr->type != LINE_SCATTER) 
			sscanf(interp->result,"%d",
			       setPtr->id+(setPtr->numPoints++)); 
	   }
	   setPtr->prevX = canvX;	setPtr->prevY = canvY;
	   setPtr->offScale = 0;
	}
    }
}

/*
 * Reads data from a file
 */

static int
readDataCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    int width, height;
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;
    graph_DataSet *setPtr;
    double xval, yval;
    FILE *fp;

    if (argc != 3) {
	Tcl_SetResult(interp,
	    "Wrong number of args.  Must be \"readData filename type\"",
	    TCL_STATIC);
	return TCL_ERROR;
    }
    if (sscanf(argv[2],"%d",&tmpInt) != 1) {
	sprintf(tmpStr,"Invalid type %s",argv[2]);
	errorReport(tmpStr);
	return TCL_ERROR;
    }
    if ((fp = fopen(argv[1],"r")) == NULL) {
	sprintf(tmpStr,"Cannot read \"%s\"",argv[1]);
	errorReport(tmpStr);
        return TCL_ERROR;
    }

    if ((setPtr = ptkPlotCreateDataset(pwInfoPtr,tmpInt,"setname",(char *)NULL,
			   (char *)NULL,-1))== NULL) {
	errorReport("Cannot create dataset");
	return TCL_ERROR;
    }
    while (fscanf(fp,"%lf %lf",&xval,&yval) == 2)
	ptkPlotFill(pwInfoPtr,xval,yval,setPtr);
    fclose(fp);
    return TCL_OK;
}

/*
 * Call Tcl routines to draw lines.
 */

static graph_PWInfo *drawInCanvas(pwInfoPtr)
    graph_PWInfo *pwInfoPtr;
{
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;

    sprintf(scmd,"mkCanvTitle %s.%s \"%s\" %lf %lf %s",pwInfoPtr->toplevel,
	    pwInfoPtr->win,pwInfoPtr->identifier,canvasInfoPtr->titleXCoord,
	    canvasInfoPtr->titleYCoord,TITLE_FONT);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanvTitle %s.%s %s %lf %lf %s", pwInfoPtr->toplevel,
	    pwInfoPtr->win, pwInfoPtr->xAxisTitle,canvasInfoPtr->xTitleXCoord,
	    canvasInfoPtr->xTitleYCoord,REG_FONT);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanvTitle %s.%s %s %lf %lf %s", pwInfoPtr->toplevel,
	    pwInfoPtr->win, pwInfoPtr->yAxisTitle,canvasInfoPtr->yTitleXCoord,
	    canvasInfoPtr->yTitleYCoord,REG_FONT);


    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanvLine %s.%s %lf %lf %lf %lf",pwInfoPtr->toplevel,
	    pwInfoPtr->win,canvasInfoPtr->llx, canvasInfoPtr->lly,
	    canvasInfoPtr->urx,canvasInfoPtr->lly);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanvLine %s.%s %lf %lf %lf %lf",pwInfoPtr->toplevel,
	    pwInfoPtr->win,canvasInfoPtr->llx, canvasInfoPtr->lly,
	    canvasInfoPtr->llx, canvasInfoPtr->ury);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanvLine %s.%s %lf %lf %lf %lf",pwInfoPtr->toplevel,
	    pwInfoPtr->win,canvasInfoPtr->llx, canvasInfoPtr->ury,
	    canvasInfoPtr->urx, canvasInfoPtr->ury);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanvLine %s.%s %lf %lf %lf %lf",pwInfoPtr->toplevel,
	    pwInfoPtr->win,canvasInfoPtr->urx, canvasInfoPtr->lly,
	    canvasInfoPtr->urx, canvasInfoPtr->ury);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }
    createMarkings(pwInfoPtr);
    redrawData(pwInfoPtr);
}

/*
 * Redraw the canvas
 */

static int
redrawCanvasCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    int width, height;
    graph_PWInfo *pwInfoPtr = (graph_PWInfo *)clientData;
    graph_CanvasInfo *canvasInfoPtr = pwInfoPtr->canvasInfoPtr;

    if (argc != 3) {
       Tcl_SetResult(interp,
	   "Wrong number of args.  Must be \"redrawCanvas width height\"",
           TCL_STATIC);
	return TCL_ERROR;
    }

    sscanf(argv[1],"%d",&width); sscanf(argv[2],"%d",&height);
    if (width == canvasInfoPtr->canvasWidth &&
	height == canvasInfoPtr->canvasHeight) return TCL_OK;
    sprintf(scmd,"%s.%s.c delete nonData",pwInfoPtr->toplevel,pwInfoPtr->win);
    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot delete canvas items");
		return TCL_ERROR;
    }

    canvasInfoPtr->canvasWidth = width; canvasInfoPtr->canvasHeight = height;
    pwInfoPtr->width = width-canvasInfoPtr->maxWidth-
	canvasInfoPtr->yTitleWidth -PADX;
    pwInfoPtr->height = height-canvasInfoPtr->maxHeight-PADY-
	canvasInfoPtr->titleHeight-canvasInfoPtr->xTitleHeight;
    computeLoc(pwInfoPtr);
    if (drawInCanvas(pwInfoPtr) == NULL) return	TCL_ERROR;

    return TCL_OK;
}


/*
 * This procedure is used for creating plot windows within 'toplevel'.
 * The arguments are described in ptkPlotInit().
 * Returns data structure to be passed to ptkPlotFill()
 * removePlotWin() might be useful.
 */

graph_PWInfo* ptkPlotMakeWin
	     (toplevel,identifier,win,loc,dim,autoscale,
	     xAxisScale,yAxisScale,xAxisTitle,yAxisTitle,
	     xMin,xMax,yMin,yMax)
    char *toplevel; char *identifier; char *win;
    char *loc; char *dim;
    int autoscale; int xAxisScale; int yAxisScale;
    char *xAxisTitle; char *yAxisTitle;
    double xMin; double xMax; double yMin; double yMax;
{
    graph_PWInfo *pwInfoPtr;
    graph_CanvasInfo *canvasInfoPtr;

    if ((pwInfoPtr = (graph_PWInfo *)malloc(sizeof(graph_PWInfo))) == NULL) {
	errorReport("Cannot allocate pwInfo structure pointer");
	return (graph_PWInfo *)NULL;
    }
    pwInfoPtr->toplevel = STRDUP(toplevel);
    pwInfoPtr->identifier = STRDUP(identifier);
    pwInfoPtr->win = STRDUP(win);
    pwInfoPtr->loc = STRDUP(loc);
    pwInfoPtr->autoscale = autoscale;
    pwInfoPtr->xAxisScale = xAxisScale;
    pwInfoPtr->yAxisScale = yAxisScale;
    pwInfoPtr->xAxisTitle = STRDUP(xAxisTitle);
    pwInfoPtr->yAxisTitle = STRDUP(yAxisTitle);
    pwInfoPtr->zoomNum = 0;
    pwInfoPtr->marks = TICKMARKS;
    pwInfoPtr->showAxes = 0;
    if (xMax < xMin) {
	tmpDouble = xMax; xMax = xMin; xMin = tmpDouble;
    } else if (xMax == xMin) {
	xMin = (xMin * 0.9) -1.0;
	xMax = (xMax * 1.1) +1.0;
    }
    if (yMax < yMin) {
	tmpDouble = yMax; yMax = yMin; yMin = tmpDouble;
    } else if (yMax == yMin) {
	yMin = (yMin * 0.9) -1.0;
	yMax = (yMax * 1.1) +1.0;
    }

    pwInfoPtr->xMin = ((pwInfoPtr->xAxisScale == LOG_SCALE) ? LOG10(xMin) :
		       xMin);
    pwInfoPtr->xMax = ((pwInfoPtr->xAxisScale == LOG_SCALE) ? LOG10(xMax) :
		       xMax);
    pwInfoPtr->yMin = ((pwInfoPtr->yAxisScale == LOG_SCALE) ? LOG10(yMin) :
		       yMin);
    pwInfoPtr->yMax = ((pwInfoPtr->yAxisScale == LOG_SCALE) ? LOG10(yMax) :
		       yMax);
    pwInfoPtr->curXMin = pwInfoPtr->curXMax = pwInfoPtr->curYMin =
	pwInfoPtr->curYMax = 0.0;
    pwInfoPtr->numSets = 0;
    pwInfoPtr->firstSetPtr = (graph_DataSet *)NULL;
    pwInfoPtr->lastSetPtr = (graph_DataSet *)NULL;
    pwInfoPtr->canvasInfoPtr = (graph_CanvasInfo *)NULL;
    pwInfoPtr->red = COLOR_LOWER; pwInfoPtr->green = COLOR_LOWER;
    pwInfoPtr->blue = COLOR_LOWER; pwInfoPtr->color = COLOR_RED;

    if (sscanf(dim,"%dx%d",&(pwInfoPtr->width),&(pwInfoPtr->height)) != 2) {
	sprintf(tmpStr,"Invalid geometry: %s",dim);
	errorReport(tmpStr);
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"makePlotMenu %s \"%s\" %s",toplevel,identifier,win);
    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot make the menubar");
	return (graph_PWInfo *)NULL;
    }

    if ( (canvasInfoPtr = computeLabel(pwInfoPtr)) == NULL) {
	errorReport("Cannot compute proper x and y tick labels");
	return (graph_PWInfo *)NULL;
    }

    if ((canvasInfoPtr = computeGeo(pwInfoPtr)) == NULL) {
	errorReport("Cannot compute locations");
	return (graph_PWInfo *)NULL;
    }

    sprintf(scmd,"mkCanv %s.%s %d %d", pwInfoPtr->toplevel,pwInfoPtr->win,
	    canvasInfoPtr->canvasWidth,canvasInfoPtr->canvasHeight);

    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create canvas");
	return (graph_PWInfo *)NULL;
    }

    if (drawInCanvas(pwInfoPtr) == NULL) return (graph_PWInfo *)NULL;
    else return pwInfoPtr;
}

static int
zoomInCmd(clientData,interp,argc,argv)
    ClientData clientData;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    graph_PWInfo *pwSrcInfoPtr = (graph_PWInfo *)clientData;
    graph_PWInfo *pwInfoPtr;
    graph_DataSet *setPtr, *srcSetPtr;
    double xMin,xMax,yMin,yMax;
    int i;
    char dim[15];

    if (argc != 5) {
        Tcl_SetResult(interp,"Wrong number of args.  \"zoomIn x1 y1 x2 y2\"",
                      TCL_STATIC);
        return TCL_ERROR;
    }

    if (sscanf(*++argv,"%lf",&xMin) != 1) {
	sprintf(tmpStr,"Non-integer argument: %s",*argv);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (sscanf(*++argv,"%lf",&yMin) != 1) {
	sprintf(tmpStr,"Non-integer argument: %s",*argv);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (sscanf(*++argv,"%lf",&xMax) != 1) {
	sprintf(tmpStr,"Non-integer argument: %s",*argv);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (sscanf(*++argv,"%lf",&yMax) != 1) {
	sprintf(tmpStr,"Non-integer argument: %s",*argv);
	Tcl_SetResult(interp,tmpStr,TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (xMin > xMax) {
	tmpDouble = xMax;
	xMax = xMin;
	xMin = tmpDouble;
    }

    if (yMin > yMax) {
	tmpDouble = yMax;
	yMax = yMin;
	yMin = tmpDouble;
    }
    sprintf(tmpStr,"%s_zoom%d",pwSrcInfoPtr->win,pwSrcInfoPtr->zoomNum++);
    sprintf(dim,"%dx%d",pwSrcInfoPtr->width,pwSrcInfoPtr->height);
    if ((pwInfoPtr = makePlotWin(pwSrcInfoPtr->toplevel,
				 pwSrcInfoPtr->identifier,
				 tmpStr,pwSrcInfoPtr->loc,
				 dim,-1,
				 pwSrcInfoPtr->xAxisScale,
				 pwSrcInfoPtr->yAxisScale,
				 pwSrcInfoPtr->xAxisTitle,
				 pwSrcInfoPtr->yAxisTitle,
				 xMin,xMax,yMin,yMax)) == NULL) {
	Tcl_SetResult(interp,"Cannot create plot window",TCL_STATIC);
	return TCL_ERROR;
    }

    for ( srcSetPtr=pwSrcInfoPtr->firstSetPtr; srcSetPtr != NULL;
	 srcSetPtr = srcSetPtr->nextSetPtr) {
	if ((setPtr =
	     ptkPlotCreateDataset(pwInfoPtr,srcSetPtr->type,srcSetPtr->setName,
		   srcSetPtr->color,srcSetPtr->ovalColor, -1)) == NULL) {
			Tcl_SetResult(interp,
				      "Cannot create dataset",TCL_STATIC);
			return TCL_ERROR;
	}
        if (srcSetPtr->moveOvalFlag == MOVE) {
	    if (WITHIN(xMin,xMax,srcSetPtr->discardedX) &&
		WITHIN(yMin,yMax,srcSetPtr->discardedY) &&
		srcSetPtr->type != SCATTER) {
			ptkPlotFill(pwInfoPtr,srcSetPtr->discardedX,
				     srcSetPtr->discardedY,setPtr);
	    }
            for (i = srcSetPtr->numPoints; i < srcSetPtr->dataMemSize; i++) {
		if (WITHIN(xMin,xMax,srcSetPtr->xvec[i]) &&
		    WITHIN(yMin,yMax,srcSetPtr->yvec[i])) {
			ptkPlotFill(pwInfoPtr,srcSetPtr->xvec[i],
				     srcSetPtr->yvec[i],setPtr);
		}
	    }
	}
	for (i = 0; i < srcSetPtr->numPoints; i++) {
		if (WITHIN(xMin,xMax,srcSetPtr->xvec[i]) &&
		    WITHIN(yMin,yMax,srcSetPtr->yvec[i])) {
			ptkPlotFill(pwInfoPtr,srcSetPtr->xvec[i],
				     srcSetPtr->yvec[i],setPtr);
		}
	}
    }
    return TCL_OK;
}

/*
 * This procedure is used for creating top level containing possibly
 *   multiple plot windows.
 * Multiple top level plot windows may be initialized.
 *
 * toplevel	The Tk toplevel window; Requires a preceding '.'
 * identifier	The identifer used for the icon and title names.
 * win		The window and icon name of the initialized plot window 
 * loc		The geometry location of the toplevel window in 
 *		  root window coordinates [+-]y[+-]x
 * dim		The dimensions of the plot window
 * autoscale	Gives the number of total points plotted after which
 *		  scaling the axes to the data is desired
 *		  With a value of "-1", no scaling is performed
 * xAxisScale	The scale of the x-axis.  Either log or linear.
 * yAxisScale	The scale of the y-axis.  Either log or linear.
 * xAxisTitle	The x-axis title
 * yAxisTitle	The y-axis title
 * xMin, yMin	Gives coordinates of the lower left corner of the plot window
 * xMax, yMax   Gives coordinates of the upper right corner of the plot window 
 * 
	* Note:	.win with the preceding '.' must be a valid window name
 * 
 */

graph_PWInfo* ptkPlotInit
	     (toplevel,identifier,win,loc,dim,autoscale,
	     xAxisScale,yAxisScale,xAxisTitle,yAxisTitle,
	     xMin,xMax,yMin,yMax)
    char *toplevel; char *identifier; char *win;
    char *loc; char *dim;
    int autoscale; int xAxisScale; int yAxisScale;
    char *xAxisTitle; char *yAxisTitle;
    double xMin; double xMax; double yMin; double yMax;
{
    graph_PWInfo *pwInfoPtr;
    static int declared = 0;

    sprintf(scmd,"ptkPlotInit %s \"%s\" %s", toplevel,identifier,loc);
    if (Tcl_Eval(interp,scmd,0,(char **)NULL) != TCL_OK) {
	errorReport("Cannot create toplevel window");
	return (graph_PWInfo *)NULL;
    }
    if ((pwInfoPtr = ptkPlotMakeWin(toplevel,identifier,win,loc,dim,autoscale,
				 xAxisScale,yAxisScale,xAxisTitle,yAxisTitle,
				 xMin,xMax,yMin,yMax)) == NULL) {
	errorReport("Cannot create plot window");
	return (graph_PWInfo *)NULL;
    }

/* Should be (ClientData)&pwInfoPtr.. */

    if (!declared) {
	Tcl_CreateCommand(interp,"rescale",rescaleCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"saveToFile",saveToFileCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"redrawCanvas",redrawCanvasCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"readData",readDataCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"redrawData",redrawDataCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"revMap",revMapCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"zoomIn",zoomInCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"showGrid",showGridCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"showXYAxes",showXYAxesCmd,
			  (ClientData)pwInfoPtr,(Tcl_CmdDeleteProc *)NULL);

	declared = 1;
    }

    return pwInfoPtr;
}
