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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"

/*
 * application routines for VEM
 *
 * Rick L Spickelmier, 6/1/86, modified 3/2/87
 */

/* #include "message.h" */
/* #include "selset.h"  */
/* #include "region.h"  */
/* #include "vemDM.h"   */
/* #include "list.h"    */
#include "rpcApp.h"
#include <X11/Xlib.h>

rpcStatus RPCSendSpot
	ARGS((RPCSpot *spot, STREAM stream));


/*
 * send a message to the VEM console display
 *
 * Arguments:
 *   string	message to be logged/displayed
 b*   options	log?/display?
 *
 * Returns;
 *   nothing
 *
 */
void
vemMessage(string, options)
char *string;
int options;
{
    RPCASSERT(RPCSendLong(VEM_MESSAGE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(string, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    return;
}


/*
 * flush the redraw events
 */
vemStatus
vemWnQRegion(facet, theExtent)
octObject *facet;
struct octBox *theExtent;
{
    long returnCode = VEM_OK;
    
    RPCASSERT(RPCSendLong(WN_QREGION_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) facet->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctBox(theExtent, RPCSendStream));
    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */

    return (vemStatus) returnCode;
}


vemStatus
vemWnFlush()
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(WN_FLUSH_FUNCTION, RPCSendStream));
    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */

    return (vemStatus) returnCode;
}


/*
 * open a vem window on a facet
 */
Window
vemOpenWindow(facet, geo)
octObject *facet;
char *geo;
{
    long returnCode;

    RPCASSERT(RPCSendLong(VEM_OPEN_WINDOW_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(facet, RPCSendStream));
    RPCASSERT(RPCSendString(geo, RPCSendStream));

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    return (Window) returnCode;
}

/*
 * open a vem window on a facet, and start a new rpc application in it
 */
Window
vemOpenRPCWindow(facet, rpcSpec, geoSpec)
octObject *facet;
char *rpcSpec;
char *geoSpec;
{
    long returnCode;
  
    RPCASSERT(RPCSendLong(VEM_OPEN_RPC_WINDOW_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(facet, RPCSendStream));
    RPCASSERT(RPCSendString(rpcSpec, RPCSendStream));
    RPCASSERT(RPCSendString(geoSpec, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    return (Window) returnCode;
}


/*
 * print a vem prompt and redisplay the command list
 */

void
vemPrompt()
{
    RPCASSERT(RPCSendLong(VEM_PROMPT_FUNCTION, RPCSendStream));
    return;
}


/*
 * execute a random vem command (just like from vem itself)
 */
vemStatus
vemCommand(vemCmd, spot, cmdList, userOptionWord)
char *vemCmd;
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    long returnCode;

    RPCASSERT(RPCSendLong(VEM_COMMAND_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(vemCmd, RPCSendStream));
    RPCASSERT(RPCSendVemArgs(spot, cmdList, userOptionWord, RPCSendStream));

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    return (vemStatus) returnCode;
}



/*
 * selected set stuff
 */

vemSelSet
vemNewSelSet(setFacet, red, green, blue, len, lineStyle, width, height, fillpat)
octId setFacet;
unsigned short red, green, blue;
int len;
int lineStyle;
int width, height;
char *fillpat;
{
    long set;

    RPCASSERT(RPCSendLong(VEM_NEW_SEL_SET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) setFacet, RPCSendStream));
    RPCASSERT(RPCSendLong((long) red, RPCSendStream));
    RPCASSERT(RPCSendLong((long) green, RPCSendStream));
    RPCASSERT(RPCSendLong((long) blue, RPCSendStream));
    RPCASSERT(RPCSendLong((long) len, RPCSendStream));
    RPCASSERT(RPCSendLong((long) lineStyle, RPCSendStream));
    RPCASSERT(RPCSendLong((long) width, RPCSendStream));
    RPCASSERT(RPCSendLong((long) height, RPCSendStream));
    RPCASSERT(RPCSendString(fillpat, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&set, RPCReceiveStream));
    return (vemSelSet) set;
}


vemStatus
vemFreeSelSet(selset)
vemSelSet selset;
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(VEM_FREE_SEL_SET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) selset, RPCSendStream));

    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
}


vemStatus
vemClearSelSet(selset)
vemSelSet selset;
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(VEM_CLEAR_SEL_SET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) selset, RPCSendStream));

    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
}


vemStatus
vemAddSelSet(selset, id)
vemSelSet selset;
octId id;
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(VEM_ADD_SEL_SET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) selset, RPCSendStream));
    RPCASSERT(RPCSendLong((long) id, RPCSendStream));

    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
    
}


vemStatus
vemDelSelSet(selset, id)
vemSelSet selset;
octId id;
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(VEM_DEL_SEL_SET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) selset, RPCSendStream));
    RPCASSERT(RPCSendLong((long) id, RPCSendStream));

    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */

    return (vemStatus) returnCode;
}


vemStatus
vemZoomSelSet(selset, win, scale)
vemSelSet selset;
Window win;
double scale;
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(VEM_ZOOM_SEL_SET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) selset, RPCSendStream));
    RPCASSERT(RPCSendLong((long) win, RPCSendStream));
    RPCASSERT(RPCSendFloat(scale, RPCSendStream));

    RPCFLUSH(RPCSendStream);

