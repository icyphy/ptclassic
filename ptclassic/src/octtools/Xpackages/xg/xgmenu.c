#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/***********************************************************************
 * Handle menus for XG library
 * Filename: xgmenu.c
 * Program: XG - Plot library
 * Environment: Oct/Vem
 * Date: March, 1989
 * Professor: Richard Newton
 * Oct Tools Manager: Rick Spickelmier
 * XGraph: David Harrison
 * Organization:  University Of California, Berkeley
 *                Electronic Research Laboratory
 *                Berkeley, CA 94720
 * Routines:
 * Modifications:
 *	Andrea Casotto				Created from XGraph			1988
 *	Rodney Lai					Ported to X11				1989
 * Note: File is formatted with 4 space tab.
 */
#include "copyright.h"
#include "port.h"
#define MENU_XG
#include "xglocal.h"

#ifdef X11
static xpa_menu	xgMenu;
#else
static XMenu	*xgMenu;

struct menuEntry {
	XG_PFI	func;		/* function to be called when this entry is selected */
	char	*pane;		/* pane in which the selection must appear */
	char	*selection;	/* selection label */
	int		activeFlag;	/*  */
}; /* menuEntry... */
#endif

/* The prefix XGM_ stands for XGraph Menu */
int XGM_showAll();
int XGM_zoomOut();
int XGM_grid();
int XGM_bbox();
int XGM_clear();
int XGM_save();
int XGM_logX();
int XGM_logY();
int XGM_lines();
int XGM_spline();
int XGM_hermite();
int XGM_mark();
int XGM_pixel();
int XGM_bar();
int XGM_hhmmss();

