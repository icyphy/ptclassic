#ifndef _XYPlot_h
#define _XYPlot_h 1
/**************************************************************************
Version identification:
$Id$

Author: Edward A. Lee and D. Niehaus

Defines and maintains a TK XY plot.
It uses the C functions defined in $PTOLEMY/src/ptklib/ptkPlot.c,
but puts an object-oriented wrapper around them.

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

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

/* Do the right thing for sol2 boolean defs.  sol2compat.h must be included
 * first so sys/types.h is included correctly.  See octtools/include/port.h
 */
#include "sol2compat.h"

#include "Block.h"
#include "InfString.h"
extern "C" {
#include "ptk.h"
#include "ptkPlot_defs.h"
}

// XYPlot will be instantiated by each star that uses a XY Plot

class XYPlot {
public:
	// constructor and destructor
	XYPlot ();
	~XYPlot ();

	// Create the XY Plot
	// Returns FALSE if setup fails, TRUE otherwise.
	int setup (Block* star,		// The star I am in
		   char*  desc,		// Label for the bar chart
		   int    persistence,	// The number of data points to retain
		   int    refresh, 	// The number of data points between refreshes
		   char*  geometry,	// Geometry for the window
		   char*  xTitle,       // Title for X-axis
		   double xMin,         // minimum X range value
		   double xMax,         // Max X range value
		   char*  yTitle,       // Title for Y-axis
		   double yMin,	        // minimum Y range value
		   double yMax,         // max Y range value 
		   int    numSets = 1,  // number of datasets
		   int    style = 0);   // plot style

	// Write inputs to the XY Plot
	int addPoint (double x, double y, int set = 1);

	// Break the line in a continuous line-drawing plot
	int breakPlot (int set = 1);
		   
	// Window name to use
	InfString winName;

	// Return true if a window with the name winName already exists.
	int windowExists();

protected:
	Block* myStar;

	// A static variable used to create names that are
	// guaranteed to be unique.  After each use, it should
	// be incremented.
	static unique;

	// A unique string for each instance of this object identifies
	// the star within which it sits for the benefit of Tcl routines.
	InfString starID;

	// Hold all the persistent data associated with the graph
	ptkPlotWin plot;
};
#endif
