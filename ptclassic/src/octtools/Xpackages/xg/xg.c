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
/***********************************************************************
 * XGraph procedure library
 * Filename: xg.c
 * Program: XG - plot library.
 * Environment: Oct/Vem
 * Date: March, 1989
 * Professor: Richard Newton
 * Oct Tools Manager: Rick Spickelmier
 * XGraph: David Harrison
 * Organization:  University Of California, Berkeley
 *                Electronic Research Laboratory
 *                Berkeley, CA 94720
 * Routines:
 *	void FreeAllData() - free all data points associated with graph.
 *	static int16 xgiAllocManager() - allocate data points.
 *	static int16 xgiFindSet() - find data set within graph.
 *	static void xgiDrawTitle() - draw graph title.
 *	static char *xgiCheckPtr() - check if pointer was allocated.
 *	int16 xgDestroyWindow() - destroy graph window and deallocate graph struct.
 *	void xgHandleZoom() - handle interactive zoom. 
 *	void xgiPrintCoord() - print current pointer coordinates.
 *	void xgMeasure() - handle interactive measure.
 *	int16 xgBoundMeasure() - set the measure function.
 *	static PIXEL xgiGetColor() - allocate color.
 *	static int GlobalInitSets() - initialize global variables.
 *	static int InitSets() - intialize data set.
 *	static void ReversePixel() - reverse color pixel.
 *	static void ReadDefaults() - read x default parameters.
 *	XGraph xgNewWindow() - create new graph window.
 *	int16 xgSuspendRedraw() - suspend redraw.
 *	int16 xgSetRange() - set graph range.
 *	int16 xgGetRange() - get graph range.
 *	static void xgiIntegerTohhmmss() - convert x-coordinate to 'hh:mm:ss' format
 *	int16 xgClearGraph() - clear graph.
 *	static void xgiShowPoint() - display point on graph.
 *	int16 xgAnimateGraph() - animate graph.
 *	void xgiAddPadding() - add padding to plot window.
 *	static int xgiTransformCompute() - calculate axis scales.
 *	static double RoundUp() - round value up.
 *	static void xgiDrawGridAndAxis() - draw grid and axis.
 *	static void xgiWriteValue() - write value.
 *	static HERMITE ComputeCubicHermite() - compute coefficients for
 *		cubic hermite interpolation.
 *	static void DrawHermiteSegment() - generate points for hermite segment.
 *	static XPOINT ComputeCubicBSpline() - compute cubic b-spline point.
 *	static void DrawBSplineSegment() - generate points for b-spline segment.
 *	static int16 ClipCheck() - do clip checking.
 *	static int16 Clipper() - the clipper.
 *	static void xgiDrawData() - draw data.
 *	static void xgiDrawPoint() - draw point.
 *	void xgSetFlags() - set flags.
 *	void xgClearFlags() - clear flags.
 *	static int16 xgiRetrieveXG - retrieve graph structure for given event.
 *	int16 xgRPCHandleEvent() - rpc version of x event handler.
 *	int16 xgHandleEvent() - x event handler.
 *	int16 xgProcessAllEvents() - process all x events.
 *	void xgRPCProcessAllEvents() - process all x events for rpc program.
 *	int16 xgSendPoint() - send point to graph.
 *	int16 xgSendArray() - send array of points to graph.
 *	void xgRedrawGraph() - redraw graph.
 * Modifications:
 *  David Harrison  Created                                 1986,1987
 *  Andrea Casotto  Modified for use as program subroutines 1988
 *  Rodney Lai      Ported to X11                           1989
 * Note: File is formatted with 4 space tab.
 **************************************************************************/

#include "copyright.h"
#include "port.h"
#define MAIN_XG
#include "xglocal.h"
#ifdef PTSVR4
#include "utility.h"
#endif

static void xgiWriteValue
	ARGS((char *str, double	val, int exp));


/**************************************************************************
 * FREEALLDATA
 * Free all data points assocaited with the specified graph.
 *
 * Parameters:
 *	xg (XG *) - the graph. (Input/Output)
 */
static void FreeAllData(xg)
	XG		*xg;
{
	int16	set;

	for (set = 0; set < xg->numPlots ; set++) {
		FREE(xg->dataSet[set].setName);
		FREE(xg->dataSet[set].xvec);
		FREE(xg->dataSet[set].yvec);
		xg->dataSet[set].allocSize = 0;
		xg->dataSet[set].numPoints = 0;
	} /* for... */
	if (xg->allocXpoints > 0) {
		FREE(xg->Xpoints);
		xg->allocXpoints = 0;
	} /* if... */
	xg->numPlots = 0;
} /* FreeAllData... */

/*************************************************************************
 * XGIALLOCMANAGER
 * Allocate 'n' number of points for graph 'set' in window 'xg'.
 *
 * Paremeters:
 *	xg (XG *) -  the xgraph structure.
 *	set (int16) - the graph within the window.
 *	n (int16) - the number of points to allocate.
 *
 * Return Value:
 *	XG_OK - success.
 *	XG_ERROR - something went wrong, not enough memory to
 *		allocate all points.
 */
static int16 xgiAllocManager(xg,set,n)
	XG		*xg;
	int16	set;
	int16	n;
{
	int16	newArraySize;

	if (set > XG_MAXSETS ) {
		(void) fprintf(stderr,"Illegal set %d. Max # of sets is %d\n",
			set, XG_MAXSETS);
		return(XG_ERROR);
	} /* if... */
  
	if (xg->dataSet[set].allocSize == 0) {
		newArraySize = MAX(n,128);	/* Begin with at least 128 points */
		xg->dataSet[set].allocSize = newArraySize;
		xg->dataSet[set].xvec = XG_ALLOC(double,xg->dataSet[set].allocSize);
		xg->dataSet[set].yvec = XG_ALLOC(double,xg->dataSet[set].allocSize);
	} else if (xg->dataSet[set].allocSize <= n) {
		/* Time to make the arrays bigger (or initialize them) */
		newArraySize = MAX( n , xg->dataSet[set].allocSize * 2);
		xg->dataSet[set].allocSize = newArraySize;
		XG_REALLOC(xg->dataSet[set].xvec,double,xg->dataSet[set].allocSize);
		XG_REALLOC(xg->dataSet[set].yvec,double,xg->dataSet[set].allocSize);
	} else {
		newArraySize = xg->dataSet[set].allocSize;
	} /* if... */
	if (xg->dataSet[set].xvec == NIL(double) || 
		xg->dataSet[set].yvec == NIL(double)) {
		(void)fprintf(stderr,"XG: out of memory: graph %s is lost at line %d\n",
			xg->titleText , __LINE__);
		FreeAllData(xg);
		return(XG_ERROR);
	} /* if... */

	if (xg->allocXpoints == 0) {
		xg->allocXpoints = newArraySize;
		xg->Xpoints = XG_ALLOC(XPOINT,newArraySize);
	} else if (newArraySize > xg->allocXpoints) {
		xg->allocXpoints = newArraySize;
		XG_REALLOC(xg->Xpoints,XPOINT,xg->allocXpoints);
	} /* if... */
	if (xg->Xpoints == NIL(XPOINT)) {
		(void) fprintf(stderr,"XG: out of memory: graph %s is lost (line %d)\n",
			xg->titleText , __LINE__);
		FreeAllData(xg);
		return(XG_ERROR);
	} /* if... */
	return(XG_OK);
} /* xgiAllocManager... */

/************************************************************************
 * XGIFINDSET
 * Find the specified data set by name.  If not found then create new
 * data set.
 *
 * Parameters:
 *	xg (XG *) - the graph to search for data set in. (Input)
 *	setName (char *) - the name of the data set to search for. (Input)
 *	set (int16 *) - the index of the set. (Output)
 *
 * Return Value:
 *	XGI_FOUND, if set was found.
 *	XGI_NEWSET, if a new set had to be create.
 *	XGI_ERROR, if set was not found and a new set could not be created.
 */
static int16 xgiFindSet(xg, setName , set)
	XG		*xg;
	char	*setName;
	int16	*set;		/* RETURN */
{
	int16	i = 0;

	while (i++ < xg->numPlots) {
		if (strcmp(setName,xg->dataSet[i - 1].setName)==0) {
			*set = i - 1;
			return(XGI_FOUND);
		} /* if... */
	} /* while... */
	/*** First look if there is any deleted set ***/
	i = 0; 
	while (i++ < xg->numPlots) {
		if (strcmp("<deleted>",xg->dataSet[i - 1].setName)==0) {
			*set = i - 1;
			xg->dataSet[*set].setName   = util_strsav(setName);
			xg->dataSet[*set].numPoints = 0;
			xg->dataSet[*set].allocSize = 0;
			return(XGI_NEWSET);
		} /* if... */
	} /* while... */
	if (xg->numPlots >= XG_MAXSETS ) {
		(void) fprintf(stderr,
			"Cannot start a new set. Mas # of sets is %d\n", XG_MAXSETS);
		return(XGI_ERROR);
	} /* if... */
	/* Initialize the new set */
	*set = xg->numPlots++;
	xg->dataSet[*set].setName   = util_strsav(setName);
	xg->dataSet[*set].numPoints = 0;
	xg->dataSet[*set].allocSize = 0;
	return(XGI_NEWSET);
} /* xgiFindSet... */

/*************************************************************************
 * XGIDRAWTITLE
 * This routine draws the title of the graph centered in
 * the window.  It is spaced down from the top by an amount
 * specified by the constant PADDING.  The font must be
 * fixed width.
 *
 * Parameters:
 *	xg (XG *) - the graph to draw the title in. (Input)
 */
static void xgiDrawTitle(xg)
	XG			*xg;
{
	LocalWin	*wi = xg->win_info;
	int16		titleWidth;

	titleWidth = strlen(xg->titleText) * XFONTWIDTH(titleFont);
#ifdef X11
	XSetFont(xg->display,normGC,titleFont->fid);
	XDrawImageString (xg->display,xg->win,normGC,
		(wi->width / 2) - (titleWidth/2), PADDING + XFONTHEIGHT(titleFont),
		xg->titleText,strlen(xg->titleText));
#else
	XText(xg->win, (wi->width / 2) - (titleWidth/2), PADDING, xg->titleText, 
		strlen(xg->titleText), titleFont->id, normPixel, bgPixel);
#endif
} /* xgiDrawTitle... */

/**************************************************************************
 * XGICHECKPTR
 * Check if the given pointer is not 0.  If it is then print an
 * error message.
 *
 * Parameters:
 *	p (char *) - the pointer to check. (Input)
 *	n (unsigned) - the number bytes allocated to pointer. (Input)
 *
 * Return Value:
 *	The pointer. (char *)
 */
char *xgiCheckPtr(p,n)
	char		*p;
	unsigned	n;
{
	if (p == NIL(char)) {
		(void) fprintf(stderr,"Failed allocation of %d bytes\n", n) ;
	} /* if... */
	return((char *) p);
} /* xgiCheckPtr... */

/**************************************************************************
 * XGDESTROYWINDOW
 * Destry graph window.  Deallocate all structures and close window.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *
 * Return Value:
 *	XG_OK, successful destructon of graph.
 */
int16 xgDestroyWindow(xgraph)
	XGraph	xgraph;
{
	XG		*xg = (XG *) xgraph;

	(void) xgClearGraph((XGraph) xg,NIL(char));
	FREE(xg->win_info);
	FREE(xg->dataSet);
	FREE(xg->titleText);
	FREE(xg->YUnits);
	FREE(xg->XUnits);
#ifdef X11
	XDeleteContext(xg->display,xg->win,win_table);
#else
	XDeleteAssoc(win_table, (XId) xg->win);
#endif
	XDESTROYWINDOW(xg->display,xg->win);
	XFLUSH(xg->display);
	FREE(xg);
	return(XG_OK);
} /* xgDestroyWindow... */

#ifndef X11
static Vertex boxPoints[5];
static Vertex vl[2], hl[2];
#endif

/************************************************************************
 * DRAWBOX
 */
#ifdef X11
#define DRAWBOX \
	XSetFunction (xg->display,xgGC,GXxor);\
	XSetForeground (xg->display,xgGC,echoPixel);\
	XSetLineAttributes(xg->display,xgGC,1,LineSolid,CapNotLast,JoinMiter);\
	XDrawRectangle(xg->display, xg->win, xgGC, MIN(curX,startX), MIN(curY,startY),\
		ABS(startX-curX), ABS(startY-curY))
#else
#define DRAWBOX \
	boxPoints[0].x = startX; \
	boxPoints[0].y = startY; \
	boxPoints[1].x = curX;   \
	boxPoints[1].y = startY; \
	boxPoints[2].x = curX;   \
	boxPoints[2].y = curY;   \
	boxPoints[3].x = startX; \
	boxPoints[3].y = curY;   \
	boxPoints[4].x = startX; \
	boxPoints[4].y = startY; \
	XDraw(xg->win, boxPoints, 5, 1, 1, echoPixel, GXxor, AllPlanes)
#endif
/***********************************************************************
 * DRAWCROSS
 */
#ifdef X11
#define DRAWCROSS \
	XSetFunction (xg->display,xgGC,GXxor);\
	XSetForeground (xg->display,xgGC,echoPixel);\
	XDrawLine (xg->display,xg->win,xgGC,curX,0,curX,wi->height);\
	XDrawLine (xg->display,xg->win,xgGC,0,curY,wi->width,curY)
#else
#define DRAWCROSS \
	vl[0].x = curX; \
	vl[0].y = 0;    \
	vl[1].x = curX; \
	vl[1].y = wi->height;  \
	hl[0].x = 0;    \
	hl[0].y = curY; \
	hl[1].x = wi->width;  \
	hl[1].y = curY; \
	XDraw(xg->win, vl, 2, 1, 1, echoPixel, GXxor, AllPlanes) ; \
	XDraw(xg->win, hl, 2, 1, 1, echoPixel, GXxor, AllPlanes)
#endif

/************************************************************************
 * TRANX
 */
#define TRANX(xval) \
	(((double) ((xval) - wi->XOrgX)) * wi->XUnitsPerPixel + wi->UsrOrgX)

/*************************************************************************
 * TRANY
 */
#define TRANY(yval) \
	(wi->UsrOppY - (((double) ((yval) - wi->XOrgY)) * wi->YUnitsPerPixel))

