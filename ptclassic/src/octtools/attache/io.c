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
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "options.h"                /* PBP - MCC 12/09/89 */
#include <curses.h>
#include "errtrap.h"                /* PBP - MCC 12/09/89 */
#include "io.h"

char eraseChar = '\b';
char wordEraseChar = CNTRL('W');
char lineKillChar = CNTRL('U');

static char *wrapString = 0;
static int wrapMargin = 0;

IOinit()
{
    if ((int) initscr() == ERR) {
	/* PBP - MCC 12/09/89 : Don't call exit() in case attache
	 * was invoked in another program. */
	errRaise(optProgName, 0, "can't initialize curses");
    }
#if defined(hpux)
    nonl();
    cbreak();
    noecho();
    eraseChar = erasechar();
    lineKillChar = killchar();
#else
    crmode();
    noecho();
#ifdef _IBMR2
    eraseChar = '\010';
    lineKillChar = '\025';
#else
    eraseChar = _tty.sg_erase;
    lineKillChar = _tty.sg_kill;
#endif
#endif
}

IOend()
{
    IObotHome();
    IOrefresh();
    endwin();
    putchar('\n');
}

IOgetchar()
{
    int ch;

    ch = getch();
    if (ch == ERR) {
	return(EOF);
    } else {
	return(ch);
    }
}

IOputchar(ch)
int ch;
{
    int x, y;

    switch(ch) {
	case '\t':
	    IOgetyx(&y, &x);
	    do {
		IOputchar(' ');
	    } while ((++x % 8) != 0);
	    break;
	case '\n':
	    addch(ch);
	    break;
	case CNTRL('K'):
	    IOgetyx(&y, &x);
	    IOmove(y-1, x);
	    break;
	default:
	    IOgetyx(&y, &x);
	    if (x < COLS-1) addch(ch);
    }
}

IOputs(str)
char *str;
{
    while (*str != '\0') {
	IOputchar(*str++);
    }
}

IOwrap(str)
char *str;
{
    char *cp;

    wrapString = str;
    if (str) {
	cp = strchr(str, '\n');
	if (cp) {
	    wrapMargin = cp - str + 1;
	} else {
	    wrapMargin = -10000;
	}
    }
}

IOputcharSee(ch)
int ch;
{
    int x, y;

    if (isprint(ch)) {
	IOgetyx(&y, &x);
	if (wrapString && x >= COLS - wrapMargin) {
	    IOputs(wrapString);
	} else {
	    IOputchar(ch);
	}
    } else {
	switch (ch) {
	    case '\n':
		IOputs(wrapString ? wrapString : "\\n");
		break;
	    case '\r':
		IOputsSee("\\r");
		break;
	    case '\t':
		IOputsSee("\\t");
		break;
	    default:
		if (isprint(CNTRL(ch))) {
		    IOputcharSee('^');
		    IOputcharSee(CNTRL(ch));
		} else {
		    IOputcharSee('\\');
		    IOputcharSee((ch/64) % 8);
		    IOputcharSee((ch/8) % 8);
		    IOputcharSee(ch % 8);
		}
	}
    }
}

IOputsSee(str)
char *str;
{
    while (*str != '\0') {
	IOputcharSee(*str++);
    }
}

IOhome()
{
    move(0, 0);
}

IObotHome()
{
    move(LINES-1, 0);
}

IOmove(y, x)
int y, x;
{
    move(y, x);
}

IOgetyx(yPtr, xPtr)
int *yPtr, *xPtr;
{
    getyx(stdscr, *yPtr, *xPtr);
}

IOstandout()
{
    standout();
}

IOstandend()
{
    standend();
}

IOclearScreen()
{
    int i;

    IOhome();
    for (i = 1; i < LINES; i++) IOputchar('\n');
}

IOforceClear()
{
    clear();
}

IOclearLine()
{
    clrtoeol();
}

IOrefresh()
{
    refresh();
}
