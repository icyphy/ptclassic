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
 * Schematic path drawing 
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This is the new interface to drawing schematic paths.
 */

#include "general.h"
#include "oct.h"
#include "commands.h"
#include "list.h"
#include "message.h"
#include "vemUtil.h"
#include "defaults.h"
#include "tap.h"
#include "symbolic.h"
#include "se.h"

static int schemDCF(nLayers, layerList, inst, why, data)
int nLayers;			/* Number of connections */
tapLayerListElement *layerList;	/* Layers themselves     */
octObject *inst;		/* Instance returned     */
char why[SYM_MAX_MSG];		/* Error message         */
char *data;			/* User data (not used)  */
/*
 * This is the connector function for creating new segments in
 * schematic.  If there are more than two connections, it slightly
 * modifies the description to cause TAP to find the solder dot
 * connector instead of the normal connector.  This is done by
 * increasing the width of the segments just slightly.
 */
{
    int i;

    if (nLayers > 2) {
	for (i = 0;  i < nLayers;  i++) {
	    if (layerList[i].width == 0) layerList[i].width = 10;
	}
    }
    if (tapGetConnector(nLayers, layerList, inst) == 0) {
	char buf[256];		/* Used to build good error message. */
	inst->objectId = oct_null_id;
	(void) sprintf(why, "No appropriate connector found: %d layers\n", nLayers);
	for ( i = 0; i < nLayers; i++ ) {
	    (void) sprintf( buf, "%s(%ld) ",
			   layerList[i].layer.contents.layer.name,
			   (long)layerList[i].width );
	    (void) strcat( why, buf );
	}
	return 0;
    } else {
	return 1;
    }
}

static symConFunc schemConFunc = { schemDCF, (char *) 0 };


vemStatus schemCreatePath(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This routine parses a command of the following form:
 *  lines(n) lines(n) ... lines(n)
 * The layer is taken to be the default layer for schematics.
 */
{
    octObject target_fct, pathLayer;

    /* All arguments should be in same buffer */
    if (vuCkBuffer(cmdList, &(target_fct.objectId)) != VEM_OK) return VEM_ARGRESP;
    if (bufWritable(target_fct.objectId) != VEM_OK) {
	vemMsg(MSG_C, "Buffer is not writable\n");
	return VEM_ARGRESP;
    }
    /* Determine the layer */
    VEM_OCTCKRVAL(octGetById(&target_fct), VEM_CORRUPT);
    pathLayer.type = OCT_LAYER;
    dfGetString("schem.layer", &(pathLayer.contents.layer.name));
    VEM_OCTCKRVAL(octGetOrCreate(&target_fct, &pathLayer), VEM_CORRUPT);

    return seBasePath(&target_fct, cmdList, &pathLayer, &schemConFunc);
}

