/*  ganttIfc.c  edg
Version identification:
$Id$
All Gantt functions have been removed since there is yet no kernel
support for displaying SDF schedules.  All that remains is the
RpcFindStar() function which is useful in debugging schematics.
*/

/* Includes */
#include <stdio.h>
#include "local.h"
#include "rpc.h"
#include "err.h"
#include "ganttIfc.h"
#include "oh.h"
#include "vemInterface.h"

/* MAX_DEPTH
Maximum nesting depth for galaxies, eg. a star in a galaxy in a
galaxy in a universe has depth 3.
*/
#define MAX_DEPTH 64

/*
Copy the first component of s into buf.  Components are separated by
'%' characters or '\0'.
Caveats: assumes enough room in buf.
*/
static void
head(s, buf)
char *s;
char *buf;
{
    char *end;

    end = strchr(s, '%');
    if (end == NULL) {
	strcpy(buf, s);
    } else {
	while (s != end) {
	    *buf++ = *s++;
	}
	*buf = '\0';
    }
}

/*
Advance s to point after the next '%' character in the string or
else return NULL.
*/
static
char *
incr(s)
char *s;
{
    char *end;

    end = strchr(s, '%');
    return (end == NULL) ? NULL : ++end;
}

/* 3/28/90
Highlight a star with a color and keep track of allocated vemSelSets.
You can call this with out using Gantt, see RpcFindStar().
Inputs:
    rootFacetPtr = facet to start looking for name in
    name = name of star, components are separated by '%'
    color = frame color
    sets = array of unused vemSelSets, normally all elements empty
    depth = index to first unused slot in sets[], normally 0
Outputs:
    sets = elements filled with allocated vemSelSets
    depth = updated to indicate total # of sets
*/
static boolean
FrameStar(rootFacetPtr, name, color, sets, depth)
octObject *rootFacetPtr;
char *name;
RgbValue *color;
vemSelSet sets[];
int *depth;
{
    static char *pattern = "00010000 00010000 00010000 11111111 00010000 00010000 00010000 00010000";
    octObject facet, inst;
    char word[256];
    vemSelSet ss;

    if (name == NULL) return TRUE;
    facet = *rootFacetPtr;
    while (1) {
	head(name, word);
	if (ohGetByInstName(&facet, &inst, word) == OCT_NOT_FOUND) {
	    return FALSE;
	}
	ss = vemNewSelSet(facet.objectId, color->red,
	    color->green, color->blue, 1, 1, 8, 8, pattern);
	sets[(*depth)++] = ss;
	vemAddSelSet(ss, inst.objectId);
	if ((name = incr(name)) == NULL) break;
	if (!MyOpenMaster(&facet, &inst, "contents", "r")) {
	    return FALSE;
	}
    }
    return TRUE;
}

/* The next 2 vars store vemSelSets for RpcFindName() */
static vemSelSet findSets[MAX_DEPTH];
static int findDepth = 0;

/*
Erases highlighting after a find command (below).
*/
void
FindClear()
{
    int i;

    if (findDepth == 0) return;
    for (i = 0; i < findDepth; i++) {
	vemFreeSelSet(findSets[i]);
    }
    findDepth = 0;
}
    
/* 3/28/90
Find a name starting at facet under cursor.  Name can have '%' between
components and all components in heirarchy will be marked.
*/
int 
RpcFindName(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static RgbValue color = {65535, 42495, 0};
    static dmTextItem item = {"Name", 1, 100, NULL, NULL};
    octObject facet;
    vemStatus status;

    ViInit("find-name");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    if (dmMultiText("Find Name", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        ViDone();
    }
    FindClear();
    if (!FrameStar(&facet, item.value, &color, findSets, &findDepth)) {
	ErrAdd("Cannot find name in facet");
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}
