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
/*LINTLIBRARY*/
/*
 * Andrea Casotto
 * Benjamin Chen
 * University of California,  Berkeley
 * July 1988
 * Converted to X11 in April 1989 by A. Casotto
 * Multiple IV windows are now allowed 
 */

#include "port.h"
#include "ansi.h"
#include <sys/time.h>
#include "ualloc.h" 
#include "iv.h"
#include "ivGetLine.h"
/*
 * Can't include prototype for ivGetLine in ivGetLine.h because
 * hppa.cfront cc fails to compile ivGetLine.c otherwise.
 */
int ivGetLine
	ARGS((IVWindow *IVwin, XKeyEvent *keyEventPtr, int x, int y,
	      int bufferSize, bool keepGoing));
#include "ivBuf.h"
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include "xformatevent.h"
#include "button.bitmap.11"
#include "pushed.bitmap.11"
#include "plus.bitmap.11"
#include "plusPush.bitmap.11"
#include "minus.bitmap.11"
#include "minusPush.bitmap.11"

#ifndef MAXFLOAT
#define MAXFLOAT HUGE
#endif


#ifndef MAXINT
#define MAXINT 2147483647  /* Assuming 32 bit architecture */
#endif
/* MAXFLOAT defined in math.h, since floats are more machine dependent. */

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)		((x) < 0 ? -(x) : (x))
#endif
    
#define STRDUP(xx)	(strcpy(malloc((unsigned) (strlen(xx)+1)), (xx)))
    
#define FONT_WIDTH( f )  ((f)->max_bounds.rbearing - (f)->max_bounds.lbearing)  
#define FONT_HEIGHT( f ) ((f)->max_bounds.ascent + (f)->max_bounds.descent)
#define TOGGLE 0
#define PLUS   1
#define MINUS  2
#define NORM   0
#define PUSHED 1


static int debugXEvents = 0;
static XContext globalIvContext = (XContext)0;
static int global_number_of_IVwindows = 0;

static XFontStruct *titleFont;	/* Font for title                   */
static XFontStruct *textFont;	/* Font for text                    */
static XFontStruct *numberFont;	/* Font for numbers                 */

static int GetColor();		/* Forward declaration. */

/* Other defaults */
     
#define TITLE_FONT  "-*-courier-bold-r-*-*-*-100-*-*-*-*-*-*"	
#define TEXT_FONT   "-*-courier-medium-r-*-*-*-100-*-*-*-*-*-*"	
#define EDIT_FONT   "-*-courier-medium-r-*-*-*-100-*-*-*-*-*-*"	

int	IVbuttonWidth;		/* width of the plus/minus buttons */
int     IVbdrWidth = 1;
int	IVrowPadding = 1;	/* padding on each row ,above and below text */
int	IVrowHeight;		/* height of each row */
int	IVtitleHeight;		/*  */



#define IV_TEXT_WIN	0x0001
#define IV_VALUE_WIN   	0x0002
#define IV_TOGGLE_WIN	0x0004
#define IV_PLUS_BUTTON	0x0008
#define IV_MINUS_BUTTON 0x0010
#define IV_ROOT_WIN	0x0020
#define IV_ROOT_INDEX 	-1

#define LeftButton   Button1
#define MiddleButton Button2
#define RightButton  Button3

#define IV_MIN_ROWS 1
#define IV_MAX_ROWS 100

/* Forward decl.  Could this be static? */
int ivWhichSelect
	ARGS((IVWindow *IVwin, int index));
void ReadDefaults
	ARGS((char* progName, IVWindow *IVwin));


void ivError(s)
     char *s;
{
    (void) fprintf(stderr,"IV: Error: %s\n",s);
}

static XFontStruct* ivLoadFont( display, name )
    Display* display;
    char* name;
{
    XFontStruct* font;
    char** fontList;
    int actual;

    fontList = XListFonts( display, name, 3, &actual );
    if ( !actual ) {
	char buffer[256];
	sprintf( buffer, "No such font `%s'", name );
	ivError( buffer );
	return 0;
    }
    font = XLoadQueryFont( display, fontList[0] );
    if ( font == (XFontStruct*)0) {
	char buffer[256];
	sprintf( buffer, "No such font `%s'", fontList[0] );
	ivError( buffer );
    }
    XFreeFontNames( fontList );
    return font;
}



/*  These are milli seconds */
#define	BUTTONWAIT    400  /* 0.3  seconds */
#define FASTWAIT       80  /* 0.08 seconds */

#define CARRIAGE_RETURN '\015'


int ivRealloc( IVwin )
    IVWindow* IVwin;
{
    int IVrowsInc = 1;		/*  */

    if ( IVwin->rows >= IV_MAX_ROWS ) {
	return IV_ERROR;
    }
    IVwin->rows += IVrowsInc;
    IVwin->array = REALLOC( IVvarType, IVwin->array, IVwin->rows);
    if ( IVwin->array == 0) {
	ivError("Out of memory");
    }
    return IV_OK;
}


void ivAtoi( IVwin, string, n )
    IVWindow *IVwin;
    char *string;
    int n;			/* index of var in array */
{
    double      tmp;
    IVvarType* IVArray = IVwin->array;

    switch (IVArray[n].type) {
    case IV_DOUBLE:
	if ( strncmp( string, "Infinity", 8 ) == 0) {
	    *(IVArray[n].varP.real) = HUGE;
	} else if ( strncmp(string, "MAXFLOAT", 8) == 0) {    
	    *(IVArray[n].varP.real) = MAXFLOAT;
	} else {
	    *(IVArray[n].varP.real) = atof(string);
	}
	break;
    case IV_INT:
	if(strncmp(string, "MAXINT", 6) == 0) {
	    tmp = MAXINT;
	} else {
	    tmp = atof(string);
	    if(ABS(tmp) > MAXINT) {
		tmp = (tmp > 0) ? MAXINT : -MAXINT;
	    }		    		
	}
	*(IVArray[n].varP.integer) = (int) tmp;
	break;
    case IV_BOOLEAN:
	if (strncmp(string, "TRUE", 4) == 0) {
	    *(IVArray[n].varP.boolean) = TRUE;
	}
	if (strncmp(string, "FALSE", 5) == 0) {
	    *(IVArray[n].varP.boolean) = FALSE;
	}
	break;
    case IV_STRING:
	*(IVArray[n].varP.string) = REALLOC(char, *(IVArray[n].varP.string), strlen(string));
	(void) strcpy(*(IVArray[n].varP.string), string);
	break;
    default:
	abort();
    }
}

