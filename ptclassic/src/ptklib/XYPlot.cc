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
static const char file_id[] = "XYPlot.cc";

/**************************************************************************
Version identification:
$Id$

Author: Edward A. Lee and D. Niehaus

Defines and maintains a TK XY Plot.


**************************************************************************/

#include "XYPlot.h"
extern "C" {
#include "ptkPlot.h"
}
#include "ieee.h"

/////////////////////////////////////////////////////////////////////////
//			Methods for XYPlot class
//
// constructor
XYPlot::XYPlot () {
	starID = "ptkXYPlot";
	starID += unique++;
	// Make sure the pointers to dynamically allocated structures
	// start with zero.
	dataset.xvec = 0;
	dataset.yvec = 0;
	dataset.id = 0;

	// If it has not already happened, source the Tcl file corresponding
	// to this class.  This is done on the second call to the constructor
	// because the first call occurs when the static prototype is created
	// for the KnownList, and this occurs before the Tcl interpreter has
	// started.
	if (unique == 2) {
	  char sourceCmd[100];
	  strncpy(sourceCmd,"source $env(PTOLEMY)/src/ptklib/ptkPlot.tcl",100);
	  if (Tcl_GlobalEval(ptkInterp, sourceCmd) != TCL_OK) {
	    Error::abortRun("XYPlot constructor: failed to source ptkPlot.tcl");
	  }
	}
}

// destructor: currently empty, but we anticipate
// wanting it as the XYPlot widget is improved.
XYPlot::~XYPlot() {
	// avoid core dump if interpreter did not set up right
	if (!ptkInterp) return;
	freeDataset(&dataset);
	freePlot(ptkInterp,&plot);
}

int XYPlot::windowExists () {
	return (Tk_NameToWindow(ptkInterp,(char*)winName,ptkW) != 0);
}

int XYPlot::setup (Block* star,         // The star I am in
		   char*  desc,		// Label for the bar chart
		   int    persistence,	// The number of data points to retain
		   int    refresh, 	// The number of data points between refreshes
		   char*  geometry,	// Geometry for the window
		   char*  xTitle,       // Title for X-axis
		   double xMin,         // minimum X range value
		   double xMax,         // Max X range value
		   char*  yTitle,       // Title for Y-axis
		   double yMin,	        // minimum Y range value
		   double yMax)         // max Y range value 
{

  const char * XYPlot_window_base = ".xyPlot";

  // First check to see whether a global Tcl variable named
  // "ptkControlPanel" is defined.  If so, then use it as the
  // stem of the window name.  This means that the window will
  // be a child window of the control panel, and hence will be
  // destroyed when the control panel window is destroyed.
  if(Tcl_GlobalEval(ptkInterp,
		    "global ptkControlPanel;set ptkControlPanel") != TCL_OK) {
    winName = XYPlot_window_base;
    winName += starID;
  } else {
    winName = ptkInterp->result;
    winName += XYPlot_window_base;
    winName += starID;
  }

  if(createPlot(ptkInterp, &plot, &ptkW, (char *)winName, desc,
	     geometry, xTitle, yTitle, xMin, xMax, yMin, yMax) == 0) {
    Error::abortRun("XYPlot class: failed to create plot window");
    return FALSE;
  }

  if(createDataset(ptkInterp, &dataset, &ptkW,
		   persistence,refresh) == 0) {
    Error::abortRun("XYPlot class: failed to create dataset");
    return FALSE;
  }
  assocData(ptkInterp, &dataset, &plot);
  return TRUE;
}

int XYPlot::addPoint(double x, double y) {
  return (plotPoint(ptkInterp, &dataset, x, y));
}

// Initialize the static counter for unique names,
// and the hash table for keeping track of the data sets.
// The initialization occurs at load time.
int XYPlot::unique = 0;
