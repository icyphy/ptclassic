#ifndef PTKBARGRAPH_H
#define PTKBARGRAPH_H 1

/* 
Copyright (c) 1990-1997 The Regents of the University of California.
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

/* ptkBarGraph.h  eal
Version identification:
@(#)ptkBarGraph.h	1.6	03/08/96
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "tcl.h"
#include "tk.h"

extern int ptkMakeBarGraph( Tcl_Interp *interp,
			    Tk_Window *win,
			    char *name,
			    char *desc,
			    double **data,
			    int numTraces,
			    int numBars,
			    double top,
			    double bottom,
			    int** id,
			    char* geo,
			    double deswidth,
			    double desheight);

extern int ptkSetBarGraph( Tcl_Interp *interp,
			   Tk_Window *win,
			   char *name,
			   double **data,
			   int numTraces,
			   int numBars,
			   double top,
			   double bottom,
			   int** id);

extern void ptkFigureBarEdges( int* x0,
				int* x1,
				int *y0,
				int *y1,
				int i,
				int j,
				int numTraces,
				int numBars,
				int width,
				int height,
				double top,
				double bottom,
				double data);

#ifdef __cplusplus
}
#endif

#endif /* PTKBARGRAPH_H */
