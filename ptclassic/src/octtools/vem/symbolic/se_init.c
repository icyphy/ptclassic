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
/*
 * Symbolic Editing Initialization
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains the intitialization code for the symbolic
 * package.  Before any symbolic function is called in the library,
 * initialization is done to set up certain callback functions.
 */

#include "general.h"
#include "symbolic.h"
#include "vemDM.h"
#include "message.h"

#define NO_JUMP_NAME	";;; Unspecified Local/Global Net ;;;"

static int se_initialized = 0;



static void seJumpNameQuery(term, name, data)
octObject *term;		/* What terminal */
char name[SYM_MAX_MSG];		/* Net name      */
char *data;			/* User data     */
/*
 * This routine is called when the symbolic policy library encounters
 * a local or global net terminal whose property is empty.  This means
 * the name should come from the user.  We implement this by posting
 * a dialog.
 */
{
    dmTextItem item;

    item.itemPrompt = "Global net name";
    item.rows = 1;  item.cols = 40;
    item.value = (STR) 0;
    (void) sprintf(errMsgArea, "Terminal %s", term->contents.term.name);
    if (dmMultiText(errMsgArea, 1, &item) != VEM_OK) {
	(void) strcpy(name, NO_JUMP_NAME);
    } else {
	(void) strcpy(name, item.value);
    }
}


#define NULLNAME(actual) (STRLEN(actual) ? actual : "<No Name>" )

static int seMergeQuery(n1, n2, data)
octObject *n1, *n2;		/* Nets to merge */
char *data;			/* User data     */
/*
 * This routine is called when the symbolic policy package cannot
 * determine which of two nets to preserve when the nets must
 * be merged.  This is solved in VEM by asking the user using
 * a dialog.
 */
{
    char *name_one, *name_two;

    name_one = NULLNAME(n1->contents.net.name);
    name_two = NULLNAME(n2->contents.net.name);

    (void) sprintf(errMsgArea,
		   "Nets `%s' and `%s' must be merged.\nWhich one do you wish\nto preserve?",
		   name_one, name_two);
    if (dmConfirm("Net Maintenence", errMsgArea, name_one, name_two) == VEM_OK) {
	/* Preserve n1 */
	return 1;
    } else {
	/* Preserve n2 */
	return 0;
    }
}


static void seWarnFunc(sev, msg, data)
symSeverity sev;		/* Severity of message */
char *msg;			/* Message itself      */
char *data;			/* User data           */
/*
 * This routine is called when the symbolic package produces
 * diagnostics.  Based on the severity level, it either
 * prints it out in the console or posts a dialog.
 */
{
    switch (sev) {
    case SYM_INFO:
	vemMsg(MSG_C, "Symbolic message: %s\n", msg);
	break;
    case SYM_WARN:
	vemMsg(MSG_A, "Symbolic WARNING: %s\n", msg);
	break;
    case SYM_ERR:
	(void) dmConfirm("Symbolic Error", msg, "Dismiss", (STR) 0);
	break;
    }
}



void seInit()
/*
 * Initializes symbolic editing package.  Most of the callback functions
 * for symbolic are defined here.
 */
{
    static symJumpNameFunc jn = { seJumpNameQuery, (char *) 0 };
    static symMergeFunc merge = { seMergeQuery, (char *) 0 };
    static symWarnFunc warn = { seWarnFunc, (char *) 0 };

    if (!se_initialized) {
	symSetJumpNameFunc(&jn, (symJumpNameFunc *) 0);
	symSetMergeFunc(&merge, (symMergeFunc *) 0);
	symSetWarnFunc(&warn, (symWarnFunc *) 0);
	
	se_initialized = 1;
    }
}


