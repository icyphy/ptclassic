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
/*
 * VEM Utility Routines
 * General Purpose Routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains routines which don't fit into any other category.
 */

#include "general.h"		/* General VEM definitions */
#include <pwd.h>		/* Password file format    */
#include "oct.h"		/* Oct data manager        */
#include "vemUtil.h"		/* Self declaration        */

#define TILDE	'~'

#define MAX_STR	1024
/* Some lint definitions */



int uniq(base, nel, width, compar)
char *base;			/* Base of data array   */
int nel;			/* Number of elements   */
int width;			/* Size of each element */
int (*compar)();		/* Comparison function  */
/*
 * This routine takes a sorted list of items and removes
 * duplicates to produce a unique list.  The size of the
 * unique list is returned.
 */
{
    char *ptr, *copy, *last;
    int newnel = 1;
    
    if (nel <= 1) {
	return nel;
    }

    last = base;
    
    for(copy = ptr = base+width; --nel > 0; ptr += width) {
	if ((*compar)(last, ptr) != 0) {
	    if (copy != ptr) memcpy(copy, ptr, width);
	    last = copy;
	    copy += width;
	    newnel++;
	}
    }
    return newnel;
}   



char *vuMakeName(theObj)
octObject *theObj;		/* Some OCT object */
/*
 * This routine returns a simple text string for an OCT object.
 * The string returned is allocated by this routine and should
 * be freed after use.
 */
{
    octObject objLayer;
    octGenerator theGen;
    char text[1024];
    int spot, index;

    objLayer.type = OCT_LAYER;
    objLayer.contents.layer.name = "(no layer)";
    if (octInitGenContainers(theObj, OCT_LAYER_MASK, &theGen) == OCT_OK) {
	while (octGenerate(&theGen, &objLayer) == OCT_OK) {
	    /* Nothing */
	}
    }
    switch (theObj->type) {
    case OCT_FACET:
	/* Strip off the path of the cell name */
	spot = STRLEN(theObj->contents.facet.cell) - 1;
	for (index = spot;  index > 0;  index--) {
	    if (theObj->contents.facet.cell[index] == '/') break;
	}
	sprintf(text, "Facet '%s:%s'",
		&(theObj->contents.facet.cell[index > 0 ? index+1 : index]),
		theObj->contents.facet.view);
	break;
    case OCT_TERM:
	if (theObj->contents.term.instanceId) {
	    sprintf(text, "Actual Term named %s", theObj->contents.term.name);
	} else {
	    sprintf(text, "Formal Term named %s", theObj->contents.term.name);
	}
	break;
    case OCT_NET:
	sprintf(text, "Net named %s", theObj->contents.net.name);
	break;
    case OCT_INSTANCE:
	/* Strip off the path of the master's cell name */
	spot = STRLEN(theObj->contents.instance.master) - 1;
	for (index = spot;  index > 0;  index--) {
	    if (theObj->contents.instance.master[index] == '/') break;
	}
	sprintf(text, "Instance of %s",
		&(theObj->contents.instance.master[index]));
	break;
    case OCT_POLYGON:
	sprintf(text, "Polygon on %s", objLayer.contents.layer.name);
	break;
    case OCT_BOX:
	sprintf(text, "Box on %s", objLayer.contents.layer.name);
	break;
    case OCT_CIRCLE:
	sprintf(text, "Circle on %s", objLayer.contents.layer.name);
	break;
    case OCT_PATH:
	sprintf(text, "Path on %s", objLayer.contents.layer.name);
	break;
    case OCT_LABEL:
	sprintf(text, "Label on %s", objLayer.contents.layer.name);
	break;
    case OCT_PROP:
	sprintf(text, "Property named %s", theObj->contents.prop.name);
	break;
    case OCT_BAG:
	sprintf(text, "Bag named %s", theObj->contents.bag.name);
	break;
    case OCT_LAYER:
	sprintf(text, "Layer named %s", theObj->contents.layer.name);
	break;
    case OCT_POINT:
	sprintf(text, "Point (%ld,%ld)", (long)theObj->contents.point.x,
		(long)theObj->contents.point.y);
	break;
    case OCT_EDGE:
	sprintf(text, "Edge from (%ld,%ld) to (%ld,%ld)",
		(long)theObj->contents.edge.start.x,
		(long)theObj->contents.edge.start.y,
		(long)theObj->contents.edge.end.x,
		(long)theObj->contents.edge.end.y);
	break;
    default:
	sprintf(text, "Illegal object type: 0x%x", theObj->type);
	break;
    }
    return VEMSTRCOPY(text);
}


#ifdef OLD_TILDE
STR vuTildeExpand(out, in)
STR out;			/* Output space for expanded file name */
STR in;				/* Filename with tilde                 */
/*
 * This routine expands out a file name passed in `in' and places
 * the expanded version in `out'.  It returns `out'.
 */
{
    char username[50], *userPntr;
    struct passwd *userRecord;

    out[0] = '\0';

    /* Skip over the white space in the initial path */
    while ((*in == ' ') || (*in == '\t')) in++;

    /* Tilde? */
    if (in[0] == TILDE) {
	/* Copy user name into 'username' */
	in++;  userPntr = &(username[0]);
	while ((*in != '\0') && (*in != '/')) {
	    *(userPntr++) = *(in++);
	}
	*(userPntr) = '\0';
	/* Look up user directory in passwd file */
	if ((userRecord = getpwnam(username)) != (struct passwd *) 0) {
	    /* Found user in passwd file.  Concatenate user directory */
	    STRCONCAT(out, userRecord->pw_dir);
	}
    }

    /* Concantenate remaining portion of file name */
    STRCONCAT(out, in);
    return out;
}
#endif




STR vuCellName(cell)
octObject *cell;		/* Facet specification */
/*
 * This routine returns a suitable display string for a facet
 * specification.  The space for the string is allocated in this 
 * routine.  It should be freed after use.
 */
{
    STR str;
    char temp[MAX_STR];
    int i, cl, offset, biglen, i_flag;

    cl = STRLEN(cell->contents.facet.cell);
    offset = MAX_STR - cl;
    for (i = cl-1;  i >= 0;  i--) {
	if (cell->contents.facet.cell[i] == '/') break;
	temp[i + offset] = cell->contents.facet.cell[i];
    }
    /* Stripped starts at i+offset+1 */
    if (i >= 0) {
	/* Its path has been stripped */
	temp[(i--) + offset] = '/';
	temp[(i--) + offset] = '.';
	temp[(i--) + offset] = '.';
	temp[(i) + offset] = '.';
    } else {
	i++;
    }
    /* Prepared string begins at i + offset */
    biglen = cl = MAX_STR - (i + offset);
    biglen += STRLEN(cell->contents.facet.view);
    if (STRCOMP(cell->contents.facet.facet, "contents") != 0) {
	/* Its some interface */
	biglen += 3;		/* Length of "(i)"    */
	biglen += 2;		/* Length of dividers */
	i_flag = 1;
    } else {
	biglen += 1;		/* Length of single divider */
	i_flag = 0;
    }
    /* Ready to construct final string */
    str = VEMARRAYALLOC(char, biglen+1);
    STRNMOVE(str, &(temp[i+offset]), cl);
    (str)[cl] = '\0';
    STRCONCAT(str, ":");
    STRCONCAT(str, cell->contents.facet.view);
    if (i_flag) {
	STRCONCAT(str, ":(i)");
    }
    return str;
}