#ifdef X11
static xpa_entry displayEntry[] = {
	{"show all",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"zoom out",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"grid",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"BBox",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{NIL(char),0,NIL(char),NIL(xpa_entry)}
};
static xpa_entry flagEntry[] = {
	{"log X",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"log Y",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"lines",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"b-spline",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"interpolate",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"big mark",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"small mark",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"hhmmss",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"bar",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{NIL(char),0,NIL(char),NIL(xpa_entry)}
};
static xpa_entry miscEntry[] = {
	{"clear",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{"save plot",XPA_NOCHAR,NIL(char),NIL(xpa_entry)},
	{NIL(char),0,NIL(char),NIL(xpa_entry)}
};
static xpa_entry entrys[] = {
	{"Display",XPA_NOCHAR,"Display",displayEntry},
	{"Flags",XPA_NOCHAR,"Flags",flagEntry},
	{"Misc",XPA_NOCHAR,"Misc",miscEntry},
	{NIL(char),0,NIL(char),NIL(xpa_entry)}
};
static XG_PFI xpaFunc[3][9] = {
	{ XGM_showAll,XGM_zoomOut,XGM_grid,XGM_bbox,NULL,NULL,NULL,NULL,NULL},
	{ XGM_logX,XGM_logY,XGM_lines,XGM_spline,XGM_hermite,XGM_mark,XGM_pixel,
	  XGM_hhmmss,XGM_bar },
	{ XGM_clear,XGM_save,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
};
#else
static struct menuEntry CommandArray[] = {
	{ XGM_showAll,	"DISPLAY",	"show all",		1},
	{ XGM_zoomOut,	"DISPLAY",	"zoom out",		1},
	{ XGM_grid,		"DISPLAY",	"grid",			1},
	{ XGM_bbox,		"DISPLAY",	"BBox",			1},
	{ XGM_logX,		"FLAGS",	"log X",		0},
	{ XGM_logY,		"FLAGS",	"log Y",		0},
	{ XGM_lines,	"FLAGS",	"lines",		1},
	{ XGM_spline,	"FLAGS",	"spline",		1},
	{ XGM_hermite,	"FLAGS",	"interpolate",	1},
	{ XGM_mark,		"FLAGS",	"big mark",		1},
	{ XGM_pixel,	"FLAGS",	"small mark",	1},
	{ XGM_hhmmss,	"FLAGS",	"hhmmss",		1},
	{ XGM_bar,		"FLAGS",	"bar",			1},
	{ XGM_clear,	"MISC",		"clear",		1},
	{ XGM_save,		"MISC",		"save plot",	1}
};
#endif

/************************************************************************
 * XGSTARTXMENU
 * Start menus for xg.
 *
 * Parameters:
 *	display (Display *) - the display. (Input)
 *
 * Return Value:
 *	TRUE, successfully started menus.
 *	FALSE, unsuccessfully started menus.
 */
int16 xgStartXMenu(display)
	Display	*display;
{
	static int16	menuStarted = FALSE;
#ifndef X11
	int				pane;
	int				menuItems ;
	int				I;
	char 			*prevPane = "";	/* NULL string that works well also on SUNS */
#endif
  
	if (!menuStarted) {
#ifdef X11
		xgMenu = xpa_create(display,"xg",entrys,menuAttrMask,&menuAttr);
#else
		xgMenu = XMenuCreate(RootWindow, "xg");
		menuItems = (sizeof(CommandArray) / sizeof(struct menuEntry));
		for (I = 0 ; I < menuItems ; I++) {
			if (strcmp(prevPane,CommandArray[I].pane)!=0) {
				pane = XMenuAddPane(xgMenu, CommandArray[I].pane, 1);
				prevPane = CommandArray[I].pane;
			} /* if... */
			XMenuAddSelection(xgMenu, pane, CommandArray[I].func, CommandArray[I].selection , CommandArray[I].activeFlag);
		} /* for... */
		XMenuRecompute(xgMenu);
#endif
		menuStarted = TRUE;
	} /* if... */
	return(menuStarted);
} /* xgStartXMenu... */

#ifdef X11
/************************************************************************
 * XGIXPAHANDLER
 * Xpa event handler
 *
 * Parameters:
 *	evt (XEvent *) - the event for xpa. (Input)
 *
 * Return Value:
 *	FALSE, no errors.
 *	TRUE, error.
 */
int xgiXPAHandler(evt)
	XEvent	*evt;
{
	return(FALSE);
} /* xgiXPAHandler... */
#endif

/*************************************************************************
 * XGIMENUHANDLER
 * Menu handler
 *
 * Parameters:
 *	xg (XG *) - xg structure for graph window menu was invoked from. (Input)
 */
void xgiMenuHandler(xg)
	XG			*xg;
{
	int 		menuX;			/* x coordinate of the menu */
	int 		menuY;			/* y coordinate of the menu */
#ifdef X11
	Window		rootWin,childWin;
	int			rootX,rootY;
	unsigned int	mask;
	int			*result;
#ifdef JUNK
	int16		done = FALSE;
	XEvent		theEvent;
#endif
#else 
	Window		subW;			/* subWindow */
	static int	selection = 0;	/* previous selection */
	static int	pane = 0;		/* previous pane */
	XG_PFI		func;			/* pointer to a function returning integer */
#endif

#ifdef X11
	XQueryPointer(xg->display,xg->win,&rootWin,&childWin,&rootX,&rootY,
		&menuX,&menuY,&mask);
#ifdef JUNK
	xpa_post (xgMenu,rootX,rootY,0,NIL(int),0);
	while (!done) {
		XNEXTEVENT(xg->display,&theEvent);
		switch (theEvent.type) {
			case ButtonRelease:
				done = TRUE;
				break;
			default:
				if (xpa_filter(&theEvent,&xgMenu,&result) > 0) {
					if (&xpaFunc[result[0]][result[1]] != NIL(XG_PFI)) {
						if ((xpaFunc[result[0]][result[1]])(xg) != XG_OK) {
							(void) printf("Problem in the selected subroutine");
						} /* if... */
					} /* if... */
					done = TRUE;
				} /* if... */
				break;
		} /* switch... */
	} /* while... */
	xpa_unpost(xgMenu);
#endif
	if (xpa_moded(xgMenu,rootX,rootY,0,NIL(int),0,xgiXPAHandler,&result) > 0) {
		xpa_unpost(xgMenu);
		if ((result[0] <= 3) && (result[1] <= 9) &&
			&xpaFunc[result[0]][result[1]] != NIL(XG_PFI)) {
			if ((xpaFunc[result[0]][result[1]])(xg) != XG_OK) {
				(void) printf("Problem in the selected subroutine");
			} /* if... */
		} /* if... */
	} else {
		xpa_unpost(xgMenu);
	} /* if... */
#else
	XQueryMouse(RootWindow,&menuX,&menuY, &subW);
	switch (XMenuActivate(xgMenu,&pane,&selection,menuX,menuY,ButtonReleased,&func)) {
		case XM_SUCCESS:
			if (func != NULL) {
				if ((*func)(xg) != XG_OK) {
					(void) printf("Problem in the selected subroutine");
				} /* if... */
			} /* if... */
			break;
		case XM_NO_SELECT:
		case XM_IA_SELECT:
			break;
		case XM_FAILURE:
			(void) printf("Error Activating menu:%s",XMenuError());
			break;
	} /* switch... */
#endif
} /* xgiMenuHandler... */

/***************************************************************************
 * XGM_SHOWALL
 * Show all function.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_showAll(xg)
	XG		*xg;
{
	double	loX=0.0, loY=0.0, hiX=0.0, hiY=0.0;
	int		set;
	int		subindex;
	int		first = TRUE;

	for (set = 0 ; set < xg->numPlots ; set++) {
		if (xg->dataSet[set].numPoints != 0 ) {
			if (first) {
				hiX = loX = xg->dataSet[set].xvec[0];
				hiY = loY = xg->dataSet[set].yvec[0];
				first = FALSE;
			} /* if... */
			for (subindex=0;subindex < xg->dataSet[set].numPoints;subindex++) {
				loX = MIN( loX , xg->dataSet[set].xvec[subindex]);
				loY = MIN( loY , xg->dataSet[set].yvec[subindex]);
				hiX = MAX( hiX , xg->dataSet[set].xvec[subindex]);
				hiY = MAX( hiY , xg->dataSet[set].yvec[subindex]);
			} /* for... */
		} /* if... */
	} /* for... */
	if (!first) {	/* there is at least some point in the graph */
		xg->win_info->loX = loX;
 		xg->win_info->loY = loY;
		xg->win_info->hiX = hiX;
		xg->win_info->hiY = hiY;
		xgiAddPadding(xg);
		xgRedrawGraph((XGraph) xg);
	} /* if... */
	return(XG_OK);
} /* XGM_showAll... */

/*********************************************************************
 * XGM_ZOOMOUT
 * Zoom out function.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_zoomOut(xg) 
	XG		*xg;
{
	double	loX, loY, hiX, hiY;
	double	Xext, Yext;

	loX = xg->win_info->loX;
	loY = xg->win_info->loY;
	hiX = xg->win_info->hiX;
	hiY = xg->win_info->hiY;
	Xext = hiX - loX;
	Yext = hiY - loY;

	xg->win_info->loX = loX - (Xext/2);
	xg->win_info->loY = loY - (Yext/2);
	xg->win_info->hiX = hiX + (Xext/2);
	xg->win_info->hiY = hiY + (Yext/2);
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_zoomOut... */

/**********************************************************************
 * XGM_LOGX
 * Make x-axis logarithmic.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_logX(xg)
	XG	*xg;
{
	xg->flags ^= XGLogXFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_logX... */

/**********************************************************************
 * XGM_LOGY
 * Make y-axis logarithmic.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_logY(xg)
    XG	*xg;
{
	xg->flags ^= XGLogYFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_logY... */

/***********************************************************************
 * XGI_GRID
 * Toggle grid.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_grid(xg)
	XG	*xg;
{
	xg->flags ^= XGGridFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_grid... */

/***********************************************************************
 * XGM_BBOX
 * Toggle bounding box
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_bbox(xg)
    XG	*xg;
{
	xg->flags ^= XGBoundingBoxFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_bbox... */

/***********************************************************************
 * XGM_LINES
 * Toggle lines connecting data points.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_lines(xg)
	XG	*xg;
{
	xg->flags ^= XGLinesFlag;
	xg->flags &= ~XGPixelMarkFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_lines... */

/***********************************************************************
 * XGM_SPLINE
 * Toggle uniform cubic b-spline function.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_spline(xg)
	XG	*xg;
{
	xg->flags ^= XGBSplineFlag;
#ifndef X11
	xg->flags |= (xg->flags & XGBSplineFlag) ? XGLinesFlag : 0x0;
#endif
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_spline... */

/***********************************************************************
 * XGM_HERMITE
 * Toggle unifor cubic hermite interpolation.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_hermite(xg)
	XG	*xg;
{
	xg->flags ^= XGHermiteFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_hermite... */

/************************************************************************
 * XGM_BAR
 * Toggle bar graph of points.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_bar(xg)
	XG	*xg;
{
	xg->flags ^= XGBarFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_bar... */

/***********************************************************************
 * XGM_MARK
 * Toggle marks for data points.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_mark(xg)
	XG	*xg;
{
	xg->flags ^= XGBigMarkFlag;
	xg->flags &= ~XGPixelMarkFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_mark... */

/***********************************************************************
 * XGM_PIXEL
 * Set pixel size marks for data points.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_pixel(xg)
	XG	*xg;
{
	xg->flags ^= XGPixelMarkFlag;
	xg->flags &= ~(XGBigMarkFlag|XGLinesFlag);
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_pixel... */

/************************************************************************
 * XGM_HHMMSS
 * Set x-axis labeled in HH:MM:SS
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_hhmmss(xg)
	XG	*xg;
{
	xg->flags ^= XGHHMMSSFlag;
	xgRedrawGraph((XGraph) xg);
	return(XG_OK);
} /* XGM_hhmmss... */

/*************************************************************************
 * XGM_CLEAR
 * Clear the graph.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_clear(xg)
	XG	*xg;
{
	/*** Clear all sets in the graph ***/
	(void) xgClearGraph((XGraph) xg, NIL(char));
	return(XG_OK);
} /* XGM_clear... */

