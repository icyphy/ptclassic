#ifndef _BarGraph_h
#define _BarGraph_h 1
/**************************************************************************
Version identification:
$Id$

Author: Edward A. Lee

Defines and maintains a Tcl bar graph.
It uses the C functions defined in $PTOLEMY/src/ptklib/ptkBarGraph.c,
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

#include "Block.h"
#include "InfString.h"
extern "C" {
#include "ptk.h"
}

// BarGraph will be instantiated by each star that uses a bar graph

class BarGraph {
public:
	// constructor and destructor
	BarGraph ();
	~BarGraph ();

	// Create the bar graph.
	// Returns FALSE if setup fails, TRUE otherwise.
	int setup (Block* star,		// The star I am in
		   char* desc,		// Label for the bar chart
		   int numInputs,	// The number of data sets to plot
		   int numBars,		// The number of bars per data set
		   double top,		// Top of the range
		   double bottom,	// Bottom of the range
		   char* geometry,	// Geometry for the window
		   double width,	// Desired width of the window (cm)
		   double height);	// Desired height of the window (cm)

	// Write inputs to the bar graph
	int update (int input,		// Identifies the data set
		    int bar,		// Identifies the bar within the set
		    double newVal);	// New value for the bar
		   
	Block* myStar;

	// Window name to use
	InfString winName;

	// Redraw the bars (invoked after a configure event in the window)
	void redrawBars();

	// Rescale the graph
	void rescaleGraph(float scaleFactor);

	// Top and bottom of the plot range
	double top;
	double bottom;

	// Return true if a window with the name winName already exists.
	int windowExists();

protected:
	// A static variable used to create names that are
	// guaranteed to be unique.  After each use, it should
	// be incremented.
	static unique;

	// A unique string for each instance of this object identifies
	// the star within which it sits for the benefit of Tcl routines.
	InfString starID;

	// Pointer to dynamically allocated array of pointers to data arrays
	double** data;

	// Pointer to a dynamically allocated array of object IDs
	int** ids;

	// The size paramaters for the plot
	int noBars;		// The number of bars
	int noInputs;		// The number of inputs
	int actualWidth;	// Actual current width of the plot window
	int actualHeight;	// Actual current height of the plot window
};
#endif
