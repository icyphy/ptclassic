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
 * VEM Window Table Management
 * 
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This module maintains a number of hash tables that map window
 * identifiers to internal window structures.  Three tables are
 * maintained:  active windows,  inactive (iconified) windows,
 * and interrupted windows.  VEM windows actually consist of
 * three windows: the main window,  the title bar,  and the
 * graphics window.  All three are moved as a unit when they
 * move between the tables.  Routines are exported for adding,
 * moving, and deleting internal structures from these tables.
 */

#include "wininternal.h"
#include "errtrap.h"
#include "st.h"

/*
 * All of the tables map (Window) into (internalWin *).
 */

/* Mapped windows */
static st_table *wnActiveWins = (st_table *) 0;

/* Unmapped windows */
static st_table *wnUnmapWins = (st_table *) 0;

/* Interrupted windows */
static st_table *wnIntrWins = (st_table *) 0;



static void wnInitTables()
/*
 * Allocates necessary tables
 */
{
    /* Make new table with standard numerical compare and hash functions */
    wnActiveWins = st_init_table(st_numcmp, st_numhash);

    /* Same for unmapped window table */
    wnUnmapWins = st_init_table(st_numcmp, st_numhash);

    /* Same for deactivated window table */
    wnIntrWins = st_init_table(st_numcmp, st_numhash);

}



void _wnEnter(realwin, winstate)
internalWin *realwin;		/* Internal representation of window */
int winstate;			/* WN_ACTIVE, WN_INACTIVE, WN_INTR   */
/*
 * Enters the specified window into the indicated hash table.  All
 * three of the windows are entered into the table.
 */
{
    if (!wnActiveWins) wnInitTables();
    switch (winstate) {
    case WN_ACTIVE:
	(void) st_insert(wnActiveWins, (char *) realwin->mainWin,
			 (char *) realwin);
	(void) st_insert(wnActiveWins, (char *) realwin->XWin,
			 (char *) realwin);
	(void) st_insert(wnActiveWins, (char *) realwin->TitleBar,
			 (char *) realwin);
	break;
    case WN_INACTIVE:
	(void) st_insert(wnUnmapWins, (char *) realwin->mainWin,
			 (char *) realwin);
	(void) st_insert(wnUnmapWins, (char *) realwin->XWin,
			 (char *) realwin);
	(void) st_insert(wnUnmapWins, (char *) realwin->TitleBar,
			 (char *) realwin);
	break;
    case WN_INTR:
	(void) st_insert(wnIntrWins, (char *) realwin->mainWin,
			 (char *) realwin);
	(void) st_insert(wnIntrWins, (char *) realwin->XWin,
			 (char *) realwin);
	(void) st_insert(wnIntrWins, (char *) realwin->TitleBar,
			 (char *) realwin);
	break;
    default:
	errRaise(wn_pkg_name, VEM_BADARG, "in wnEnter: bad wintype argument");
	break;
    }
}



int _wnFind(win, realwin, winstate, wintype)
Window win;			/* Window to lookup   */
internalWin **realwin;		/* Returned structure */
int *winstate;			/* Return: WN_ACTIVE, WN_INACTIVE, WN_INTR */
int *wintype;			/* Return: WN_MAIN, WN_GRAPH, WN_TITLE     */
/*
 * Returns a non-zero status if `win' is found in one of the tables
 * maintained by this module.  The state and type of the window
 * is returned in `winstate' and `wintype' (which may be zero if
 * the caller isn't interested in this information).
 */
{
    if (!wnActiveWins) wnInitTables();
    *realwin = (internalWin *) 0;
    if (st_lookup(wnActiveWins, (char *) win, (char **) realwin)) {
	if (winstate) *winstate = WN_ACTIVE;
    } else if (st_lookup(wnUnmapWins, (char *) win, (char **) realwin)) {
	if (winstate) *winstate = WN_INACTIVE;
    } else if (st_lookup(wnIntrWins, (char *) win, (char **) realwin)) {
	if (winstate) *winstate = WN_INTR;
    } else {
	return 0;
    }
    if (*realwin) {
	if (win == (*realwin)->mainWin) {
	    if (wintype) *wintype = WN_MAIN;
	} else if (win == (*realwin)->XWin) {
	    if (wintype) *wintype = WN_GRAPH;
	} else if (win == (*realwin)->TitleBar) {
	    if (wintype) *wintype = WN_TITLE;
	} else {
	    errRaise(wn_pkg_name, VEM_CORRUPT, "in _wnFind: corrupt tables");
	}
    }
    return *realwin != (internalWin *) 0;
}




