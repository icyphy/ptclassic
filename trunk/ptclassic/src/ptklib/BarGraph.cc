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
static const char SccsId[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
$Id$

Author: Edward A. Lee

Defines and maintains a Tcl bar graph.

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
#ifdef __GNUG__
#pragma implementation
#endif

#include "BarGraph.h"
#include "ptkBarGraph.h"
#include "ieee.h"

/////////////////////////////////////////////////////////////////////////
//			Tcl Callable Procedures
//
// Define the callback procedure used by Tcl to redraw the bar graph
static int redraw(
    ClientData bargraph,	// Pointer to the BarGraph object
    Tcl_Interp*, 		// Current interpreter
    int,           		// Number of arguments
    char *[]         		// Argument strings
) {
    ((BarGraph*)bargraph)->redrawBars();
    return TCL_OK;
}

// Define the callback procedure used by Tcl to rescale the bar graph
// Returns the top and bottom of the range after rescaling
static int rescale(
    ClientData bargraph,	// Pointer to the BarGraph object
    Tcl_Interp *interp, 	// Current interpreter
    int,           		// Number of arguments
    char *argv[]         	// Argument strings
) {
    float scaleFactor;
    if(sscanf(argv[1], "%4f", &scaleFactor) != 1) {
	Tcl_AppendResult(interp,
	    "Cannot modify scale in bar graph",(char*) NULL);
	return TCL_ERROR;
    }
    ((BarGraph*)bargraph)->rescaleGraph(scaleFactor);
    InfString buf = ((BarGraph*)bargraph)->top;
    buf += " ";
    buf += ((BarGraph*)bargraph)->bottom;
    Tcl_SetResult(interp,(char*)buf, TCL_VOLATILE);
    return TCL_OK;
}

/////////////////////////////////////////////////////////////////////////
//			Methods for BarGraph class
//
// constructor
BarGraph::BarGraph () {
	starID = "ptkBarGraph";
	starID += unique++;
	data = NULL;
	ids = NULL;
	winName = "";
}

// destructor
BarGraph::~BarGraph() {
	// avoid core dump if interpreter did not set up right
	if (!ptkInterp) return;

	InfString buf;

	// Delete Tcl commands created by this object
	buf = winName;
	buf += "redraw";
	Tcl_DeleteCommand(ptkInterp, (char*)buf);

	// Free memory allocated in setup
	int i;
	if(data != NULL) {
	    for (i=0; i<noInputs; i++) { LOG_DEL; delete [] data[i]; }
	    LOG_DEL; delete [] data;
	}
	if(ids != NULL) {
	    for (i=0; i<noInputs; i++) { LOG_DEL; delete [] ids[i]; }
	    LOG_DEL; delete [] ids;
	}
}


int BarGraph::setup (Block* star,       // The star I am in
                   char* desc,          // Label for the bar chart
                   int numInputs,       // The number of data sets to plot
                   int numBars,         // The number of bars per data set
                   double initTop,      // Top of the range
                   double initBottom,   // Bottom of the range
                   char* geometry,      // Geometry for the window
                   double width,        // Desired width of the window (cm)
                   double height)  	// Desired height of the window (cm)
{
	int i;		// Loop counter repeatedly used

	// Store the parameters
	top = initTop;
	bottom = initBottom;
	myStar = star;
	noBars = numBars;
	noInputs = numInputs;

	// Allocate the memory for storing the data
	// First, delete previous versions
	if(data != NULL) {
	    for (i=0; i<noInputs; i++) { LOG_DEL; delete [] data[i]; }
	    LOG_DEL; delete [] data;
	}
	LOG_NEW; data = new double* [noInputs];
	for (i=0; i<noInputs; i++) { LOG_NEW; data[i] = new double[noBars]; }

	for (i=0; i<noInputs; i++)
	    for (int j=0; j<noBars; j++)
		data[i][j]=0;

	// Allocate the memory for storing the object IDs from Tk
	if(ids != NULL) {
	    for (i=0; i<noInputs; i++) { LOG_DEL; delete [] ids[i]; }
	    LOG_DEL; delete [] ids;
	}
	LOG_NEW; ids = new int* [noInputs];
	for (i=0; i<noInputs; i++) { LOG_NEW; ids[i] = new int[noBars]; }

	// First check to see whether a global Tcl variable named
	// "ptkControlPanel" is defined.  If so, then use it as the
	// stem of the window name.  This means that the window will
	// be a child window of the control panel, and hence will be
	// destroyed when the control panel window is destroyed.
	if(Tcl_GlobalEval(ptkInterp, "global ptkControlPanel;set ptkControlPanel") != TCL_OK) {
		winName = ".bar";
		winName += starID;
	} else {
		winName = ptkInterp->result;
		winName += ".bar";
		winName += starID;
	}

	// Define the Tcl procedure to redraw the bar graph
	InfString buf;
	buf = winName;
	buf += "redraw";
	Tcl_CreateCommand(ptkInterp, (char*)buf, redraw,
		(ClientData)this, NULL);

	// Define the Tcl procedure to rescale the bar graph
	buf = winName;
	buf += "rescale";
	Tcl_CreateCommand(ptkInterp, (char*)buf, rescale,
		(ClientData)this, NULL);

	// Create the window
	if(ptkMakeBarGraph(ptkInterp,
		&ptkW,
		(char*)winName,
		desc,
		data,
		noInputs,
		noBars,
		top,
		bottom,
		ids,
		geometry,
		width,
		height) == 0) {
		    Error::abortRun(*star, "Cannot create bar chart");
		    return FALSE;
	}

	// Query the window to determine the actual height and width
	buf = winName;
	buf += ".pf.plot";
	Tk_Window plotwin = Tk_NameToWindow(ptkInterp,(char*)buf,ptkW);
	if (plotwin == 0) return FALSE;
	actualWidth = Tk_Width (plotwin);
	actualHeight = Tk_Height (plotwin);
	
	return TRUE;
}

int BarGraph::windowExists () {
	return (Tk_NameToWindow(ptkInterp,(char*)winName,ptkW) != 0);
}

// Update a single bar of the bar graph.
// The "input" argument should be in the range from 0 to noInputs-1.
int BarGraph::update (int input,          // Identifies the data set
                      int bar,            // Identifies the bar within the set
                      double newVal)
{
	data[input][bar] = newVal;

	// Update the one bar that is affected
	int x0,y0,x1,y1;
	ptkFigureBarEdges(&x0,&x1,&y0,&y1,bar,input,
		noInputs, noBars, actualWidth, actualHeight,
		top, bottom, newVal);

	InfString buf = winName;
	buf += ".pf.plot coords ";
	buf += ids[input][bar];
	buf += " "; buf += x0;
	buf += " "; buf += y0;
	buf += " "; buf += x1;
	buf += " "; buf += y1;
	if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) return 0;

	return TRUE;
}

void BarGraph::redrawBars () {
	ptkSetBarGraph (ptkInterp,
		&ptkW,
		(char*)winName,
		data,
		noInputs,
		noBars,
		top,
		bottom,
		ids);
	// Update the size parameters, in case they have changed
	// Set plotwin member
	InfString buf = winName;
	buf += ".pf.plot";
	Tk_Window plotwin = Tk_NameToWindow(ptkInterp,(char*)buf,ptkW);
	if (plotwin == 0) return;
	actualWidth = Tk_Width (plotwin);
	actualHeight = Tk_Height (plotwin);
}

void BarGraph::rescaleGraph (float scaleFactor) {
	top *= scaleFactor;
	bottom *= scaleFactor;
	redrawBars();
}

// Initialize the static counter for unique names.
// The initialization occurs at load time.
int BarGraph::unique = 0;