/**************************************************************************
 * XGHANDLEZOOM
 * Handle interactive zoom of graph.
 *
 * Parameters:
 *	evt (XButtonEvent *) - the initial event to invoke zoom. (Input)
 *	xg (XG *) - the graph structure. (Input/Output)
 *	cur (Cursor) - the cursor to use for zoom. (Input)
 */
void xgHandleZoom(evt, xg, cur)
	XButtonEvent 	*evt;
	XG				*xg;
	Cursor 			cur;
{
	XEvent			theEvent;
	int				startX, startY, curX, curY, newX, newY;
	int16			stopFlag;
	double			loX, loY, hiX, hiY;
	LocalWin		*wi = xg->win_info;
#ifdef X11
	int				rootX, rootY;
	unsigned int	mask;
	Window			childWin, rootWin;
#else
	Window			dummy;
	int				index;
#endif

#ifdef X11
	XGrabPointer(xg->display,xg->win,True,ButtonReleaseMask|
		PointerMotionMask|PointerMotionHintMask,
		GrabModeAsync,GrabModeAsync,None,cur,CurrentTime);
#else
	XGrabMouse(xg->win, cur, ButtonReleased|MouseMoved);
#endif

	echoPixel = zoomPixel ^ bgPixel;
#ifndef X11
	for (index = 0; index < 5; index++) {
		boxPoints[index].flags = 0;
	} /* for... */
#endif

	startX = evt->x;
	startY = evt->y;
#ifdef X11
	XQueryPointer(xg->display,xg->win,&rootWin,&childWin,&rootX,&rootY,
		&curX,&curY,&mask);
#else
	XUpdateMouse(xg->win, &curX, &curY, &dummy);
#endif
	/* Draw first box */
	DRAWBOX;
	stopFlag = FALSE;
	while (!stopFlag) {
		XNEXTEVENT (xg->display,&theEvent);
		switch (theEvent.type) {
			case MotionNotify:
#ifdef X11
				XQueryPointer(xg->display,xg->win,&rootWin,&childWin,
					&rootX,&rootY,&newX,&newY,&mask);
#else
				XUpdateMouse(xg->win, &newX, &newY, &dummy);
#endif
				DRAWBOX;			/* Undraw the old one */
				curX = newX;
				curY = newY;
				DRAWBOX;			/* Draw the new one */
				break;
			case ButtonRelease:
				DRAWBOX;
				XUNGRABPOINTER(xg->display);
				stopFlag = TRUE;
				if ((startX - curX != 0) && (startY - curY != 0)) {
					/* Figure out relative bounding box */
					loX = TRANX(startX);   loY = TRANY(startY);
					hiX = TRANX(curX);     hiY = TRANY(curY);
					if (loX > hiX) {
						double temp; temp = hiX; hiX = loX; loX = temp;
					} /* if... */
					if (loY > hiY) {
						double temp; temp = hiY; hiY = loY; loY = temp;
					} /* if... */
					/* physical aspect ratio */
					xg->win_info->loX = loX;
					xg->win_info->loY = loY;
					xg->win_info->hiX = hiX;
					xg->win_info->hiY = hiY;
				} /* if... */
				break;
		} /* switch... */
	} /* while... */
#ifdef X11
	XCopyGC (xg->display,normGC,0xffff,xgGC);
#endif
} /* xgHandleZoom... */

/****************************************************************************
 * XGIPRINTCOORD
 * Print the current pointer coordinates. Use for the measure function.
 *
 * Parameters:
 *	xg (XG *) - the graph structure. (Input)
 *	x,y (double) - the current coordiantes of the pointer. (Input)
 */
static void xgiPrintCoord(xg,x,y)
	XG		*xg;
	double	x,y;
{
	LocalWin *wi = xg->win_info;
	int l,i;
	char str[80];

	if (x == XG_NO_OPT || y == XG_NO_OPT) {
		for (i = 0; i < 30 ; i++) str[i] = ' ';
		str[i] = '\0';
	} else {
		(void) sprintf(str, "%8.3g,%8.3g", x, y);
	} /* if... */
	l = strlen(str);
#ifdef X11
	XSetFont(xg->display,normGC,titleFont->fid);
	XDrawImageString(xg->display,xg->win,normGC,
		(wi->width/2)-(l*XFONTWIDTH(titleFont)/2),
		PADDING + XFONTHEIGHT(titleFont),str,l);
#else
	XText(xg->win, (wi->width/2)-(l*titleFont->width/2), PADDING, str, l,
		titleFont->id, normPixel, bgPixel);
#endif
} /* xgiPrintCoord... */

/**************************************************************************
 * XGMEASURE
 * Handle the interactive measure function.
 *
 * Parameters:
 *	evt (XButtonEvent *) - the initial event to invoke measure. (Input)
 *	xg (XG *) - the graph structure. (Input/Output)
 *	cur (Cursor) - the cursor to use for zoom. (Input)
 */
void xgMeasure(evt, xg, cur)
	XButtonEvent 	*evt;
	XG				*xg;
	Cursor			cur;
{
	XEvent			theEvent;
	int 			curX, curY, newX, newY;
	int16			stopFlag;
	LocalWin 		*wi = xg->win_info;
	double			usrX, usrY;
#ifdef X11
	int				rootX, rootY;
	unsigned int	mask;
	Window			childWin, rootWin;
#else
	int 			index;
	Window			dummy;
#endif

	/*
	 * Flush the event queue making sure that the mouse 
	 * has not been released already
	 */
	stopFlag = FALSE;
	while (XPENDING(xg->display) > 0 && !stopFlag) {
		XNEXTEVENT (xg->display,&theEvent);
		switch (theEvent.type) {
			case ButtonRelease:
				stopFlag = TRUE;
				return;
				break;
			default:      
				(void) fprintf(stderr,"Event discarded in xgMeasure\n");
				break;
		} /* switch... */
	} /* while... */

	/* The button is still pressed */
#ifdef X11
	XGrabPointer (xg->display,xg->win,True,ButtonReleaseMask|
		PointerMotionMask|PointerMotionHintMask,
		GrabModeAsync,GrabModeAsync,None,cur,CurrentTime);
#else
	XGrabMouse(xg->win, cur, ButtonReleased|MouseMoved);
#endif

	echoPixel = zoomPixel ^ bgPixel;
#ifndef X11
	for (index = 0;  index < 2;  index++) {
		vl[index].flags = 0;
		hl[index].flags = 0;
	} /* for... */
#endif

	curX = evt->x;  curY = evt->y;
#ifdef X11
	XQueryPointer(xg->display,xg->win,&rootWin,&childWin,
		&rootX,&rootY,&curX,&curY,&mask);
#else
	XUpdateMouse(xg->win, &curX, &curY, &dummy);
#endif
	/* Draw first box */
	DRAWCROSS;
	usrX = TRANX(curX); 
	if (xg->flags & XGLogXFlag) {
		usrX = pow(10.0,usrX);
	} /* if... */
	usrY = TRANY(curY); 
	if (xg->flags & XGLogYFlag) {
		usrY = pow(10.0,usrY);
	} /* if... */		
	xgiPrintCoord(xg,usrX,usrY);
	stopFlag = FALSE;
	while (!stopFlag) {
		XNEXTEVENT (xg->display,&theEvent);
		switch (theEvent.type) {
			case MotionNotify:
#ifdef X11
				XQueryPointer(xg->display,xg->win,&rootWin,&childWin,
					&rootX,&rootY,&newX,&newY,&mask);
#else
				XUpdateMouse(xg->win, &newX, &newY, &dummy);
#endif
				DRAWCROSS;		/* Undraw the old one */
				curX = newX; 
				curY = newY;
				usrX = TRANX(curX); 
				if (xg->flags & XGLogXFlag) {
					usrX = pow(10.0,usrX);
				} /* if... */
				usrY = TRANY(curY); 
				if (xg->flags & XGLogYFlag) {
					usrY = pow(10.0,usrY);
				} /* if... */
				DRAWCROSS;		/* Draw the new one */
				xgiPrintCoord(xg,usrX,usrY);
				break;
			case ButtonRelease:
				DRAWCROSS;
				XUNGRABPOINTER(xg->display);
				if (xg->measuredFunction != (XG_PFI) 0) {
					if (xg->measuredFunction(usrX,usrY) != XG_OK) {
						(void) fprintf(stderr,"The function returned something different from XG_OK");
					} /* if... */
				} /* if... */
				stopFlag = TRUE;
				break;
			default:      
				break;
		} /* switch... */
	} /* while... */
	xgiPrintCoord(xg,XG_NO_OPT,XG_NO_OPT);
	xgiDrawTitle(xg);
#ifdef X11
	XCopyGC (xg->display,normGC,0xffff,xgGC);
#endif
} /* xgMeasure... */

/**************************************************************************
 * XGBOUNDMEASURE
 * Use to set the measure function.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	f (XG_PFI) - the function to call after each measure. (Input)
 *
 * Return Value:
 *	XG_ERROR, unable to assign function.
 *  XG_OK, success.
 */
int16 xgBoundMeasure(xgraph,f)
	XGraph	xgraph;
	XG_PFI	f;
{
	XG		*xg = (XG *) xgraph;

	if (f == (XG_PFI) 0) {
		(void) fprintf(stderr,"Wrong argument for xgBoundMeasure()");
		return(XG_ERROR);
	} /* if... */
	xg->measuredFunction = f;
	return(XG_OK);
} /* xgBoundMeasure... */

/*************************************************************************
 * XGIGETCOLOR
 * Given a standard color name,  this routine fetches the associated
 * pixel value using XGetHardwareColor.  
 *
 * Parameters:
 *	display (Display *) - the display to allocate colors in. (Input)
 *	name (char *) - the name of the color. (Input)
 *
 * Return Value:
 *	XGI_NOCOLOR, unable to allocate color.
 *	if successful then return the index of the color.
 */
static PIXEL xgiGetColor(display,name)
	Display		*display;
	char		*name;
{
#ifdef X11
	XColor	def;

	if (!XParseColor(display,DefaultColormap(display,DefaultScreen(display)),
		name,&def)) {
		return(XGI_NOCOLOR);
	} /* if... */
	if (XAllocColor(display,DefaultColormap(display,DefaultScreen(display)), 
		&def)) {
		return(def.pixel);
	} else {
		return(XGI_NOCOLOR);
	} /* if... */
#else
	Color def;

	if (!XParseColor(name, &def)) {
		return(XGI_NOCOLOR);
	} /* if... */
	if (XGetHardwareColor(&def)) {
		return(def.pixel);
	} else {
		return(XGI_NOCOLOR);
	} /* if...*/
#endif
} /* xgiGetColor... */

/*************************************************************************
 * Default line styles 
 */
#ifdef X11
static char defStyle[XG_MAXSETS][XGI_DASHLEN] = 
	{{1,1,1,1,1,1},{4,4,4,4,4,4},{1,4,2,1,1,4},{3,1,3,1,3,1},
	{8,8,8,8,8,8},{1,1,1,3,1,1},{2,3,1,2,2,3},{6,2,6,2,6,2}};
#else
static unsigned short defStyle[XG_MAXSETS] = {
    0x5555, 0xf0f0, 0x5e5e, 0x1111,
    0xff00, 0x5d5d, 0x6767, 0x3030
};
#endif

/*************************************************************************
 * Default color names 
 */
static char *defColors[XG_MAXSETS] = {
    "red", "SpringGreen", "blue", "yellow",
    "cyan", "plum", "orange", "coral"
};

/****************************************************************************
 * GLOBALINITSETS
 * Initialize global variables.
 *
 * Parameters:
 *	display (Display *) - the display. (Input)
 */
static void GlobalInitSets(display)
	Display		*display;
{		   
	int16		set;
  
	bwFlag = (DISPLAYPLANES(display) < 4);
	bdrSize = 2;
	bdrPixel = BLACKPIXEL(display);
	axisFont = XLOADQUERYFONT(display,"fixed");
	titleFont = XLOADQUERYFONT(display,"fixed");
	barBase = 0.0;
	barWidth = -1.0;		/* Holder */
#ifdef X11
	normGC = XCreateGC(display,DefaultRootWindow(display),0x0,NIL(XGCValues));
	XCopyGC (display,DefaultGC(display,DefaultScreen(display)),0xffff,normGC);
	zeroGC = XCreateGC(display,DefaultRootWindow(display),0x0,NIL(XGCValues));
	XCopyGC (display,DefaultGC(display,DefaultScreen(display)),0xffff,zeroGC);
#endif
	if (bwFlag) {
		/* Its black and white */
		bgPixel = WHITEPIXEL(display);
		zoomPixel = BLACKPIXEL(display);
		lineWidth = 2;
#ifdef X11
		XSetForeground(display,zeroGC,BLACKPIXEL(display));
		XSetLineAttributes(display,zeroGC,3,LineSolid,CapNotLast,JoinMiter);
		XSetForeground(display,normGC,BLACKPIXEL(display));
		XSetLineAttributes(display,normGC,1,LineSolid,CapNotLast,JoinMiter);
		menuAttr.title_fg = WHITEPIXEL(display);
		menuAttr.title_bg = BLACKPIXEL(display);
		menuAttr.item_fg = BLACKPIXEL(display);
		menuAttr.item_bg = WHITEPIXEL(display);
		menuAttr.cur_fg = BLACKPIXEL(display);
		menuAttrMask = XPA_T_FG|XPA_T_BG|XPA_I_FG|XPA_I_BG|XPA_C_FG;
#else
		zeroSize = 3;
		zeroStyle = SolidLine;
		zeroPixel = BLACKPIXEL(display);
		normSize = 1;
		normStyle = SolidLine;
		normPixel = BLACKPIXEL(display);
#endif
		cursorPixel = BLACKPIXEL(display);
	} else {
		/* Its color */
		bgPixel = xgiGetColor(display,"LightGray");
		zoomPixel = xgiGetColor(display,"white");
		lineWidth = 1;
#ifdef X11
		XSetForeground(display,zeroGC,WHITEPIXEL(display));
		XSetLineAttributes(display,zeroGC,1,LineSolid,CapNotLast,JoinMiter);
		XSetForeground(display,normGC,BLACKPIXEL(display));
		XSetLineAttributes(display,normGC,1,LineSolid,CapNotLast,JoinMiter);
		menuAttr.title_fg = xgiGetColor(display,"yellow");
		menuAttr.title_bg = xgiGetColor(display,"brown");
		menuAttr.item_fg = xgiGetColor(display,"black");
		menuAttr.item_bg = xgiGetColor(display,"#cccccc");
		menuAttr.cur_fg = xgiGetColor(display,"red");
		menuAttrMask = XPA_T_FG|XPA_T_BG|XPA_I_FG|XPA_I_BG|XPA_C_FG;
#else
		zeroSize = 1;
		zeroStyle = SolidLine;
		zeroPixel = WHITEPIXEL(display);
		normSize = 1;
		normStyle = SolidLine;
		normPixel = BLACKPIXEL(display);
#endif
		cursorPixel = BLACKPIXEL(display);
	} /* if... */
#ifdef X11
	XSetBackground(display,normGC,bgPixel);
#endif
	/* Depends critically on whether the display has color */
	for (set = 0 ; set < XG_MAXSETS ; set++) {
		if (bwFlag) {
#ifdef X11
			AllAttrs[set].lineStyle = LineOnOffDash;
			AllAttrs[set].dashList = defStyle[set];
#else
			AllAttrs[set].lineStyle = XMakePattern(defStyle[set], 16, 1);
#endif
			AllAttrs[set].pixelValue = BLACKPIXEL(display);
		} else {
			/* Its color */
#ifdef X11
			AllAttrs[set].lineStyle = LineSolid;
#else
			AllAttrs[set].lineStyle = SolidLine;
#endif
			AllAttrs[set].pixelValue = xgiGetColor(display,defColors[set]);
		} /* if... */
	} /* for... */
	/* Store bitmap for dots */
#ifndef X11
	dotMap = XStoreBitmap(dot_width, dot_height, dot_bits);
#endif

	propertyFlags = XGBoundingBoxFlag|XGLinesFlag;
} /* GlobalInitSets... */