void ivItoa( IVwin, string, n)
    IVWindow *IVwin;
    char *string;
    int n;			/* Index. */
{
    double d;
    bool b;

    IVvarType *IVArray = IVwin->array;

    if ( n == IVwin->editIndex ) {
      (void) strcpy( string, ivBufGetString(&IVwin->editBuffer));
      return;
    }
    switch (IVArray[n].type) {
    case IV_DOUBLE:
	d = *(IVArray[n].varP.real);
	if (ABS(d)>999999.9 || ABS(d) < 0.001) {
	    (void) sprintf(string,"%.3e", d);
	} else {
	    (void) sprintf(string, IVArray[n].st.precision, d);
	}
	break;
    case IV_INT:
	(void) sprintf( string, "%d",  *(IVArray[n].varP.integer) );
	break;
    case IV_BOOLEAN:
	b = *(IVArray[n].varP.boolean);
	(void) sprintf( string, "%s", (b) ? "TRUE" : "FALSE");
	break;
    case IV_STRING:
	(void) strcpy( string, *(IVArray[n].varP.string));
	break;
    case IV_WHICH: 
	{
	    int whichOne;
	    whichOne = ivWhichSelect( IVwin, n);
	    (void) strcpy(string, IVArray[n].varP.itemArray[whichOne].value);
	    break;
	}
    default:
	break;
    }
}

void initPixmaps( IVwin, pixmaps, widths, heights )
    IVWindow *IVwin;
    Pixmap pixmaps[3][2];
    int widths[3][2], heights[3][2];
{
    Display *display = IVwin->display;
    Window w = IVwin->w;
    Screen *screen = XDefaultScreenOfDisplay(display);
    unsigned long fg = IVwin->bdrPixel;
    unsigned long bg = IVwin->buttonPixel;
    unsigned int depth = XDefaultDepthOfScreen(screen);

    if (depth < 4) {
	fg = XBlackPixelOfScreen(screen);
	bg = XWhitePixelOfScreen(screen);
    }

    pixmaps[TOGGLE][PUSHED] = XCreatePixmapFromBitmapData( display, w,pushed_bits, pushed_width, pushed_height, fg, bg, depth );
    widths[TOGGLE][PUSHED] = pushed_width; heights[TOGGLE][PUSHED] = pushed_height;
    pixmaps[TOGGLE][NORM] = XCreatePixmapFromBitmapData( display, w,button_bits,button_width, button_height, fg, bg, depth );
    widths[TOGGLE][NORM] =button_width; heights[TOGGLE][NORM] = button_height;
    pixmaps[PLUS][NORM] = XCreatePixmapFromBitmapData( display, w,plus_bits,plus_width, plus_height, fg, bg, depth );
    widths[PLUS][NORM] =plus_width; heights[PLUS][NORM] = plus_height;
    pixmaps[PLUS][PUSHED] = XCreatePixmapFromBitmapData( display, w,plusPush_bits,plusPush_width, plusPush_height , fg, bg, depth );
    widths[PLUS][PUSHED] =plusPush_width; heights[PLUS][PUSHED] = plusPush_height;
    pixmaps[MINUS][NORM] = XCreatePixmapFromBitmapData( display, w,minus_bits,minus_width, minus_height, fg, bg, depth );
    widths[MINUS][NORM] =minus_width; heights[MINUS][NORM] = minus_height;
    pixmaps[MINUS][PUSHED] = XCreatePixmapFromBitmapData( display, w,minusPush_bits,minusPush_width, minusPush_height, fg, bg, depth );
    widths[MINUS][PUSHED] =minusPush_width; heights[MINUS][PUSHED] = minusPush_height;
}


static void toggleButton( IVwin, win, button, buttonState)
    IVWindow *IVwin;
    Window win;			/* Window to draw cursor in */
    int button, buttonState;
{
    Display *display = IVwin->display;
    
    int sn = XDefaultScreen( display );

    GC  gc = XDefaultGC( display, sn );
/*    XWindowAttributes winInfo; */
    int x, y;
    unsigned int width,height;
    int button_pushed;
    unsigned int bdr, depth;
    Window root;

    
    static Pixmap pixmaps[3][2];
    static int    widths[3][2];
    static int    heights[3][2];
    static int pixmaps_initialized = 0;
    Pixmap pixmap;
    int map_width, map_height;

    if ( !pixmaps_initialized ) {
	initPixmaps( IVwin , pixmaps, widths, heights );
	pixmaps_initialized = 1;
    }

    XClearWindow( display, win);
    XGetGeometry( display, win, &root, &x, &y, &width, &height, &bdr, &depth );
    
    button_pushed = ( buttonState ) ? PUSHED : NORM;
    
    pixmap = pixmaps[button][button_pushed];
    map_width = widths[button][button_pushed];
    map_height = heights[button][button_pushed];


    x = (width - map_width) / 2;
    y = (height - map_height) / 2;
    XCopyArea( display, pixmap, win, gc, 0, 0, map_width , map_height, x, y );

    /* XFlush( display ); */
    return;
}

int ivWhichSelect( IVwin, index)
    IVWindow *IVwin;
    int index;
{
    IVvarType* IVArray = IVwin->array;
    int i, theItem = -1;
    int whichCount = IVArray[index].st.selectCount;

    for (i = 0; i < whichCount; i++) {
	if (IVArray[index].varP.itemArray[i].select) {
	    theItem = i;
	    break;
	}
    }
    if ( i == whichCount ) i = 0;
    return theItem;
}

void ivSetSelect( IVwin, index, oldWhich, newWhich)
    IVWindow *IVwin;
    int index, oldWhich, newWhich;
{
    IVvarType* IVArray = IVwin->array;
    IVArray[index].varP.itemArray[oldWhich].select = 0;
    IVArray[index].varP.itemArray[newWhich].select = 1;
}
	

