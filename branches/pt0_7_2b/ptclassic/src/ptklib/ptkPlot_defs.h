/*
 * Header file for Tk canvas-based plotting interface
 * Author: Wei-Jen Huang, E. A. Lee, and D. Niehaus
 * Version: $Id$
 */
#ifndef _ptkPlot_defs_h
#define _ptkPlot_defs_h 1

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
 * These are the definitions needed in the C code. This header
 * file is included by the C code for plotting, and included
 * by the C++ header file for the C++ wrapper code. The need
 * for two files arises bcause the C code cannot handle the C++
 * stuff, but the C++ stuff needs these definitions.
 */

#include <string.h>
#include "tk.h"

#define PLOT_PREFIX "plot_"
#define DATASET_PREFIX "dataset_"
#define STD_FONT "-Adobe-Helvetica-Bold-R-Normal--*-140-*"
#define TITLE_FONT "-Adobe-Helvetica-Bold-R-Normal--*-180-*"

/* STRDUP from Xgraph (David Harrison) */
#define STRDUP(xx)              (strcpy((char *)malloc((strlen(xx)+1)), (xx)))
#define MAX(xx,yy)			((xx) > (yy) ? (xx) : (yy))
#define MIN(xx,yy)			((xx) < (yy) ? (xx) : (yy))
#define LOG10(xx)               (xx == 0.0 ? 0.0 : log10(xx) + 1e-15)
#define INCR_ALLOC_PTS 128
#define LARGE_ENOUGH 10000000

/*FIXME: x and y ranges virtually constant, precompute for efficiency */
#define MAPX(xx)		(plotPtr->llx+ \
				 (plotPtr->urx-plotPtr->llx)* \
				 ((xx)-plotPtr->xMin)/(plotPtr->xMax- \
						       plotPtr->xMin))
#define MAPY(xx)		(plotPtr->lly- \
				 (plotPtr->lly-plotPtr->ury)* \
				 ((xx)-plotPtr->yMin)/(plotPtr->yMax- \
						       plotPtr->yMin))
#define MKPLOTSTR(xx)           (char *)(strcat(strcpy((char *) \
                                       malloc((strlen(PLOT_PREFIX)+ \
                                       strlen(xx)+1)), \
                                       PLOT_PREFIX),(xx)))

#define MKDATASETSTR(xx)        (char *)(strcat(strcpy((char *) \
                                       malloc((strlen(DATASET_PREFIX)+ \
                                       strlen(xx)+1)), \
                                       DATASET_PREFIX),(xx)))

struct plotWin;

typedef struct plotDataset {
  Tcl_Interp *interp;		/* Tcl interpreter */
  Tk_Window *win;		/* Tcl main window */
  int persistence;		/* the number of points to show */
  int refreshBatch;             /* points are buffered and drawn in batches */
  int batchCounter;             /* determines when to do the drawing */
  int idx;			/* the current index for the next point to plot */
  double *xvec;	                /* vector of x position for points */
  double *yvec;			/* vector of y position for points */
  int *id;			/* vector of Tk IDs for the points */
  struct plotDataset *prevSetPtr;
  struct plotDataset *nextSetPtr;
  struct plotWin *curPlot;	/* the plot window associated with the dataset */
} plotDataset;

typedef struct plotWin {
  Tcl_Interp *interp;		/* Tcl interpreter */
  Tk_Window *win;		/* Tcl main window */
  char *name;            	/* Tk name to use for the window */
  char *identifier;		/* title to put on the window */
  int urx, ury;
  int llx, lly;
  char *xTitle, *yTitle;	/* title for x and y axes */
  double xMin, xMax;		/* range for x */
  double yMin, yMax;		/* range for y */
  double xMinPoint, xMaxPoint;  /* largest and smallest values seen */
  double yMinPoint, yMaxPoint;  /* largest and smallest values seen */
  struct plotWin *parentPlotPtr;
  struct plotWin *childPlotPtr;
  plotDataset *firstSetPtr;	/* pointer to dataset linked list */
} plotWin;

#endif
