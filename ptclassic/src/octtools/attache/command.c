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
#include "obj.h"
#include "attache.h"
#include "template.h"
#include "update.h"

#include "command.h"

/* Forward references */
static void doFedit	ARGS((octObject *objPtr));
static int queryContinueEditing		ARGS((char *msg));

/*
 * NOTE: PBP 12/09/89 - to make attache callable as a subroutine in VEMCC,
 * I had to rename the function Fpush to Fpsh due to a name conflict in
 * akcl.
 */

#define QUOTE_CHAR	'\\'

struct cTable commandTable[] = {
    {	"halfup",	CNTRL('U'),	NOCHANGE,	Fhalfup,
		"move window half a screen up on generated object list"	},
    {	"halfdown",	CNTRL('D'),	NOCHANGE,	Fhalfdown,
		"move window half a screen down on generated object list"},
    {	"fullup",	CNTRL('B'),	NOCHANGE,	Ffullup,
		"move window a full screen up on generated object list"	},
    {	"fulldown",	CNTRL('F'),	NOCHANGE,	Ffulldown,
		"move window a full screen down on generated object list"},
    {	"redraw",	'\f',		NOCHANGE,	Fredraw,
		"redraw screen"						},
    {	"gencontainers",'^',		NOCHANGE,	Fcontainers,
		"switch to generating containers"			},
    {	"gencontents",	'v',		NOCHANGE,	Fcontents,
		"switch to generating contents"				},
    {	"setmask",	'=',		NOCHANGE,	Fsetmask,
		"set generator mask to (arg) object types"		},
    {	"addmask",	'+',		NOCHANGE,	Faddmask,
		"add (arg) object types to generator mask"		},
    {	"clearmask",	'-',		NOCHANGE,	Fclearmask,
		"remove (arg) object types from generator mask"		},
    {	"move",		'm',		NOCHANGE,	Fmove,
		"move view to object (arg)"				},
    {	"back",		'b',		NOCHANGE,	Fback,
		"move back to previous object"				},
    {	"push",		'p',		NOCHANGE,	Fpsh,
		"push into the master of instance (arg)"		},
    {	"quit",		'q',		NOCHANGE,	Fquit,
		"quit out of current facet"				},
    {	"QUITALL",	'Q',		NOCHANGE,	FQUITALL,
		"quit out of all unmodified facets"			},
    {	"name",		'n',		NOCHANGE,	Fname,
		"give name (1st arg) to object (2nd arg)"		},
    {	"create",	'c',		CHANGE,		Fcreate,
		"create and edit object"				},
    {	"edit",		'e',		NOCHANGE,	Fedit,
		"begin editing object (arg)"				},
    {	"attach",	'a',		CHANGE,		Fattach,
		"make object (1st arg) contain object (2nd arg)"	},
    {	"detach",	'd',		CHANGE,		Fdetach,
		"detach object (arg) from current object"		},
    {	"DELETE",	'D',		CHANGE,		FDELETE,
		"destroy object (arg)"					},
    {	"help",		'?',		NOCHANGE,	Fhelp,
		"print this message"					},
    {	NIL(char),	'\0',		NOCHANGE,	0,	""	}
};

octStatus
Fname()
{
    char *string;
    char **dummy;
    char *getComString();
    octId getObj();

    string = getComString();
    string = util_strsav(string);	/* put new copy of string in table */
    if (st_find_or_add(nameTable, string, &dummy)) {
	FREE(string);			/* we didn't need it */
    }
    *dummy = (char *) getObj();
    return(OCT_OK);
}

octStatus
Fdetach()
{
    octObject obj;
    octId getObj();

    obj.objectId = getObj();
    OCT_ASSERT(octGetById(&obj), "%s");
    if (currentState->genDirection == CONTENTS) {
	return(octDetach(&currentState->currentObject, &obj));
    } else {
	return(octDetach(&obj, &currentState->currentObject));
    }
}

octStatus
FDELETE()
{
    octObject obj;
    octId getObj();

    obj.objectId = getObj();
    OCT_ASSERT(octGetById(&obj), "%s");
    return(octDelete(&obj));
}

octStatus
Fattach()
{
    octObject obj1, obj2;
    octId getObj();

    obj1.objectId = getObj();
    OCT_ASSERT(octGetById(&obj1), "%s");
    obj2.objectId = getObj();
    OCT_ASSERT(octGetById(&obj2), "%s");
    return(octAttach(&obj1, &obj2));
}

octStatus
Fcreate()
{
    octObject obj;
    struct state newState;
    struct state *saveState;

    obj.objectId = oct_null_id;
    obj.type = OCT_UNDEFINED_OBJECT;

    newState = *currentState;
    saveState = currentState;
    currentState = &newState;
    (void) setjmp(currentState->jmpBuffer);

    do {
	doFedit(&obj);
	OCT_PROTECT(octCreate(&currentState->currentObject, &obj));
    } while (DID_OCT_FAIL() && queryContinueEditing(protMesg));

    currentState = saveState;
    return(OCT_OK);
}