/***************************************************************************
 * INITSETS
 * Initializes the data sets with default information.
 *
 * Parameters:
 *	xg (XG *) - the graph structure. (Input)
 */
static void InitSets(xg)
	XG	*xg;
{
	int index;

	xg->numPlots = 0;
	xg->dataSet = XG_ALLOC(DataSet,XG_MAXSETS);
	for (index = 0 ; index < XG_MAXSETS ; index++) {
		xg->dataSet[index].setName = NIL(char);
		xg->dataSet[index].numPoints = 0;
		xg->dataSet[index].allocSize = 0;
	} /* for... */
	xg->allocXpoints = 0;
	/* Set all the flags to the default value */
	xg->flags = propertyFlags;
	xg->measuredFunction = (XG_PFI) 0;
} /* InitSets... */

/************************************************************************
 * REVERSEPIXEL
 * Reverse the background and foreground color for the specified
 * pixel value.
 *
 * Parameters:
 *	display (Display *) - the display. (Input)
 *	pixValue (PIXEL *) - the pixel value. (Input/Output)
 */
static void ReversePixel(display,pixValue)
	Display		*display;
	PIXEL		*pixValue;			/* Pixel value to reverse  */
{
	if (*pixValue == WHITEPIXEL(display)) {
		*pixValue = BLACKPIXEL(display);
	} else {
		if (*pixValue == BLACKPIXEL(display)) {
			*pixValue = WHITEPIXEL(display);
		} /* if... */
	} /* if... */
} /* ReversePixel... */

/***********************************************************************
 * READDEFAULTS
 * Reads X default values which override the hard-coded defaults
 * set up by InitSets.
 *
 * Parameters:
 *	display (Display *) - the display. (Input)
 *	progname (char *) - the name of the program using 'xg'. (Input)
 */
static void ReadDefaults(display,progname)
	Display		*display;
	char		*progname;
{
#ifdef X11
	XFontStruct		*tempFont;
#else
	FontInfo		*tempFont;
#endif
	char	*value, newname[100];
	PIXEL	temp;
	int		index, style;

	RDCLR("xg.Background", bgPixel);
	RDCLR("xg.ZoomColor", zoomPixel);
	RDCLR("xg.CursorColor", cursorPixel);
	RDINT("xg.BorderSize", bdrSize);
	RDCLR("xg.Border", bdrPixel);
#ifdef X11
	if ( (value = XGETDEFAULT(display,progname,"xg.GridSize")) ) {
		XSetLineAttributes(display,normGC,atoi(value),LineSolid,
			CapNotLast,JoinMiter);
	} /* if... */
	if ( (value = XGETDEFAULT(display,progname,"xg.ZeroSize")) ) {
		XSetLineAttributes(display,zeroGC,atoi(value),LineSolid,
			CapNotLast,JoinMiter);
	} /* if... */
	if ((value = XGETDEFAULT(display,progname, "xg.Foreground")) &&
		((temp = xgiGetColor(display,value)) != XGI_NOCOLOR)) {
		XSetForeground(display,normGC,temp);
	} /* if... */
	if ((value = XGETDEFAULT(display,progname, "xg.ZeroColor")) &&
		((temp = xgiGetColor(display,value)) != XGI_NOCOLOR)) {
		XSetForeground(display,zeroGC,temp);
	} /* if... */
	RDCLR("xg.menu.title.foreground",menuAttr.title_fg);
	RDCLR("xg.menu.title.background",menuAttr.title_bg);
	RDCLR("xg.menu.item.foreground",menuAttr.item_fg);
	RDCLR("xg.menu.item.background",menuAttr.item_bg);
	RDCLR("xg.menu.cursor.color",menuAttr.cur_fg);
#else
	RDINT("xg.GridSize", normSize);
	RDINT("xg.GridStyle", normStyle);
	RDCLR("xg.Foreground", normPixel);
	RDCLR("xg.ZeroColor", zeroPixel);
	RDINT("xg.ZeroStyle", zeroStyle);
	RDINT("xg.ZeroSize", zeroSize);
#endif
	RDFNT("xg.LabelFont", axisFont);
	RDFNT("xg.TitleFont", titleFont);
	RDFLG("xg.Spline",XGBSplineFlag);
	RDFLG("xg.Grid", XGGridFlag);
	RDFLG("xg.Markers", XGBigMarkFlag);
	RDFLG("xg.BoundBox", XGBoundingBoxFlag);
	RDFLG("xg.NoLines", XGLinesFlag);
	RDFLG("xg.HHMMSS",  XGHHMMSSFlag);
	RDFLG("xg.PixelMarkers", XGPixelMarkFlag);
	RDINT("xg.LineWidth", lineWidth);

	/* Read the default line and color attributes */
	for (index = 0;  index < XG_MAXSETS;  index++) {
		(void) sprintf(newname, "xg.%d.Style", index);
		style = -1;
		RDINT(newname, style);
		if (style >= 0) {
#ifdef X11
			AllAttrs[index].lineStyle = LineSolid;
#else
			AllAttrs[index].lineStyle = XMakePattern(style, 16, 1);
#endif
		} /* if... */
		(void) sprintf(newname, "xg.%d.Color", index);
		RDCLR(newname, AllAttrs[index].pixelValue);
	} /* for... */

	if (bwFlag) {
		/* Black and white - check for reverse video */
		if (XGETDEFAULT(display,progname, "xg.ReverseVideo")) {
			ReversePixel(display,&bgPixel);
			ReversePixel(display,&bdrPixel);
#ifdef X11
			ReversePixel(display,&(((XGCValues *) zeroGC)->foreground));
			ReversePixel(display,&(((XGCValues *) normGC)->foreground));
#else
			ReversePixel(display,&zeroPixel);
			ReversePixel(display,&normPixel);
#endif
			for (index = 0;  index < XG_MAXSETS;  index++) {
				ReversePixel(display,&(AllAttrs[index].pixelValue));
			} /* for... */
		} /* if... */
	} /* if...*/
} /* ReadDefaults... */

/**************************************************************************
 * XGNEWWINDOW
 * Inits and maps a new window.  This includes allocating its
 * local structure and associating it with the XId for the window.
 * The aspect ratio is specified as the ratio of width over height.
 *
 * Parameters:
 *	display (Display *) - the X display to open window in. (Input)
 *	progName (char *) - the name of the program making the request. (Input)
 *	title (char *) - the title of the graph. (Input)
 *	Xunits (char *) - the label for the x-axis. (Input)
 *	Yunits (char *) - the label for the y-axis. (Input)
 *	w (int) - the width of the window. (Input)
 *	h (int) - the height of the window. (Input)
 *	x (int) - the x-coordinate of window. (Input)
 *	y (int) - the y-coordinate of window. (Input)
 *
 * Return Value:
 *  The identifier for the newly opened graph. (XGraph)
 */
XGraph xgNewWindow(display, progName, title, Xunits, Yunits,  w, h, x, y)
	Display	*display;
	char	*progName;		/* Name of program    */
	char	*title;
	char	*Xunits;
	char	*Yunits;
	int 	w,h,x,y;
{
#ifdef X11
	XSetWindowAttributes	attr;
#else
	OpaqueFrame		theFrame;
	char			geometry[32]; 
#endif
	double			pad;
	static int16	defaultsRead = FALSE;
	XG				*xg;

	xgDisplay = display;

	if ((xg = ALLOC(XG,1)) == NIL(XG)) {
		return((XGraph) 0);
	} /* if... */
	xg->display = display;
	xg->titleText = util_strsav(title);
	xg->YUnits = util_strsav(Yunits);
	xg->XUnits = util_strsav(Xunits);

	/* Read X defaults and override hard-coded defaults */
	/* Set up hard-wired defaults and allocate spaces */
	if (!defaultsRead) {
		GlobalInitSets(display);
		ReadDefaults(display,progName);
		defaultsRead = TRUE;
#ifdef X11
		xgGC = XCreateGC(display,DefaultRootWindow(display),0x0,NIL(XGCValues));
		XCopyGC (display,DefaultGC(display,DefaultScreen(display)),0xffff,xgGC);
#endif
#ifdef X11
		zoomCursor = XCreateFontCursor(display,XC_dotbox);
		waitCursor = XCreateFontCursor(display,XC_heart);
#else
		zoomCursor = XCreateCursor(zoom_width, zoom_height, zoom_bits, 
			zoom_bits,zoom_x_hot, zoom_y_hot, cursorPixel, bgPixel, GXcopy);
		waitCursor = XCreateCursor(wait_width, wait_height, wait_bits, 
			wait_bits, wait_x_hot, wait_y_hot, cursorPixel, bgPixel, GXcopy); 
#endif
	} /* if... */

	InitSets(xg);

	/* init xg */
	if ((xg->win_info = ALLOC(LocalWin,1)) == NIL(LocalWin)) {
		FREE(xg);
		return((XGraph) 0);
	} /* if... */
	xg->win_info->loX = 0;
	xg->win_info->hiX = 0;
	xg->win_info->loY = 0;
	xg->win_info->hiY = 0;
  
	/* Increase the padding for aesthetics */
	if (xg->win_info->hiX - xg->win_info->loX == 0.0) {
		pad = MAX(0.5,ABS(xg->win_info->hiX/2.0));
		xg->win_info->hiX += pad;
		xg->win_info->loX -= pad;
	} /* if... */
	if (xg->win_info->hiY - xg->win_info->loY == 0) {
		pad = MAX(0.5,ABS(xg->win_info->hiY/2.0));
		xg->win_info->hiY += pad;
		xg->win_info->loY -= pad;
	} /* if... */

	/* Add 10% padding to bounding box (div by 20 yeilds 5%) */
	pad = (xg->win_info->hiX - xg->win_info->loX) / 20.0;
	xg->win_info->loX -= pad;  xg->win_info->hiX += pad;
	pad = (xg->win_info->hiY - xg->win_info->loY) / 20.0;
	xg->win_info->loY -= pad;  xg->win_info->hiY += pad;

#ifndef X11
	/* Open the window */
	theFrame.bdrwidth = bdrSize;
	theFrame.border = XMakeTile(bdrPixel);
	theFrame.background = XMakeTile(bgPixel);
#endif
	if (((x == -1) && (y == -1)) || ((w == -1) && (h == -1))) {
#ifdef X11
		if ((x == -1) && (y == -1)) {
			x = y = 0;
		} /* if... */
		if ((w == -1) && (h == -1)) {
			w = h = 500;
		} /* if... */
		xg->win=XCreateSimpleWindow(xg->display,DefaultRootWindow(xg->display),
			x,y,w,h,bdrSize,bdrPixel,bgPixel);
		xg->win_info->width = w;
		xg->win_info->height = h;
#else
		if ((w != -1) && (h != -1)) {
			theFrame.width = w;
			theFrame.height = h;
			(void) sprintf (geometry,"=%dx%d",w,h);
		} else {
			(void) sprintf (geometry,"=500x500");
		} /* if... */
		xg->win = XCreate("XGraph Create Window",progName,geometry,"",
			&theFrame,MAX(w,100),MAX(h,100));
		xg->win_info->width = theFrame.width;
		xg->win_info->height = theFrame.height;
#endif
	} else {
#ifdef X11
		attr.override_redirect = TRUE;
		attr.background_pixel = bgPixel;
		attr.border_pixel = bdrPixel;
		xg->win = XCreateWindow(xg->display,DefaultRootWindow(xg->display),
			x,y,w,h,bdrSize,CopyFromParent,CopyFromParent,CopyFromParent,
			CWBackPixel|CWBorderPixel|CWOverrideRedirect,&attr);
#else
		xg->win = XCreateWindow(RootWindow,x,y,w,h,bdrSize,WhitePixmap,
			XMakeTile(bgPixel));
#endif
		xg->win_info->width = MAX(w,50);
		xg->win_info->height = MAX(h,50);
	} /* if... */
	XSTORENAME(xg->display, xg->win, "xg_graph");

#ifndef X11
	XFreePixmap(theFrame.border);
	XFreePixmap(theFrame.background);
#endif
	if (xg->win) {
#ifdef X11
		XSelectInput(xg->display,xg->win, ExposureMask|ButtonReleaseMask|
			KeyPressMask|ButtonPressMask|StructureNotifyMask);
#else
		XSelectInput(xg->win, ExposeRegion|KeyPressed|ButtonPressed|ButtonReleased);
#endif
		if (!theCursor) {
#ifdef X11
			theCursor = XCreateFontCursor(xg->display,XC_crosshair);
#else
			theCursor = XCreateCursor(XGcursor_width, XGcursor_height,
				XGcursor_bits, XGcursor_bits,
				XGcursor_x_hot, XGcursor_y_hot,
				cursorPixel, bgPixel, GXcopy);
#endif
		} /* if... */
		XDEFINECURSOR(xg->display,xg->win, theCursor);
		if (!win_table) {
#ifdef X11
			win_table = XUniqueContext();
#else
			win_table = XCreateAssocTable(64);
#endif
		} /* if... */
#ifdef X11
		XSaveContext (xg->display,xg->win,win_table,(caddr_t) xg);
#else
		XMakeAssoc(win_table, (XId) xg->win, (char *) xg);
#endif
		XMAPWINDOW(xg->display,xg->win);
		XFLUSH(xg->display);
	} else {
		FREE(xg);
		return(0);
	} /* if... */
	if (!suspendRedraw) xgRedrawGraph((XGraph) xg);
	(void) xgStartXMenu(xg->display);
	return((XGraph) xg);
} /* xgNewWindow... */