void ivCreateRow( IVwin, i)
    IVWindow *IVwin;
    int i;  /* Index to row */
{
    Display *display = IVwin->display;
    Window  win = IVwin->w;
    IVvarType* IVArray = IVwin->array;

    IVAssocEntry *theEntry;
    int  x, y;
    int  type;
    int  width, height;

    type = IVArray[i].type;
    y = IVtitleHeight + (i * (IVrowHeight + IVbdrWidth + IVrowPadding));

    /* Text window first */
    x = - IVbdrWidth;	/* overlap the borders */
    width =  ( type == IV_TOGGLE ) ?  IVwin->docWidth + IVwin->valueWidth + IVbdrWidth : IVwin->docWidth ;
    height = IVrowHeight;
    IVArray[i].TextWin = XCreateSimpleWindow( display , win, x, y, width, height , 0, IVwin->bdrPixel, IVwin->bgPixel);
    XSelectInput( display, IVArray[i].TextWin, ExposureMask );
    theEntry = NEW(IVAssocEntry);
    theEntry->IVWinType = IV_TEXT_WIN;
    theEntry->index = i;
    theEntry->IVwin = IVwin;
    XSaveContext( display, IVArray[i].TextWin, globalIvContext,
		 (XPointer)theEntry);
    XMapWindow( display, IVArray[i].TextWin);
    
    /* Value window is next */
    x = x + IVbdrWidth;
    if ( type != IV_TOGGLE ) {
	width =  (type == IV_STRING) ?  IVwin->valueWidth + 2 *
	  (IVbuttonWidth+IVbdrWidth) : IVwin->valueWidth; 
	IVArray[i].ValueWin = XCreateSimpleWindow( display, win, x +
						  IVwin->docWidth, y,
						  width, height, 
						  IVbdrWidth,
						  IVwin->bdrPixel,
						  IVwin->editBgPixel
						  ); 
	XSelectInput( display, IVArray[i].ValueWin, ExposureMask |
		     KeyPressMask | ButtonPressMask ); 
	theEntry = NEW(IVAssocEntry);
	theEntry->IVWinType = IV_VALUE_WIN;
	theEntry->index = i; 
	theEntry->IVwin = IVwin;
	XSaveContext( display, IVArray[i].ValueWin, globalIvContext,
		     (XPointer)theEntry ); 
	XMapWindow( display, IVArray[i].ValueWin );
    }

    if(type == IV_BOOLEAN || type == IV_TOGGLE) {
	x = x + IVbdrWidth;
	IVArray[i].ToggleWin = XCreateSimpleWindow( display, win,
					     x + IVwin->docWidth + IVwin->valueWidth + IVrowPadding, y,
					     (2 * IVbuttonWidth) + IVbdrWidth,
					     IVrowHeight, 0,
					     IVwin->bdrPixel, IVwin->bgPixel);
	XSelectInput( display, IVArray[i].ToggleWin, ExposureMask | ButtonPressMask | ButtonReleaseMask );
	theEntry = NEW(IVAssocEntry);
	theEntry->IVWinType = IV_TOGGLE_WIN;
	theEntry->index = i; 
	theEntry->IVwin = IVwin;

	XSaveContext( display, IVArray[i].ToggleWin, globalIvContext,
		     (XPointer)theEntry); 
	XMapWindow( display, IVArray[i].ToggleWin );
    } else if ( type != IV_STRING ) {
	x = x + IVbdrWidth;
	IVArray[i].PlusButton = XCreateSimpleWindow( display, win,
					      x + IVwin->docWidth + IVwin->valueWidth + IVrowPadding, y,
					      IVbuttonWidth, IVrowHeight, 0,
					      IVwin->bdrPixel, IVwin->bgPixel);
	x = x + IVbdrWidth;
	IVArray[i].MinusButton = XCreateSimpleWindow( display, win,
					       x+IVwin->docWidth+IVwin->valueWidth+ IVbuttonWidth + IVrowPadding, y,
					       IVbuttonWidth, IVrowHeight, 0,
						     IVwin->bdrPixel, IVwin->bgPixel);
	XSelectInput( display, IVArray[i].PlusButton,  ExposureMask | ButtonPressMask | ButtonReleaseMask);
	XSelectInput( display, IVArray[i].MinusButton, ExposureMask | ButtonPressMask | ButtonReleaseMask);
	theEntry = NEW(IVAssocEntry);
	theEntry->IVWinType = IV_PLUS_BUTTON;
	theEntry->index = i; 
	theEntry->IVwin = IVwin;

	XSaveContext( display, IVArray[i].PlusButton, globalIvContext,
		     (XPointer)theEntry); 
	theEntry = NEW(IVAssocEntry);
	theEntry->IVWinType = IV_MINUS_BUTTON;
	theEntry->index = i; 
	theEntry->IVwin = IVwin;

	XSaveContext( display, IVArray[i].MinusButton,
		     globalIvContext, (XPointer) theEntry); 
	XMapWindow( display, IVArray[i].PlusButton );
	XMapWindow( display, IVArray[i].MinusButton);
    }
}

void ivDestroyRow( IVwin, i )
    IVWindow *IVwin;
    int i;
{
    Display *display = IVwin->display;
    IVvarType* IVArray = IVwin->array;

    XDeleteContext( display, globalIvContext, IVArray[i].TextWin );
    XDestroyWindow( display , IVArray[i].TextWin );
    if ( IVArray[i].type != IV_TOGGLE ) {
	XDeleteContext( display, globalIvContext, IVArray[i].ValueWin );
	XDestroyWindow( display, IVArray[i].ValueWin );
    }
    if ( IVArray[i].type == IV_TOGGLE || IVArray[i].type == IV_BOOLEAN ) {
	XDeleteContext( display, globalIvContext, IVArray[i].ToggleWin );
	XDestroyWindow( display, IVArray[i].ToggleWin );
    } else if ( IVArray[i].type != IV_STRING ) {
	XDeleteContext( display, globalIvContext, IVArray[i].PlusButton );
	XDeleteContext( display, globalIvContext, IVArray[i].MinusButton );
	XDestroyWindow( display, IVArray[i].PlusButton );
	XDestroyWindow( display, IVArray[i].MinusButton );
    }
}


int ivCallFunction( IVwin, index)
    IVWindow *IVwin;
    int index;
{
    IVvarType* IVArray = IVwin->array;

    /* Call the function associated with this variable */
    if (IVArray[index].func != IV_NULL_FUNCTION) {
	if ( (*IVArray[index].func)() != IV_OK ) {
	    (void) printf("IV:Warning: Function called failed\n");
	    return IV_ERROR;
	}
    }
    return IV_OK;
}


int search_IVar( IVwin, varP) 
    IVWindow *IVwin;
    int *varP;
{
    IVvarType* IVArray = IVwin->array;

    int index, status = IV_ERROR;

    /* Number of vars shouldn't be TOO long, so do sequential search. */
    for (index = 0; index < IVwin->numberOfVars; index++) {
	if (IVArray[index].id == (long) varP) {
	    status = index;
	    break;
	}
    }
    return status;
}

