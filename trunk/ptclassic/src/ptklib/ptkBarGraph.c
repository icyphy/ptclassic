/*
 * C interface files for creating a bar graph
 *
 * Author: Edward A. Lee
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

#include "tk.h"
#include <math.h>		/* Pick up isnan() and isinf(). */

/* Number of colors defined in the ptkOptions database */
#define NUMBER_OF_COLORS 12
/*
 * Note that although the following include is in the ptolemy kernel
 * directory, it does not pull in any C++ code.
 */
#include "ieee.h"
#define COMMANDSIZE 512
#define REPORT_TCL_ERRORS 1

static char command[COMMANDSIZE];

/*
 *----------------------------------------------------------------------
 * Compute the edges of a rectagle given a bunch of parameters
 *----------------------------------------------------------------------
 *	x0,x1,y0,y1:	pointers to integers into which to store the results
 *	i,j:		bar number and trace number
 *	numTraces:	number of traces
 *	numBars:	number of bars
 *	width:		width the of the canvas
 *	height:		height the of the canvas
 *	top:		top of the range
 *	bottom:		bottom of the range
 *	data:		data to plot, relative to top and bottom
 */
void ptkFigureBarEdges(x0,x1,y0,y1,
		i,j,
		numTraces, numBars, width, height,
		top, bottom, data)
    int *x0, *x1, *y0, *y1, i, j, numTraces, numBars, width, height;
    double top, bottom, data;
{
	if (numTraces > 1) {
	    *x0 = ((i+0.25-j*0.5/numTraces)*width)/numBars;
	    *x1 = ((i+0.75-j*0.5/numTraces)*width)/numBars;
	} else {
	    *x0 = ((i+0.125)*width)/numBars;
	    *x1 = ((i+0.875)*width)/numBars;
	}
	    *y0 = height*top/(top - bottom);
	    if (*y0 > height) *y0 = height;
	    if (*y0 < 0) *y0 = 0;
	    if(IsNANorINF(data) || data > top) *y1 = 0;
	    else if(data < bottom) *y1 = height;
	    else *y1 = height*(top-data)/(top-bottom);
}

/*
 *----------------------------------------------------------------------
 * Create a bar graph in its own window
 *----------------------------------------------------------------------
 *     interp: pointer to the tcl interpreter
 *     name: name of the top level window
 *     desc: description to be put in the window
 *     data: an array of arrays of data to be plotted
 *     numTraces: number of traces in the bar graph
 *     numBars: number of bars in the bar graph
 *     top: top of the range of bar graph
 *     bottom: bottom of the range of bar graph
 *     id: an array of arrays to fill with item IDs
 *     geo: geometry of the overall window, in the form =AxB+C+D
 *     width: width of the bar graph itself
 *     height: height of the bar graph itself
 *
 * Returns 1 if bar graph is successfully created, 0 otherwise.
 */