/*************************************************************************
 * XGSUSPENDREDRAW
 * Suspend the redrawing of the graphs.
 *
 * Parameters:
 *	None.
 *
 * Return Value:
 *	XG_OK, success.
 */
int16 xgSuspendRedraw()
{
	suspendRedraw = TRUE;
	return(XG_OK);
} /* xgSuspendRedraw... */

/************************************************************************
 * XGSETRANGE
 * Set the range of the specified graph.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	minX, minY, maxX, maxY (double) - the bounds of the graph. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int16 xgSetRange(xgraph, minX, maxX, minY , maxY)
	XGraph	xgraph;
	double	minX, minY, maxX, maxY;
{
	XG		*xg = (XG *) xgraph;
	int16	changedBB = FALSE;

	if (minX != XG_NO_OPT) {
		changedBB = TRUE;
		xg->win_info->loX = minX;
	} /* if... */
	if (maxX != XG_NO_OPT) {
		changedBB = TRUE;
		xg->win_info->hiX = maxX;
	} /* if... */
	if (minX != XG_NO_OPT) {
		changedBB = TRUE;
		xg->win_info->loY = minY;
	} /* if... */
	if (maxY != XG_NO_OPT) {
		changedBB = TRUE;
		xg->win_info->hiY = maxY;
	} /* if... */
	if (changedBB) {
		if (xg->win_info->loX == xg->win_info->hiX) {
			(void) fprintf(stderr, "XG:Cannot set X range to (%g,%g)\n", 
				xg->win_info->loX,xg->win_info->loX);
			xg->win_info->hiX = xg->win_info->loX + 0.0001;
		} else if (xg->win_info->loX > xg->win_info->hiX) {
			SWAP(double, xg->win_info->loX ,xg->win_info->hiX);
		} /* if... */

		if (xg->win_info->loY == xg->win_info->hiY) {
			(void) fprintf(stderr, "XG:Cannot set Y range to (%g,%g)\n", xg->win_info->loY,xg->win_info->loY);
			xg->win_info->hiY = xg->win_info->loY + 0.0001;
		} else if (xg->win_info->loY > xg->win_info->hiY) {
			SWAP(double, xg->win_info->loY ,xg->win_info->hiY);
		} /* if... */
		if (!suspendRedraw) xgRedrawGraph((XGraph) xg);
	} /* if... */
	return(XG_OK);
} /* xgSetRange... */

/************************************************************************
 * XGGETRANGE
 * Get the current range of the specified graph.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	minX, minY, maxX, maxY (double *) - the graph range. (Output)
 *
 * Return Value:
 *	XG_OK, success.
 */
int16 xgGetRange(xgraph, minX, maxX, minY , maxY)
	XGraph	xgraph;
	double	*minX, *minY, *maxX, *maxY;
{
	XG		*xg = (XG *) xgraph;

	*minX = xg->win_info->loX;
	*maxX = xg->win_info->hiX;
	*minY = xg->win_info->loY;
	*maxY = xg->win_info->hiY;
	return(XG_OK);
} /* xgGetRange... */

/************************************************************************
 * XGIINTEGERTOHHMMSS
 * Convert x-coordinate into 'hh:mm:ss' format.
 *
 * Parameters:
 *	xcoord (int) - the x-coordinate. (Input)
 *	st (char *) - the x-coordinate in the format 'hh:mm:ss'. (Output)
 */
static void xgiIntegerTohhmmss(xcoord,st)
	int		xcoord;
	char	*st;
{
	(void) sprintf (st, "%02d:%02d:%02d" , xcoord/3600, (xcoord%3600)/60,
		(xcoord%3600)%60);
} /* xgiIntegerTohhmmss... */

/**************************************************************************
 * XGCLEARWGRAPH
 * Clear the specified graph.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	setname (char *) - the name of the plot within the graph to
 *		clear. if no name is specified then all plots within the
 *		graph are cleared. (Input)
 *
 * Return Value:
 *	XG_OK, success.	
 */
int16 xgClearGraph(xgraph,setname)
	XGraph	xgraph;
	char	*setname;
{
	XG		*xg = (XG *) xgraph;
	int16	set;

	if (setname == NIL(char)) {
		FreeAllData(xg);
	} else {
		(void) xgiFindSet(xg,setname, &set);
		FREE(xg->dataSet[set].setName);
		FREE(xg->dataSet[set].xvec);
		FREE(xg->dataSet[set].yvec);
		xg->dataSet[set].setName = util_strsav("<deleted>");
		xg->dataSet[set].allocSize = 0;
		xg->dataSet[set].numPoints = 0;
	} /* if... */
	if (!suspendRedraw) {
		xgRedrawGraph((XGraph) xg);
	} /* if... */
	return(XG_OK);
} /* xgClearGraph... */

/************************************************************************
 * XGISHOWPOINT
 * Display point in graph.
 *
 * Parameters:
 *	xg (XG *) - the graph structure. (Input)
 *	set (int16) - the index of the set within the graph. (Input)
 *	spot (int16) - the index of the point within the set. (Input)
 *	pixel (PIXEL) - the color of the point. (Input)
 */
static void xgiShowPoint (xg,set,spot,pixel)
	XG		*xg;
	int16	set;
	int16	spot;
	PIXEL	pixel;
{
	int16	Xx,Xy;

	Xx = SCREENX(xg->win_info, xg->dataSet[set].xvec[spot]);
	Xy = SCREENY(xg->win_info, xg->dataSet[set].yvec[spot]);
#ifdef X11
	XSetForeground(xg->display,xgGC,pixel);
#endif
	/* Draw markers if requested */
	if (xg->flags & XGBigMarkFlag) {
#ifdef X11
		XFillArc(xg->display,xg->win,xgGC,Xx-2,Xy-2,4,4,0,360*64);
#else
		XBitmapBitsPut(area, Xx - (dot_width>>1),Xy - (dot_height>>1),
			dot_width, dot_height, dot_bits, pixel, bgPixel,
			dotMap, GXcopy, AllPlanes);
#endif
	} /* if... */

	/*** Draw pixel ***/
	if (xg->flags & XGPixelMarkFlag) {
#ifdef X11
		XDrawPoint(xg->display,xg->win,xgGC,Xx,Xy);
#else
		XPixSet(area, Xx, Xy, 1, 1, pixel);
#endif
	} /* if... */

	/*** Draw bar elements if requested ***/
	if (xg->flags & XGBarFlag) {
		int barPixels, baseSpot;
		XPOINT	line[2];

		barPixels = (int) ((barWidth / xg->win_info->XUnitsPerPixel) + 0.5);
		if (barPixels <= 0) barPixels = 1;
		baseSpot = SCREENY(xg->win_info, barBase);
		line[0].x = line[1].x = Xx - (barPixels >> 1);
		line[0].y = baseSpot;  line[1].y = Xy;
#ifdef X11
		XSetLineAttributes(xg->display,xgGC, barPixels, 
			AllAttrs[set].lineStyle,CapNotLast,JoinMiter);
		if (AllAttrs[set].lineStyle == LineOnOffDash) {
			XSetDashes (xg->display,xgGC,0,AllAttrs[set].dashList,XGI_DASHLEN);
		} /* if... */
		XDrawLines(xg->display,xg->win,xgGC,line,2,CoordModeOrigin);
#else
		line[0].flags = line[1].flags = 0;
		XDrawDashed(area, line, 2, barPixels, 1, pixel, 
			AllAttrs[set].lineStyle,GXcopy, AllPlanes);
#endif
	} /* if... */
} /* xgiShowPoint... */

/***************************************************************************
 * XGANIMATEGRAPH
 * Animate graph.
 *
 * Parameters:
 *	xgraph (XGraph) - graph identifier. (Input)
 *	start (int16) - index in points array to start drawing graph. (Input)
 *	length (int16) - number of points to draw. (Input)
 *	Xarray (double *) - array of x-coordinates. (Input)
 *	Yarray (double *) - array of y-coordinates. (Input)
 *	setName (char *) - the name of the set to plot. (Input)
 *
 * Return Value:
 *	XG_OK, success. 
 */
int16 xgAnimateGraph(xgraph, start, length, Xarray , Yarray , setName)
	XGraph		xgraph;
	int16		start,length;
	double		*Xarray;
	double		*Yarray;
	char		*setName;
{
	XG			*xg = (XG *) xgraph;
	int16 		spot;			/* index in the array of points */
	int16 		i;
	int16		set;
	LocalWin	*wi = xg->win_info;
	int16		end;
	double		x,y;
#ifdef X11
	XRectangle	rectangle;
#endif

	(void) xgiFindSet(xg,setName, &set);

	end = start + length;

	(void) xgiAllocManager(xg,set, end);
 
#ifdef X11
	rectangle.width = wi->XOppX - wi->XOrgX + 1;
	rectangle.height = wi->XOppY - wi->XOrgY + 1;
	rectangle.x = wi->XOrgX;
	rectangle.y = wi->XOrgY;
	XSetClipRectangles(xg->display,xgGC,0,0,&rectangle,1,Unsorted);
#else
	area = XCreateTransparency(xg->win, wi->XOrgX, wi->XOrgY,
									wi->XOppX - wi->XOrgX + 1,
									wi->XOppY - wi->XOrgY + 1);
	XMAPWINDOW(xg->display,area);
#endif

	for (i = 0, spot = start ; spot < end ; i++,spot++) {
		if (spot < xg->dataSet[set].numPoints) {
			xgiShowPoint(xg,set,spot,bgPixel);
		} /* if... */
		/* Add new point */
		xg->dataSet[set].xvec[spot] = x = Xarray[i];
		xg->dataSet[set].yvec[spot] = y = Yarray[i];
		if (xg->flags & XGLogXFlag) {
			if (x <= 0.0) {
				(void) fprintf(stderr,"Negative X value in logarithmic mode\n");
				return(XG_ERROR);
			} /* if... */
			xg->dataSet[set].xvec[spot] =  log10(x);
		} /* if... */
		if (xg->flags & XGLogYFlag) {
			if (y <= 0.0) {
				(void) fprintf(stderr,"Negative Y value in logarithmic mode\n");
				return(XG_ERROR);
			} /* if... */
			xg->dataSet[set].yvec[spot] = log10(y);
		} /* if... */
		xgiShowPoint(xg,set,spot,AllAttrs[set].pixelValue);
	} /* for... */
	XFLUSH(xg->display);
#ifdef X11
	XSetClipMask(xg->display,xgGC,None);
#else
	XDESTROYWINDOW(xg->display,area);
#endif
	if (xg->dataSet[set].numPoints < end) {
		xg->dataSet[set].numPoints = end;
	} /* if... */
	return(XG_OK);
} /* xgAnimateGraph... */

/********************************************************************
 * XGIADDPADDING
 * Add padding to plot window.
 *
 * Parameters:
 *	xg (XG *) - the xg structure. (Input/Output)
 */
void xgiAddPadding(xg)
	XG		*xg;
{
	double	pad;

	/* Add 10% padding to bounding box (div by 20 yields 5%) */
	pad = (xg->win_info->hiX - xg->win_info->loX) / 20.0;
	xg->win_info->loX -= pad;  xg->win_info->hiX += pad;
	pad = (xg->win_info->hiY - xg->win_info->loY) / 20.0;
	xg->win_info->loY -= pad;  xg->win_info->hiY += pad;
} /* xgiAddPadding... */

/**********************************************************************
 * XGITRANSFORMCOMPUTE
 * This routine figures out how to draw the axis labels and grid lines.
 * Currently,  the system only supports linear axes.  Axis labels are
 * drawn in engineering notation.  The power of the axes are labeled
 * in the normal axis labeling spots.  The routine also figures
 * out the necessary transformation information for the display
 * of the points (it touches XOrgX, XOrgY, UsrOrgX, UsrOrgY, and
 * UnitsPerPixel).
 *
 * Parameters:
 *	xg (XG *) - the xg structure. (Input/Output)
 *
 * Return Value:
 *	TRUE, success.
 *	FALSE, unsuccessful.
 */