static void
ivPrintLine( IVwin, n, WinMask, rv )
    IVWindow *IVwin;
    register int n;		/* Which line to draw */
    register int WinMask;	/* Which windows in the line to draw. */
    int rv;			/* If non-zero, toggles reverse video. Not valid for buttons.*/
{
    Display *display = IVwin->display;
    Window  win = IVwin->w;
    IVvarType* IVArray = IVwin->array;

    register int type;
    register int enable = 0;
    XWindowAttributes IVwin_info;

    /* If the window is not mapped, just forget about printing anything */
    XGetWindowAttributes( display, win, &IVwin_info);
    if (IVwin_info.map_state == IsUnmapped ) return;

    if (WinMask & IV_ROOT_WIN) {
	int  titleWidth;
	titleWidth = XTextWidth(titleFont,  IVwin->titleString, strlen( IVwin->titleString));
	XDrawImageString( display, win, IVwin->titleGC , (IVwin_info.width - titleWidth) / 2,
			 IVrowPadding + FONT_HEIGHT(titleFont), IVwin->titleString, strlen(IVwin->titleString) );
    }
    if (WinMask & IV_TEXT_WIN) {
	char *s    = IVArray[n].doc;
	int docLen = strlen( s );
	int X      = IVwin->docWidth - (2 * IVbdrWidth) -  XTextWidth( textFont, s, docLen);
	int Y      = textFont->max_bounds.ascent + IVrowPadding;
	XDrawImageString( display, IVArray[n].TextWin, IVwin->docGC, X, Y, s, docLen );
    }
    if (WinMask & IV_TOGGLE_WIN) {
	enable = IVArray[n].enable;
	if (enable) {
	    toggleButton( IVwin, IVArray[n].ToggleWin, TOGGLE, NORM);
	} else {
	    XClearWindow( display, IVArray[n].ToggleWin);
	}
    }
    if (WinMask & IV_VALUE_WIN) {
	char s[40];
	GC gc ;
	int  bg = IVwin->bgPixel;

	ivItoa(IVwin, s, n);
	if ( !  IVArray[n].enable ) {
	    gc = IVwin->docGC;
	} else {
	    if ( rv || n == IVwin->editIndex ) {
		gc = IVwin->editValueGC;
		bg = IVwin->editFontPixel;
	    } else {
		gc = IVwin->normValueGC;
		bg = IVwin->editBgPixel;
	    }
	}
	XSetWindowBackground( display, IVArray[n].ValueWin, bg );
	XClearWindow( display, IVArray[n].ValueWin);
	XDrawImageString( display, IVArray[n].ValueWin, gc, IVrowPadding,
			 numberFont->max_bounds.ascent + IVrowPadding, s, strlen(s) ); 
    }
    if ( WinMask & ( IV_PLUS_BUTTON | IV_MINUS_BUTTON )) {
	type = IVArray[n].type;
	if(WinMask & IV_PLUS_BUTTON) {
	    if (enable || type == IV_WHICH) {
		toggleButton( IVwin, IVArray[n].PlusButton, PLUS, NORM);
	    } else {
		XClearWindow( display, IVArray[n].PlusButton );
	    }
	}
	if(WinMask & IV_MINUS_BUTTON) {
	    if (enable || type == IV_WHICH) {
		toggleButton( IVwin, IVArray[n].MinusButton, MINUS, NORM);
	    } else {
		XClearWindow( display, IVArray[n].MinusButton );
	    }
	}
    }
    /* VOID FUNCTION */
}


void ivEditVar( IVwin, theEventP, index, mode)
    IVWindow *IVwin;
    XAnyEvent *theEventP;
    int    index;
    int	   mode;		/* 0 -> ???, 1 -> ???, 2 -> stop editing  */
{
    Display *display = IVwin->display;
    IVvarType* IVArray = IVwin->array;

    XWindowAttributes theWinInfo;
    XKeyEvent *keyEventP = (XKeyEvent*)theEventP;
    XButtonEvent *buttonEventP = (XButtonEvent*)theEventP;
    int status = 0, bufsize, tmpIndex;
    bool   keepGoing;
    
    /* Should we abort previous editing ? */
    if ( mode == 2 || ( theEventP->window != IVwin->theWin &&
		      theEventP->type == ButtonPress && !IVwin->firstIter ) ) {
	
        int tmpIndex = IVwin->editIndex;

	IVwin->editIndex = IV_ROOT_INDEX;
	IVwin->firstIter = TRUE;
	XSetWindowBackground( display, IVwin->theWin, IVwin->editBgPixel );
	if (mode == 2) {	/* exit the edit loop */
	    ivPrintLine( IVwin, tmpIndex, IV_VALUE_WIN, 0 );
	    return;
	}
	ivPrintLine( IVwin, tmpIndex, IV_VALUE_WIN, 0 );
    }
			       
    if ( IVwin->firstIter) {
	char string[128];
	if ( (char)ivKeyOk( keyEventP ) == CARRIAGE_RETURN) {
	    return;
	}

	ivItoa( IVwin, string , index);		/* Put current value in string */

	IVwin->firstIter = FALSE;
	IVwin->theWin = keyEventP->window;
	IVwin->editIndex = index;


	ivBufClear( &IVwin->editBuffer );
	ivBufClear( &IVwin->buffer  );
	ivBufClear( &IVwin->oldBuffer  );
	if ( ! IVwin->eraseFlag ) {
	    ivBufAddString( &IVwin->buffer, string ); /* .. and in the buffer. */
	}
	ivBufAddString( &IVwin->editBuffer, string );
	ivBufAddString( &IVwin->oldBuffer, string );

	XSetWindowBackground( display, buttonEventP->window, IVwin->textFontPixel );
	XClearWindow( display, IVwin->theWin );
    } else {
	/* IVwin->eraseFlag = FALSE; */
    }
    keepGoing = (mode == 1) ? TRUE : FALSE;

    /* buttonEventP->window = IVwin->theWin; */
    XGetWindowAttributes( display, IVArray[IVwin->editIndex].ValueWin, &theWinInfo);
    bufsize = theWinInfo.width / FONT_WIDTH(numberFont);
    do {
	XEvent localEvent;
	IVAssocEntry *theEntry;
	caddr_t ptr;

	status = ivGetLine( IVwin, keyEventP, 
			   IVrowPadding, 
			   IVrowPadding + numberFont->max_bounds.ascent,
			   bufsize, keepGoing );
	switch (status) {
	case Expose:
	    while(QLength( display ) > 0) {
		XNextEvent( display, &localEvent);
		XFindContext( display, localEvent.xany.window,
			     globalIvContext, &ptr );
		theEntry = (IVAssocEntry *) ptr;
		tmpIndex= theEntry->index;
		switch(localEvent.type) {
		case Expose:
		    ivPrintLine( IVwin,tmpIndex, theEntry->IVWinType, ( index == tmpIndex) );
		    break;
		case IV_OK:
		    keepGoing = FALSE;
		    break;
		default:
		    break;
		}
	    }
	}
    } while ( status != IV_OK && keepGoing );
    
    if (status == IV_OK) {
	IVwin->firstIter = TRUE;
	ivAtoi( IVwin, ivBufGetString(&IVwin->buffer), IVwin->editIndex);
	if ( strcmp( ivBufGetString( &IVwin->oldBuffer ), ivBufGetString( &IVwin->buffer) ) != 0) {
	    /* Call the function associated with this variable */
	    ivCallFunction( IVwin, IVwin->editIndex); 
	}
	tmpIndex = IVwin->editIndex;
	IVwin->editIndex = IV_ROOT_INDEX;
	XSetWindowBackground( display, IVwin->theWin, IVwin->editBgPixel );
	ivPrintLine( IVwin, tmpIndex, IV_VALUE_WIN, 0);
    }
}

void constructIVWindow( IVwin, display )
    IVWindow *IVwin;
    Display *display;
{
    int sn = XDefaultScreen( display );
    int bwFlag = (DisplayPlanes( display, sn ) < 4);

    IVwin->display = display;

    if (bwFlag) {			/* Its black and white */
	IVwin->bdrPixel = WhitePixel( display, sn );
	IVwin->textFontPixel = WhitePixel( display, sn );
	IVwin->cursorPixel = WhitePixel( display, sn );
	IVwin->bgPixel = BlackPixel( display, sn );
	IVwin->buttonPixel = WhitePixel( display, sn );
	IVwin->editBgPixel = BlackPixel( display, sn );
	IVwin->editFontPixel = WhitePixel( display, sn );
	IVwin->titleFontPixel = WhitePixel( display, sn );
    } else {
	/* Its color */
	IVwin->bdrPixel = GetColor( display, "DarkSlateBlue" );
	IVwin->bgPixel = GetColor( display, "LightGrey");
	IVwin->buttonPixel = GetColor( display, "Yellow");
	IVwin->editBgPixel = GetColor( display, "LightBlue");
	IVwin->editFontPixel = GetColor( display, "Red");
        IVwin->textFontPixel = GetColor( display, "Blue");
	IVwin->titleFontPixel = GetColor( display, "DarkSlateBlue");
	IVwin->cursorPixel = GetColor( display, "Green");
    }
    IVwin->firstIter = TRUE;
    IVwin->docWidth = 0;
    IVwin->docLen = 0;
}