octStatus
Fedit()
{
    octObject oldObj, playObj;
    octId getObj();
    struct state newState;
    struct state *saveState;

    oldObj.objectId = getObj();
    OCT_ASSERT(octGetById(&oldObj), "%s");
    (void) memcpy((char *) &playObj, (char *) &oldObj, sizeof(playObj));
    
    newState = *currentState;
    saveState = currentState;
    currentState = &newState;
    (void) setjmp(currentState->jmpBuffer);

reEdit:
    doFedit(&playObj);
    if (memcmp((char *) &oldObj, (char *) &playObj, sizeof(playObj)) &&
						    querySaveChanges()) {
	OCT_PROTECT(octModify(&playObj));
	if (DID_OCT_FAIL()) {
	    if (queryContinueEditing(protMesg)) goto reEdit;
	} else {
	    saveState->changeFlag = 1;
	}
    }

    currentState = saveState;
    return(OCT_OK);
}

static void doFedit(objPtr)
octObject *objPtr;
{
    static int interrupted = 0;

    initGetTemplate(RESET_FIELD);
    do {
	if (interrupted == 0) {
	    interrupted = 1;
	    IOclearScreen();
	    updateHeader(objPtr);
	    initGetTemplate(KEEP_FIELD);
	    IOwrap("|<<\n\t>>|");
	    printTemplate(detailedTemplate, objPtr);
	    IOwrap(NIL(char));
	    IOrefresh();
	}
	interrupted = 0;
    } while (getTemplate());
}

octStatus
Fmove()
{
    histElement *histEltPtr;
    octId getObj();

    currentState->historyList = REALLOC(histElement, currentState->historyList,
					    currentState->historyLength+1);
    histEltPtr = &currentState->historyList[currentState->historyLength];
    histEltPtr->ID = currentState->currentObject.objectId;
    histEltPtr->genPos = currentState->genPos;
    histEltPtr->genDirection = currentState->genDirection;
    currentState->currentObject.objectId = getObj();
    OCT_ASSERT(octGetById(&currentState->currentObject), "%s");
    currentState->historyLength++;
    currentState->genPos = 1;
    return(OCT_OK);
}

octStatus
Fback()
{
    octObject retreat;
    histElement *histEltPtr;

    if (currentState->historyLength <= 0) {
	errRaise(optProgName, 0, "You've gone back to the beginning");
    }
    histEltPtr = &currentState->historyList[--currentState->historyLength];
    retreat.objectId = histEltPtr->ID;
    OCT_ASSERT(octGetById(&retreat), "%s");
    currentState->currentObject = retreat;
    currentState->genPos = histEltPtr->genPos;
    currentState->genDirection = histEltPtr->genDirection;
    return(OCT_OK);
}

octStatus
Fpsh()
{
    octObject obj;		/* instance whose master we want */
    octObject master;		/* master into which we're pushing */
    octId getObj();

    obj.objectId = getObj();
    OCT_ASSERT(octGetById(&obj), "%s");
    if (obj.type != OCT_INSTANCE) {
	errRaise(optProgName, 0, "Can only push into an instance's master");
    }
    master = currentState->currentFacet;

    editFacet(&obj, &master);
    return(OCT_OK);
}

octStatus
Fquit()
{
    return(OCT_GEN_DONE);
}

octStatus
FQUITALL()
{
    quitAll = 1;
    return(OCT_GEN_DONE);
}

octStatus
Fhalfup()
{
    currentState->genPos -= nObjs/2;
    return(OCT_OK);
}

octStatus
Fhalfdown()
{
    currentState->genPos += nObjs/2;
    return(OCT_OK);
}

octStatus
Ffullup()
{
    currentState->genPos -= nObjs;
    return(OCT_OK);
}

octStatus
Ffulldown()
{
    currentState->genPos += nObjs;
    return(OCT_OK);
}

octStatus
Fredraw()
{
    OCT_ASSERT(octGetById(&currentState->currentObject), "%s");
    IOforceClear();
    return(OCT_OK);
}

octStatus
Fcontents()
{
    currentState->genDirection = CONTENTS;
    return(OCT_OK);
}

octStatus
Fcontainers()
{
    currentState->genDirection = CONTAINERS;
    return(OCT_OK);
}

octStatus
Fsetmask()
{
    octObjectMask getMask();

    currentState->genMask = getMask();
    return(OCT_OK);
}

octStatus
Faddmask()
{
    octObjectMask getMask();

    currentState->genMask |= getMask();
    return(OCT_OK);
}

octStatus
Fclearmask()
{
    octObjectMask getMask();

    currentState->genMask &= ~getMask();
    return(OCT_OK);
}

static char *helpArgTypes = "\
An argument that specifies an object can be:\n\
    The external ID of an object in the current facet\n\
    The character `.' for the current object\n\
    The character `/' for the current facet\n\
    A name that was associated with an object by the name command\n\
\n\
An argument that specifies a set of object types (the generator mask)\n\
is a string of letters, like the ones in the top right corner of the\n\
display (case is ignored):\n\
    f - Facet			p - Property\n\
    i - Instance		b - Bag\n\
    t - Terminal		l - Layer\n\
    n - Net			g - All geometric objects\n\
		a - All object types\n\
\n\
";