static int xgiTransformCompute(xg)
	XG			*xg;
{
	LocalWin	*wi = xg->win_info;		/* Window information */
	double		bbCenX, bbCenY, bbHalfWidth, bbHalfHeight;
	int16		maxName, leftWidth;
	int16		set;
	double		pad;
	int16		addPaddingFlag = FALSE;

	/* Increase the padding for aesthetics */
	if (xg->win_info->hiX - xg->win_info->loX == 0.0) {
		pad = MAX(0.5,ABS(xg->win_info->hiX/2.0));
		xg->win_info->hiX += pad;
		xg->win_info->loX -= pad;
		addPaddingFlag = TRUE;
	} /* if... */
	if (xg->win_info->hiY - xg->win_info->loY == 0) {
		pad = MAX(0.5,ABS(xg->win_info->hiY/2.0));
		xg->win_info->hiY += pad;
		xg->win_info->loY -= pad;
		addPaddingFlag = TRUE;
	} /* if... */
  
	if (addPaddingFlag) xgiAddPadding(xg);

	/*
	 * First,  we figure out the origin in the X window.  Above
	 * the space we have the title and the Y axis unit label.
	 * To the left of the space we have the Y axis grid labels.
	 */

	wi->XOrgX = PADDING + (7 * XFONTWIDTH(axisFont)) + PADDING;
	wi->XOrgY = PADDING + XFONTHEIGHT(titleFont) + PADDING +
		XFONTHEIGHT(axisFont) + XFONTHEIGHT(axisFont)/2 + PADDING;
	/*
	 * Now we find the lower right corner.  Below the space we
	 * have the X axis grid labels.  To the right of the space we
	 * have the X axis unit label and the legend.  We assume the 
	 * worst case size for the unit label.
	 */

	maxName = 0;
	for (set= 0 ; set < xg->numPlots ; set++) {
		maxName = MAX(maxName, strlen(xg->dataSet[set].setName));
	} /* for... */

	/* Worst case size of the X axis label: */
	leftWidth = (strlen(xg->XUnits)+8) * XFONTWIDTH(axisFont);
	if ((maxName*XFONTWIDTH(axisFont))+PADDING > leftWidth) {
		leftWidth = maxName * XFONTWIDTH(axisFont) + PADDING;
	} /* if... */
    
	wi->XOppX = wi->width - PADDING - leftWidth;
	wi->XOppY = wi->height - PADDING - XFONTHEIGHT(axisFont) - PADDING;
	if ((wi->XOrgX >= wi->XOppX) || (wi->XOrgX >= wi->XOppY)) {
		(void) fprintf(stderr, "Drawing area is too small\n");
		return(FALSE);
	} /* if... */

	/* 
	 * We now have a bounding box for the drawing region.
	 * Figure out the units per pixel using the data set bounding box.
	 */
	wi->XUnitsPerPixel = (wi->hiX - wi->loX)/((double) (wi->XOppX - wi->XOrgX));
	wi->YUnitsPerPixel = (wi->hiY - wi->loY)/((double) (wi->XOppY - wi->XOrgY));

	/*
	 * Find origin in user coordinate space.  We keep the center of
	 * the original bounding box in the same place.
	 */
	bbCenX = (wi->loX + wi->hiX) / 2.0;
	bbCenY = (wi->loY + wi->hiY) / 2.0;
	bbHalfWidth = ((double) (wi->XOppX - wi->XOrgX))/2.0 * wi->XUnitsPerPixel;
	bbHalfHeight = ((double) (wi->XOppY - wi->XOrgY))/2.0 * wi->YUnitsPerPixel;
	wi->UsrOrgX = bbCenX - bbHalfWidth;
	wi->UsrOrgY = bbCenY - bbHalfHeight;
	wi->UsrOppX = bbCenX + bbHalfWidth;
	wi->UsrOppY = bbCenY + bbHalfHeight;

	/*
	 * Everything is defined so we can now use the SCREENX and SCREENY
	 * transformations.
	 */
	return(TRUE);
} /* xgiTransformCompute... */

/*** To get around an inaccurate log ***/

#define nlog10(x)	(x == 0.0 ? 0.0 : log10(x) + 1e-15)

/***********************************************************************
 * ROUNDUP
 * This routine rounds up the given positive number such that
 * it is some power of ten times either 1, 2, or 5.  It is
 * used to find increments for grid lines.
 *
 * Parameters:
 *	val (double) - the value to round up. (Input)
 *
 * Return Value:
 *	The rounded number. (double)
 */
static double RoundUp(val)
	double val;			/* Value */
{
	int exponent, index;

	exponent = (int) floor(nlog10(val));
	if (exponent < 0) {
		for (index = exponent;  index < 0; index++) {
			val *= 10.0;
		} /* for... */
	} else {
		for (index = 0;  index < exponent; index++) {
			val /= 10.0;
		} /* for... */
	} /* if... */
	if (val > 5.0) {
		val = 10.0;
	} else {
		if (val > 2.0) {
			val = 5.0;
		} else {
			if (val > 1.0) {
				val = 2.0;
			} else {
				val = 1.0;
			} /* if... */
		} /* if... */
	} /* if... */
	if (exponent < 0) {
		for (index = exponent;  index < 0;  index++) {
			val /= 10.0;
		} /* for... */
	} else {
		for (index = 0;  index < exponent;  index++) {
			val *= 10.0;
		} /* for... */
	} /* if... */
	return(val);
} /* RoundUp... */

/***********************************************************************
 * XGIDRAWGRIDANDAXIS
 * This routine draws grid line labels in engineering notation,
 * the grid lines themselves, and unit labels on the axes.
 *
 * Parameters:
 *	xg (XG *) - the xg structure. (Input/Output)
 */
static void xgiDrawGridAndAxis(xg)
	XG			*xg;
{
	LocalWin	*wi = xg->win_info;	/* Window information */
	int			expX, expY;			/* Engineering powers */
	int			startX;
	int			Yspot, Xspot;
	char		power[10], value[20];
	double		Xincr, Yincr, Xstart, Ystart, Yindex, Xindex, larger;
#ifdef X11
	XSegment	segments[2];
	int			numSegments;
#else
	Vertex		line[4];
	int			numPoints;
#endif

	/*
	 * Grid display powers are computed by taking the log of
	 * the largest numbers and rounding down to the nearest
	 * multiple of 3.
	 */
	if (ABS(wi->UsrOrgX) > ABS(wi->UsrOppX)) {
		larger = ABS(wi->UsrOrgX);
	} else {
		larger = ABS(wi->UsrOppX);
	} /* if... */
	expX = ((int) floor(nlog10(larger)/3.0)) * 3;
	if (ABS(wi->UsrOrgY) > ABS(wi->UsrOppY)) {
		larger = ABS(wi->UsrOrgY);
	} else {
		larger = ABS(wi->UsrOppY);
	} /* if... */
	expY = ((int) floor(nlog10(larger)/3.0)) * 3;

	/*
	 * With the powers computed,  we can draw the axis labels.
	 */
#ifdef X11
	XSetFont(xg->display,normGC,axisFont->fid);
	XDrawImageString(xg->display,xg->win,normGC, PADDING,
		PADDING + XFONTHEIGHT(titleFont) + PADDING + 
		(XFONTHEIGHT(axisFont)/2),xg->YUnits,strlen(xg->YUnits));
#else
	XText(xg->win, PADDING,
		PADDING + titleFont->height + PADDING + (axisFont->height/2),
		xg->YUnits, strlen(xg->YUnits), axisFont->id, normPixel, bgPixel);
#endif
	if ((expY != 0) || (xg->flags & XGLogYFlag)) {
#ifdef X11
		XDrawImageString(xg->display,xg->win,normGC, PADDING
			+ (strlen(xg->YUnits) * XFONTWIDTH(axisFont)),
			PADDING + XFONTHEIGHT(titleFont) + 
			PADDING + (XFONTHEIGHT(axisFont)/2),
			" x 10", 5);
#else
		XText(xg->win, PADDING + (strlen(xg->YUnits) * axisFont->width),
			PADDING + titleFont->height + PADDING + (axisFont->height/2),
			" x 10", 5, axisFont->id, normPixel, bgPixel);
#endif
		if (xg->flags & XGLogYFlag) {
			(void) strcpy(power, "y");
		} else {
			(void) sprintf(power, "%d", expY);
		} /* if... */
#ifdef X11
		XDrawImageString(xg->display,xg->win,normGC, PADDING
			+ ((strlen(xg->YUnits)+5) * XFONTWIDTH(axisFont)),
			PADDING + XFONTHEIGHT(titleFont) + PADDING,
			power, strlen(power));
#else
		XText(xg->win, PADDING + ((strlen(xg->YUnits)+5) * axisFont->width),
			PADDING + titleFont->height + PADDING,
			power, strlen(power), axisFont->id, normPixel, bgPixel);
#endif
	} /* if... */
	startX = wi->width - PADDING - (strlen(xg->XUnits) * 
		XFONTWIDTH(axisFont));
	if ((expX != 0) || (xg->flags & XGLogXFlag)) {
		if (xg->flags & XGLogXFlag) {
			(void) strcpy(power, "x");
			startX -= 6 * XFONTWIDTH(axisFont);
		} else {
			(void) sprintf(power, "%d", expX);
			startX -= (strlen(power) + 5) * XFONTWIDTH(axisFont);
		} /* if... */
	} /* if... */
#ifdef X11
	XDrawImageString(xg->display,xg->win,normGC, 
		startX, wi->XOppY - (XFONTHEIGHT(axisFont)/2),
		xg->XUnits, strlen(xg->XUnits));
#else
	XText(xg->win, startX, wi->XOppY - (axisFont->height/2),
		xg->XUnits, strlen(xg->XUnits), axisFont->id, normPixel, bgPixel);
#endif
	if ((expX != 0) || (xg->flags & XGLogXFlag)) {
#ifdef X11
		XDrawImageString(xg->display,xg->win,normGC, 
			startX + (strlen(xg->XUnits) * XFONTWIDTH(axisFont)),
			wi->XOppY - (XFONTHEIGHT(axisFont)/2),
			" x 10", 5);
		XDrawImageString(xg->display,xg->win,normGC, 
			wi->width - PADDING - (strlen(power) * XFONTWIDTH(axisFont)),
			wi->XOppY - XFONTHEIGHT(axisFont),
			power, strlen(power));
#else
		XText(xg->win, startX + (strlen(xg->XUnits) * axisFont->width),
			wi->XOppY - (axisFont->height/2),
			" x 10", 5, axisFont->id, normPixel, bgPixel);
		XText(xg->win, wi->width - PADDING - (strlen(power) * axisFont->width),
			wi->XOppY - axisFont->height,
			power, strlen(power), axisFont->id, normPixel, bgPixel);
#endif
	} /* if... */

	/*
	 * Now,  we can figure out the grid line labels and grid lines
	 */
	Yincr = (SPACE + XFONTHEIGHT(axisFont)) * wi->YUnitsPerPixel;
	Yincr = RoundUp(Yincr);
	Ystart = (floor(wi->UsrOrgY / Yincr) + 1.0) * Yincr;
	for (Yindex = Ystart;  Yindex < wi->UsrOppY;  Yindex += Yincr) {
#ifdef X11
		Yspot = SCREENY(wi, Yindex);
#else
		Yspot = SCREENY(wi, Yindex) + wi->XOrgY;
#endif
		/* Write the axis label */
		xgiWriteValue(value, Yindex, expY);
#ifdef X11
		XDrawImageString(xg->display,xg->win,normGC, PADDING,
			Yspot + (XFONTHEIGHT(axisFont)/2), value, strlen(value));
#else
		XText(xg->win, PADDING, Yspot - (axisFont->height/2),
			value, strlen(value), axisFont->id, normPixel, bgPixel);
#endif
		/* Draw the grid line or tick mark */
		if (xg->flags & XGGridFlag) {
#ifdef X11
			segments[0].x1 = wi->XOrgX;
			segments[0].x2 = wi->XOppX;
			segments[0].y1 = segments[0].y2 = Yspot;
			numSegments = 1;
#else
			line[0].x = wi->XOrgX;  
			line[1].x = wi->XOppX;
			line[0].y = line[1].y = Yspot;
			line[0].flags = line[1].flags = 0;
			numPoints = 2;
#endif
		} else {
#ifdef X11
			segments[0].x1 = wi->XOrgX;
			segments[0].x2 = wi->XOrgX + TICKLENGTH;
			segments[1].x1 = wi->XOppX - TICKLENGTH;
			segments[1].x2 = wi->XOppX;
			segments[0].y1 = segments[0].y2 = segments[1].y1 = segments[1].y2 = Yspot;
			numSegments = 2;
#else
			line[0].x = wi->XOrgX;  
			line[1].x = wi->XOrgX + TICKLENGTH;
			line[2].x = wi->XOppX - TICKLENGTH;  
			line[3].x = wi->XOppX;
			line[0].y = line[1].y = line[2].y = line[3].y = Yspot;
			line[0].flags = line[1].flags = line[3].flags = 0;
			line[2].flags = VertexDontDraw;
			numPoints = 4;
#endif
		} /* if... */
		if (Yindex == 0.0) {
#ifdef X11
			XDrawSegments(xg->display,xg->win,zeroGC,segments,numSegments);
#else
			XDrawDashed(xg->win, line, numPoints, zeroSize, zeroSize,
				zeroPixel, zeroStyle, GXcopy, AllPlanes);
#endif
		} else {
#ifdef X11
			if (numSegments == 1) {
				XSetLineAttributes(xg->display,normGC, 1, LineOnOffDash,
					CapNotLast,JoinMiter);
			} else {
				XSetLineAttributes(xg->display,normGC, 1, LineSolid,
					CapNotLast,JoinMiter);
			} /* if... */
			XDrawSegments(xg->display,xg->win,normGC,segments,numSegments);
#else
			XDrawDashed(xg->win, line, numPoints, normSize, normSize,
				normPixel, normStyle, GXcopy, AllPlanes);
#endif
		} /* if... */
	} /* for... */

	Xincr = (SPACE + (XFONTWIDTH(axisFont) * 7)) * wi->XUnitsPerPixel;
	Xincr = RoundUp(Xincr);
	Xstart = (floor(wi->UsrOrgX / Xincr) + 1.0) * Xincr;
	for (Xindex = Xstart;  Xindex < wi->UsrOppX;  Xindex += Xincr) {
#ifdef X11
		Xspot = SCREENX(wi, Xindex);
#else
		Xspot = SCREENX(wi, Xindex) + wi->XOrgX;
#endif
		/* Write the axis label */
		if (xg->flags & XGHHMMSSFlag) {
			xgiIntegerTohhmmss((int) Xindex, value);
		} else {
			xgiWriteValue(value, Xindex, expX);
		} /* if... */
#ifdef X11
		XDrawImageString(xg->display,xg->win,normGC, 
			Xspot - (XFONTWIDTH(axisFont)*7)/2, wi->height-PADDING,
			value, strlen(value));
#else
		XText(xg->win, Xspot - (axisFont->width*7)/2,
			wi->height-PADDING-axisFont->height,
			value, strlen(value), axisFont->id, normPixel, bgPixel);
#endif
		/* Draw the grid line or tick marks */
		if (xg->flags & XGGridFlag) {
#ifdef X11
			segments[0].x1 = segments[0].x2 = Xspot;
			segments[0].y1 = wi->XOrgY;
			segments[0].y2 = wi->XOppY;
			numSegments = 1;
#else
			line[0].x = line[1].x = Xspot;
			line[0].y = wi->XOrgY; 
			line[1].y = wi->XOppY;
			line[0].flags = line[1].flags = 0;
			numPoints = 2;
#endif
		} else {
#ifdef X11
			segments[0].x1 = segments[0].x2 = segments[1].x1 = segments[1].x2 = Xspot;
			segments[0].y1 = wi->XOrgY;
			segments[0].y2 = wi->XOrgY + TICKLENGTH;
			segments[1].y1 = wi->XOppY - TICKLENGTH;
			segments[1].y2 = wi->XOppY;
			numSegments = 2;
#else
			line[0].x = line[1].x = line[2].x = line[3].x = Xspot;
			line[0].y = wi->XOrgY;  
			line[1].y = wi->XOrgY + TICKLENGTH;
			line[2].y = wi->XOppY - TICKLENGTH;  
			line[3].y = wi->XOppY;
			numPoints = 4;
#endif
		} /* if... */
		if (Xindex == 0.0) {
#ifdef X11
			XDrawSegments(xg->display,xg->win,zeroGC,segments,numSegments);
#else
			XDrawDashed(xg->win, line, numPoints, zeroSize, zeroSize,
				zeroPixel, zeroStyle, GXcopy, AllPlanes);
#endif
		} else {
#ifdef X11
			if (numSegments == 1) {
				XSetLineAttributes(xg->display,normGC, 1, LineOnOffDash,
					CapNotLast,JoinMiter);
			} else {
				XSetLineAttributes(xg->display,normGC, 1, LineSolid,
					CapNotLast,JoinMiter);
			} /* if... */
			XDrawSegments(xg->display,xg->win,normGC,segments,numSegments);
#else
			XDrawDashed(xg->win, line, numPoints, normSize, normSize,
				normPixel, normStyle, GXcopy, AllPlanes);
#endif
		} /* if... */
	} /* for... */
	/* Check to see if he wants a bounding box */
#ifdef X11
	XSetLineAttributes(xg->display,normGC, 1, LineSolid,
		CapNotLast,JoinMiter);
#endif
	if (xg->flags & XGBoundingBoxFlag) {
#ifdef X11
		XDrawRectangle(xg->display,xg->win,normGC,wi->XOrgX,wi->XOrgY,
			wi->XOppX - wi->XOrgX, wi->XOppY - wi->XOrgY);
#else
		Vertex bb[5];

		/* Draw bounding box */
		bb[0].x = bb[3].x = bb[4].x = wi->XOrgX;
		bb[0].y = bb[1].y = bb[4].y = wi->XOrgY;
		bb[1].x = bb[2].x = wi->XOppX;
		bb[2].y = bb[3].y = wi->XOppY;
		bb[0].flags = bb[1].flags = bb[2].flags = bb[3].flags = bb[4].flags = 0;
		XDraw(xg->win, bb, 5, normSize, normSize, normPixel, GXcopy, AllPlanes);
#endif
	} /* if... */
} /* xgiDrawGridAndAxis... */