void createGCs( IVwin ) 
    IVWindow *IVwin;
{
    Display *display = IVwin->display;

    IVwin->docGC = XCreateGC( display, IVwin->w, 0, 0 );
    XSetFont( display, IVwin->docGC, textFont->fid );
    XSetForeground( display, IVwin->docGC, IVwin->editFontPixel );
    XSetBackground( display, IVwin->docGC, IVwin->bgPixel );

    IVwin->normValueGC = XCreateGC( display, IVwin->w, 0, 0 );
    IVwin->editValueGC = XCreateGC( display, IVwin->w, 0, 0 );
    XSetFont( display, IVwin->normValueGC, numberFont->fid );
    XSetForeground( display, IVwin->normValueGC, IVwin->editFontPixel );
    XSetBackground( display, IVwin->normValueGC, IVwin->editBgPixel );
    XSetFont( display, IVwin->editValueGC, numberFont->fid );
    XSetForeground( display, IVwin->editValueGC, IVwin->editFontPixel );
    XSetBackground( display, IVwin->editValueGC, IVwin->textFontPixel );

    IVwin->titleGC = XCreateGC( display, IVwin->w, 0, 0 );
    XSetFont( display, IVwin->titleGC, titleFont->fid );
    XSetForeground( display, IVwin->titleGC, IVwin->titleFontPixel );
    XSetBackground( display, IVwin->titleGC, IVwin->bgPixel );

    IVwin->valueFont = numberFont;
}


/* initializes IV, opening the IV window 
 * Connection to a display must have been established already
 *
 * Inits and maps a new window.  This includes allocating its
 * local structure and associating it with the XID for the window.
 * The aspect ratio is specified as the ratio of width over height.
 */
IVWindow*
iv_InitWindow( display, progName, title, maxValChars )
    Display 	*display;
    char    	*progName;
    char	*title;		/* title string */
    int		maxValChars;	/* maximum number of chars in value window */
{
    static char* firstProgName = 0; /* All calls should use this */
    int sn = XDefaultScreen( display );
    IVWindow *newIVwin = 0;
    IVAssocEntry  *theEntry;
    Window w;
    int buf_length = 0;		/* Edit buffer length */
	

    if ( globalIvContext == 0 ) globalIvContext = XUniqueContext();

    /* Set up hard-wired defaults and allocate spaces */
    IVbdrWidth = 1;
    titleFont = ivLoadFont( display, TITLE_FONT );
    textFont = ivLoadFont( display, TEXT_FONT );
    numberFont = ivLoadFont(display, EDIT_FONT );
    
    if ( ! firstProgName ) {
	firstProgName = STRDUP( progName );
    } else {
	progName = firstProgName;
    }
    
    newIVwin = (IVWindow*)malloc( (unsigned)sizeof(IVWindow) );
    constructIVWindow( newIVwin, display );
    ReadDefaults( progName, newIVwin );

    ivTextInit( );
    buf_length = 2 * maxValChars; /* A bit more than actually needed. */
    ivBufInit( &newIVwin->buffer, buf_length , display );
    ivBufInit( &newIVwin->oldBuffer, buf_length, display );
    ivBufInit( &newIVwin->editBuffer, buf_length, display );
    
    newIVwin->editIndex = IV_ROOT_INDEX;   /* Nothing being edited */
    newIVwin->titleString = STRDUP(title);
    newIVwin->numberOfVars = 0;
    newIVwin->oldNumOfVars = 0;
    newIVwin->rows = IV_MIN_ROWS;
    newIVwin->docLen = 0;
    newIVwin->docWidth = 0;
    IVrowHeight = MAX( FONT_HEIGHT(textFont), FONT_HEIGHT(numberFont) ) + 2 * IVrowPadding;
    newIVwin->valueWidth = (maxValChars * FONT_WIDTH(numberFont) ) + (2 * IVrowPadding);
    IVbuttonWidth = IVrowHeight;
    IVtitleHeight = FONT_HEIGHT(titleFont) + 4 * IVrowPadding;
    
    IVrowHeight = IVrowHeight + IVbdrWidth;
    newIVwin->w = w = XCreateSimpleWindow( display, RootWindow( display , sn),
					  0, 0, 
					  1,
					  IVtitleHeight+(newIVwin->rows * (IVrowHeight + IVbdrWidth + IVrowPadding)), 
					  IVbdrWidth,
					  newIVwin->bdrPixel, newIVwin->bgPixel
					  );
    
    if ( newIVwin == 0 ) {
	ivError("Could not open window");
	return((IVWindow*) 0);
    }
    global_number_of_IVwindows++;


    XStoreName( display, w, "interactive_variables");

    createGCs( newIVwin );


    XSelectInput( display, w,  ExposureMask | KeyPressMask );

    theEntry = NEW(IVAssocEntry);
	theEntry->IVWinType = IV_ROOT_WIN;
	theEntry->index = IV_ROOT_INDEX;
	theEntry->IVwin = newIVwin ;
	XSaveContext( display, w, globalIvContext, (XPointer)theEntry);
	newIVwin->array = NEWVEC(IVvarType, newIVwin->rows);
	
    return newIVwin;
}

int iv_MapWindow( IVwin, x, y, opt)
    IVWindow 	*IVwin;
    int x, y;			/* Location where to put mouse.  Ignored if opt is set */
    int opt;			/* Option specifying where to put the window           */
{
    Display *display = IVwin->display;
    Window  w = IVwin->w;
    int sn = XDefaultScreen( display );
    unsigned int width, height;
    XWindowAttributes IVwinInfo;
    XSetWindowAttributes winAttr;
    /* char geoSpec[30], defSpec[30]; */
    /*     char prompt[120]; */
    int index, titleWidth;

    /* Place using the window manager, or override ? */
    winAttr.override_redirect = (opt == IV_NO_OPT) ? 1 : 0 ;
    XChangeWindowAttributes( display, w, CWOverrideRedirect , &winAttr );

    /* All subwindows might have to be repositioned */

    XGetWindowAttributes( display, w, &IVwinInfo );
    if ( IVwinInfo.map_state != IsUnmapped ) {
	XUnmapWindow( display, w );
	for (index = 0; index < IVwin->oldNumOfVars; index++) ivDestroyRow( IVwin, index);
    }
    IVwin->rowWidth = IVwin->docWidth + IVwin->valueWidth + (2 * IVbuttonWidth) + (5 * IVrowPadding);
    titleWidth = XTextWidth(titleFont,  IVwin->titleString, strlen( IVwin->titleString)) + 4 * IVrowPadding;
    IVwin->rowWidth = MAX(IVwin->rowWidth, titleWidth);

    width  = IVwin->rowWidth;
    height =   IVtitleHeight + (IVwin->rows * (IVrowHeight + IVbdrWidth + IVrowPadding ));
    XResizeWindow( display, w, width, height );
		
    for ( index = 0; index < IVwin->numberOfVars; index++) {
	ivCreateRow( IVwin, index );
    }

    /* Modify position to make sure its on screen */
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + IVwinInfo.width > DisplayWidth( display, sn ))
      x = DisplayWidth( display, sn ) - IVwinInfo.width;
    if (y + IVwinInfo.height > DisplayHeight( display, sn ))
      y = DisplayHeight( display, sn ) - IVwinInfo.height;

    /* First,  move the window to the specified spot */
    XMoveWindow( display, w, x, y );
    /* XGetWindowAttributes( display, w, &IVwinInfo); */
    IVwin->oldNumOfVars = IVwin->numberOfVars;
    XMapWindow( display, w );
    return IV_OK;
}


