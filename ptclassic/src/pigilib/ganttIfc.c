/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/*  ganttIfc.c  edg
Version identification:
$Id$
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

/* hack function to detect an instance corresponding to a multiport
   star with specified number of ports: name.port=num .  It returns
   true if there is exactly one "." character and exactly one "="
   character.
 */

static boolean
specialInstance(s)
char *s;
{
    char *p = strchr(s, '.');
    if (!p) return 0;
    if (strchr(p+1,'.')) return 0;
    return strchr(p+1,'=') ? 1 : 0;
}

/*
Copy the first component of s into buf.  Components are separated by
'.' characters or '\0'.  Handle "special instance names" (see above).
Caveats: assumes enough room in buf.
*/
static void
head(s, buf)
char *s;
char *buf;
{
    char *end;

    end = strchr(s, '.');
    if (end == NULL || specialInstance(s)) {
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
else return NULL.    Handle "special instance names" (see above).
*/
static
char *
incr(s)
char *s;
{
    char *end;

    end = strchr(s, '.');
    if (end && specialInstance(s)) end = NULL;
    return (end == NULL) ? NULL : ++end;
}

/* 3/28/90
Highlight a star with a color and keep track of allocated vemSelSets.
You can call this with out using Gantt, see RpcFindStar().
Inputs:
    rootFacetPtr = facet to start looking for name in
    name = name of star, components are separated by '.'
    color = frame color
    sets = array of unused vemSelSets, normally all elements empty
    depth = index to first unused slot in sets[], normally 0
Outputs:
    sets = elements filled with allocated vemSelSets
    depth = updated to indicate total # of sets
*/
static boolean
FrameStar(rootFacetPtr, name, color, sets, depth, usePattern)
octObject *rootFacetPtr;
char *name;
RgbValue *color;
vemSelSet sets[];
int *depth;
int usePattern;
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
	if (usePattern)
		ss = vemNewSelSet(facet.objectId, color->red,
				  color->green, color->blue, 1, 1, 8, 8, 
				  pattern);
	else
		ss = vemNewSelSet(facet.objectId, color->red,
				  color->green, color->blue, 2, 3, 1, 1, "0");
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
Find a name starting at facet under cursor.  Name can have '.' between
components and all components in heirarchy will be marked.
*/
int 
RpcFindName(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static RgbValue color = {65535, 0, 0};
    static dmTextItem item = {"Name", 1, 100, NULL, NULL};
    octObject facet;

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
    if (!FrameStar(&facet, item.value, &color, findSets, &findDepth, 1)) {
	ErrAdd("Cannot find name in facet");
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}

FindAndMarkError(facetP, name)
octObject *facetP;
char *name;
{
    static RgbValue color = {65535, 0, 0};
/* name contains the universe name as well, which must be stripped */
    name = incr(name);
    if (!name) return;
    FrameStar(facetP, name, &color, findSets, &findDepth, 0);
}

/* Gantt chart support, from ~gabriel/src/ggirpc/ganttIfc.c */

static vemSelSet (*sets)[MAX_DEPTH];  /* stores vemSelSets until freed */
static int *procDepth;  /* number of vemSelSets used for each processor */
extern octObject lastFacet;  /* root facet for name lookup */
static RgbValue *procColors;  /* points to array of processor color values */
static int procN;  /* number of processors in Gantt chart */

/* CreateFrames  2/26/90 7/6/88
Call this first, before any other Gantt routine.
Inputs:
    colors = each array entry describes the color of one frame
    n = number of frames to create
*/
int
CreateFrames(colors, n)
RgbValue colors[];
int n;
{
    int i;

    procColors = colors;
    procN = n;
    if ((sets = (vemSelSet (*)[MAX_DEPTH]) malloc(n * sizeof(*sets))) == NULL) {
	return 0;
    }
    if ((procDepth = (int *) malloc(n * sizeof(int))) == NULL) {
	free(sets);
	return 0;
    }
    for (i = 0; i < n; i++) {
	procDepth[i] = 0;
    }
    return 1;
}

/* ClearFrames  7/6/88
Hides all frames from view but does not free storage associated with
frames.
*/
int
ClearFrames()
{
    int i, j;

    for (i = 0; i < procN; i++) {
	for (j = 0; j < procDepth[i]; j++) {
	    vemFreeSelSet(sets[i][j]);
	}
	procDepth[i] = 0;
    }
    return (1);
}

/* DestroyFrames  7/6/88
Hides all frames and frees all storage associated with frames.  Must
call CreateFrames() again before calling any other function.
*/
int
DestroyFrames()
{
    ClearFrames();
    free(sets);
    free(procDepth);
    return (1);
}

/* Frame  3/28/90 7/16/88 7/6/88
Inputs: stars = array of strings.  Assumes number of strings equals
    number of frames that were created.  If a pointer in stars[] is
    NULL then the corresponding frame will not be used.

Star names are assumed not to contain the universe name.
*/
int
Frame(stars)
char *stars[];
{
    int i;

    for (i = 0; i < procN; i++) {
	if (!FrameStar(&lastFacet, stars[i], &procColors[i], sets[i],
	    &procDepth[i], 1)) {
	    return 0;
	}
    }
    return 1;
}

