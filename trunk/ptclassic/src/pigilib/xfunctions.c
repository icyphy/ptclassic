/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
 * X11 window interface routines for ptolemy.
 * Ported from version 1.4 of xfunctions.c for Gabriel (of 6/20/90)
 * All are designed to do something reasonable if X is not running.
 */

/*
 * Copyright 1989, 1990 The Regents of the University of California.
 * All Rights Reserved.
 */

#ifndef lint
static char	sccsid_xfunctions[] = "$Id$";
#endif

#include <X11/Xlib.h>    /* declarations for X library routines */
#include <X11/Xutil.h>

#include <stdio.h>


/* message window types
 *
 * NOTIFY windows are opened by win_msg(msg) and dissappear after
 * user input.
 *
 * MESSAGE windows are opened by open_msg(msg) and disappear only
 * after a close_msg(window) call.  Note that open_msg returns the
 * window id for close_msg.
 *
 * QUERY windows are opened by query(msg) and return 0 for yes or
 * 1 for no after user input.
 */
#define NOTIFY		0
#define MESSAGE		1
#define QUERY		2

#define FONT_WIDTH(font) ((font)->max_bounds.width)
#define FONT_HEIGHT(font) ((font)->ascent + (font)->descent)

/* message window text boundaries */
#define X_BORDER	10
#define Y_BORDER	2

/* message window border width */
#define WINDOW_BORDER	2

/* minimun width of the query window (in chars) */
#define MIN_QUERY_WIDTH	8

/* char used to break a line in a message */
/* was '$' in ggi; we use the more natural '\n' */
#define line_break	'\n'

/* number of columns per tab */
#define TAB_WIDTH	8

#define exit_msg "Press any button or key in window."

/* maximum total number of characters that can be involved
 * in a profile request that uses x windows
 */
#define max_profile	2000

/* global string used to accumulate the characters for a profile */
static char profile[max_profile];

/* X related variables */
static Display *msgDisplay;
static XFontStruct *msgFont;
static GC msgGC, msgInvGC;
static unsigned long msgForeground, msgBackground;

static int line_count();
static int win_msg_event();
static Window open_msg_type();

extern void win_msg(), close_msg();

struct string {
	short	t, m;
	short	ST_hasfillp;
	short	ST_adjustable;
	int	ST_dim;
	int	ST_fillp;
	char	*ST_self;
};
#define Qst(p)			((struct string *)((p) - T_OTHER))
#define st_hasfillp(p)		(Qst(p)->ST_hasfillp)
#define st_adjustable(p)	(Qst(p)->ST_adjustable)
#define st_dim(p)		(Qst(p)->ST_dim)
#define st_fillp(p)		(Qst(p)->ST_fillp)
#define st_self(p)		(Qst(p)->ST_self)
#define st_uself(p)		(*(unsigned char **)&st_self(p))

/*******************************************************************

				open_display

********************************************************************

  This function opens the display for X windows output, if possible.
  It returns a 1 if successful, a 2 otherwise.
  It cannot be relied on to fail when there is in fact no x server available.

  If Ptolemy aquires an exit command, then close_display should be called.
*/

int
open_display(display)
	char *display;
{
	char *msg = "No X window display available.  Using standard output.";
	XGCValues xgcv;

	/* Open the display */
	if ((msgDisplay = XOpenDisplay(display))==NULL) {
		fputs(msg, stdout);
		return 2;
	}

	/* Display successfully opened.  Now open the font. */
	msgFont = XLoadQueryFont(msgDisplay, "9x15");
	if (msgFont == (XFontStruct *)NULL) {
		/* try server default fixed font */
		msgFont = XLoadQueryFont(msgDisplay, "fixed");
		if (msgFont == (XFontStruct *)NULL)
			return 2;
	}

	msgForeground = BlackPixel(msgDisplay, DefaultScreen(msgDisplay));
	msgBackground = WhitePixel(msgDisplay, DefaultScreen(msgDisplay));

	xgcv.font = msgFont->fid;
	xgcv.foreground = msgForeground;
	xgcv.background = msgBackground;
	msgGC = XCreateGC(msgDisplay, DefaultRootWindow(msgDisplay),
			  GCFont|GCForeground|GCBackground, &xgcv);
	xgcv.foreground = msgBackground;
	xgcv.background = msgForeground;
	msgInvGC = XCreateGC(msgDisplay, DefaultRootWindow(msgDisplay),
			     GCFont|GCForeground|GCBackground,
			     &xgcv);

	return 1;
}

void
close_display()
{
	if (msgDisplay) {
		XFreeFont(msgDisplay, msgFont);
		XCloseDisplay(msgDisplay);
		msgDisplay = (Display *)NULL;
	}
}