int iv_AddVar(IVwin,docString, varP, type, precision, func)
    IVWindow *IVwin;
     char 	*docString;
     int 	*varP;	   /* the address of the variable to display */
     int 	type;
     int        precision;
     int	(*func)();
{
    IVvarType* IVArray = 0;
    int  docLen;		/* Length of the documentation field. */
    int  docWidth;
    int  i;			/* The index in the array for this variable. */
    
    if (IVwin->numberOfVars >= IVwin->rows) {
	if (ivRealloc( IVwin )!=IV_OK) {
	    ivError("Could not increase the number of rows");
	    return(IV_ERROR);
	}
    }

    IVArray = IVwin->array;
    
    i = IVwin->numberOfVars++;
    
    docLen = strlen(docString);
    IVwin->docLen = MAX(docLen, IVwin->docLen);
    docWidth = XTextWidth( textFont, docString, docLen ) + 5 * IVrowPadding;
    IVwin->docWidth = MAX( IVwin->docWidth, docWidth );

    IVArray[i].doc = STRDUP(docString);
    IVArray[i].type = type;
    IVArray[i].func = (iv_PFI ) func;
    IVArray[i].id = (long) varP;
    IVArray[i].enable = TRUE;
    
    switch (type) {
    case IV_DOUBLE:
	IVArray[i].varP.real = (double *) varP;
	IVArray[i].st.precision = NEWVEC(char,  precision + 10); /* 10 for sign, e-xx and margin */
	(void) sprintf(IVArray[i].st.precision, "%%.%df", precision);
	break;
    case IV_INT:
	IVArray[i].varP.integer = varP;
	break;
    case IV_BOOLEAN:
    case IV_TOGGLE:
	IVArray[i].varP.boolean = varP;
	break;
    case IV_STRING:
	IVArray[i].varP.string = (char **) varP;
	break;
    case IV_WHICH:
	IVArray[i].varP.itemArray = (ivWhichItem *) varP;
	IVArray[i].st.selectCount = precision;
	IVArray[i].enable = FALSE;
	break;
    default:
	return IV_ERROR;
    }
    return IV_OK;
}

int iv_PromptVar( IVwin, varP, bell )
    IVWindow *IVwin;
    int *varP;   /* The address of the var to prompt. */
    int bell;    /* Specifies the bell volume, which can range
                  * from -100 to 100 inclusive.
                  */
{
    Display *display = IVwin->display;
    Window  win = IVwin->w;
    IVvarType* IVArray = IVwin->array;

    XEvent theEvent;
    int index, status = IV_ERROR;

    index = search_IVar( IVwin, varP );

    if (index == IV_ERROR) {
	return index;
    }
    XRaiseWindow( display, win );
    XRaiseWindow( display, IVArray[index].ValueWin );
    ivPrintLine( IVwin, index, IV_ROOT_WIN, 0 );
    while(QLength( display ) > 0) {
	XNextEvent( display, &theEvent );   
	status = iv_HandleEvent(&theEvent);
	if(status == IV_ERROR) return status;
    }
    /* XSync( display, False ); */
    XBell( display, bell ); 
    /* Highlight the appropriate window. */
    XSetWindowBackground( display, IVArray[index].ValueWin, IVwin->textFontPixel );
    XClearWindow( display, IVArray[index].ValueWin );
    ivPrintLine( IVwin, index, IV_VALUE_WIN, 1);
    theEvent.xany.window = IVArray[index].ValueWin;
    theEvent.xany.type = KeyPress;
    ivEditVar( IVwin,  (XAnyEvent*)&theEvent, index, 1 );
    status = IV_OK;
    return status;
}

/* 
 * Easier way to handle the event queue in the case that 
 * iv is the only user of X in the application program
 */
int iv_ProcessAllEvents( display )
    Display *display;
{
    int extraneousEvents = 0;
    XEvent theEvent;
    while ( XPending( display ) > 0 ) {
	XNextEvent( display, &theEvent );
	if (iv_HandleEvent( &theEvent ) != IV_OK)
	  extraneousEvents++;
    }
    if (extraneousEvents) {
	(void) fprintf(stderr,"iv_ProcessAllEvents: %d extraneous event\n", extraneousEvents);
	return IV_ERROR;
    } else {
	return IV_OK;
    }
}

int iv_UpdateVar( IVwin, varP )
    IVWindow *IVwin;
    int *varP;
{
    /* Display *display = IVwin->display; */
    /* Window  win = IVwin->w; */
    IVvarType* IVArray = IVwin->array;
    int index;

    if ( varP == (int *) IV_NO_OPT ) {
	for (index = 0; index < IVwin->numberOfVars; index++) {
	    if ( IVArray[index].type != IV_TOGGLE ) {
		ivPrintLine( IVwin, index, IV_VALUE_WIN, 0 );
	    }
	}
	return IV_OK;
    }
    if ( (index = search_IVar( IVwin, varP )) == IV_ERROR )  {
	return IV_ERROR;
    }
    if ( IVArray[index].type == IV_TOGGLE ) {
	return IV_ERROR;
    }
    ivPrintLine( IVwin, index, IV_VALUE_WIN, 0 );
    return IV_OK;
}

int iv_SetEnableFlag( IVwin, varP, flag )
    IVWindow *IVwin;
     int *varP;
     int flag;
{
    IVvarType* IVArray = IVwin->array;
    int index;
    
    if ((index = search_IVar( IVwin, varP )) == IV_ERROR)  {
	return IV_ERROR;
    } else {
	IVArray[index].enable = flag;
	if ( IVArray[index].type == IV_TOGGLE || IVArray[index].type == IV_BOOLEAN ) {
	    ivPrintLine( IVwin, index, IV_TOGGLE_WIN, 0 );
	}
	if ( IVArray[index].type != IV_TOGGLE ) {
	    ivPrintLine( IVwin, index, IV_VALUE_WIN, 0 );
	}
	return IV_OK;
    }
}
    