int ptkMakeBarGraph
(interp, win, name, desc, data, numTraces, numBars, top, bottom,
id, geo, deswidth, desheight)
    Tcl_Interp *interp;
    Tk_Window *win;
    char *name, *desc;
    double **data, top, bottom;
    int numTraces, numBars;
    int **id;
    char *geo;
    double deswidth, desheight;
{
    int i,j,color;
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Make the bar graph */
    sprintf(command, "ptkMakeBarGraph %s \"%s\" \"%s\" %d %f %f [curuniverse]",
	name, desc, geo, numBars, deswidth, desheight);
    if(Tcl_Eval(interp, command) != TCL_OK)
        return 0;

    /* Get the actual plot window size, which may differ from that requested */
    sprintf(command, "%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,*win);
    if (plotwin == 0) return 0;
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    for (j=0;j<numTraces;j++) {
	for (i=0;i<numBars;i++) {
	    ptkFigureBarEdges(&x0,&x1,&y0,&y1, i,j,
                numTraces, numBars, width, height,
                top, bottom, data[j][i]);

	    color = j%NUMBER_OF_COLORS + 1;
            sprintf(command,
		"%s.pf.plot create rect %d %d %d %d -fill [option get . plotColor%d PlotColor%d]",
		name, x0, y0, x1, y1, color, color);
            if(Tcl_Eval(interp, command) != TCL_OK)
                return 0;
            else
                sscanf(interp->result,"%d",&id[j][i]);

            sprintf(command, "%s.pf.plot coords %d %d %d %d %d",
		name, id[j][i], x0, y0, x1, y1);
	}
    }
    return 1;
}

/*
 * For maximal efficiency, there are several versions of the routines that
 * set the bars in the bar graph.  Use the one most suited.
 */

/*
 *----------------------------------------------------------------------
 * Set all the bars in a bar graph
 *----------------------------------------------------------------------
 *     interp: pointer to the tcl interpreter
 *     win: a pointer to a reference window (top level window)
 *     name: name of the top level window
 *     data: an array of arrays of data to be plotted
 *     numBars: number of bars in the bar graph
 *     top: top of the range of bar graph
 *     bottom: bottom of the range of bar graph
 *     id: an array of arrays of item IDs filled by makeBarChart
 *
 * Returns 1 if bar graph is successfully updated, 0 otherwise.
 * This procedure queries the window for its actual current width.
 */

int ptkSetBarGraph (interp, win, name, data, numTraces,
	numBars, top, bottom, id)
    Tcl_Interp *interp;
    Tk_Window *win;
    char *name;
    double **data;
    int numTraces, numBars;
    double top, bottom;
    int **id;
{
    int i,j;
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Get the current plot window size */
    sprintf(command, "%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,*win);
    if (plotwin == 0) return 0;
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    for (j=0;j<numTraces;j++) {
	for (i=0;i<numBars;i++) {
	    ptkFigureBarEdges(&x0,&x1,&y0,&y1, i,j,
                numTraces, numBars, width, height,
                top, bottom, data[j][i]);

            sprintf(command, "%s.pf.plot coords %d %d %d %d %d",
		name, id[j][i], x0, y0, x1, y1);
            if(Tcl_Eval(interp, command) != TCL_OK)
                return 0;
	}
    }
    return 1;
}

/*
 *----------------------------------------------------------------------
 * Set the a single bar in a bar graph
 *----------------------------------------------------------------------
 *     interp: pointer to the tcl interpreter
 *     win: a pointer to a reference window (top level window)
 *     name: name of the top level window
 *     data: an array of arrays of data to be plotted
 *     numBars: number of bars in the bar graph
 *     top: top of the range of bar graph
 *     bottom: bottom of the range of bar graph
 *     id: an array of arrays of item IDs filled by makeBarChart
 *     whichTrace: the number of the trace being updated
 *     whichBar: the number of the bar to update
 *
 * Returns 1 if bar graph is successfully updated, 0 otherwise.
 * This procedure queries the window for its actual current width.
 */

int ptkSetOneBar (interp, win, name, data, numTraces,
	numBars, top, bottom, id, whichTrace, whichBar)
    Tcl_Interp *interp;
    Tk_Window *win;
    char *name;
    double **data;
    int numTraces, numBars;
    double top, bottom;
    int **id;
    int whichTrace, whichBar;
{
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Get the current plot window size */
    sprintf(command, "%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,*win);
    if (plotwin == 0) return 0;
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    ptkFigureBarEdges(&x0,&x1,&y0,&y1, whichBar, whichTrace,
		      numTraces, numBars, width, height,
		      top, bottom, data[whichTrace][whichBar]);

    sprintf(command, "%s.pf.plot coords %d %d %d %d %d",
	    name, id[whichTrace][whichBar], x0, y0, x1, y1);
    if(Tcl_Eval(interp, command) != TCL_OK)
      return 0;
    return 1;
}
/*
 *----------------------------------------------------------------------
 * Set the a family of bars in a bar graph (all traces, one bar)
 *----------------------------------------------------------------------
 *     interp: pointer to the tcl interpreter
 *     win: a pointer to a reference window (top level window)
 *     name: name of the top level window
 *     data: an array of arrays of data to be plotted
 *     numBars: number of bars in the bar graph
 *     top: top of the range of bar graph
 *     bottom: bottom of the range of bar graph
 *     id: an array of arrays of item IDs filled by makeBarChart
 *     whichBar: the number of the bar to update
 *
 * Returns 1 if bar graph is successfully updated, 0 otherwise.
 * This procedure queries the window for its actual current width.
 */

int ptkSetOneFamily (interp, win, name, data, numTraces,
	numBars, top, bottom, id, whichBar)
    Tcl_Interp *interp;
    Tk_Window *win;
    char *name;
    double **data;
    int numTraces, numBars;
    double top, bottom;
    int **id;
    int whichBar;
{
    int j;
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Get the current plot window size */
    sprintf(command, "%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,*win);
    if (plotwin == 0) return 0;
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    for (j=0;j<numTraces;j++) {
      ptkFigureBarEdges(&x0,&x1,&y0,&y1, whichBar, j,
			numTraces, numBars, width, height,
			top, bottom, data[j][whichBar]);

      sprintf(command, "%s.pf.plot coords %d %d %d %d %d",
	      name, id[j][whichBar], x0, y0, x1, y1);
      if(Tcl_Eval(interp, command) != TCL_OK)
	return 0;
    }
    return 1;
}
