#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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

/* test the xg.a routines */

#include "copyright.h"
#include "port.h"
#include "st.h"          /* TO force linking of st routines */
#include <X11/Xlib.h>
#include "xg.h"

static void dummyfuncToloadST()
{
    st_free_table( (st_table*)0);
}


#define  SIZE 400

/**************************************************************************
 * FUNC
 */
int func(mx,my)
	double mx,my;
{
	(void) printf("Measured value: (%f,%f)\n", mx,my);
	return(XG_OK);
} /* func... */

/**************************************************************************
 * MAIN
 */
int
main(argc,argv)
	int		argc;
	char	**argv;
{
	double	x,y;
	char	*displayName;
	Display	*display;
	XGraph	xgraph1, xgraph2 , xg3;
	double	X[SIZE], Y[SIZE];
	int		i,b;
	double	t = 0.0;
	char	*getenv();

	displayName = getenv("DISPLAY");
	if ((display = XOpenDisplay(displayName)) == (Display *) 0 ) {
		(void) printf ("Cannot open display.\n");
		exit(0);
	} /* if... */
	xgraph1 = xgNewWindow(display,argv[0], "simple test 1", "XX1", "YY1", 
		400, 300, 400, 100);
	xgraph2 = xgNewWindow(display,argv[0], "simple test 2", "XX2", "YY2", 
		400, 300, 400, 500);
	xg3 = xgNewWindow(display,argv[0], "simple animation", "XX2", "YY2", 
		400, 400, 0, 0);
	xgSetFlags(xgraph1,XGLogXFlag);
	xgSetFlags(xg3,XGPixelMarkFlag);
	(void) xgBoundMeasure(xgraph1, func);

	for (x = 10.0; x > 0.0; x -= 2.0) {
		y = x*x;
		(void) xgSendPoint(xgraph1, x,  y, "set0");
		(void) xgSendPoint(xgraph1, x, -y, "set1");
		(void) xgSendPoint(xgraph1, x,  y /2.0, "set2testing");
		(void) xgSendPoint(xgraph2, -x, 10.0 + 20.0 * x - x*x , "OOO");

		(void) xgProcessAllEvents(display);
	} /* for... */

	(void) xgClearGraph(xgraph1, "set1");

	(void) xgClearGraph(xgraph2, (char *) 0);
	for (i = 0 ; i < SIZE ; i++) {
		X[i] = i * 0.01 * cos(i*0.03);
		Y[i] = i * 0.01 * sin(i*0.03);
		(void) xgProcessAllEvents(display);
	} /* for... */
	(void) xgSendArray(xgraph1, SIZE , X , Y, "Array2rep");
	(void) xgSendArray(xgraph2, SIZE , X , Y, "Array2");
	(void) xgSetRange(xgraph2, -10.0 , 10.0 , -10.0 , 10.0);

	/* infinite loop, to play indefinetly with the graphs */
	(void) xgSetRange(xg3, 0.0 , 1000.0, -1.0 , 1.0);

	for (b = 1 ; b < 20; b++) {
		t += 0.1;
		for (i = 0 ; i < SIZE; i++) {
			X[i] = i*2;
			Y[i] = sin((double) i * 0.01 + t);
			(void) xgProcessAllEvents(display);
		} /* for... */
		(void) xgAnimateGraph(xg3, 0, SIZE, X , Y, "Sin");
	} /* for... */

	alarm(30);

	while (1) (void) xgProcessAllEvents(display);
	return 0;
} /* main... */