void iv_SetEraseFlag( IVwin, flag)
    IVWindow *IVwin;
    int flag;
{
    IVwin->eraseFlag = flag;
}

int iv_WhichSelect( IVwin, varP )
    IVWindow *IVwin;
    ivWhichItem *varP;
{
    int index;

    if ((index = search_IVar( IVwin, (int*)varP )) == IV_ERROR)  {
	return IV_ERROR;
    }
    return ivWhichSelect( IVwin, index);
}
     

int
iv_HandleEvent( theEventP )
     register XEvent *theEventP;
{
    XKeyEvent *keyEventP = (XKeyEvent*)theEventP;
    XButtonEvent *buttonEventP = (XButtonEvent*)theEventP;

    IVAssocEntry 	*theEntry;
    caddr_t		ptr;
    register int	index;
    int	 button;		/* the button being pressed */
    extern int ivKeyOk();
    extern void Timer();
    int 		WinType, varType = 0, enable = -1;
    Window w = theEventP->xany.window;
    Display *display = theEventP->xany.display;
    IVWindow *IVwin;
    IVvarType* IVArray;

    if (debugXEvents) xFormatEvent( display, theEventP );
    if ( XFindContext( display, w, globalIvContext , &ptr ) == XCNOENT ) {
	return IV_EXTRANEOUS_EVENT;
    }
    theEntry = (IVAssocEntry *) ptr;
    index = theEntry->index;
    WinType = theEntry->IVWinType;
    IVwin = theEntry->IVwin;
    IVArray = IVwin->array;

    if ( index != IV_ROOT_INDEX ) {
	enable = IVArray[index].enable;
	varType = IVArray[index].type;
    }
    switch (theEventP->type) {
    case NoExpose:
	/* XCopyArea requests generate these events which we can ignore */
	break;
    case Expose:
	{
	    XExposeEvent *exposeEvent = (XExposeEvent*)theEventP;
	    /* Ignore exposure if more exposure events are coming */
	    if (exposeEvent->count == 0) {
		if (index != IV_ROOT_INDEX && index == IVwin->editIndex && 
		    WinType == IV_VALUE_WIN) {
		    ivEditVar( IVwin, (XAnyEvent*)theEventP, index, 0);
		} else {
		    ivPrintLine(IVwin, index, WinType, 0);
		}
	    }
	}
	break;
    case KeyPress:
	keyEventP = (XKeyEvent *) theEventP;
	if ( ivKeyOk( keyEventP ) == IV_ERROR ) {
	    break;		/* Ignore this key press */
	}
	if ((WinType == IV_VALUE_WIN && enable) || IVwin->editIndex != IV_ROOT_INDEX) {
	    ivEditVar( IVwin, (XAnyEvent*)theEventP, index, 0);
	} 
	break;
    case ButtonPress:
	if( WinType == IV_TEXT_WIN || WinType == IV_ROOT_WIN ) {
	    break;
	}
	if ( WinType == IV_VALUE_WIN && enable ) {
	    ivEditVar( IVwin, (XAnyEvent*)theEventP, index, 0);
	    break;
	}
	if (varType == IV_STRING) {
	    break;
	}
	if (IVwin->editIndex != IV_ROOT_INDEX) {   /* Exit current editing window */
	    ivEditVar( IVwin, (XAnyEvent*)theEventP, index, 2);
	}
	if (!enable && !(varType == IV_WHICH)) {
	    break;
	}
	if(varType == IV_BOOLEAN) {
	    XSetWindowBackground( display, IVArray[index].ValueWin, IVwin->textFontPixel );
	    *(IVArray[index].varP.boolean) = !*(IVArray[index].varP.boolean);
	    ivPrintLine( IVwin, index, IV_VALUE_WIN, 1);
	    toggleButton( IVwin, IVArray[index].ToggleWin, TOGGLE, PUSHED);
	    do {
		XNextEvent( display, theEventP );
		if (debugXEvents) xFormatEvent( display, theEventP);
	    } while( theEventP->type != ButtonRelease );
	    XSetWindowBackground( display, IVArray[index].ValueWin, IVwin->editFontPixel );
	    ivPrintLine( IVwin, index, IV_VALUE_WIN, 0);
	    toggleButton( IVwin, IVArray[index].ToggleWin, TOGGLE, NORM);

	    ivCallFunction( IVwin, index); 	    /* Call the function associated with this variable */
	} else if( WinType == IV_TOGGLE ) {
	    toggleButton( IVwin,  IVArray[index].ToggleWin, TOGGLE, PUSHED );
	    *(IVArray[index].varP.boolean) = !*( IVArray[index].varP.boolean );
	    do {
		XNextEvent( display, theEventP);
		if (debugXEvents) xFormatEvent( display, theEventP);
	    } while(theEventP->type != ButtonRelease );
	    ivCallFunction( IVwin, index); 	    /* Call the function associated with this variable */
	    toggleButton( IVwin, IVArray[index].ToggleWin, TOGGLE, NORM );
	} else if(WinType == IV_PLUS_BUTTON || WinType == IV_MINUS_BUTTON) {
	    double tmp;
	    bool FirstIter = TRUE;

	    XSetWindowBackground( display, IVArray[index].ValueWin, IVwin->textFontPixel );

	    if (IVArray[index].PlusButton == theEventP->xany.window) {
		toggleButton( IVwin, IVArray[index].PlusButton, PLUS, PUSHED);
	    } else {
		toggleButton( IVwin, IVArray[index].MinusButton, MINUS, PUSHED);
	    }
	    while (1) {
		button = buttonEventP->button;
		/* PLUS BUTTON */
		if (IVArray[index].PlusButton == buttonEventP->window) {
		    switch (varType) {
		    case IV_DOUBLE:
			if (button == RightButton) {
			    *(IVArray[index].varP.real) *= 2.0; 
			} else if (button ==  MiddleButton) {
			    *(IVArray[index].varP.real) *= 1.1;
			} else {
			    *(IVArray[index].varP.real) *= 1.01;
			}
			break;
		    case IV_INT: 
			{			
			    tmp = *(IVArray[index].varP.integer);
			    if  (button == RightButton) {
				tmp *= 2; 
			    } else if (button == MiddleButton) {
				tmp *= 1.1;
			    } else {
				if (tmp < 0) tmp -= 1.0; else tmp += 1.0;
			    }
			    if(ABS(tmp) > MAXINT) {
				XBell ( display, (int) 1); XBell ( display, (int) 1); XFlush(display);
				tmp = (tmp > 0) ? MAXINT : -MAXINT;
			    }		    		
			    *(IVArray[index].varP.integer) = (int) tmp;	    
			    break;
			case IV_WHICH: {
			    int oldWhich;
			    oldWhich = ivWhichSelect( IVwin, index);
			    if (oldWhich == IVArray[index].st.selectCount - 1) {
				ivSetSelect( IVwin, index, oldWhich, 0);
			    } else {
				ivSetSelect( IVwin, index, oldWhich, oldWhich + 1);
			    }
			    break;
			}
			default:
			    abort();
			}
		    }
		    /* MINUS BUTTON */
		} else if (IVArray[index].MinusButton == buttonEventP->window) {
		    switch (varType) {
		    case IV_DOUBLE:
			if (button == RightButton) {
			    *(IVArray[index].varP.real) /= 2.0;
			} else if (button ==  MiddleButton) {
			    *(IVArray[index].varP.real) /= 1.1;
			} else {
			    *(IVArray[index].varP.real) /= 1.01;
			}
			break;
		    case IV_INT:
			tmp = *(IVArray[index].varP.integer);
			if  (button == RightButton) {
			    tmp /= 2; 
			} else if (button == MiddleButton) {
			    tmp /= 1.1;
			} else {
			    if (tmp < 0) tmp += 1.0; else tmp -= 1.0;
			}
			if(ABS(tmp) > MAXINT) {
			    XBell ( display, (int) 1); XBell ( display, (int) 1); XFlush( display );
			    tmp = (tmp > 0) ? MAXINT : -MAXINT;
			}		    		
			*(IVArray[index].varP.integer) = (int) tmp;	    
			break;
		    case IV_WHICH:
			{
			    int oldWhich;
			    oldWhich = ivWhichSelect( IVwin, index);
			    if (oldWhich == 0) {
				ivSetSelect( IVwin, index, oldWhich, IVArray[index].st.selectCount - 1);
			    } else {
				ivSetSelect( IVwin, index, oldWhich, oldWhich - 1);
			    }
			    break;
			}
		    default:
			abort();
		    }
		}
		ivPrintLine( IVwin, index, IV_VALUE_WIN, 1);
		if( FirstIter ) {
		    FirstIter = FALSE;
		    Timer( BUTTONWAIT * 1000 );
		} else {
		    Timer( FASTWAIT * 1000); 		/* Delay */
		}

		/* if (QLength( display ) == 0) {XSync( display, False );} */
		if ( XCheckMaskEvent( display, ButtonReleaseMask, theEventP) == True ) {
		    /* xFormatEvent( display, theEventP); */
		    break;	/* While(1) loop */
		}
	    } 
	    XSetWindowBackground( display, IVArray[index].ValueWin, IVwin->editFontPixel );
	    ivPrintLine( IVwin, index, IV_VALUE_WIN, 0);
	    ivCallFunction( IVwin, index ); /* Call the function associated with this variable */
	    if (IVArray[index].PlusButton == theEventP->xany.window) {
		ivPrintLine( IVwin, index, IV_PLUS_BUTTON, 0);
	    } else {
		ivPrintLine( IVwin, index, IV_MINUS_BUTTON, 0);
	    }
	}
	break;
    default:
	/* (void) fprintf(stderr, "Unknown event type: %x\n", theEventP->type); */
	break;
    }
    return IV_OK;		/* no more events in the queue */
}