/*************************************************************************
 * XGM_save
 * Save the graph.
 *
 * Parameters:
 *	xg (XG *) - xg structure. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 */
int XGM_save(xg)
	XG	*xg;
{
	(void) xgSaveGraph((XGraph) xg,"");
	return(XG_OK);
} /* XGM_save... */

/***********************************************************************
 * XGSAVEGRAPH
 * Save the graph.
 *
 * Parameters:
 *	xgraph (XGraph) - the graph identifier. (Input)
 *	filename (char *) - the name of the file to save graph in,
 *		if 'filename' == NULL then routine will generate file name. (Input)
 *
 * Return Value:
 *	XG_OK, success.
 *	XG_ERROR, unsuccessful.
 */
int16 xgSaveGraph(xgraph,filename)
	XGraph	xgraph;
	char	*filename;
{
	XG		*xg = (XG *) xgraph;
	char	dataFileName[100];
	FILE	*DFfile;
	int		set;
	int		i;

	if (strcmp(filename,"") == 0) {
		(void) sprintf(dataFileName,"XG_%s.%s",xg->XUnits, xg->YUnits); 
	} else {
		(void) sprintf(dataFileName,"%s",util_file_expand(filename)); 
	} /* if... */
	if ((DFfile = fopen(dataFileName, "w")) == NIL(FILE)) {
		(void) printf("Cannot open file %s\n", dataFileName);
		return(XG_ERROR);
	} /* if... */
	for (set = 0 ; set < xg->numPlots ; set++) {
		for (i=0; i<xg->dataSet[set].numPoints ; i++) {
			(void) fprintf(DFfile,"%g %g\n", xg->dataSet[set].xvec[i], xg->dataSet[set].yvec[i]);
		} /* for... */
		(void) fprintf(DFfile,"\" %s\n\n", xg->dataSet[set].setName);
	} /* for... */
	(void) fclose(DFfile);
	(void) printf("Data saved in file %s\n", dataFileName);
	return(XG_OK);
} /* xgSaveGraph... */