/*******************************************************************

				accum_string

********************************************************************
  This function accumulates strings passed to it in the global
  string "profile" for later printing using pr_accum_string
*/

int
accum_string(string)
	char *string;
{
	int length, left;
#define TOO_LONG "$...$PROFILE TOO LONG!$..."

	length = strlen(string);
	left = (max_profile - 1) - strlen(profile);

	/* if the strings are collectively too long, append an
	 * error message.
	 */
	if (length > left) {
		strncat(profile, string, left);
		strcpy(profile + max_profile - (sizeof (TOO_LONG) + 2),
		       TOO_LONG);
		return 1;
	}
	strcat(profile, string);
	return 0;
}

/*******************************************************************

				pr_accum_string

********************************************************************
  This function prints the accumulated string "profile".
*/
void
pr_accum_string()
{
	(void)win_msg_event(profile, NOTIFY);
}

/*******************************************************************

				clr_accum_string

********************************************************************
  This function clears the accumulated string "profile".
*/
void
clr_accum_string()
{
	profile[0] = '\0';
}


/*******************************************************************

                       write_message_type

********************************************************************

    The following subroutine writes the message, character by
    character, inserting newlines as appropriate.
    The type of message determines whether any additional messages
    are printed at the bottom of the window.
*/

#define WINDOW_WIDTH 100

static void
write_message_type(w, msg, type)
	Window w;
	char *msg;
	int type;
{
	int y, c, col;
	char buf[ WINDOW_WIDTH ], *ptr = msg;

	y = FONT_HEIGHT(msgFont) + Y_BORDER;

	for (;;) {
		for (col = 0; (col < WINDOW_WIDTH) &&
		   (c = buf[col] = *ptr++); col++) {
			if (c == line_break)
				break ;
			else if (c == '\t') {
				int fill;

				fill = TAB_WIDTH - (col % TAB_WIDTH);
				while (--fill >= 0)
					buf[col++] = ' ';
				--col;
			}
		}
		XDrawString(msgDisplay, w, msgGC, X_BORDER, y, buf, col);
		y += FONT_HEIGHT(msgFont);

		if (c == '\0') break ;
	}

	switch (type) {
	case NOTIFY:
		XDrawString(msgDisplay, w, msgGC, X_BORDER, y,
			    exit_msg, strlen(exit_msg));
		break ;
	case QUERY: {
		int width;

		(void)line_count(msg, &width);
		if (width < MIN_QUERY_WIDTH) width = MIN_QUERY_WIDTH;
		XDrawImageString(msgDisplay, w, msgInvGC,
				 X_BORDER, y, "YES", 3);
		XDrawImageString(msgDisplay, w, msgInvGC,
				 (width - 2) * FONT_WIDTH(msgFont) + X_BORDER,
				 y, "NO", 2);
	}
	default:
		break ;
	}
}


/*******************************************************************

				win_msg

********************************************************************

  This function pops an X window, if possible, with a multi line message.
  The window is destroyed when the user responds with some action in it.
  It is assumed that open_display has been called.
  The lines are separated by the special character line_break.
*/

void
win_msg(omsg)
char * omsg;
{
	(void)win_msg_event(omsg, NOTIFY);
}

static int
win_msg_event(msg, type)
	char *msg;
	int type;
{
	Window w;
	int done = 0, code = 0;
	XEvent event;   /* input event */

	w = open_msg_type(msg, type);

#define EVENT_MASK KeyPressMask|ButtonPressMask|ButtonReleaseMask|ExposureMask

	/* Select input from keyboard  & Mouse */ 
	XSelectInput(msgDisplay, w, EVENT_MASK);

	/* Wait for a key or button to be pressed */
	while (!done) {
		XWindowEvent(msgDisplay, w, EVENT_MASK, &event);

		switch (event.type) {
		case Expose:
			/* need to refresh the window */
			write_message_type(w, msg, type);
			break;
		case KeyPress:
			if (type != QUERY) done = 1;
			break ;
		case ButtonPress:
			if (type == QUERY) {
				int x, width;

				(void)line_count(msg, &width);
				if (width < MIN_QUERY_WIDTH)
				    width = MIN_QUERY_WIDTH;
				width = width * FONT_WIDTH(msgFont)
					+ 2 * X_BORDER;
				x = ((XButtonPressedEvent *)&event)->x;
				code = (x >= width/2);
			}
			break;
		case ButtonRelease:
			done = 1;   /* exit wait loop */
			break ;
		}
	}
	close_msg(w);

	return code;
}

/*******************************************************************

				open_msg

********************************************************************

  Calls open_msg_type with type set to MESSAGE.

*/

Window
open_msg(msg)
char *msg;
{
	Window w;

	if ((w = open_msg_type(msg, MESSAGE)) != (Window)NULL)
		write_message_type(w, msg, MESSAGE);
	return w;
}


