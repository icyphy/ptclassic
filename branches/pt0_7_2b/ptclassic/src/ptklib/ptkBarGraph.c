/*
 * C interface files for creating a bar graph
 *
 * Author: Edward A. Lee
 * Version: $Id$
 */
/*
Copyright (c) 1993 The Regents of the University of California.
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

#include "tkConfig.h"
#include "tkInt.h"
#define COMMANDSIZE 512
#define REPORT_TCL_ERRORS 1

static char command[COMMANDSIZE];


/*
 *----------------------------------------------------------------------
 * Internal functions for bar graph
 *----------------------------------------------------------------------
 * Routine to scale the fullScale data member of the owner of a bar graph.
 */
static int
verticalScale(fullScale, interp, argc, argv)
    double* fullScale;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    float temp;
    if(sscanf(argv[1], "%4f", &temp) != 1) {
	Tcl_AppendResult(interp,
		"Cannot reset full scale in bar graph",(char*) NULL);
	return TCL_ERROR;
    }
    *fullScale = *fullScale * temp;

    /* Return new full scale value to tcl */
    sprintf(interp->result, "%f", *fullScale);

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 * Create a bar graph in its own window
 *----------------------------------------------------------------------
 *     interp: pointer to the tcl interpreter
 *     name: name of the top level window
 *     desc: description to be put in the window
 *     data: an array of data to be plotted
 *     numBars: number of bars in the bar graph
 *     fullScale: pointer to value of the full scale on the bar graph
 *     id: an array to fill with item IDs
 *     geo: geometry of the overall window, in the form =AxB+C+D
 *     width: width of the bar graph itself
 *     height: height of the bar graph itself
 *
 * Returns 1 if bar graph is successfully created, 0 otherwise.
 */

int ptkMakeBarGraph
(interp, win, name, desc, data, numBars, fullScale, id, geo, deswidth, desheight)
    Tcl_Interp *interp;
    Tk_Window *win;
    char *name, *desc;
    double *data, *fullScale;
    int numBars;
    int *id;
    char *geo;
    double deswidth, desheight;
{
    int i;
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Register the function to reset fullScale with Tcl */
    sprintf(command, "%sverticalScale", name);
    Tcl_CreateCommand (interp, command, verticalScale,
            (ClientData) fullScale, (void (*)()) NULL);

    /* Make the bar graph */
    sprintf(command, "ptkMakeBarGraph %s \"%s\" \"%s\" %d %f %f",
	name, desc, geo, numBars, deswidth, desheight);
    if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
        return 0;

    /* Get the actual plot window size, which may differ from that requested */
    sprintf(command, "%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,win);
    if (plotwin == 0) return 0;
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    for (i=0;i<numBars;i++) {
	x0 = ((i+0.1)*width)/numBars;
	y0 = height/2.0;
	x1 = ((i+0.9)*width)/numBars;
	y1 = (1-data[i]/(*fullScale))*height/2;
        sprintf(command, "%s.pf.plot coords %d %d %d %d %d",
		name, id[i], x0, y0, x1, y1);

        sprintf(command,
		"%s.pf.plot create rect %fc %fc %fc %fc -fill red3",
		name, x0, y0, x1, y1);
        if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
            return 0;
        else
            sscanf(interp->result,"%d",&id[i]);
    }
    return 1;
}

/*
 *----------------------------------------------------------------------
 * Set the bars in a bar graph
 *----------------------------------------------------------------------
 *     interp: pointer to the tcl interpreter
 *     win: a pointer to a reference window (top level window)
 *     name: name of the top level window
 *     data: an array of data to be plotted
 *     numBars: number of bars in the bar graph
 *     fullScale: pointer to value of the full scale on the bar graph
 *     id: an array of item IDs filled by makeBarChart
 *
 * Returns 1 if bar graph is successfully updated, 0 otherwise.
 */

/* this check traps IEEE infinities and NaN values
 * the ifdef condition should really be: implement this function if the
 * processor uses IEEE arithmetic.
 */
#ifdef mips
#include <nan.h>
#endif
#ifdef sun
#define IsNANorINF(X) (isnan(X) || isinf(X))
#endif
#ifndef IsNANorINF
#define IsNANorINF(X) 0
#endif

int ptkSetBarGraph (interp, win, name, data, numBars, fullScale, id)
    Tcl_Interp *interp;
    Tk_Window *win;
    char *name;
    double *data;
    int numBars;
    double *fullScale;
    int *id;
{
    int i;
    int width, height;
    int x0, y0, x1, y1;
    Tk_Window plotwin;

    /* Get the current plot window size */
    sprintf(command, "%s.pf.plot", name);
    plotwin = Tk_NameToWindow(interp,command,win);
    if (plotwin == 0) return 0;
    width = Tk_Width (plotwin);
    height = Tk_Height (plotwin);

    for (i=0;i<numBars;i++) {
	x0 = ((i+0.1)*width)/numBars;
	y0 = height*0.5;
	x1 = ((i+0.9)*width)/numBars;
	if(IsNANorINF(data[i]) || data[i] > *fullScale) y1 = 0.0;
	else if(data[i] < -(*fullScale)) y1 = height;
	else y1 = (1-data[i]/(*fullScale))*height*0.5;
        sprintf(command, "%s.pf.plot coords %d %d %d %d %d",
		name, id[i], x0, y0, x1, y1);
        if(Tcl_Eval(interp, command, 0, (char**)NULL) != TCL_OK)
            return 0;
    }
    return 1;
}