/*********************************************************************
 * XGIWRITEVALUE
 * Writes the value provided into 'str' in a fixed format
 * consisting of seven characters.  The format is: -ddd.dd
 *
 * Parameters:
 *	str (char *) - the string. (Output)
 *	val (double) - value to print. (Input)
 *	exp (double) - the exponent of the value. (Input)
 */
static void xgiWriteValue(str, val, exp)
	char	*str;			/* String to write into */
	double	val;			/* Value to print       */
	int		exp;			/* Exponent             */
{
	int		index;

	if (exp < 0) {
		for (index = exp;  index < 0;  index++) {
			val *= 10.0;
		} /* for... */
	} else {
		for (index = 0;  index < exp;  index++) {
			val /= 10.0;
		}
	} /* if... */
	(void) sprintf(str, "%7.2f", val);
} /* xgiWriteValue... */

/*************************************************************************
 * COMPUTECUBICHERMITE
 * Compute coefficients for uniform cubic hermite equations.
 *
 * Parameters:
 *	point1, points2 (XPOINT) - the end points of the curve. (Input)
 *	derviative1,derivative2 (XPOINT) - the derivative at the end points. (Input)
 *
 * Return Value:
 *	Coefficients for the hermite interpolation parametrics equations. (HERMITE)
 */
static HERMITE ComputeCubicHermite (point1,point2,derivative1,derivative2)
	XPOINT	point1,point2;
	XPOINT	derivative1,derivative2;
{
	HERMITE	coeff;

	coeff.x.a = point1.x;
	coeff.x.b = derivative1.x;
	coeff.x.c = 3*(point2.x-point1.x)-2*derivative1.x-derivative2.x;
	coeff.x.d = 2*(point1.x-point2.x)+derivative1.x+derivative2.x;
	coeff.y.a = point1.y;
	coeff.y.b = derivative1.y;
	coeff.y.c = 3*(point2.y-point1.y)-2*derivative1.y-derivative2.y;
	coeff.y.d = 2*(point1.y-point2.y)+derivative1.y+derivative2.y;
	return(coeff);
} /* ComputeCubicHermite... */

/*************************************************************************
 * DRAWHERMITESEGMENT
 * Generate the points list for a hermite segment.
 *
 * Parameters:
 *	coeff (HERMITE) - coefficients to the parametrics equations. (Input)
 *	points (XPOINT *) - list of points. (Output)
 *	w (int16 *) - the number of points generated. (Output)
 */
static void DrawHermiteSegment(coeff,points,w)
	HERMITE	coeff;
	XPOINT	*points;
	int16	*w;
{
	double		u;

	for (u=0.0,*w=0;u<=1.0;u+=0.125,(*w)++) {
		points[*w].x = 
			(short) (coeff.x.a + coeff.x.b*u + coeff.x.c*u*u + coeff.x.d*u*u*u);
		points[*w].y = 
			(short) (coeff.y.a + coeff.y.b*u + coeff.y.c*u*u + coeff.y.d*u*u*u);
#ifndef X11
		points[*w].flags = 0x0;
#endif
	} /* for... */
} /* DrawHermiteSegment... */

/*************************************************************************
 * COMPUTECUBICBSPLINE
 * Compute a uniform cubic b-spline point.
 *
 * Paramters:
 *	p0,p1,p2,p3 (XPOINT) - b-spine reference points. (Input)
 *	u (double) - parametrics index. (Input)
 *
 * Return Value:
 *	The point. (XPOINT)
 */
static XPOINT ComputeCubicBSpline (p0,p1,p2,p3,u)
	XPOINT	p0,p1,p2,p3;
	double	u;
{
	static double	f = 0.166666666666;
	double			b0,b1,b2,b3;
	XPOINT			result;

	b0 = f * u * u * u;
	b1 = f * (1 + 3 * u + 3 * u * u - 3 * u * u * u);
	b2 = f * (4 - 6 * u * u + 3 * u * u * u);
	b3 = f * (1 - 3 * u + 3 * u * u - u * u * u);
	result.x = (short) (b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x);
	result.y = (short) (b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y);
#ifndef X11
	result.flags = 0x0;
#endif
	return(result);
} /* ComputeCubicBSpline... */

/************************************************************************
 * DRAWBSPLINESEGMENT
 * Generate a list of points for uniform cubic b-spline segment.
 *
 * Parameters:
 *	p0,p1,p2,p3 (XPOINT) - b-spline reference points. (Input)
 *	points (XPOINT *) - the points. (Output)
 *	w (int16 *) - the number of points. (Output)
 */
static void DrawBSplineSegment (p0,p1,p2,p3,points,w)
	XPOINT		p0,p1,p2,p3;
	XPOINT		*points;
	int16		*w;
{
	double		u;

	for (u=0.0,*w=0;u<=1.0;u+=0.125,(*w)++) {
		points[*w] = ComputeCubicBSpline(p0,p1,p2,p3,u);
	} /* for... */
} /* DrawBSplineSegment... */

/************************************************************************
 * CLIPCHECK
 * Clip graph to window.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *	wi (LocalWin *) - window information. (Input)
 *	Xx,Xy (int16) - coordinates of new point. (Input)
 *	areaWidth, areaHeight (int16) - size of window. (Input)
 *	set (int16) - the plot set index. (Input)
 *	subindex (int16 *) - index in the set. (Input/Output)
 *	pc (int16 *) - index in the plot array. (Input/Output)
 *	clip (int16 *) - are we clipping?. (Input/Output)
 *
 * Return Value:
 *	TRUE, point clipped.
 *	FALSE, point not clipped.
 */
static int16 ClipCheck (xg,wi,Xx,Xy,areaWidth,areaHeight,set,subindex,pc,clip)
	XG			*xg;
	LocalWin	*wi;
	int16		Xx,Xy;
	int16		areaWidth,areaHeight;
	int16		set;
	int16		*subindex;
	int16		*pc;
	int16		*clip;
{
	if (*subindex < xg->dataSet[set].numPoints-1) {
		int16		x,y;
		int			i;

		x = SCREENX(wi, xg->dataSet[set].xvec[*subindex+1]);
		y = SCREENY(wi, xg->dataSet[set].yvec[*subindex+1]);

		if (x < 0 || x > areaWidth || y < 0 || y > areaHeight) {
			if ((x < 0 && Xx < 0) || 
				(x > areaWidth && Xx > areaWidth) ||
				(y < 0 && Xy < 0) ||
				(y > areaHeight || Xy > areaHeight)) {
				if (xg->flags & (XGBSplineFlag|XGHermiteFlag)) {
					if (*subindex < xg->dataSet[set].numPoints-2) {
						x=SCREENX(wi,xg->dataSet[set].xvec[*subindex+2]);
						y=SCREENY(wi,xg->dataSet[set].yvec[*subindex+2]);
						if (x<0 || x>areaWidth || y<0 || y>areaHeight) {
							if (*subindex < xg->dataSet[set].numPoints-3) {
								x=SCREENX(wi,xg->dataSet[set].xvec[*subindex+3]);
								y=SCREENY(wi,xg->dataSet[set].yvec[*subindex+3]);
								if (x<0 || x>areaWidth || y<0 || y>areaHeight) {
									return(TRUE);
								} else {
									i = 3;
								} /* if... */
							} else {
								return(TRUE);
							} /* if... */
						} else {
							i = 2;
						} /* if... */
					} else {
						return(TRUE);
					} /* if... */
	
					while (i > 0) {
						xg->Xpoints[*pc].x = 
							SCREENX(wi, xg->dataSet[set].xvec[*subindex]);
						xg->Xpoints[(*pc)++].y = 
							SCREENY(wi, xg->dataSet[set].yvec[(*subindex)++]);
						i--;
					} /* while... */
				} /* if... */
				return(TRUE);
			} else {
				*clip = FALSE;
			} /* if... */
		} else {
			*clip = FALSE;
		} /* if... */
	} else {
		return(TRUE);
	} /* if... */
	return(FALSE);
} /* ClipCheck... */

/************************************************************************
 * CLIPPER
 * Clip graph to window.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *	wi (LocalWin *) - window information. (Input)
 *	Xx,Xy (int16) - coordinates of new point. (Input)
 *	areaWidth, areaHeight (int16) - size of window. (Input)
 *	set (int16) - the plot set index. (Input)
 *	subindex (int16 *) - index in the set. (Input/Output)
 *	pc (int16 *) - index in the plot array. (Input/Output)
 *	clip (int16 *) - are we clipping?. (Input/Output)
 *
 * Return Value:
 *	TRUE, point clipped.
 *	FALSE, point not clipped.
 */
static int16 Clipper (xg,wi,Xx,Xy,areaWidth,areaHeight,set,subindex,pc,clip)
	XG			*xg;
	LocalWin	*wi;
	int16		Xx,Xy;
	int16		areaWidth,areaHeight;
	int16		set;
	int16		*subindex;
	int16		*pc;
	int16		*clip;
{
	/* Is the point outside the screen or not ? */
	/* Use locality of data to speed up clipping */
	if (Xx < 0 || Xx > areaWidth || Xy < 0 || Xy > areaHeight) {	
		/*** Point is out. Draw the first, clip the others ***/
		if (*clip) {
			return(ClipCheck(xg,wi,Xx,Xy,areaWidth,areaHeight,set,subindex,pc,clip));
		} else {
			*clip = TRUE;
			if (*subindex == 0) {
				return(ClipCheck(xg,wi,Xx,Xy,areaWidth,areaHeight,set,subindex,pc,clip));
			} /* if... */
		}  /* if... */
	} else {
		*clip = FALSE;		/* stop clipping */
	} /* if... */
	return(FALSE);
} /* Clipper... */

/**************************************************************************
 * XGIDRAWDATA
 * This routine draws the data sets themselves using the macros
 * for translating coordinates.
 *
 * Parameters:
 *	xg (XG *) - the xg structure. (Input)
 */
