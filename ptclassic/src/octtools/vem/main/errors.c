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
/*LINTLIBRARY*/
/*
 * VEM Error Handlers
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains code for implementing the VEM error handlers.
 * The error handlers included are:
 *  vemFail:		VEM assertion failure
 *  vemOctFail:		Oct call has failed
 *  vemMultiFail:	Offer user options to counter failure
 *  vemXError:		Handles normal X errors
 *  vemXtError:		Toolkit error
 */

#include "errors.h"		/* Self declaration   */
#include "oct.h"		/* Oct data manager   */
#include "vemDM.h"		/* High level dialogs */
#include "display.h"		/* Display handling   */
#include "mm.h"			/* For mm errors      */

#define Fprintf	(void) fprintf



#define MAX_CONTEXTS	50
#define MAX_ERROR	1024

static int err_state_loc = 0;
static jmp_buf err_state[MAX_CONTEXTS];

/* Some libraries do not define this protocol number */
#ifndef X_StippleFill
#define X_StippleFill	54
#endif

void vemFail(name, filename, line)
char *name;
char *filename;
int line;
/* Assertion failed */
{
    errRaise("vem", VEM_CORRUPT, "Assertion failure: %s, file %s, line %d",
	     name, filename, line);
    /*NOTREACHED*/
}

void vemOctFail(filename, line)
char *filename;
int line;
/* Assertion failed */
{
    errRaise("oct", VEM_CORRUPT, "Oct call failed: file %s, line %d:\n  %s\n",
	     filename, line, octErrorString());
    /*NOTREACHED*/
}

int vemMultiFail(title, numopts, options, filename, line)
char *title;
int numopts;
char *options[];
char *filename;
int line;
/* Assertion failed -- offer user a number of options */
{
    char c[10];
    int idx;

    Fprintf(stderr, "\007\007\007%s\n", title);
    Fprintf(stderr, "File: `%s', Line: %d\n", filename, line);
    Fprintf(stderr, "You have the following options:\n");
    for (idx = 0;  idx < numopts;  idx++) {
	Fprintf(stderr, "   %2d %s\n", idx+1, options[idx]);
    }
    Fprintf(stderr, "Enter number of choice:\n");
    (void) gets(c);
    return atoi(c)-1;
}

int vemXError(theDisp, theError)
Display *theDisp;		/* Effected display  */
XErrorEvent *theError;		/* Error description */
{
    char message[MAX_ERROR];
    char number[MAX_ERROR];
    char request[MAX_ERROR];
    char c[10];

    /* Get the text message */
    XGetErrorText(theDisp, theError->error_code, message, MAX_ERROR);
    (void) sprintf(number, "%d", (int) theError->request_code);
    XGetErrorDatabaseText(theDisp, "XRequest", number, number, request, MAX_ERROR);

    Fprintf(stderr, "\007A Serious X Error has occurred:\n");
    Fprintf(stderr, "   %s\n", message);
    Fprintf(stderr, "   Request %s (minor code %d)\n", request,
	    (int) theError->minor_code);
    Fprintf(stderr, "Type 'y' to continue, 'n' to exit, 'a' to abort:\n");
    (void) gets(c);
    switch (c[0]) {
    case 'y':
	return 0;
    case 'n':
	exit(1);
    case 'a':
	abort();
    }
    return 0;
}


void vemXtError(msg)
char *msg;
{
    char c[10];

    Fprintf(stderr, "A Fatal Xt Toolkit Error has occurred:\n  %s\n",
	    msg);
    Fprintf(stderr, "Type 'y' to continue, 'n' to exit, 'a' to abort\n(continuing may have unpredictable consequences):\n");
    (void) gets(c);
    switch (c[0]) {
    case 'y':
	return;
    case 'n':
	exit(1);
    case 'a':
	abort();
    }
    return;
}



/*
 * Support for error trap package
 */

void vemPushErrContext(env)
jmp_buf env;
/*
 * Pushes setjmp contexts onto a local stack
 */
{
    if (err_state_loc < MAX_CONTEXTS) {
	(void) memcpy((char *) err_state[err_state_loc++], (char *) env,
	       sizeof(jmp_buf));
    } else {
	/* Horrible error */
	errRaise("vem", VEM_CORRUPT, "Too many error contexts");
    }
} 

void vemPopErrContext()
/*
 * Pops off top level error context
 */
{
    if (err_state_loc > 0) {
	err_state_loc--;
    } else {
	errRaise("vem", VEM_CORRUPT, "Attempt to pop non-existent context");
    }
}

#define MAX_MESSAGE	4096

/*ARGSUSED*/
void vemDefaultTrapHandler(name, code, message)
STR name;			/* Package name      */
int code;			/* Error code number */
STR message;			/* Error message     */
/*
 *
 */
{
    char out_message[MAX_MESSAGE];

    /* In the special case of out of memory -- total collapse */
    if ((strcmp(name, MM_PKG_NAME) == 0) && (code == MM_OUT_OF_MEMORY)) {
	Fprintf(stderr, "\007Fatal error detected in %s:\n\t%s\n", name, message);
	Fprintf(stderr, "Memory allocation failure -- impossible to recover\n");
	Fprintf(stderr, "Perhaps you don't have enough swap space allocated?\n");
	exit(1);
    }

    /* Force end of display sequence (if any) */
    dspEnd();
    (void) sprintf(out_message, "%s error: \n", name);
    (void) strcat(out_message, message);
    if (err_state_loc > 0) {
	if (dmConfirm("Fatal Error", out_message, "Continue", "Abort") == VEM_OK) {
	    longjmp(err_state[err_state_loc-1], VEM_FALSE);
	}
    } else {
	/* No resume possible */
	(void) dmConfirm("Fatal Nonrecoverable Error", out_message,
			 "Terminate Program", (char *) 0);
    }
    /* Falling off the end will cause transfer upward */
}
