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
 * Highlight Set Access Routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file contains the initialization routines for a globally accessible
 * highlight selected set.  To use the highlight set,  the caller calls
 * the routine vemGetHiLite which gives control to caller.  No one else
 * can use the set until it is released by vemRelHiLite.
 *
 * What is passed back is a vemSelSet which has been initialized and cleared.
 * Objects can be added and deleted from it to highlight geometry.
 *
 */

#include "general.h"		/* General VEM definitions */
#include <X11/Xlib.h>		/* C Interface to X        */
#include "oct.h"		/* Oct Data Manager        */
#include "attributes.h"		/* Attribute handler       */
#include "selset.h"		/* Selected set routines   */
#include "defaults.h"		/* Defaults module         */
#include "message.h"		/* Message handling        */
#include "buffer.h"		/* Buffer maintenence      */
#include "hilite.h"		/* Self-declaration        */

/* The set over which everyone contends */
static vemOneArg *HiLiteSet = (vemOneArg *) 0;

/* Flag indicating whether set is in use */
static int InUse = 0;

/* Statically created attributes for the set */
static atrHandle HiLiteAttr = (atrHandle) 0;


static int newHiLite(newFctId)
octId newFctId;
/*
 * This routine is called when a caller requests a highlight
 * set which has a different facet than the current one.
 * This requires the selected set to be deleted and recreated
 * with the new buffer.
 */
{
    octObject hiLiteFacet;

    if (HiLiteAttr == (atrHandle) 0) {
	/* Initalize the attributes */
	dfGetAttr("select", &HiLiteAttr);
    }
    if (HiLiteSet) {
	/* Get rid of the old one (preserving the attributes) */
	vemFreeAttrSet(HiLiteSet, &HiLiteAttr);
    }
    /* Allocate a new set with the right buffer */
    hiLiteFacet.objectId = newFctId;
    VEM_OCTCKRVAL(octGetById(&hiLiteFacet), 0);

    HiLiteSet = vemAttrSelSet(hiLiteFacet.objectId, HiLiteAttr);
    if (HiLiteSet == (vemOneArg *) 0) {
	vemMessage("Unable to create highlight set\n", MSG_DISP);
	return 0;
    }
    return 1;
}


vemStatus vemGetHiLite(fctId, theSet)
octId fctId;			/* Facet for set          */
vemOneArg **theSet;		/* Returned selection set */
/*
 * If not already in use,  this routine returns the identifier
 * for the highlight set.  The set is allocated if necessary and
 * the attributes are obtained from the defaults module.  If the
 * set is already in use,  it returns VEM_INUSE.  The buffer
 * must be non-zero.
 */
{
    if (fctId == oct_null_id) return VEM_CORRUPT;

    if (!InUse) {
	if (!newHiLite(fctId)) return VEM_CORRUPT;
	*theSet = HiLiteSet;
	InUse = 1;
	return VEM_OK;
    } else {
	return VEM_INUSE;
    }
}



vemStatus vemRelHiLite()
/*
 * This routine releases control of the global VEM highlight set
 * so someone else can use it.  It clears the selected set so
 * that it will be fresh for the next user.
 */
{
    if (InUse) {
	vemFreeAttrSet(HiLiteSet, &HiLiteAttr);
	HiLiteSet = (vemOneArg *) 0;
	InUse = 0;
	return VEM_OK;
    } else {
	return VEM_BADSTATE;
    }
}
