/* Version Identification:
 * $Id$
 */
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
/*************************************************************************
 * XGraph procedure library
 * Organization:	University Of California, Berkeley
 *					Electronics Research Laboratory
 *					Berkeley, CA 94720
 * Modifications:
 *	David Harrison	Created									1986,1987
 *	Andrea Casotto	Modified for use as program subroutines	1988
 *	Rodney Lai		Ported to X11							1989
 **************************************************************************/

#ifndef XG_H
#define XG_H

#define XGR_PKG_NAME			"xg"

#define XG_OK					1
#define XG_EXTRANEOUS_EVENT		2
#define XG_ERROR				-1
#define XG_NO_VALUE				2

#define XG_MAXSETS		8

#define		XGBSplineFlag		(1L<<0)
#define		XGHermiteFlag		(1L<<1)
#define		XGGridFlag			(1L<<2)
#define		XGBigMarkFlag		(1L<<3)
#define		XGBoundingBoxFlag	(1L<<4)
#define		XGBarFlag			(1L<<5)
#define		XGLinesFlag			(1L<<6)
#define		XGPixelMarkFlag		(1L<<7)
#define		XGLogXFlag			(1L<<8)
#define		XGLogYFlag			(1L<<9)
#define		XGHHMMSSFlag		(1L<<10)

/* XG_NO_OPT is a weird number. If you have very little chance
 * to actually use this number in your application 
 */
#define XG_NO_OPT		1.356789012423E-12


typedef int (*XG_PFI)();	/* Pointer to function returning integer */
				/* Usable for xg_boundMeasure() */

typedef char	*XGraph;

#include "ansi.h"

extern XGraph	xgNewWindow
	ARGS((Display *display, char *progName, char *title, char *Xunits,
		char *Yunits, int w, int h, int x, int y));
extern int16	xgDestroyWindow
	ARGS((XGraph xgraph));
extern int16	xgSendPoint
	ARGS((XGraph xgraph, double x, double y, char *setName));
extern int16	xgSendArray
	ARGS((XGraph xgraph, int16 n, 
		double *Xarray, double *Yarray, char *setName));
extern int16	xgAnimateGraph
	ARGS((XGraph xgraph, int16 start, int16 length, 
		double *Xarray, double *Yarray, char *setName));
extern int16	xgSuspendRedraw
	ARGS(());
extern void		xgRedrawGraph
	ARGS((XGraph xgraph));
extern int16	xgSetRange
	ARGS((XGraph xgraph, double minX, double minY, double maxX, double maxY));
extern int16	xgGetRange
	ARGS((XGraph xgraph, 
		double *minX, double *minY, double *maxX, double *maxY));
extern int16	xgBoundMeasure
	ARGS((XGraph xgraph, XG_PFI f));
extern int16	xgClearGraph
	ARGS((XGraph xgraph, char *setname));
extern int16	xgSaveGraph
	ARGS((XGraph xgraph, char *filename));
extern int16	xgHandleEvent
	ARGS((XEvent *theEventP));
extern int16	xgRPCHandleEvent
	ARGS((XEvent *theEventP));
extern int16	xgProcessAllEvents
	ARGS((Display *display));
extern void		xgRPCProcessAllEvents
	ARGS((int number, int *read, int *write, int *except));
extern void		xgSetFlags
	ARGS((XGraph xgraph, unsigned long flags));
extern void		xgClearFlags
	ARGS((XGraph xgraph, unsigned long flags));

#endif /* XG_H */
