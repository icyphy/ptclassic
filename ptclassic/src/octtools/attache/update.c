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
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "errtrap.h"
#include "io.h"
#include "internal.h"
#include "obj.h"
#include "template.h"

#include "update.h"

void updateScreen()
{
    octGenerator gen;
    octStatus status = OCT_OK;
    octObject obj;
    int i;

    IOclearScreen();

    updateHeader(&currentState->currentObject);

    if (currentState->genDirection == CONTENTS) {
	OCT_ASSERT(octInitGenContents(&currentState->currentObject,
				    currentState->genMask, &gen), "%s");
    } else {
	OCT_ASSERT(octInitGenContainers(&currentState->currentObject,
				    currentState->genMask, &gen), "%s");
    }

    if (currentState->genPos < 1) currentState->genPos = 1;
    for (i = 0; i < currentState->genPos; i++) {
	if ((status = octGenerate(&gen, &obj)) != OCT_OK) break;
    }
    OCT_ASSERT(status, "%s");
    if (i == 0) return;
    currentState->genPos = i;
    for (i = 0; i < nObjs; i++) {
	printObjectSummary(&obj);
	IOputchar('\n');
        if ((status = octGenerate(&gen, &obj)) != OCT_OK) break;
    }
    OCT_ASSERT(status, "%s");
}

void updateHeader(objPtr)
octObject *objPtr;
{
    struct maskEntry *mePtr;

    IOhome();
    IOclearLine();
    IOputs(currentState->currentFacet.contents.facet.cell);
    IOputchar(':');
    IOputs(currentState->currentFacet.contents.facet.view);
    IOputchar(':');
    IOputs(currentState->currentFacet.contents.facet.facet);
    IOmove(0, COLS-20);
    IOputs(currentState->genDirection == CONTENTS ?
		    "Contents   " :
		    "Containers ");
    for (mePtr = maskTable; mePtr->mask != 0; mePtr++) {
	if ((currentState->genMask & mePtr->mask) == mePtr->mask) {
	    IOputchar(toupper(mePtr->codeChar));
	} else {
	    IOputchar(mePtr->codeChar);
	}
    }
    IOputs("\n\n");
    IOstandout();
    printObjectSummary(objPtr);
    IOstandend();
    IOputchar('\n');
}

void printObjectSummary(objPtr)
octObject *objPtr;
{
    if (objPtr == NIL(octObject)) return;

    printTemplate(summaryTemplate, objPtr);
}

void PROMPT(str)
char *str;
{
    quitAll = 0;
    IObotHome();
    IOclearLine();
    IOputs(str);
    IOputchar(' ');
    IOrefresh();
}
