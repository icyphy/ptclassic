/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Contains some utility programs to support the reading and writing of
 * Oct flowgraphs.
 */

#include "flowgraph.h"
#include "oct.h"
#include "oh.h"
#include "flowOct.h"

char *Intern();

/*
 * Error passing routines
 */

octStatus myLastStatus;

void
myOctFailed(file, line, why, offending_object)
char *file;
int line;
char *why;
octObject *offending_object;
{

    printmsg(NULL, octErrorString());
    if (why != NULL)
        printmsg(NULL, "Reason : %s\n\n", why);
    else
        printmsg(NULL, "\n");
}

int
IsFormalTerminal(term)
octObject *term;
{
    return ((term->contents.term.instanceId == oct_null_id) ? TRUE : FALSE);
}

int
IsInputTerminal(term)
octObject *term;
{
    octObject prop;

    return (ohGetByPropName(term, &prop, "DIRECTION") != OCT_OK ||
	    !strcmp(prop.contents.prop.value.string, "IN")) ? TRUE : FALSE;
}

char *
myGetName(object)
octObject *object;
{
    return (Intern(ohGetName(object)));
}

int
GetNthTerminal(container, term, N, Class)
octObject *container;
octObject *term;
int N;
char *Class;
{
    /* Find the Nth terminal of DIRECTION Class on container */

    octObject prop;
    octGenerator termGenerator;
    int i;

    ASSERT(octInitGenContents(container, OCT_TERM_MASK, &termGenerator));

    i = 0;
    while (octGenerate(&termGenerator, term) == OCT_OK) {
	ASSERT(ohGetByPropName(term, &prop, "DIRECTION"));
	if (strcmp(prop.contents.prop.value.string, Class))
	    continue;
	if (i == N)
	    return (TRUE);
	else
	    i++;
    }
    return (FALSE);
}
