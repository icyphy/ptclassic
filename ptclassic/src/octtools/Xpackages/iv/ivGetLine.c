#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/

/* ivGetLine.c --
 *
 *     Benjamin Chen
 *     (Original by George Carvalho)
 *     (Prof. E. S. Kuh)
 *
 * Copyright (C) 1988-1994 Regents of the University of California
 * All rights reserved.
 */

#include "copyright.h"
#include "port.h"
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include "ualloc.h"
#include "cursor.bitmap.11"
#include "iv.h"
#include "ivBuf.h"

/* Can't place this in ivBuf.h, otherwise hppa.cfront cc won't compile
   ivBuf.c   
 */
void ivBufAddChar
        ARGS((ivBuf* buf, char c));

#include "xformatevent.h"

#include "ivGetLine.h"

#ifdef HAS_TERMIOS
#include <termios.h>
#endif

#define IVRINGBELL( d ) { XBell ((d),50); XFlush( (d) ); }

#define BACKSPACE       '\010'
#define BELL            '\07'	     /* (control-g) ring the bell !!         */
#define CARRIAGE_RETURN '\015'
#define CONTROL_L       '\014'	     /* Redraw the window the mouse is in.   */
#define CONTROL_U       '\025'	     /* Erase to the beginning of the line.  */
#define CONTROL_W       '\027'	     /* Erase the last word on the line.     */
#define DELETE          '\177'
#define NULL_CHAR       '\0'


static int debugXEvents = 0;

char ezEofChar;                     /* users end-of-file character          */
char ezIntrChar;                    /* users interrupt character            */
char ezQuitChar;                    /* users quit character                 */

int ivTextInit( )
{
#ifdef HAS_TERMIOS
    struct termios tty;

    if ( tcgetattr( fileno(stdin), &tty) < 0 ) {
        return IV_ERROR;
    } else {
        ezEofChar = tty.c_cc[VEOF];
        ezIntrChar = tty.c_cc[VINTR];
        ezQuitChar = tty.c_cc[VQUIT];
	return IV_OK;
    }
#else
    struct tchars tty;

    if ( ioctl( fileno(stdin), TIOCGETC, (char*)&tty) < 0 ) {
        return IV_ERROR;
    } else {
        ezEofChar = tty.t_eofc;
        ezIntrChar = tty.t_intrc;
        ezQuitChar = tty.t_quitc;
	return IV_OK;
    }
#endif
}


/*
 * This routine draws the cursor in the proper spot at curLoc+1.
 */
static void drawCursor( IVwin, win, curLoc, gc)
    IVWindow *IVwin;
    Window win;			/* Window to draw cursor in */
    int curLoc;
    GC  gc;
{
    Display *display = IVwin->display;
    Screen *screen = XDefaultScreenOfDisplay(display);
    XWindowAttributes winInfo;
    static Pixmap getLnPixmap = (Pixmap) 0;

    if (! getLnPixmap ) {
	getLnPixmap = XCreatePixmapFromBitmapData(display, win,
		               cursor_bits, cursor_width, cursor_height,
			       IVwin->editFontPixel, IVwin->textFontPixel,
			       XDefaultDepthOfScreen(screen));
    }
    XGetWindowAttributes( display, win, &winInfo);
    XCopyArea( display, getLnPixmap, win, gc,  0, 0, 
	      cursor_width, cursor_height,
	      curLoc+1, (winInfo.height-cursor_height-2) );
    /* VOID */
}

int ivKeyOk(keyEventPtr)
     XKeyEvent   *keyEventPtr;
{
    Display     *display = keyEventPtr->display;
    char xTextPtr[10], c;
    KeySym keysym;

    /* Is the (XEvent *) cast right? */
    if (debugXEvents) xFormatEvent( display, (XEvent *)keyEventPtr);

    XLookupString( keyEventPtr, xTextPtr , 1, &keysym, NULL);
    if ( IsCursorKey( keysym ) || IsModifierKey( keysym ) || IsFunctionKey( keysym) ) {
	return IV_ERROR;
    }
    c = xTextPtr[0];
    return (int)c;
      
}

/*
 * Note that if the user types his "interrupt", "end-of-file", or "quit"
 * characters 'buffer' will contain EOF.
 */
int ivGetLine( IVwin, keyEventPtr, x, y, bufferSize, keepGoing)
    IVWindow    *IVwin;
    XKeyEvent   *keyEventPtr;
    int         x, y;
    int         bufferSize;
    bool        keepGoing;
{
    ivBuf* 	buffer = &IVwin->buffer;
    Display     *display = keyEventPtr->display;
    XEvent      report;
    int         k;
    int  	textLength = 0;
    int  	curLoc, status = IV_ERROR;
    char        xTextPtr[BUFSIZ];
    char	c;

    do {        
	textLength = XLookupString( keyEventPtr, xTextPtr , 1, NULL, NULL);
        if (textLength > BUFSIZ) {
            IVRINGBELL( display );
	}
	for (k = 0; k < textLength ;k++) {
            c = xTextPtr[k];
            if (c == BACKSPACE || c == DELETE) {
		ivBufDelChar( buffer );
            } else if (c == BELL) {
		IVRINGBELL( display );
            } else if (c == CONTROL_L) { /* Redraw the window the mouse is in. */
	    } else if (c == CONTROL_U) { /* Undo and restore old value. */
		ivBufClear( buffer );
		ivBufAddString( buffer, ivBufGetString(&IVwin->oldBuffer) );
	    } else if (c == CONTROL_W) {
		ivBufDelWord( buffer );
	    } else if (c == ezEofChar || c == ezIntrChar || c == ezQuitChar) {
		ivBufAddChar( buffer, EOF );
		continue;
	    } else if (c == CARRIAGE_RETURN) {
		keepGoing = FALSE;
		status = IV_OK;
		break;
	    } else if (c == '\t' || c == ' ') {
		ivBufAddChar( buffer, ' ' );
	    } else {
		ivBufAddChar( buffer, c );
	    }
	}
	
	{
	    char* string = ivBufGetString( buffer );
	    int   length = ivBufGetLength( buffer );
	    GC gc = IVwin->editValueGC;
	    Window w = IVwin->theWin ;

	    XClearWindow( display, w  );
	    XDrawImageString( display, w, gc, x, y, string, length );
	    curLoc = XTextWidth( IVwin->valueFont, string, length );
	    drawCursor( IVwin, IVwin->theWin, curLoc, gc );
	    if ( keepGoing ) {	/* Wait for next useful event */
		while (1) {
		    XNextEvent( display, &report );
		    if (debugXEvents) xFormatEvent( display, &report);
		    if ( report.xany.type == Expose  ) {
			XPutBackEvent( display, &report );
			return report.xany.type; /* Notice RETURN here */
		    } else if ( report.xany.type == NoExpose ) {
			continue; /* Ignore this event and continue looping */
		    } else {
			break;	/* Exit loop and handle event */
		    }
		}
		keyEventPtr = (XKeyEvent *) &report;
	    }
	}
    } while ( keepGoing );

    return (status == IV_OK ) ? IV_OK : IV_EXTRANEOUS_EVENT;
}



void ivTextDestroy() 
{
    /* XFreePixmap(getLnPixmap); */
}
