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
#include "copyright.h"
#define DEFINE_ATTACHE_EXTERN
#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "errtrap.h"
#include "io.h"
#include "internal.h"
#include "edit.h"
#include "update.h"

#include "attache.h"

int attacheCalledFromMainP = 0;
static void errHandler();

void
inspectFacet(facet)
octObject *facet;
{
    struct state outerState;

    if (!attacheCalledFromMainP) {
        nameTable = st_init_table(strcmp, st_strhash);
	outerState.genMask = OCT_ALL_MASK;
	currentState = &outerState;
    }

    IOinit();
    nObjs = LINES - 5;

    if (setjmp(currentState->jmpBuffer) == 0) {
	errPushHandler(errHandler);
	editFacet(NIL(octObject), facet);
    }

    IOend();

    return;
}

void
editFacet(instPtr, facetPtr)
octObject *instPtr;	/* instance (if any) whose master we're opening */
octObject *facetPtr;	/* the facet to be edited */
{
    octStatus status;
    struct state newState;
    struct state *saveState;

    /* Open the facet */
    newState.currentFacet = *facetPtr;
    newState.currentFacet.contents.facet.mode = "a";
    status = instPtr ?
		octOpenMaster(instPtr, &newState.currentFacet) :
		octOpenFacet(&newState.currentFacet);
    if (status == OCT_NO_PERM) {
	newState.currentFacet.contents.facet.mode = "r";
	status = instPtr ?
		    octOpenMaster(instPtr, &newState.currentFacet) :
		    octOpenFacet(&newState.currentFacet);
    }
    if (status < OCT_OK) {
	errRaise(optProgName, 0, "Can't open cell \"%s:%s:%s\" (%s)",
		newState.currentFacet.contents.facet.cell,
		newState.currentFacet.contents.facet.view,
		newState.currentFacet.contents.facet.facet,
		octErrorString());
    }

    /* Save the previous state */
    saveState = currentState;
    currentState = &newState;

    currentState->currentObject = currentState->currentFacet;
    currentState->historyList = NIL(histElement);
    currentState->historyLength = 0;
    currentState->changeFlag = 0;
    currentState->genDirection = CONTENTS;
    currentState->genMask = saveState->genMask;
    currentState->genPos = 1;

    edit(setjmp(currentState->jmpBuffer) == 0);

    if (currentState->changeFlag && querySaveChanges()) {
	if (strcmp(currentState->currentFacet.contents.facet.mode, "r") == 0) {
	    errRaise(optProgName, 0, "Read-only facet -- can't save changes");
	}
	if (octCloseFacet(&currentState->currentFacet) != OCT_OK) {
	    errRaise(optProgName, 0, "Can't write out cell \"%s:%s:%s\"",
		    currentState->currentFacet.contents.facet.cell,
		    currentState->currentFacet.contents.facet.view,
		    currentState->currentFacet.contents.facet.facet);
	}
    }

    /* Restore the previous state */
    currentState = saveState;
    FREE(newState.historyList);
}

void
edit(drawFlag)
int drawFlag;
{
    struct cTable *cmdPtr;
    int cmd;
    octStatus status = OCT_OK;

    if (drawFlag &&
	    strcmp(currentState->currentFacet.contents.facet.mode, "r") == 0) {
	updateScreen();
	PROMPT("Read-only facet");
	drawFlag = 0;
    }

    do {
	if (drawFlag) updateScreen();
	drawFlag = 1;

	IObotHome();
	IOrefresh();
	cmd = IOgetchar();
	IOclearLine();
	for (cmdPtr=commandTable; cmdPtr->fullCommand!=NIL(char); cmdPtr++) {
	    if (cmdPtr->binding == cmd) {
		PROMPT(cmdPtr->fullCommand);
		status = (*cmdPtr->func) ();
		OCT_ASSERT(status, "%s");
		if (cmdPtr->changeP == CHANGE) {
		    currentState->changeFlag = 1;
		}
		break;
	    }
	}
	if (cmdPtr->fullCommand == NIL(char)) {
	    putchar('\7');
	    status = OCT_OK;
	}
    } while (status != OCT_GEN_DONE && ! quitAll);
}

int
querySaveChanges()
{
    int answer;

    do {
	PROMPT("Save changes?");
	answer = IOgetchar();
	IOputchar(answer);
	IOrefresh();
    } while (answer != 'y' && answer != 'n');
    return(answer == 'y');
}


/*ARGSUSED*/
static void errHandler(pkgName, code, message)
char *pkgName;
int code;
char *message;
{
    quitAll = 0;
    IOwrap(NIL(char));
    IObotHome();
    IOclearLine();
    IOstandout();
    IOputs(message);
    IOstandend();
    IOrefresh();
    longjmp(currentState->jmpBuffer, -1);
}