static void xgiDrawData(xg)
	XG			*xg;
{
	LocalWin	*wi = xg->win_info;
	int16		set, subindex;
	int16		pc;						/* points counter */
	int16		Xx,Xy;					/* X coordinates for a point */
	int16		w;
	XPOINT		points[10];
	XPOINT		slope1,slope2;
#ifdef X11
	int			areaWidth = wi->XOppX + 1;
	int			areaHeight = wi->XOppY + 1;
#else
	int			areaWidth = wi->XOppX - wi->XOrgX + 1;
	int			areaHeight = wi->XOppY - wi->XOrgY + 1;
#endif
	int16		clip = FALSE;			/* Flag, to help manual clipping */
#ifdef X11
	XRectangle	rectangle;
#endif

#ifdef X11
	rectangle.width = wi->XOppX - wi->XOrgX + 1;
	rectangle.height = wi->XOppY - wi->XOrgY + 1;
	rectangle.x = wi->XOrgX;
	rectangle.y = wi->XOrgY;
	XSetClipRectangles(xg->display,xgGC,0,0,&rectangle,1,Unsorted);
#else
	area = XCreateTransparency(xg->win,wi->XOrgX, wi->XOrgY,areaWidth, areaHeight);
	XMAPWINDOW(xg->display,area);
#endif
	if (xg->allocXpoints > 500) {
#ifdef X11
		XDEFINECURSOR(xg->display,xg->win,waitCursor); 
#else
		XDEFINECURSOR(xg->display,area,waitCursor); 
#endif
		XFLUSH(xg->display);
	} /* if... */
	for (set = 0 ; set < xg->numPlots ; set++) {
		clip = FALSE;
		pc = 0;
#ifdef X11
		XSetForeground(xg->display,xgGC,AllAttrs[set].pixelValue);
		XSetLineAttributes(xg->display,xgGC, lineWidth, 
			AllAttrs[set].lineStyle, CapNotLast,JoinMiter);
		if (AllAttrs[set].lineStyle == LineOnOffDash) {
			XSetDashes (xg->display,xgGC,0,AllAttrs[set].dashList, XGI_DASHLEN);
		} /* if... */
#endif
		for (subindex = 0; subindex < xg->dataSet[set].numPoints; subindex++) {
			Xx = SCREENX(wi, xg->dataSet[set].xvec[subindex]);
			Xy = SCREENY(wi, xg->dataSet[set].yvec[subindex]);

			if (Clipper(xg,wi,Xx,Xy,areaWidth,areaHeight,set,&subindex,&pc,&clip)) {
				continue;
			} /* if... */

			xgiShowPoint (xg,set,subindex,AllAttrs[set].pixelValue);
      
			/* Prepare array for line drawing*/
			xg->Xpoints[pc].x = Xx;
			xg->Xpoints[pc].y = Xy;
			if ((xg->flags & XGBSplineFlag) && (pc > 0)) {
				DrawBSplineSegment(xg->Xpoints[((pc-3)<0) ? 0 : pc-3],
					xg->Xpoints[((pc-2)<0) ? 0 : pc-2],
					xg->Xpoints[pc-1],xg->Xpoints[pc],points,&w);
#ifdef X11
				XDrawLines(xg->display,xg->win,xgGC,points,w,CoordModeOrigin);
#else
				XDrawDashed(area, points, w, lineWidth, lineWidth, 
					AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
					GXcopy, AllPlanes);
#endif
			} /* if... */
			if ((xg->flags & XGHermiteFlag) && (pc > 1)) {
				if (pc == 2) {
					slope1.x = slope1.y = 0;
				} else {
					slope1.x = (xg->Xpoints[pc-1].x - xg->Xpoints[pc-3].x) / 2;
					slope1.y = (xg->Xpoints[pc-1].y - xg->Xpoints[pc-3].y) / 2;
				} /* if... */
				slope2.x = (xg->Xpoints[pc].x - xg->Xpoints[pc-2].x) / 2;
				slope2.y = (xg->Xpoints[pc].y - xg->Xpoints[pc-2].y) / 2;
				DrawHermiteSegment(ComputeCubicHermite(xg->Xpoints[pc-2],
					xg->Xpoints[pc-1],slope1,slope2),points,&w);
#ifdef X11
				XDrawLines(xg->display,xg->win,xgGC,points,w,CoordModeOrigin);
#else
				XDrawDashed(area, points, w, lineWidth, lineWidth, 
					AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
					GXcopy, AllPlanes);
#endif
			} /* if... */
#ifndef X11
			if ((xg->flags & XGBSplineFlag) && (pc > 0)) { 
				xg->Xpoints[pc].flags |= VertexCurved;
			} /* if... */
#endif
			pc++;
			if ((clip) && (pc > 0) && (xg->flags & XGLinesFlag)) {
#ifdef X11
				XDrawLines(xg->display,xg->win,xgGC,xg->Xpoints,pc,CoordModeOrigin);
#else
				xg->Xpoints[pc-1].flags = VertexDrawLastPoint;
				XDrawDashed(area, xg->Xpoints, pc, lineWidth, lineWidth,
					AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
					GXcopy, AllPlanes);
#endif
				pc = 0;
			} /* if... */
		} /* for... */
		if (xg->flags & XGBSplineFlag) {
			DrawBSplineSegment(xg->Xpoints[pc-2],xg->Xpoints[pc-1],
				xg->Xpoints[pc-1],xg->Xpoints[pc-1],points,&w);
#ifdef X11
			XDrawLines(xg->display,xg->win,xgGC,points,w,CoordModeOrigin);
#else
			XDrawDashed(area, points, w, lineWidth, lineWidth, 
				AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
				GXcopy, AllPlanes);
#endif
			DrawBSplineSegment(xg->Xpoints[pc-3],xg->Xpoints[pc-2],
				xg->Xpoints[pc-1],xg->Xpoints[pc-1],points,&w);
#ifdef X11
			XDrawLines(xg->display,xg->win,xgGC,points,w,CoordModeOrigin);
#else
			XDrawDashed(area, points, w, lineWidth, lineWidth, 
				AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
				GXcopy, AllPlanes);
#endif
		} /* if... */
		if (xg->flags & XGHermiteFlag) {
			slope1.x = (xg->Xpoints[pc-1].x - xg->Xpoints[pc-3].x) / 2;
			slope1.y = (xg->Xpoints[pc-1].y - xg->Xpoints[pc-3].y) / 2;
			slope2.x = slope2.y = 0;
			DrawHermiteSegment(ComputeCubicHermite(xg->Xpoints[pc-2],
				xg->Xpoints[pc-1],slope1,slope2),points,&w);
#ifdef X11
			XDrawLines(xg->display,xg->win,xgGC,points,w,CoordModeOrigin);
#else
			XDrawDashed(area, points, w, lineWidth, lineWidth, 
				AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
				GXcopy, AllPlanes);
#endif
		} /* if... */
		if (pc > 0 && (xg->flags & XGLinesFlag)) {
#ifdef X11
			XDrawLines(xg->display,xg->win,xgGC,xg->Xpoints,pc,CoordModeOrigin);
#else
			xg->Xpoints[pc-1].flags = VertexDrawLastPoint;
			XDrawDashed(area, xg->Xpoints, pc, lineWidth, lineWidth, 
				AllAttrs[set].pixelValue, AllAttrs[set].lineStyle,
				GXcopy, AllPlanes);
#endif
		} /* if... */
	} /* for... */
#ifdef X11
	XDEFINECURSOR(xg->display,xg->win,theCursor); 
	XSetClipMask(xg->display,xgGC,None);
#else
	XDESTROYWINDOW(xg->display,area);
#endif
} /* xgiDrawData... */

#define REDRAWN_POINTS 3

/*********************************************************************
 * XGIDRAWPOINT
 * This routine draws only the point indexed by spot, using the macros
 * for translating coordinates.
 *
 * Parameters:
 *	xg (XG *) - the xg structure. (Input)
 *	spot (int) - index of the point to draw. (Input)
 *	set (int) - index of the set to select point from. (Input)
 */
static void xgiDrawPoint(xg , spot, set)
	XG			*xg;
	int			spot;
	int			set;
{
	LocalWin	*wi  = xg->win_info;
	int			subindex;
	int			i;
	int			points;
	double		tmp;
#ifdef X11
	XRectangle	rectangle;
#endif

	/* Check that the point will not cause overflow */
	while ((tmp = (xg->dataSet[set].xvec[spot] - wi->UsrOrgX)/wi->XUnitsPerPixel),
		ABS(tmp) > 1E9) {
/*		(void) printf("xg: data point too big (X)\n"); */
		wi->XUnitsPerPixel *= 2.0;
	} /* while... */
  
	while ((tmp = (xg->dataSet[set].yvec[spot] - wi->UsrOrgY)/wi->YUnitsPerPixel),
		ABS(tmp) > 1E9) {
/*		(void) printf("xg: data point too big (Y)\n"); */
		wi->YUnitsPerPixel *= 2.0;
	} /* while... */

#ifdef X11
	rectangle.width = wi->XOppX - wi->XOrgX + 1;
	rectangle.height = wi->XOppY - wi->XOrgY + 1;
	rectangle.x = wi->XOrgX;
	rectangle.y = wi->XOrgY;
	XSetClipRectangles(xg->display,xgGC,0,0,&rectangle,1,Unsorted);
#else
	area = XCreateTransparency(xg->win, wi->XOrgX, wi->XOrgY,
		wi->XOppX - wi->XOrgX + 1, wi->XOppY - wi->XOrgY + 1);
	XMAPWINDOW(xg->display,area);
#endif

	/* Draw the last three points for this set */
	points = 0;
	for (i  = 0; i < REDRAWN_POINTS ; i++) {
		subindex = spot - REDRAWN_POINTS + i + 1;
		if (subindex < 0) continue;
		xg->Xpoints[points].x = SCREENX(wi, xg->dataSet[set].xvec[subindex]);
		xg->Xpoints[points].y = SCREENY(wi, xg->dataSet[set].yvec[subindex]);
#ifndef X11
		xg->Xpoints[points].flags = (xg->flags & XGBSplineFlag) ?  VertexCurved : 0;
#endif
		points++;
	} /* for... */
#ifndef X11
	xg->Xpoints[points - 1].flags = VertexDrawLastPoint;
#endif

#ifdef X11
	XSetForeground(xg->display,xgGC,AllAttrs[set].pixelValue);
#endif
	if (xg->flags & XGLinesFlag) {
#ifdef X11
		XSetLineAttributes(xg->display,xgGC, lineWidth, AllAttrs[set].lineStyle,
			CapNotLast,JoinMiter);
		if (AllAttrs[set].lineStyle == LineOnOffDash) {
			XSetDashes (xg->display,xgGC,0,AllAttrs[set].dashList, XGI_DASHLEN);
		} /* if... */
		XDrawLines(xg->display,xg->win,xgGC,xg->Xpoints,points,CoordModeOrigin);
#else
		XDrawDashed(area, xg->Xpoints, points, lineWidth, lineWidth, 
			AllAttrs[set].pixelValue,AllAttrs[set].lineStyle,GXcopy,AllPlanes);
#endif
	} /* if... */

	xgiShowPoint (xg,set,spot,AllAttrs[set].pixelValue);

#ifdef X11
	XSetClipMask(xg->display,xgGC,None);
#else
	XDESTROYWINDOW(xg->display,area);
#endif
} /* xgiDrawPoint... */

/***********************************************************************
 * XGIDRAWLEGEND
 * This draws a legend of the data sets displayed.  Only those that
 * will fit are drawn.
 *
 * Parameters:
 *	xg (XG *) - the xg structure. (Input)
 */
static void xgiDrawLegend(xg)
	XG			*xg;
{
	LocalWin	*wi = xg->win_info;
	int			set, spot, lineLen, oneLen;
	XPOINT		line[2];

	spot = wi->XOrgY;
	lineLen = 0;
	/* First pass draws the text */
	for (set = 0; set < XG_MAXSETS; set++) {
		if ((xg->dataSet[set].numPoints > 0) && 
			(spot + XFONTHEIGHT(axisFont) + 2 < wi->XOppY)) {
			/* Meets the criteria */
			oneLen = strlen(xg->dataSet[set].setName);
			if (oneLen > lineLen) lineLen = oneLen;
#ifdef X11
			XSetFont(xg->display,normGC,axisFont->fid);
			XDrawImageString(xg->display,xg->win,normGC, 
				wi->XOppX + PADDING, spot+2 + XFONTHEIGHT(axisFont),
				xg->dataSet[set].setName, oneLen);
			spot += 2 + XFONTHEIGHT(axisFont) + PADDING;
#else
			XText(xg->win, wi->XOppX+PADDING, spot+2, xg->dataSet[set].setName,
				oneLen, axisFont->id, normPixel, bgPixel);
			spot += 2 + axisFont->height + PADDING;
#endif
		} /* if... */
	} /* for... */
	lineLen = lineLen * XFONTWIDTH(axisFont);
	line[0].x = wi->XOppX + PADDING;
	line[1].x = line[0].x + lineLen;
#ifndef X11
	line[0].flags = line[1].flags = 0;
#endif
	spot = wi->XOrgY;
	/* second pass draws the lines */
	for (set = 0;  set < XG_MAXSETS;  set++) {
		if ((xg->dataSet[set].numPoints > 0) && 
			(spot + XFONTHEIGHT(axisFont) + 2 < wi->XOppY)) {
			line[0].y = line[1].y = spot;
#ifdef X11
			XSetLineAttributes(xg->display,xgGC, 1, AllAttrs[set].lineStyle,
				CapNotLast,JoinMiter);
			if (AllAttrs[set].lineStyle == LineOnOffDash) {
				XSetDashes (xg->display,xgGC,0,AllAttrs[set].dashList, 
					XGI_DASHLEN);
			} /* if... */
			XSetForeground(xg->display,xgGC,AllAttrs[set].pixelValue);
			XDrawLines(xg->display,xg->win,xgGC,line,2,CoordModeOrigin);
#else
			XDrawDashed(xg->win, line, 2, 1, 1, AllAttrs[set].pixelValue,
				AllAttrs[set].lineStyle, GXcopy, AllPlanes);
#endif
			spot += 2 + XFONTHEIGHT(axisFont) + PADDING;
		} /* if... */
	} /* for... */
} /* xgiDrawLegend... */

/**************************************************************************
 * XGSETFLAGS
 * Set flags.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	flags (unsigned long) - the flags. (Input)
 */
void xgSetFlags (xgraph,flags)
	XGraph			xgraph;
	unsigned long	flags;
{
	((XG *) xgraph)->flags |= flags;
} /* xgSetFlags... */

/*************************************************************************
 * XGCLEARFLAGS
 * Clear flags.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	flags (unsigned long) - the flags. (Input)
 */
void xgClearFlags (xgraph,flags)
	XGraph			xgraph;
	unsigned long	flags;
{
	((XG *) xgraph)->flags &= ~flags;
} /* xgClearFlags... */

/************************************************************************
 * XGIRETRIEVEXG
 * Retrieve the 'xg' structure for the given event.
 *
 * Parameters:
 *	theEventP (XAnyEvent *) - the event. (Input)
 *	xg (XG **) - the xg structure. (Input)
 *
 * Return Value:
 *	XG_EXTRANEOUS_EVENT, not an xg event.
 *	XG_OK, success.
 */
static int16 xgiRetrieveXG (theEventP,xg)
	XAnyEvent	*theEventP;
	XG			**xg;
{
#ifdef X11
        caddr_t ptr;
	if (XFindContext(theEventP->display,theEventP->window,
		win_table,&ptr) != XCSUCCESS) {
		return(XG_EXTRANEOUS_EVENT);
	} /* if... */
        *xg = (XG *) ptr;
#else
	if ((*xg = (XG *)XLookUpAssoc(win_table,(XId)theEventP->window))==0) {
		return(XG_EXTRANEOUS_EVENT);
	} /* if... */
#endif
	return (XG_OK);
} /* xgiRetrieveXG... */