void _wnOut(realwin)
internalWin *realwin;		/* Window to remove */
/*
 * Removes all references to `realwin' in all tables.
 */
{
    Window key;
    internalWin *rtn;

    if (!wnActiveWins) wnInitTables();
    key = realwin->mainWin;
    if (st_delete(wnActiveWins, (char **) &key, (char **) &rtn)) {
	key = realwin->XWin;
	st_delete(wnActiveWins, (char **) &key, (char **) &rtn);
	key = realwin->TitleBar;
	st_delete(wnActiveWins, (char **) &key, (char **) &rtn);
    } else if (st_delete(wnUnmapWins, (char **) &key, (char **) &rtn)) {
	key = realwin->XWin;
	st_delete(wnUnmapWins, (char **) &key, (char **) &rtn);
	key = realwin->TitleBar;
	st_delete(wnUnmapWins, (char **) &key, (char **) &rtn);
    } else if (st_delete(wnIntrWins, (char **) &key, (char **) &rtn)) {
	key = realwin->XWin;
	st_delete(wnIntrWins, (char **) &key, (char **) &rtn);
	key = realwin->TitleBar;
	st_delete(wnIntrWins, (char **) &key, (char **) &rtn);
    }
}



void _wnChange(realwin, winstate)
internalWin *realwin;		/* Window record     */
int winstate;			/* New window state  */
/*
 * Changes all windows of `realwin' to the state `winstate'.
 */
{
    if (!wnActiveWins) wnInitTables();
    _wnOut(realwin);
    _wnEnter(realwin, winstate);
}



void _wnForeach(winstates, func, arg)
int winstates;			/* What states to examine */
enum st_retval (*func)();	/* Function               */
char *arg;			/* User data              */
/*
 * Visits each internalWin structure in the tables given
 * by `winstates' and calls `func' with the arguments:
 *   (*func)(key, value, arg)
 *   char *key;    (Window)
 *   char *value;  (internalWin *)
 *   char *arg;    (Same as above)
 * The called function should return the same values
 * as that described for st_foreach.  Exception:  ST_DELETE
 * isn't supported.
 */
{
    st_generator *gen;
    Window win;
    internalWin *realwin;
    enum st_retval rtn;

    rtn = ST_STOP;

    if (!wnActiveWins) wnInitTables();
    if (winstates & WN_ACTIVE) {
	gen = st_init_gen(wnActiveWins);
	while (st_gen(gen, (char **) &win, (char **) &realwin)) {
	    if (win == realwin->XWin) {
		rtn = (*func)((char *) win, (char *) realwin, (char *) arg);
		if (rtn == ST_CONTINUE) {
		    /* Nothing */
		} else if (rtn == ST_STOP) {
		    break;
		} else if (rtn == ST_DELETE) {
		    errRaise(wn_pkg_name, VEM_BADARG,
			     "in wnForeach: unsupported return value");
		} else {
		    errRaise(wn_pkg_name, VEM_BADARG,
			     "in wnForeach: unknown return value");
		}
	    }
	}
	st_free_gen(gen);
	if (rtn == ST_STOP) winstates = 0;
    }
    if (winstates & WN_INACTIVE) {
	gen = st_init_gen(wnUnmapWins);
	while (st_gen(gen, (char **) &win, (char **) &realwin)) {
	    if (win == realwin->XWin) {
		rtn = (*func)((char *) win, (char *) realwin, (char *) arg);
		if (rtn == ST_CONTINUE) {
		    /* Nothing */
		} else if (rtn == ST_STOP) {
		    break;
		} else if (rtn == ST_DELETE) {
		    errRaise(wn_pkg_name, VEM_BADARG,
			     "in wnForeach: unsupported return value");
		} else {
		    errRaise(wn_pkg_name, VEM_BADARG,
			     "in wnForeach: unknown return value");
		}
	    }
	}
	st_free_gen(gen);
	if (rtn == ST_STOP) winstates = 0;
    }
    if (winstates & WN_INTR) {
	gen = st_init_gen(wnIntrWins);
	while (st_gen(gen, (char **) &win, (char **) &realwin)) {
	    if (win == realwin->XWin) {
		rtn = (*func)((char *) win, (char *) realwin, (char *) arg);
		if (rtn == ST_CONTINUE) {
		    /* Nothing */
		} else if (rtn == ST_STOP) {
		    break;
		} else if (rtn == ST_DELETE) {
		    errRaise(wn_pkg_name, VEM_BADARG,
			     "in wnForeach: unsupported return value");
		} else {
		    errRaise(wn_pkg_name, VEM_BADARG,
			     "in wnForeach: unknown return value");
		}
	    }
	}
	st_free_gen(gen);
	if (rtn == ST_STOP) winstates = 0;
    }
}
