/*
 * Header file for C++ Tk canvas-based plotting interface
 * Author: Wei-Jen Huang, E. A. Lee, and D. Niehaus
 * Version: $Id$
 */

#ifndef _ptkPlot_h
#define _ptkPlot_h 1

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

/*
 * include the basic C code style definitions from the
 * file included by the C code. In this file wqe define the templates
 * required by the C++ code providing the wrappings tot he C plot routines.
 */
#include "ptkPlot_defs.h"

void ptkInitPlot(ptkPlotWin *plotPtr);

int ptkCreatePlot(Tcl_Interp *interp,
		  ptkPlotWin* plotPtr,
		  Tk_Window *win,
		  char *name,
		  char *identifier,
		  char *geometry,
		  char *xTitle,
		  char  *yTitle,
		  double xMin, 
		  double xMax, 
		  double yMin, 
		  double yMax,
		  int numsets,
		  int batch,
		  int style,
		  int persistence);

int ptkPlotPoint(Tcl_Interp *interp,
		 ptkPlotWin *plotPtr,
		 int setnum,
		 double xval,
		 double yval);

int ptkPlotBreak(Tcl_Interp *interp,
		 ptkPlotWin *plotPtr,
		 int setnum);

void ptkFreePlot(Tcl_Interp *interp, ptkPlotWin *plotPtr);

const char *ptkPlotErrorMsg();

#endif