/* line_count returns the number of lines in msg (determined by counting
 * line_break characters) and also returns the width, in characters,
 * of the longest line via width_ptr.
 */
static int
line_count(msg, width_ptr)
	char *msg;
	int *width_ptr;
{
	int c, rows, cols, max;

	rows = 1;
	cols = max = 0;
	while (c = *msg++) {
		switch (c) {
		case line_break:
			rows++;
			if (cols > max) max = cols;
			cols = 0;
			break ;
		case '\t':
			cols += TAB_WIDTH - (cols % TAB_WIDTH);
			break ;
		default:
			cols++;
			break ;
		}
		if(cols >= WINDOW_WIDTH) {
			cols = 0;
			rows++;
			max = WINDOW_WIDTH;
		}
	}
	if (cols > max) max = cols;

	*width_ptr = max;
	return rows;
}

/*******************************************************************

				open_msg_type

********************************************************************

  This function pops an X window, if possible, with a multi line message.
  The window identifier is returned.
  The window is not desctroyed until close_msg is called.
  It is assumed that open_display has been called.
  The lines are separated by the special character line_break.
    If flag is zero, instructions appear at the bottom of the
    window to press a key.  Otherwise, no such instructions
    appear.
*/

static Window
open_msg_type(msg, type)
	char *msg;
	int type;
{
	Window w;
	XSizeHints hints;
	XWMHints wmhints;
	XEvent event;
	int window_w, window_h, window_x, window_y;

	window_h = line_count(msg, &window_w);
	switch (type) {
	case NOTIFY:
		if (window_w < strlen(exit_msg)) window_w = strlen(exit_msg);
		++window_h;
		break ;
	case QUERY:
		if (window_w < MIN_QUERY_WIDTH) window_w = MIN_QUERY_WIDTH;
		++window_h;
		break ;
	default:
		break ;
	}

	window_h = window_h * FONT_HEIGHT(msgFont) + 3 * Y_BORDER;
	window_w = window_w * FONT_WIDTH(msgFont) + 2 * X_BORDER;

	window_x = DisplayWidth(msgDisplay, DefaultScreen(msgDisplay)) / 2
	    - window_w / 2;
	window_y = DisplayHeight(msgDisplay, DefaultScreen(msgDisplay)) / 2
	    - window_h / 2;

	w = XCreateSimpleWindow(msgDisplay, DefaultRootWindow(msgDisplay),
				window_x, window_y,
				window_w, window_h,
				WINDOW_BORDER,
				msgForeground, msgBackground);
	if (w == (Window)NULL) return 0;

	XStoreName(msgDisplay, w, "Ptolemy message window");

	wmhints.flags = InputHint|StateHint;
	wmhints.input = True;
	wmhints.initial_state = NormalState;
	XSetWMHints(msgDisplay, w, &wmhints);

	hints.x = window_x;
	hints.y = window_y;
	hints.min_width = hints.width = window_w;
	hints.min_height = hints.height = window_h;

#ifdef PBaseSize		/* basically "#if X11R4" */
	hints.flags = PBaseSize|PPosition|USPosition|PSize|PMinSize;
	hints.base_width = window_w;
	hints.base_height = window_h;
	XSetWMNormalHints(msgDisplay, w, &hints);
#else				/* older X11s */
	hints.flags = PPosition|USPosition|PSize|PMinSize;
	XSetNormalHints(msgDisplay, w, &hints);
#endif
	    
	XSelectInput(msgDisplay, w, ExposureMask);
	XMapWindow(msgDisplay, w);      /* Map the window */
	XWindowEvent(msgDisplay, w, ExposureMask, &event);

	write_message_type(w, msg, type);    /* display the message */
	XFlush(msgDisplay);

	return w;
}


/*******************************************************************

				close_msg

********************************************************************

  This function destroys the given window.
*/

void
close_msg(w)
	Window w;
{
	XDestroyWindow(msgDisplay, w);
	XFlush(msgDisplay);   
}


/*******************************************************************

			NoXquery

********************************************************************

   The following function replaces query when query fails to open
   an X window.
*/

int
noxquery(omsg)       /* print an error message and exit */
	char * omsg;
{
	char buf[256];

	fputs(omsg, stdout);
	fputs(" (y or n)", stdout);
	fflush(stdout);
	if (fgets(buf, sizeof (buf), stdin))
		return (buf[0] == 'y' ? 0 : 1);
	return 1;
}

/*******************************************************************

			query

********************************************************************

   The following function brings up a query window with a "yes-no"
   prompt and returns a 0 if the answer is yes and a 1 if the answer
   is no.
   It is assumed that open_display has been called.
*/

int
query(omsg)
	char * omsg;
{
	if (msgDisplay) return win_msg_event(omsg, QUERY);
	else return noxquery(omsg);
}