/*************************************************************************
 * XGRPCHANDLEEVENTS
 * RPC X event handler for xg.
 *
 * Parameters:
 *	theEventP (XEvent *) - the event. (Input)
 *
 * Return Value:
 *	XG_EXTRANEOUS_EVENT, not an xg event.
 *	XG_OK, success.
 */
int16 xgRPCHandleEvent(theEventP)
	XEvent	*theEventP;
{
	XG		*xg;

	suspendRedraw = FALSE;	/* Reset ability to redraw automatically */
	switch (theEventP->type) {
		case Expose:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
#ifdef X11
			if (((XExposeEvent *) theEventP)->count == 0) {
				xgRedrawGraph((XGraph) xg);
			} /* if... */
#else
			xgRedrawGraph((XGraph) xg);
#endif
			break;
#ifdef X11
		case ConfigureNotify:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
			if ((xg->win_info->width != ((XConfigureEvent *)theEventP)->width) || 
				(xg->win_info->height != ((XConfigureEvent *)theEventP)->height)) {
				XCLEARWINDOW(xg->display,xg->win);
				xg->win_info->width = ((XConfigureEvent *) theEventP)->width;
				xg->win_info->height = ((XConfigureEvent *) theEventP)->height;
				xgRedrawGraph((XGraph) xg);
			} /* if... */
#else
		case ExposeWindow:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
			XCLEARWINDOW(xg->display,xg->win);
			xg->win_info->width = ((XExposeEvent *) theEventP)->width;
			xg->win_info->height = ((XExposeEvent *) theEventP)->height;
			xgRedrawGraph((XGraph) xg);
#endif
			break;
		case KeyPress:
			break;
		case ButtonPress:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
#ifdef X11
			switch (((XButtonEvent *) theEventP)->button) {
#else
			switch (((XButtonEvent *) theEventP)->detail & 0xff) {
#endif
				case Button1:
					xgHandleZoom((XButtonEvent *) theEventP,xg,zoomCursor);
					xgRedrawGraph((XGraph) xg);
					break;
				case Button3:
					xgMeasure((XButtonEvent *) theEventP, xg, zoomCursor);
					break;
			} /* switch... */
			break;
		case ButtonRelease:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
#ifdef X11
			if (((XButtonEvent *) theEventP)->button == Button2) {
#else
			if ((((XButtonEvent *) theEventP)->detail & 0xff) == MiddleButton) {
#endif
				xgiMenuHandler(xg);
			} /* if... */
			break;
		case MotionNotify:
			break;
#ifdef X11
		case CirculateNotify:
		case CreateNotify:
		case DestroyNotify:
		case GravityNotify:
		case MapNotify:
		case ReparentNotify:
		case UnmapNotify:
			break;
#endif
		default:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
			(void) fprintf(stderr, "Unknown event type: %x\n", theEventP->type);
		break;
	} /* switch... */
	return(XG_OK);		/* no more events in the queue */
} /* xgRPCHandleEvents... */

/*************************************************************************
 * XGHANDLEEVENTS
 * X event handler
 *
 * Parameters:
 *	theEventP (XEvent *) - the event. (Input)
 *
 * Return Value:
 *	XG_EXTRANEOUS_EVENT, not an xg event.
 *	XG_OK, success.
 */
int16 xgHandleEvent(theEventP)
	XEvent	*theEventP;
{
	XG		*xg;

	suspendRedraw = FALSE;	/* Reset ability to redraw automatically */
	switch (theEventP->type) {
		case Expose:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
#ifdef X11
			if (((XExposeEvent *) theEventP)->count == 0) {
				xgRedrawGraph((XGraph) xg);
			} /* if... */
#else
			xgRedrawGraph((XGraph) xg);
#endif
			break;
#ifdef X11
		case ConfigureNotify:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
			if ((xg->win_info->width != ((XConfigureEvent *)theEventP)->width) || 
				(xg->win_info->height != ((XConfigureEvent *)theEventP)->height)) {
				XCLEARWINDOW(xg->display,xg->win);
				xg->win_info->width = ((XConfigureEvent *) theEventP)->width;
				xg->win_info->height = ((XConfigureEvent *) theEventP)->height;
				xgRedrawGraph((XGraph) xg);
			} /* if... */
#else
		case ExposeWindow:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
			XCLEARWINDOW(xg->display,xg->win);
			xg->win_info->width = ((XExposeEvent *) theEventP)->width;
			xg->win_info->height = ((XExposeEvent *) theEventP)->height;
			xgRedrawGraph((XGraph) xg);
#endif
			break;
		case KeyPress:
			break;
		case ButtonPress:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
#ifdef X11
			switch (((XButtonEvent *) theEventP)->button) {
#else
			switch (((XButtonEvent *) theEventP)->detail & 0xff) {
#endif
				case Button1:
					xgHandleZoom((XButtonEvent *) theEventP,xg,zoomCursor);
					xgRedrawGraph((XGraph) xg);
					break;
				case Button2:
					xgiMenuHandler(xg);
					break;
				case Button3:
					xgMeasure((XButtonEvent *) theEventP, xg, zoomCursor);
					break;
			} /* switch... */
			break;
		case ButtonRelease:
		case MotionNotify:
			break;
#ifdef X11
		case CirculateNotify:
		case CreateNotify:
		case DestroyNotify:
		case GravityNotify:
		case MapNotify:
		case ReparentNotify:
		case UnmapNotify:
			break;
#endif
		default:
			if (xgiRetrieveXG((XAnyEvent *) theEventP,&xg) != XG_OK) {
				return(XG_EXTRANEOUS_EVENT);
			} /* if... */
			(void) fprintf(stderr, "Unknown event type: %x\n", theEventP->type);
			break;
	} /* switch... */
	return(XG_OK);		/* no more events in the queue */
} /* xgHandleEvents... */

/*************************************************************************
 * XGPROCESSALLEVENTS
 * Easier way to handle the event queue in the case that 
 *  xg is the only user of X.
 *
 * Parameters:
 *	display (Display *) - the display. (Input)
 *
 * Return Value:
 *	XG_OK, success. all events processed.
 */
int16 xgProcessAllEvents(display)
	Display		*display;
{
	XEvent		theEvent;

	while (XPENDING(display) > 0) {
		XNEXTEVENT (display,&theEvent);
		if (xgHandleEvent(&theEvent) != XG_OK) {
			(void) fprintf(stderr,"xgProcessAllEvents: Extraneous event\n");
		} /* if... */
	} /* while... */
	return(XG_OK);
} /* xgProcessAllEvents... */

/*************************************************************************
 * XGRPCPROCESSALLEVENTS
 * Easier way to handle the event queue in the case that 
 *  xg is the only user of X
 *
 * Parameters:
 *	number (int) -
 *	read,write,except (int *) -
 */
void xgRPCProcessAllEvents(number,read,write,except)
	int		number;
	int		*read,*write,*except;
{
	XEvent theEvent;

	while (XPENDING(xgDisplay) > 0) {
		XNEXTEVENT (xgDisplay,&theEvent);
		if (xgRPCHandleEvent(&theEvent) != XG_OK) {
			(void) fprintf(stderr,"xgRPCProcessAllEvents: Extraneous event\n");
		} /* if... */
	} /* while... */
} /* xgRPCProcessAllEvents... */

/*************************************************************************
 * XGSENDPOINT
 * Send a point to the specified graph.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	x,y (double) - the point to plot. (Input)
 *	setName (char *) - the name of the set in the graph to send
 *		the point to. (Input)
 *
 * Return Value:
 *	XG_ERROR, unable to plot point.
 *	XG_OK, success.
 */
int16 xgSendPoint(xgraph, x, y , setName)
	XGraph	xgraph;
	double	x;
	double	y;
	char	*setName;
{
	XG		*xg = (XG *) xgraph;
	int16	spot;			/* index in the array of points */
	int16	changedBB;
	int16	set;
	int16	newSetFlag;

	if ((newSetFlag = xgiFindSet(xg,setName, &set)) == XGI_ERROR) {
		return(XG_ERROR);
	} /* if... */
	(void) xgiAllocManager(xg, set, xg->dataSet[set].numPoints + 1);
  
	spot = xg->dataSet[set].numPoints++;
	xg->dataSet[set].xvec[spot] = x;
	xg->dataSet[set].yvec[spot] = y;

	if (xg->flags & XGLogXFlag) {
		if (xg->dataSet[set].xvec[spot] <= 0.0) {
			(void) fprintf(stderr, "Negative X value in logarithmic mode\n");
			return(XG_ERROR);
		} /* if... */
		xg->dataSet[set].xvec[spot] =  log10(xg->dataSet[set].xvec[spot]);
	} /* if... */
	if (xg->flags & XGLogYFlag) {
		if (xg->dataSet[set].yvec[spot] <= 0.0) {
			(void) fprintf(stderr, "Negative Y value in logarithmic mode\n");
			return(XG_ERROR);
		} /* if... */
		xg->dataSet[set].yvec[spot] = log10(xg->dataSet[set].yvec[spot]);
	} /* if... */

	xgiDrawPoint(xg,spot,set);

	/* Update bounding box */
	changedBB = FALSE;
	if (xg->dataSet[set].xvec[spot] < xg->win_info->loX) {
		changedBB = TRUE;
		xg->win_info->loX = xg->dataSet[set].xvec[spot];
	} /* if... */
	if (xg->dataSet[set].xvec[spot] > xg->win_info->hiX) {
		changedBB = TRUE;
		xg->win_info->hiX = xg->dataSet[set].xvec[spot];
	} /* if... */
	if (xg->dataSet[set].yvec[spot] < xg->win_info->loY) {
		changedBB = TRUE;
		xg->win_info->loY = xg->dataSet[set].yvec[spot];
	} /* if... */
	if (xg->dataSet[set].yvec[spot] > xg->win_info->hiY) {
		changedBB = TRUE;
		xg->win_info->hiY = xg->dataSet[set].yvec[spot];
	} /* if... */
	if (((newSetFlag == XGI_NEWSET) || (changedBB)) && (!suspendRedraw)) {
		xgRedrawGraph((XGraph) xg);
	} /* if... */
	return(XG_OK);
} /* xgSendPoint... */

/************************************************************************
 * XGSENDARRAY
 * Send an array of points to the specified graph.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	n (int16) - the number of points being sent. (Input)
 *	Xarray, Yarray (double *) - the array of points. (Input)
 *	setName (char *) - the name of the set in the graph to
 *		send points to. (Input)
 *
 * Return Value:
 *	XG_ERROR, unable to plot points.
 *	XG_OK, success.
 */
int16 xgSendArray(xgraph, n,  Xarray , Yarray , setName)
	XGraph	xgraph;
	int16	n;
	double	*Xarray;
	double	*Yarray;
	char	*setName;
{
	XG		*xg = (XG *) xgraph;
	int16	spot;			/* index in the array of points */
	int16	i;
	int16	set;
	int16	newArraySize;
	double	x,y;
	double	minX , maxX, minY , maxY;

	(void) xgiFindSet(xg,setName, &set);
	newArraySize = xg->dataSet[set].numPoints + n;
	(void) xgiAllocManager(xg, set, newArraySize);

	minX = xg->win_info->loX;
	maxX = xg->win_info->hiX;
	minY = xg->win_info->loY;
	maxY = xg->win_info->hiY;
  
	for (i=0,spot=xg->dataSet[set].numPoints;spot<newArraySize;i++,spot++) {
		xg->dataSet[set].xvec[spot] = x = Xarray[i];
		xg->dataSet[set].yvec[spot] = y = Yarray[i];
		if (xg->flags & XGLogXFlag) {
			if (x <= 0.0) {
				(void) fprintf(stderr, "Negative X value in logarithmic mode\n");
				return(XG_ERROR);
			} /* if... */
			xg->dataSet[set].xvec[spot] =  log10(x);
		} /* if... */
		if (xg->flags & XGLogYFlag) {
			if (y <= 0.0) {
				(void) fprintf(stderr, "Negative Y value in logarithmic mode\n");
				return(XG_ERROR);
			} /* if... */
			xg->dataSet[set].yvec[spot] = log10(y);
		} /* if... */
		if (x > maxX) maxX = x;
		if (y > maxY) maxY = y;
		if (x < minX) minX = x;
		if (y < minY) minY = y;
	} /* for... */
	xg->dataSet[set].numPoints += n;

	/* Update bounding box */
	if (minX < xg->win_info->loX) {
		xg->win_info->loX = minX;
	} /* if... */
	if (maxX > xg->win_info->hiX) {
		xg->win_info->hiX = maxX;
	} /* if... */
	if (minY < xg->win_info->loY) {
		xg->win_info->loY = minY;
	} /* if... */
	if (maxY > xg->win_info->hiY) {
		xg->win_info->hiY = maxY;
	} /* if... */

	if (!suspendRedraw) xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* xgSendArray... */

/************************************************************************
 * XGREDRAWGRAPH
 * Draws the data in the window. clear the window.
 * The data is scaled so that all of the data will fit.
 * Grid lines are drawn at the nearest power of 10 in engineering
 * notation.  Draws axis numbers along bottom and left hand edges.
 * Centers title at top of window.
 *
 * Parameters:
 *	xgraph (XGraph *) - the graph identifier. (Input)
 */
void xgRedrawGraph(xgraph)
	XGraph	xgraph;
{
	XG		*xg = (XG *) xgraph;

	suspendRedraw = FALSE;		/* Reset ability to redraw automatically */
	XCLEARWINDOW(xg->display,xg->win);
	/* Figure out the transformation constants */
	if (xgiTransformCompute(xg)) {
		xgiDrawTitle(xg);		/* Draw the title */
		xgiDrawGridAndAxis(xg);	/* Draw the axis unit labels, grid lines, 
									grid labels */
		xgiDrawData(xg);		/* Draw the data sets themselves */
		xgiDrawLegend(xg);		/* Draw the legend */
		XFLUSH(xg->display);
	} /* if... */
} /* xgRedrawGraph... */