octStatus
Fhelp()
{
    struct cTable *cTabPtr;
    char line[1000];

    IOclearScreen();
    IOhome();
    IOputs("Key  Command         Action\n");
    for (cTabPtr = commandTable; cTabPtr->binding != '\0'; cTabPtr++) {
	if (iscntrl(cTabPtr->binding)) {
	    IOputchar('^');
	    IOputchar(0100 ^ cTabPtr->binding);
	} else {
	    IOputchar(' ');
	    IOputchar(cTabPtr->binding);
	}
	(void) sprintf(line, "    %-15s %s\n",
		    cTabPtr->fullCommand, cTabPtr->synopsis);
	IOputs(line);
    }
    IObotHome();
    IOputs("\t<any character to continue>");
    IOrefresh();
    (void) IOgetchar();

    IOforceClear();
    IOhome();
    IOputs(helpArgTypes);
    IObotHome();
    IOputs("\t<any character to continue>");
    IOrefresh();
    (void) IOgetchar();

    return(OCT_OK);
}

octId
getObj()
{
    int32 xid;
    char *text, *dummy;
    octObject obj;
    char *getComString();
    long tmplong;		/* Read into a long, because int32
				   could be into or long */

    text = getComString();
    if (sscanf(text, "%ld", &tmplong) == 1) {
	xid=tmplong;
	OCT_ASSERT(octGetByExternalId(&currentState->currentFacet, xid, &obj),
									"%s");
	return(obj.objectId);
    } else if (strcmp(text, ".") == 0) {
	return(currentState->currentObject.objectId);
    } else if (strcmp(text, "/") == 0) {
	return(currentState->currentFacet.objectId);
    } else {
	if (st_lookup(nameTable, text, &dummy) == 0) {
	    errRaise(optProgName, 0, "unknown name");
	    /*NOTREACHED*/
	}
	return((octId) dummy);
    }
}

octObjectMask
getMask()
{
    octObjectMask mask;
    char *text;
    struct maskEntry *mePtr;
    char *getComString();

    mask = 0;
    text = getComString();
    while (*text != '\0') {
	for (mePtr = maskTable; mePtr->mask != 0; mePtr++) {
	    if (*text == mePtr->codeChar) {
		mask |= mePtr->mask;
		break;
	    }
	}
	if (mePtr->mask == 0) {
	    errRaise(optProgName, 0, "unknown object type");
	}
	text++;
    }
    return(mask);
}

char *
getComString()
{
    static char string[500];
    static char stopList[] = "  \t\n\r";
    int stopCh;

    stopList[0] = lineKillChar;

    stopCh = getString(string, string, stopList);
    if (stopCh == lineKillChar) errRaise(optProgName, 0, "");
    return(string);
}

int
getString(start, cPtr, stopList)
char *start, *cPtr;
char *stopList;
{
    int ch;

    while ((ch = IOgetchar()) != EOF) {
	if (strchr(stopList, ch)) {
	    IOputchar(' ');
	    *cPtr = '\0';
	    IOrefresh();
	    return(ch);
	} else if (ch == eraseChar) {
	    if (cPtr > start) {
		IOputs("\b \b");
		cPtr--;
	    }
	} else if (ch == wordEraseChar) {
	    while (cPtr > start && isspace(*cPtr)) {
		IOputs("\b \b");
		cPtr--;
	    }
	    while (cPtr > start && !isspace(*cPtr)) {
		IOputs("\b \b");
		cPtr--;
	    }
	} else if (ch == lineKillChar) {
	    while (cPtr > start) {
		IOputs("\b \b");
		cPtr--;
	    }
	} else if (ch == QUOTE_CHAR) {
	    IOputchar(QUOTE_CHAR);
	    IOputchar('\b');
	    IOrefresh();
	    ch = IOgetchar();
	    IOputcharSee(ch);
	    *cPtr++ = ch;
	} else {
	    IOputcharSee(ch);
	    *cPtr++ = ch;
	}
	IOrefresh();
    }
    *cPtr = '\0';
    return(ch);
}

void
strUpcase(strPtr)
char *strPtr;
{
    while (*strPtr != '\0') {
	if (islower(*strPtr)) *strPtr = toupper(*strPtr);
	if (*strPtr == '-') *strPtr = '_';
	strPtr++;
    }
}

static int queryContinueEditing(msg)
char *msg;
{
    int answer;

    IObotHome();
    IOputchar(CNTRL('K'));
    IOstandout();
    IOputs(msg);
    IOstandend();

    do {
	PROMPT("keep editing obj?");
	answer = IOgetchar();
	IOputchar(answer);
	IOrefresh();
    } while (answer != 'y' && answer != 'n');

    return(answer == 'y');
}
