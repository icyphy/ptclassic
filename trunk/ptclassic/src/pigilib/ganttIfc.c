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
/*  ganttIfc.c  edg
Version identification:
$Id$
*/

/* Includes */
#include "local.h"
#include <stdio.h>
#include "oct.h"		/* define octObject data structure */
#include "rpc.h"
#include "err.h"
#include "ganttIfc.h"
#include "oh.h"
#include "octIfc.h"		/* Pick up MyOpenMaster */
#include "ptk.h"		/* Interpreter name, window name, etc. -aok */
#include "handle.h"

/* MAX_DEPTH
Maximum nesting depth for galaxies, eg. a star in a galaxy in a
galaxy in a universe has depth 3.
*/
#define MAX_DEPTH 64

/* Gantt chart support, from ~gabriel/src/ggirpc/ganttIfc.c */

static vemSelSet (*sets)[MAX_DEPTH];  /* stores vemSelSets until freed */
static int *procDepth;  /* number of vemSelSets used for each processor */
extern octObject lastFacet;  /* root facet for name lookup */
static RgbValue *procColors;  /* points to array of processor color values */
static int procN;  /* number of processors in Gantt chart */

/* The next 2 vars store vemSelSets for FindNameSet() */
static vemSelSet findSets[MAX_DEPTH];
static int findDepth = 0;


/* 
 * Function to detect a name that is extended with a modifier like
 * name.ext1=num.ext2=num.  It returns true if there is at least one
 * "=" character after the first period.
 */

static boolean
specialInstance(s)
char *s;
{
    char *eq, *p2;
    char *p = strchr(s, '.');
    if (!p) return 0;		/* No more periods in the string */
    if ( (eq = strchr(p+1,'=')) ) {
	/* If the '=' occurs before the next '.', return 1 */
	p2 = strchr(p+1, '.');
	if ( !p2 || p2 > eq) return 1;
    }
    return 0;
}

/*
Copy the first component of s into buf.
Components are separated by '.' characters.
If the buffer is not large enough, copy only what fits.
*/
static void
head(s, buf, buflen)
char *s;
char *buf;
int buflen;
{
    char *end;

    end = strchr(s, '.');
    if (end == NULL || specialInstance(s)) {
	strncpy(buf, s, buflen);
    } else {
	while (s != end) {
	    *buf++ = *s++;
	}
	*buf = '\0';
    }
}

/*
Advance s to point after the next '.' character in the string or
else return NULL if there are no more such characters.
*/
char *
incr(s)
const char *s;
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

    octObject facet = {OCT_UNDEFINED_OBJECT},
	      inst = {OCT_UNDEFINED_OBJECT};
    char word[256];
    vemSelSet ss;

    if (name == NULL) return TRUE;
    facet = *rootFacetPtr;
    while (1) {
	head(name, word, 256);
	/* If name is wormhole, the following get will fail because */
	/* there is no icon.  Skip the name in this case.	    */
	if (ohGetByInstName(&facet, &inst, word) != OCT_NOT_FOUND) {
	    if (usePattern)
		ss = vemNewSelSet(facet.objectId, color->red,
				  color->green, color->blue,
				  1, 1, 8, 8, pattern);
	    else
		ss = vemNewSelSet(facet.objectId, color->red,
				  color->green, color->blue,
				  2, 3, 1, 1, "0");
	    sets[(*depth)++] = ss;
	    vemAddSelSet(ss, inst.objectId);
	    if (!MyOpenMaster(&facet, &inst, "contents", "r")) {
	        return FALSE;
	    }
	    FreeOctMembers(&facet);
	    FreeOctMembers(&inst);
	}
	if ((name = incr(name)) == NULL) break;
    }
    return TRUE;
}

/* extern created to call FrameStar w/o losing static declaration */
boolean
FrameStarCall(char *name, RgbValue color, int usePattern) {

  return FrameStar(&lastFacet, name, &color, findSets, &findDepth, usePattern);

}


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

int
FindNameSet(facetPtr, name)
octObject *facetPtr;
char *name;
{
    static RgbValue color = {65535, 0, 0};

    ErrClear();
    FindClear();

    if (!FrameStar(facetPtr, name, &color, findSets, &findDepth, 1)) {
        ErrAdd("Cannot find name in facet");
        return 0;
    }

    return 1;
}

void
FindAndMark(facetP, name, pattern)
octObject* facetP;
const char* name;
int pattern;
{
    char* strippedName;
    static RgbValue color = {65535, 0, 0};
/* name contains the universe name as well, which must be stripped */
    strippedName = incr(name);
    if (!strippedName) return;
    FrameStar(facetP, strippedName, &color, findSets, &findDepth, pattern);
}

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

void
FindAndMarkColor(facetP, name, pattern,Color)
octObject* facetP;
const char* name;
int pattern;
const char* Color;
{
	char* strippedName;
        static RgbValue color;
        if(strcmp(Color,"red")==0)
        {
                color.red = 65535;
                color.green = 0;
                color.blue = 0;
        }
        else if(strcmp(Color,"blue")==0)
        {
                color.red = 0;
                color.green = 0;
                color.blue = 65535;
        }
        else if(strcmp(Color,"green")==0)
        {
                color.red = 0;
                color.green = 65535;
                color.blue = 0;
        }
        else if(strcmp(Color,"orange")==0)
        {
                color.red = 65535;
                color.green = 42405;
                color.blue = 0;
        }
        else if(strcmp(Color,"violet")==0)
        {
                color.red = 61166;
                color.green = 33410;
                color.blue = 61166;
        }
        else if(strcmp(Color,"olive")==0)
        {
                color.red = 27499;
                color.green = 36494;
                color.blue = 8995;
        }
	/* name contains the universe name as well, which must be stripped */
        strippedName = incr(name);
        if (!strippedName) return;
        FrameStar(facetP, strippedName, &color, findSets, &findDepth, pattern);
}
