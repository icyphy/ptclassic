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
 * General Symbolic Functions
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains general purpose functions used for setting
 * certain state in the symbolic package.
 */

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#define _VA_LIST
#include "port.h"

#include "uprintf.h"
#include "internal.h"

char *sym_pkg_name = "sym";
char *sym_str_zero = "";
int symVerbose = 0;
#define MAX_AREA	2048

static void defWarning();

static symWarnFunc symMsgFunc = { defWarning, (char *) 0 };

void symSetWarnFunc(new, old)
symWarnFunc *new;		/* New warning function */
symWarnFunc *old;		/* Old warning function */
/*
 * This routine sets the warning function for the symbolic policy
 * library.  This function is called when a non-fatal error is
 * detected or a questionable condition arises.  Fatal errors
 * are signalled using the errTrap package.  The function
 * has the following form:
 *   void func(severity, message, data)
 *   symSeverity severity;
 *   char *message;
 *   char *data;
 * The parameter `severity' will indicate the severity of the error.
 * `message' will contain text describing the error or warning.
 * `data' is the same as that provided by the user.  If `old' is
 * non-zero, the old function is returned so that it can be reinstated
 * at some later time if desired.  The default function prints the
 * messages to standard error.
 */
{
    if (old) {
	*old = symMsgFunc;
    }
    symMsgFunc = *new;
}

/*ARGSUSED*/
static void defWarning(severity, message, data)
symSeverity severity;		/* Severity of error  */
char *message;			/* Text message       */
char *data;			/* User data (unused) */
/*
 * This is the default message handling function for the
 * symbolic library.  It produces messages to standard error.
 */
{
    switch (severity) {
    case SYM_INFO:
	(void) fprintf(stderr, "Symbolic policy message:\n  %s\n", message);
	break;
    case SYM_WARN:
	(void) fprintf(stderr, "Warning from symbolic policy library:\n  %s\n",
		       message);
	break;
    case SYM_ERR:
	(void) fprintf(stderr, "ERROR detected in symbolic policy library:\n  %s\n",
		       message);
	break;
    default:
	(void) fprintf(stderr, "Symbolic policy library message:\n  %s\n",
		       message);
	break;
    }
}


/*
 * Self parsing message handler for the symbolic package
 */

#ifndef lint

#ifdef __STDC__
void symMsg(symSeverity sev, char *fmt, ...)
#else
/*VARARGS2*/
void symMsg(va_alist)
va_dcl
#endif
/*
 * Format: symMsg(symSeverity flags, char *format, ...)
 * Calls the settable symbolic message function with the formatted
 * output string.  The severity is passed unchanged.
 */
{
    va_list ap;
    static char area[MAX_AREA];
#ifdef __STDC__
    va_start(ap, fmt);
#else
    char *fmt;
    symSeverity sev;

    va_start(ap);
    sev = va_arg(ap, symSeverity);
    fmt = va_arg(ap, char *);
#endif
    uprintf(area, fmt, &ap);
    (*symMsgFunc.func)(sev, area, symMsgFunc.data);
}

#endif /* lint not defined */



int symLessAttach(obj, contents_p, mask, count)
octObject *obj;			/* Object to examine       */
int contents_p;			/* If non-zero, contents   */
octObjectMask mask;		/* What objects to look at */
int count;			/* Maximum number          */
/*
 * The contents `obj' are examined for objects of type `mask'.
 * If there are less than `count' objects,  the routine returns
 * a non-zero status.  Otherwise,  it returns zero.
 */
{
    octObject cnt_obj;
    octGenerator gen;
    int i;

    if (contents_p) {
	SYMCK(octInitGenContents(obj, mask, &gen));
    } else {
	SYMCK(octInitGenContainers(obj, mask, &gen));
    }
    i = 0;
    while (octGenerate(&gen, &cnt_obj) == OCT_OK) {
	if (++i >= count) {
	    octFreeGenerator(&gen);
	    return 0;
	}
    }
    return 1;
}



#ifdef PRINT_TEST

extern char *vuDispObject();
extern puts();

void p_obj(obj)
octObject *obj;
/* Prints out a nice representation of `obj' to stdout */
{
    (void) puts(vuDispObject(obj));
}

void contents(start)
octObject *start;
/* Prints out all contents of `obj' */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContents(start, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	p_obj(&obj);
    }
}

void containers(start)
octObject *start;
/* Prints out all containers of `obj' */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContainers(start, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	p_obj(&obj);
    }
}
#endif