#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */

    return (vemStatus) returnCode;
}


/*
 * the foreign function interface does not handle floating
 * point numbers very well, so this is in here
 *
 *  specific to the remote application Critic
 *
 *  XXX this should be fixed
 */
#ifdef VAXLISP
void
vemZoomSelSetCritic(set, window)
vemSelSet set;
long window;
{
    vemZoomSelSet(set, window, 0.3);
}
#endif /* VAXLISP */


/*
 * vem utility package
 */

vemStatus
vuFindSpot(spot, rtnObj, mask)
RPCSpot *spot;
octObject *rtnObj;
octObjectMask mask;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong(VU_FIND_SPOT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendSpot(spot, RPCSendStream));
    RPCASSERT(RPCSendLong((long) mask, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    if ((vemStatus) returnCode == VEM_OK) {
	RPCASSERT(RPCReceiveOctObject(rtnObj, RPCReceiveStream));
    }
    
    return (vemStatus) returnCode;
}


vemStatus
vuFindLayer(facet, point, layername)
octObject *facet;
struct octPoint *point;
char *layername;
{
    long returnCode;
    char *buffer;
    
    RPCASSERT(RPCSendLong(VU_FIND_LAYER_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) facet->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctPoint(point, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    if ((vemStatus) returnCode == VEM_OK) {
	RPCASSERT(RPCReceiveString(&buffer, RPCReceiveStream));
	(void) strcpy(layername, buffer);
        RPCFREE(buffer);
    }
    
    return (vemStatus) returnCode;
}


void
vemFlushTechInfo(facet)
octObject *facet;
{
    RPCASSERT(RPCSendLong(VEM_FLUSH_TECH_INFO_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) facet->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}


void
vemSetDisplayType(name, chromatism)
char *name;
int chromatism;
{
    RPCASSERT(RPCSendLong(VEM_SET_DISPLAY_TYPE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(name, RPCSendStream));
    RPCASSERT(RPCSendLong((long) chromatism, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}


    char *nm;
    long chrom;

    
void
vemGetDisplayType(name, chromatism)
char **name;
int *chromatism;
{
    RPCASSERT(RPCSendLong(VEM_GET_DISPLAY_TYPE_FUNCTION, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveString(&nm, RPCReceiveStream));
    RPCASSERT(RPCReceiveLong(&chrom, RPCReceiveStream));
    *name = nm;
    *chromatism = chrom;
    return;
}


vemStatus
vemWnGetOptions(window, options)
Window window;
wnOpts *options;
/*
 * fill in the wnOpts structure for a window
 */
{
    long returnCode;

    
    RPCASSERT(RPCSendLong(WN_GET_OPTIONS_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) window, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    if ((vemStatus) returnCode == VEM_OK) {
	long dummy;
	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->disp_options = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->lambda = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->snap = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->grid_base = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->grid_minbase = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->grid_majorunits = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->grid_minorunits = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->grid_diff = (int) dummy;

	RPCASSERT(RPCReceiveFloat(&options->bb_thres, RPCReceiveStream));
	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->bb_min = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->con_min = (int) dummy;

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->solid_thres = (int) dummy;

	RPCASSERT(RPCReceiveString(&options->interface, RPCReceiveStream));

	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	options->inst_prio = (int) dummy;
    }

    return (vemStatus) returnCode;
}


vemStatus
vemWnSetOptions(window, options)
Window window;
wnOpts *options;
/*
 * set the wnOpts structure for a window
 */
{
    long returnCode = VEM_OK;

    
    RPCASSERT(RPCSendLong(WN_SET_OPTIONS_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) window, RPCSendStream));

    /* read only properties */
    options->disp_options &= ~VEM_REMOTERUN;
    options->disp_options &= ~VEM_NODRAW;
    RPCASSERT(RPCSendLong((long) options->disp_options, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->lambda, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->snap, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->grid_base, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->grid_minbase, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->grid_majorunits, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->grid_minorunits, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->grid_diff, RPCSendStream));
    RPCASSERT(RPCSendFloat(options->bb_thres, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->bb_min, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->con_min, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->solid_thres, RPCSendStream));
    RPCASSERT(RPCSendString(options->interface, RPCSendStream));
    RPCASSERT(RPCSendLong((long) options->inst_prio, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
}
	

vemStatus
vemWnTurnOffLayer(window, layerName)
Window window;
char *layerName;
/*
 * turn off the display of 'layerName' in a window
 */
{
    long returnCode = VEM_OK;

    
    RPCASSERT(RPCSendLong(WN_TURN_OFF_LAYER_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) window, RPCSendStream));
    RPCASSERT(RPCSendString(layerName, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
}


vemStatus
vemWnTurnOnLayer(window, layerName)
Window window;
char *layerName;
/*
 * turn on the display of 'layerName' in a window
 */
{
    long returnCode = VEM_OK;

    
    RPCASSERT(RPCSendLong(WN_TURN_ON_LAYER_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) window, RPCSendStream));
    RPCASSERT(RPCSendString(layerName, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
}


vemStatus
bufLock(facetId)
octId facetId;
{
    long returnCode = VEM_OK;

    RPCASSERT(RPCSendLong(VEM_BUF_LOCK_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) facetId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */
    
    return (vemStatus) returnCode;
}