void iv_FreeWindow( IVwin )
    IVWindow *IVwin;
{
    Display *display;
    Window  win;

    if ( IVwin == 0 ) {
	ivError("NULL pointer passed to iv_FreeWindow");
	return;
    }

    display = IVwin->display;
    win = IVwin->w;

    IVwin->numberOfVars = 0;
    FREE(IVwin->array);
    FREE(IVwin->titleString);
    XDestroyWindow( display, win );
    /* XFlush( display ); */

    if ( global_number_of_IVwindows )  {
	global_number_of_IVwindows--;
	ivTextDestroy();
    }
    FREE( IVwin );
}

/* 
 * Given a standard color name,  this routine fetches the associated
 * pixel value using XAllocNamedColor.  If it can't find or allocate
 * the color,  it returns -1.
*/
static int GetColor( display, name)
    Display *display;
    char *name;
{
    XColor def;
    XColor def_exact;
    Colormap colormap = DefaultColormap( display , 0 );

    if (XAllocNamedColor( display, colormap, name, &def, &def_exact)) {
	return def.pixel;
    } else {
	return -1;
    }
}

#define NO_COLOR	-1

/* Reads a color default - uses char *value, int temp, and char *progName */
#define RDCLR(name, pix) \
    if ( (value = XGetDefault( display, progName, (name))) ) \
    if ((temp = GetColor( display, value)) != NO_COLOR) (pix) = temp
    
    /* Reads an integer default - uses char *value, and char *progName */
#define RDINT(name, newval) \
    if ( (value = XGetDefault( display, progName, (name))) ) \
    newval = atoi(value)

/* Reads a font - uses temporary XFontStruct *tempFont - must be fixed width */
#define RDFWFNT(name, fnt) \
    if (value = XGetDefault( display, progName, name)) \
    if (tempFont = ivLoadFont(value)) \
    if (tempFont->fixedwidth) {\
       (fnt) = tempFont; \
    } else { \
        (void) sprintf(IVerrorBuffer,"Font %s is not fixed width", value); \
        ivError(IVerrorBuffer); \
        exit(1); \
    }

#define RDFNT(name, fnt) \
    if ( (value = XGetDefault( display, progName, name)) ) \
    if ( (tempFont = ivLoadFont( display,  value)) ) \
           (fnt) = tempFont;

/* Reads a flag value (basically whether or not the default is present */
#define RDFLG(name, flag) \
    if (value = XGetDefault( display, progName, name)) (flag) = 1;

/*
 * Reads X default values which override the hard-coded defaults
 * set up by InitSets.
 */
void ReadDefaults( progName, IVwin )
    char* progName;
    IVWindow *IVwin;
{
    Display *display = IVwin->display;
    /* Window  win = IVwin->w; */
    int sn = XDefaultScreen( display );

    int bwFlag = (DisplayPlanes( display, sn) < 4);
    XFontStruct *tempFont;
    char *value;
    int temp;
    
    if ( (value = XGetDefault( display, progName, "iv.EraseValue")) ) {
	if (!strncmp(value, "on", 2)) {
	    IVwin->eraseFlag = TRUE;
	} else {
	    IVwin->eraseFlag = FALSE;
	}
    }
    
    RDFNT("iv.TitleFont", titleFont);
    RDFNT("iv.TextFont", textFont);
    RDFNT("iv.EditFont", numberFont);

    RDINT("iv.BorderWidth", IVbdrWidth);
    RDINT("iv.Padding", IVrowPadding);
    if (!bwFlag) {
	RDCLR("iv.Background", IVwin->bgPixel);
	RDCLR("iv.EditBackground", IVwin->editBgPixel);
	RDCLR("iv.EditFontColor", IVwin->editFontPixel);
	RDCLR("iv.TextFontColor", IVwin->textFontPixel);
	RDCLR("iv.TitleFontColor", IVwin->titleFontPixel);
	RDCLR("iv.CursorColor", IVwin->cursorPixel);
	RDCLR("iv.ButtonColor", IVwin->buttonPixel);
	RDCLR("iv.BorderColor", IVwin->bdrPixel);
    }
}

