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

/*
 * VEM section of RPC server
 *
 * Rick L Spickelmier, 3/20/86, last modified 4/3/87
 */

#include <X11/Xlib.h>
#include "xvals.h"
#include "list.h"
#include "message.h"
#include "buffer.h"
#include "selset.h"
#include "region.h"
#include "windows.h"
#include "vemDM.h"
#include "rpcServer.h"
#include "vemRPC.h"
#include "serverUtils.h"
#include "vemUtil.h"
#include "vemMain.h"		/* Pick up vemPrompt() */

#include "serverVem.h"

#include <fcntl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>

extern struct RPCApplication *RPCApplication;

int RPCFloatingPointSame = 0;

/* Forward decls */
rpcStatus RPCvemPrompt();


/*
 * set up the remote application information
 *
 * An application calls vemInitialize with an array of menu strings and
 * functions to call.  The menu strings are sent to VEM and used to build
 * application specific menus.  When a menu item is invoked, RPCUserFunction
 * is called with the application and function numbers, spot and agList
 * as arguments.
 *
 * Arguments:
 *   application - number of the application
 *                 integer from 0 to RPCMaxApplications - 1
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 */

#ifndef VEM_OK
#define VEM_OK RPC_OK
#endif /* VEM_OK */

rpcStatus
RPCVemInitializeApplication(application, receiveStream, sendStream)
int application;
STREAM sendStream, receiveStream;
{
    long version;
    long pid;
    struct RPCApplication *app;
    long *lngs;
    double dbl;

    app = &(RPCApplication[application]);

    RPCASSERT(RPCReceiveLong(&version, receiveStream), RPC_ERROR);

    if (version != RPC_VERSION) {
	char buf[1024];

	(void) sprintf(buf,
		       "RPC Error: librpc.a (RPC Version %ld) out of date with respect to vem (RPC Version %ld)\n",
		version, RPC_VERSION);
        vemMessage(buf, MSG_DISP);
        return RPC_ERROR;
    }

    /* get the remote process id - for possible killing */
    RPCASSERT(RPCReceiveLong(&pid, receiveStream), RPC_ERROR);
    
    /* get the floating point number format */
    lngs = (long*) &dbl;
    RPCASSERT(RPCReceiveLong(lngs, receiveStream), RPC_ERROR);
    lngs = lngs + 1;
    RPCASSERT(RPCReceiveLong(lngs, receiveStream), RPC_ERROR);

    if (dbl == RPCDOUBLE) {
	RPCFloatingPointSame = 1;
    } else {
	RPCFloatingPointSame = 0;
    }
    
    app->pid = pid;

    /* send the floating point number format information */
    if (RPCFloatingPointSame) {
	RPCASSERT(RPCSendLong((long) OCT_OK, sendStream), RPC_ERROR);
    } else {	
	RPCASSERT(RPCSendLong((long) OCT_ERROR, sendStream), RPC_ERROR);
    }

    /* send the display */
    RPCASSERT(RPCSendString(app->display, sendStream), RPC_ERROR);

    /* send the initial vem args */

    RPCASSERT(RPCSendVemArgs(app->spot, app->argList, app->userOptionWord, sendStream),
	      RPC_ERROR);

    return RPC_OK;
}


rpcStatus
RPCvemSendMenu(application, receiveStream, sendStream)
int application;
STREAM receiveStream, sendStream;
{
    char *str;
    long lng;
    long count;
    int i;
    struct RPCApplication *app;

    app = &(RPCApplication[application]);

    RPCASSERT(RPCReceiveLong(&count, receiveStream), RPC_ERROR);

    app->menuSize = count;
    app->paneString = RPCARRAYALLOC(char *, count);
    app->menuString = RPCARRAYALLOC(char *, count);
    app->keyBinding = RPCARRAYALLOC(char *, count);
    app->userOptionWords = RPCARRAYALLOC(long, count);
    
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveString(&str, receiveStream), RPC_ERROR);
	app->paneString[i] = str;
	
	RPCASSERT(RPCReceiveString(&str, receiveStream), RPC_ERROR);
	app->menuString[i] = str;
	
	RPCASSERT(RPCReceiveString(&str, receiveStream), RPC_ERROR);
	app->keyBinding[i] = str;
	
	RPCASSERT(RPCReceiveLong(&lng, receiveStream), RPC_ERROR);
	app->userOptionWords[i] = lng;
    }

    if (vemAltBindings(app->spot->theWin,
		       app->host,
		       app->name,
		       application,
		       (int) count,
		       app->paneString,
		       app->menuString,
                       app->keyBinding) != VEM_OK) {
         RPCASSERT(RPCSendLong((long) OCT_ERROR, sendStream), RPC_ERROR);
	 return RPC_ERROR;
     }
    
    RPCASSERT(RPCSendLong((long) OCT_OK, sendStream), RPC_ERROR);

    return RPC_OK;
}



/*
 * receive a remote vemMessage
 *
 * Arguments:
 *   stream - stream to send the message on
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 */
/*ARGSUSED*/
rpcStatus
RPCvemMessage(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *str;
    long options;
    
    RPCASSERT(RPCReceiveString(&str, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&options, receiveStream), RPC_ERROR);
    vemMessage(str, (int) options);

    RPCFREE(str);
    return RPC_OK;
}


/*
 * display the vem prompt and redisplay the argument list
 */
rpcStatus
RPCvemPrompt()
{
    (void) vemPrompt();
    return RPC_OK;
}


/*
 * queue region
 */
/*ARGSUSED*/
rpcStatus
RPCvemWnQRegion(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
#ifndef NOWAIT
    long returnCode;
#endif
    octObject facet;
    long id;
    struct octBox box;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    facet.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveOctBox(&box, receiveStream), RPC_ERROR);

    if (octGetById(&facet) < OCT_OK) {
	octError("facet by id");
	return RPC_ERROR;
    }

    /* This is now just in case */
    (void) _bufFindBuffer(facet.objectId);

#ifndef OLD_DISPLAY
    vemMsg(MSG_L, "RPC application requested redisplay -- ignored\n");
#else
#ifndef NOWAIT
    returnCode = (long) wnQRegion(facet.objectId, &box);
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
#else
    (void) wnQRegion(facet.objectId, &box);
#endif /* NOWAIT */
#endif /* OLD_DISPLAY */

    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemWnGetOptions(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    vemStatus returnCode;
    Window window;
    wnOpts options;
    

    RPCASSERT(RPCReceiveLong((long *) &window, receiveStream), RPC_ERROR);

    /* get the options */
    returnCode = wnGetInfo(window, &options, NIL(octId), NIL(octId),
			   NIL(bdTable), NIL(struct octBox));

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    

    if (returnCode == VEM_OK) {
	RPCASSERT(RPCSendLong((long) options.disp_options, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.lambda, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.snap, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.grid_base, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.grid_minbase, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.grid_majorunits, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.grid_minorunits, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.grid_diff, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendFloat((double) options.bb_thres, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.bb_min, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.con_min, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.solid_thres, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendString(options.interface, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendLong((long) options.inst_prio, sendStream), RPC_ERROR);
    }
    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemWnSetOptions(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    Window window;
    vemStatus returnCode;
    wnOpts options;
    int disp_options;
    long temp;
    
    
    RPCASSERT(RPCReceiveLong((long *) &window, receiveStream), RPC_ERROR);

    /* get the old options information - for the layer table and read-only stuff */
    returnCode = wnGetInfo(window, &options, NIL(octId), NIL(octId),
			   NIL(bdTable), NIL(struct octBox));

    disp_options = options.disp_options;
    RPCASSERT(RPCReceiveLong(&temp, receiveStream), RPC_ERROR);
    options.disp_options = (int) temp;
    if ((disp_options & VEM_REMOTERUN) == VEM_REMOTERUN) {
	options.disp_options |= VEM_REMOTERUN;
    } else {
	options.disp_options &= ~VEM_REMOTERUN;
    }
    
    if ((disp_options & VEM_NODRAW) == VEM_NODRAW) {
	options.disp_options |= VEM_NODRAW;
    } else {
	options.disp_options &= ~VEM_NODRAW;
    }
    
    RPCASSERT(RPCReceiveLong((long *) &options.lambda, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.snap, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.grid_base, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.grid_minbase, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.grid_majorunits, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.grid_minorunits, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.grid_diff, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveFloat((double *) &options.bb_thres, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.bb_min, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.con_min, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.solid_thres, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&options.interface, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong((long *) &options.inst_prio, receiveStream), RPC_ERROR);

    if (returnCode != VEM_OK) {
#ifndef NOWAIT	
	RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#endif
	return RPC_OK;
    }
    
    returnCode = wnSetInfo(window, &options, NIL(bdTable));
#ifndef NOWAIT
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#endif

    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemWnTurnOffLayer(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    Window window;
    char *layerName;
    wnOpts options;
    vemStatus returnCode;
    static char *dummy;

    
    RPCASSERT(RPCReceiveLong((long *) &window, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&layerName, receiveStream), RPC_ERROR);
    
    if ((returnCode = wnGetInfo(window, &options, NIL(octId), NIL(octId),
				NIL(bdTable), NIL(struct octBox))) != VEM_OK) {
#ifndef NOWAIT	
	RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#endif
	return RPC_OK;
    }
    if (options.off_layers == NIL(st_table)) {
	options.off_layers = st_init_table(strcmp, st_strhash);
    }
    st_insert(options.off_layers, layerName, dummy);
    returnCode = wnSetInfo(window, &options, NIL(bdTable));
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#endif
    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemWnTurnOnLayer(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    Window window;
    char *layerName;
    wnOpts options;
    vemStatus returnCode = VEM_OK;
    char *dummy;

    
    RPCASSERT(RPCReceiveLong((long *) &window, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&layerName, receiveStream), RPC_ERROR);
    
    if ((returnCode = wnGetInfo(window, &options, NIL(octId), NIL(octId),
				NIL(bdTable), NIL(struct octBox))) != VEM_OK) {
#ifndef NOWAIT	
	RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#endif
	return RPC_OK;
    }
    if (options.off_layers != NIL(st_table)) {
	st_delete(options.off_layers, &layerName, &dummy);
	returnCode = wnSetInfo(window, &options, NIL(bdTable));
    }
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#endif
    return RPC_OK;
}


/*
 * flush
 */
/*ARGSUSED*/
rpcStatus
RPCvemWnFlush(sendStream)
STREAM sendStream;
{
#ifndef NOWAIT
    long returnCode;
    returnCode = (long) wnQuickFlush();
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    (void) wnQuickFlush();
#endif
    return RPC_OK;
}


/*
 * open a VEM window
 */

extern Window vemOpenWindow();

rpcStatus
RPCvemOpenWindow(application, receiveStream, sendStream)
int application;
STREAM receiveStream, sendStream;
{
    octObject facet;
    Window window;
    char *geo;

    RPCASSERT(RPCReceiveOctObject(&facet, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&geo, receiveStream), RPC_ERROR);

    window = vemOpenWindow(&facet, geo, RPCApplication[application].host,
				   RPCApplication[application].name,
				   application);

    RPCASSERT(RPCSendLong((long) window, sendStream), RPC_ERROR);

    RPCFREE(geo);

    return RPC_OK;
}


/*
 * open a VEM window, and start another rpc application running
 */

extern Window vemOpenRPCWindow();

rpcStatus
RPCvemOpenRPCWindow(application, receiveStream, sendStream)
int application;
STREAM receiveStream, sendStream;
{
    octObject facet;
    Window window;
    char *rpcSpec;
    char *geoSpec;

    RPCASSERT(RPCReceiveOctObject(&facet, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&rpcSpec, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&geoSpec, receiveStream), RPC_ERROR);

    window = vemOpenRPCWindow(&facet, rpcSpec, geoSpec);

    RPCASSERT(RPCSendLong((long) window, sendStream), RPC_ERROR);

    RPCFREE(rpcSpec);
    RPCFREE(geoSpec);

    return RPC_OK;
}


/*
 * technology stuff
 */

extern void vemFlushTechInfo();
extern void vemSetDisplayType();
extern void vemGetDisplayType();


rpcStatus
RPCvemFlushTechInfo(receiveStream)
STREAM receiveStream;
{
    octObject facet;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    facet.objectId = (octId) id;

    (void) octGetById(&facet);

    vemFlushTechInfo(&facet);

    return RPC_OK;
}


rpcStatus
RPCvemSetDisplayType(receiveStream)
STREAM receiveStream;
{
    char *name;
    long chromatism;

    RPCASSERT(RPCReceiveString(&name, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&chromatism, receiveStream), RPC_ERROR);

    vemSetDisplayType(name, (int) chromatism);
    
    RPCFREE(name);

    return RPC_OK;
}


rpcStatus
RPCvemGetDisplayType(sendStream)
STREAM sendStream;
{
    char *name;
    int chromatism;

    vemGetDisplayType(&name, &chromatism);

    RPCASSERT(RPCSendString(name, sendStream), RPC_ERROR);
    RPCASSERT(RPCSendLong((long) chromatism, sendStream), RPC_ERROR);

    return RPC_OK;
}



/*
 * vem selected set stuff
 */

rpcStatus
RPCvemNewSelSet(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    long setFacet;
    long red, green, blue;
    long len, lineStyle;
    long width, height;
    char *fillpat;
    vemOneArg *selset;
    
    RPCASSERT(RPCReceiveLong(&setFacet, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&red, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&green, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&blue, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&len, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&lineStyle, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&width, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&height, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&fillpat, receiveStream), RPC_ERROR);

    selset = vemNewSelSet((octId) setFacet,
			  (unsigned short) red,
			  (unsigned short) green,
			  (unsigned short) blue,
			  (int) len,
			  (int) lineStyle,
			  (int) width,
			  (int) height, fillpat);
    
    RPCASSERT(RPCSendLong((long) selset, sendStream), RPC_ERROR);
    
    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemFreeSelSet(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
#ifndef NOWAIT
    long returnCode;
#endif
    long selset;

    RPCASSERT(RPCReceiveLong(&selset, receiveStream), RPC_ERROR);
    
#ifndef NOWAIT
    returnCode = (long) vemFreeSelSet((vemOneArg *) selset);
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
#else
    (void) vemFreeSelSet((vemOneArg *) selset);
#endif

    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemClearSelSet(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
#ifndef NOWAIT
    long returnCode;
#endif
    long selset;

    RPCASSERT(RPCReceiveLong(&selset, receiveStream), RPC_ERROR);
    
#ifndef NOWAIT
    returnCode = (long) vemClearSelSet((vemOneArg *) selset);
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
#else
    (void) vemClearSelSet((vemOneArg *) selset);
#endif
    
    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemAddSelSet(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
#ifndef NOWAIT
    long returnCode;
#endif
    long selset;
    long id;

    RPCASSERT(RPCReceiveLong(&selset, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);

#ifndef NOWAIT
    returnCode = (long) vemAddSelSet((vemOneArg *) selset, (octId) id);
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
#else
    (void) vemAddSelSet((vemOneArg *) selset, (octId) id);
#endif
    
    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemDelSelSet(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
#ifndef NOWAIT
    long returnCode;
#endif
    long selset;
    long id;

    RPCASSERT(RPCReceiveLong(&selset, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);

#ifndef NOWAIT
    returnCode = (long) vemDelSelSet((vemOneArg *) selset, (octId) id);
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
#else
    (void) vemDelSelSet((vemOneArg *) selset, (octId) id);
#endif

    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemZoomSelSet(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
#ifndef NOWAIT
    long returnCode;
#endif
    long selset;
    long win;
    double scale;

    RPCASSERT(RPCReceiveLong(&selset, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&win, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveFloat(&scale, receiveStream), RPC_ERROR);

#ifndef NOWAIT
    returnCode = (long) vemZoomSelSet((vemOneArg *) selset, (Window) win, scale);
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
#else
    (void) vemZoomSelSet((vemOneArg *) selset, (Window) win, scale);
#endif

    return RPC_OK;
}



/*
 * remote region package
 */

rpcStatus
RPCvemRegInit(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    octObject parentObj;
    struct octBox someExtent;
    long someMask;
    regGenerator generator;

    RPCASSERT(RPCReceiveOctObject(&parentObj, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveOctBox(&someExtent, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&someMask, receiveStream), RPC_ERROR);
    regInit(&parentObj, &someExtent, (octObjectMask) someMask, &generator);
    RPCASSERT(RPCSendRegionGenerator(&generator, sendStream), RPC_ERROR);
    return RPC_OK;
}

rpcStatus
RPCvemRegNext(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    long returnCode;
    regGenerator generator;
    octObject object;
    int num_points;
    struct octPoint *points;
    struct octBox size;
    int i;

    RPCASSERT(RPCReceiveRegionGenerator(&generator, receiveStream), RPC_ERROR);

    returnCode = (long) regNext(&generator, &object, &num_points, &points, &size);

    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    RPCASSERT(RPCSendRegionGenerator(&generator, sendStream), RPC_ERROR);

    if (returnCode == (long) REG_OK) {
    
	/* send the actual object generated */
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
	
	if ((object.type == OCT_POLYGON) || (object.type == OCT_PATH)) {
	    RPCASSERT(RPCSendLong((long) num_points, sendStream), RPC_ERROR);

	    for (i = 0; i < num_points; i++) {
		RPCASSERT(RPCSendOctPoint(&(points[i]), sendStream), RPC_ERROR);
	    }
	}
	RPCASSERT(RPCSendOctBox(&size, sendStream), RPC_ERROR);
    }

    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCvemRegEnd(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    regGenerator generator;

    RPCASSERT(RPCReceiveRegionGenerator(&generator, receiveStream), RPC_ERROR);
    regEnd(&generator);
    return RPC_OK;
}


rpcStatus
RPCregObjStart(readStream, sendStream)
STREAM readStream, sendStream;
{
    octObject theFacet;
    struct octBox region;
    long mask;
    long returnCode;
    long allInside;
    regObjGen theGen;
    long id;
    
    
    RPCASSERT(RPCReceiveLong(&id, readStream), RPC_ERROR);
    theFacet.objectId = (octId) id;
    octGetById(&theFacet);
    
    RPCASSERT(RPCReceiveOctBox(&region, readStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&mask, readStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&allInside, readStream), RPC_ERROR);
    
    returnCode = (long) regObjStart(&theFacet, &region, (octObjectMask) mask,
				    &theGen, (int) allInside);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCSendLong((long) theGen, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


rpcStatus
RPCregObjNext(readStream, sendStream)
STREAM readStream, sendStream;
{
    octObject theObj;
    long theGen;
    long returnCode;
        
    RPCASSERT(RPCReceiveLong(&theGen, readStream), RPC_ERROR);
      
    returnCode = (long) regObjNext((regObjGen) theGen, &theObj);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCSendOctObject(&theObj, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


/*ARGSUSED*/
rpcStatus
RPCregObjFinish(readStream, sendStream)
STREAM readStream, sendStream;
{
    long theGen;
   
    RPCASSERT(RPCReceiveLong(&theGen, readStream), RPC_ERROR);
    regObjFinish((regObjGen) theGen);
    return RPC_OK;
}

    
rpcStatus
RPCregFindActual(readStream, sendStream)
STREAM readStream, sendStream;
{
    octObject theFacet;
    long id;
    struct octPoint thePoint;
    octObject termObject;
    struct octBox implBB;
    long returnCode;
    
    RPCASSERT(RPCReceiveLong(&id, readStream), RPC_ERROR);
    theFacet.objectId = (octId) id;
    RPCASSERT(RPCReceiveOctPoint(&thePoint, readStream), RPC_ERROR);
    
    (void) octGetById(&theFacet);
    
    returnCode = (long) regFindActual(&theFacet, &thePoint, &termObject, &implBB);

    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCSendOctObject(&termObject, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendOctBox(&implBB, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


rpcStatus
RPCregFindImpl(readStream, sendStream)
STREAM readStream, sendStream;
{
    octObject theTerm;
    long id;
    struct octBox implBB;
    long returnCode;
    
    RPCASSERT(RPCReceiveLong(&id, readStream), RPC_ERROR);
    theTerm.objectId = (octId) id;
    
    (void) octGetById(&theTerm);
    
    returnCode = (long) regFindImpl(&theTerm, &implBB);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCSendOctBox(&implBB, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


rpcStatus
RPCregFindNet(readStream, sendStream)
STREAM readStream, sendStream;
{
    octObject obj, netObj;
    long id;
    long returnCode;
    
    RPCASSERT(RPCReceiveLong(&id, readStream), RPC_ERROR);
    obj.objectId = (octId) id;
    
    (void) octGetById(&obj);
    
    returnCode = (long) regFindNet(&obj, &netObj);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCSendOctObject(&netObj, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


rpcStatus
RPCregErrorString(readStream, sendStream)
STREAM readStream, sendStream;
{
    char error_string[1024];

    (void) strcpy(error_string, regErrorString());
    RPCASSERT(RPCSendString(error_string, sendStream), RPC_ERROR);
    
    return RPC_OK;
}


/*
 * vem dialog manager stuff
 */

rpcStatus
RPCdmMultiWhich(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *title, *help_buf;
    int i;
    long count;
    dmWhichItem *items;
    long returnCode;
                    
    RPCASSERT(RPCReceiveString(&title, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&count, receiveStream), RPC_ERROR);

    items = RPCARRAYALLOC(dmWhichItem, count);
        
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveString(&(items[i].itemName), receiveStream), RPC_ERROR);
	items[i].userData = (Pointer) 0;
	items[i].flag = 0;
    }
    
    RPCASSERT(RPCReceiveString(&help_buf, receiveStream), RPC_ERROR);
    
    returnCode = (long) dmMultiWhich(title, (int) count, items,
				     (int (*) ()) 0, help_buf);

    RPCFREE(title);
    RPCFREE(help_buf);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    for (i = 0; i < count; i ++) {
	RPCASSERT(RPCSendLong((long) items[i].flag, sendStream), RPC_ERROR);
    }
    
    RPCFLUSH(sendStream, RPC_ERROR);
    
    return RPC_OK;
}


rpcStatus
RPCdmWhichOne(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    int i, returnCode, itemSelected = 0;
    long count;
    char *title, *help_buf;
    dmWhichItem *items;
            
    RPCASSERT(RPCReceiveString(&title, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&count,  receiveStream), RPC_ERROR);

    items = RPCARRAYALLOC(dmWhichItem, count);
        
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveString(&(items[i].itemName), receiveStream), RPC_ERROR);
	items[i].userData = (Pointer) 0;
	items[i].flag = 0;
    }
    
    RPCASSERT(RPCReceiveString(&help_buf, receiveStream), RPC_ERROR);
    
    returnCode = (long) dmWhichOne(title, (int) count, items, &itemSelected,
				   (int (*) ()) 0, help_buf);
    RPCFREE(title);
    RPCFREE(help_buf);
        
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    
    RPCASSERT(RPCSendLong((long) itemSelected, sendStream), RPC_ERROR);
    
    for (i = 0; i < count; i ++) {
	RPCASSERT(RPCSendLong((long) items[i].flag, sendStream), RPC_ERROR);
    }
    
    RPCFLUSH(sendStream, RPC_ERROR);
    
    return RPC_OK;
}


rpcStatus
RPCdmMultiText(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *title;
    int i;
    long count, dummy;
    dmTextItem *items;
    long returnCode;
    
    RPCASSERT(RPCReceiveString(&title, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&count, receiveStream), RPC_ERROR);

    items = RPCARRAYALLOC(dmTextItem, count);
        
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveString(&(items[i].itemPrompt), receiveStream), RPC_ERROR);
	RPCASSERT(RPCReceiveLong(&dummy, receiveStream), RPC_ERROR);
	items[i].rows = (int) dummy;
	
	RPCASSERT(RPCReceiveLong(&dummy, receiveStream), RPC_ERROR);
	items[i].cols = (int) dummy;
	
	RPCASSERT(RPCReceiveString(&(items[i].value), receiveStream), RPC_ERROR);
	items[i].userData = (Pointer) 0;
    }
    
    returnCode = (int) dmMultiText(title, (int) count, items);
    
    RPCFREE(title);
    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);

    for (i = 0; i < count; i ++) {
	RPCASSERT(RPCSendString(items[i].value, sendStream), RPC_ERROR);
    }
    
    RPCFLUSH(sendStream, RPC_ERROR);
    
    return RPC_OK;
}


rpcStatus
RPCdmConfirm(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *title, *message, *yesButton, *noButton;
    long returnCode;
    
    RPCASSERT(RPCReceiveString(&title, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&message, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&yesButton, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&noButton, receiveStream), RPC_ERROR);

    returnCode = (long) dmConfirm(title, message, yesButton, noButton);
    
    RPCFREE(title);
    RPCFREE(message);
    RPCFREE(yesButton);
    RPCFREE(noButton);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);

    RPCFLUSH(sendStream, RPC_ERROR);
    
    return RPC_OK;
}



/* 
 * execute a random vem command (just like from vem itself)
 */
rpcStatus
RPCvemCommand(application)
int application;
{
    STREAM sendStream, receiveStream;
    char *str;
    RPCSpot spot;
    lsList cmdList;
    long userOptionWord;
    vemStatus returnCode;

    octObject facet;
    vemPoint vemSpot;
    lsList vemCmdList;

    receiveStream = RPCApplication[application].ReceiveStream;
    sendStream = RPCApplication[application].SendStream;

    RPCASSERT(RPCReceiveString(&str, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveVemArgs(&spot, &cmdList, &userOptionWord, receiveStream),
			       RPC_ERROR);

    facet.objectId = spot.facet;
    if (octGetById(&facet) < OCT_OK) {
	octError("RPCvemCommand: can not get facet by id");
	return RPC_ERROR;
    }

    /* set up the vemSpot */
    vemSpot.theWin = spot.theWin;

    vemSpot.x = 0;
    vemSpot.y = 0;

    /* This is now just in case */
    (void) _bufFindBuffer(facet.objectId);

    vemSpot.theFct = facet.objectId;
    vemSpot.thePoint = spot.thePoint;

    /* translate the RPC arg list to a VEM arglist */

    (void) RPCTranslateArgsToVEM(cmdList, &vemCmdList);

    returnCode = vemCommand(str, &vemSpot, vemCmdList, userOptionWord);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
/*    RPCFLUSH(sendStream, RPC_ERROR); */
    return RPC_OK;
}



/*
 * call a remote user function and pass the arguments
 *
 * how remote functions are invoked and what is done:
 *
 * A VEM menu select on an application specific menu causes
 * VEM call RPCUserFunction with the application number, function number,
 * remote spot and remote argList.
 * The remote spot and remote argList are stripped down versions of the
 * tightly bound ones.
 *
 * The following is sent to the remote application:
 *
 * function number
 * spot
 * number of arguments
 * type of arg1
 * length of arg1
 * arg1
 * type of arg2
 * length of arg2
 * arg2
 * ....
 * ....
 * type of argN
 * length of argN
 * argN
 *
 * Arguments:
 *   application - number of the application
 *                 integer from 0 to RPCMaxApplications - 1
 *   functionNumber - number of the remote function to be performed
 *                    integer from 0 to ??
 *   Spot - vem spot
 *   argList - vem arglist
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 */

rpcStatus
RPCUserFunction(application, functionNumber, Spot, argList)
int application;
long functionNumber;
vemPoint *Spot;
lsList argList;
{
    STREAM stream;
    RPCSpot spot;
    lsList cmdList;

    if (RPCApplication[application].state == RPC_BUSY) {
/* fprintf(stderr, "aborting out of user function since vem is busy\n"); */
	vemMessage("Something is already running, wait awhile\n",
		   MSG_NOLOG|MSG_DISP);
	return RPC_ERROR;
    }

    /*
     * build the stripped down spot
     */
    spot.theWin = Spot->theWin;
    spot.thePoint = Spot->thePoint;
    spot.facet = Spot->theFct;

    RPCApplication[application].state = RPC_BUSY;
/* fprintf(stderr, "going busy (user function request)\n"); */

    stream = RPCApplication[application].SendStream;

    /* send the function number */
    RPCASSERT(RPCSendLong(functionNumber, stream), RPC_ERROR);

    /*
     * strip and send the arguments
     */

    /* create a cloned bag list */
    RPCApplication[application].clonedBags = lsCreate();
    (void) RPCTranslateArgsToRPC(argList, &cmdList, spot.facet, application);
    
    RPCASSERT(RPCSendVemArgs(&spot, cmdList, RPCApplication[application].userOptionWords[functionNumber], stream),
			    RPC_ERROR);

    RPCFLUSH(stream, RPC_ERROR);
    return RPC_OK;
}


rpcStatus
RPCVemRemoteFunctionComplete(application, sendStream)
int application;
STREAM sendStream;
{
    extern octStatus octDelete();

    RPCApplication[application].state = RPC_IDLE;
/* fprintf(stderr, "going idle (function complete)\n"); */

    /* process all outstanding queued regions */
    wnQuickFlush();

    /* delete all cloned bags */
    lsDestroy(RPCApplication[application].clonedBags, (void (*)()) octDelete);

    /* XXXXX */
#ifdef notdef
    vemRemoteFunctionComplete(application);
#endif /* notdef */

    RPCASSERT(RPCSendLong((long) 0, sendStream), RPC_ERROR);
/*    RPCFLUSH(sendStream, RPC_ERROR); */

    return RPC_OK;
}



rpcStatus
RPCvuFindSpot(readStream, sendStream)
STREAM readStream, sendStream;
{
    RPCSpot spot;
    octObject facet, rtnObj;
    long returnCode;
    long mask;
    vemPoint vemSpot;
    
    RPCASSERT(RPCReceiveSpot(&spot, readStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&mask, readStream), RPC_ERROR);
    
    /* set up the vemSpot */

    facet.objectId = spot.facet;
    octGetById(&facet);
    
    vemSpot.theWin = spot.theWin;

    vemSpot.x = 0;
    vemSpot.y = 0;

    /* This is now just in case */
    (void) _bufFindBuffer(facet.objectId);

    vemSpot.theFct = facet.objectId;
    vemSpot.thePoint = spot.thePoint;

    returnCode = (long) vuFindSpot(&vemSpot, &rtnObj, (octObjectMask) mask);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((vemStatus) returnCode == VEM_OK) {
	RPCASSERT(RPCSendOctObject(&rtnObj, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


rpcStatus
RPCvuFindLayer(readStream, sendStream)
STREAM readStream, sendStream;
{
    octObject facet;
    long id;
    struct octPoint point;
    long returnCode;
    char buffer[1024];
    
    RPCASSERT(RPCReceiveLong(&id, readStream), RPC_ERROR);
    facet.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveOctPoint(&point, readStream), RPC_ERROR);
    
    octGetById(&facet);
    
    returnCode = (long) vuFindLayer(&facet, &point, buffer);
    
    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);
    
    if ((vemStatus) returnCode == VEM_OK) {
	RPCASSERT(RPCSendString(buffer, sendStream), RPC_ERROR);
    }
    
    return RPC_OK;
}


rpcStatus
RPCcleanExit(application, readStream)
int application;
STREAM readStream;
{
    long status;
    char buffer[1024];
    
    RPCASSERT(RPCReceiveLong(&status, readStream), RPC_ERROR);

    /* tell the USER that the application closed */
    (void) sprintf(buffer, "\n%s running on %s exited with status %ld\n",
	    RPCApplication[application].name,
	    RPCApplication[application].host,
	    status);
    vemMessage(buffer, MSG_NOLOG|MSG_DISP);

    /* tell RPC that the application closed */
    RPCServerEnd(application);

    /*
     * tell VEM that the application closed
     */
    (void) vemCloseApplication(RPCApplication[application].host,
			       RPCApplication[application].name,
			       application);
    return RPC_OK;
}


rpcStatus
RPCExitVem(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    vemMsg(MSG_A, "RPC Application has requested termination\n");
    sleep(1);
    octEnd();
    exit(0);
    /*NOTREACHED*/
    return RPC_ERROR;
}


rpcStatus
RPCRegisterDemon(receiveStream, sendStream, app)
STREAM receiveStream, sendStream;
int app;
{
    long id;
    struct rpcDemon *demon;
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    demon = ALLOC(struct rpcDemon, 1);
    demon->clId = (octId) id;
    demon->app = app;
    lsNewEnd(RPCDemonList, (Pointer) demon, 0);
    /* fprintf(stderr, "new demon registered\n"); */
    return RPC_OK;
}


rpcStatus
RPCXGetDefault(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *program, *name, *buffer;

    RPCASSERT(RPCReceiveString(&program, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&name, receiveStream), RPC_ERROR);

    buffer = XGetDefault(xv_disp(), program, name);

    RPCFREE(program);
    RPCFREE(name);

    RPCASSERT(RPCSendString(buffer, sendStream), RPC_ERROR);
    
    return RPC_OK;
}


extern long lseek();

rpcStatus
RPCfileOpen(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *file, *mode;
    int socket, newsocket, port, flags;

    RPCASSERT(RPCReceiveString(&file, receiveStream), RPC_ERROR);

    RPCASSERT(RPCReceiveString(&mode, receiveStream), RPC_ERROR);
    
    /* socket, bind, and listen */
    if ((socket = RPCSetUpSocket("inet", &port)) == RPC_ERROR) {
	return RPC_ERROR;
    }

    /* send port */
    RPCASSERT(RPCSendLong((long) port, sendStream), RPC_ERROR);
    RPCFLUSH(sendStream, RPC_ERROR);

    /* accept, Dave is going to hate this!  BLOCK!! */
    if ((newsocket = accept(socket, RPCNIL(struct sockaddr), RPCNIL(int))) < 0) {
	perror("file accept");
	return RPC_ERROR;
    }

    (void) close(socket);

    /*
     * set to non-blockin - want to be able to detect when no more data
     * is available
     */
    if (fcntl(newsocket, F_SETFL, FNDELAY) < 0) {
	perror("file fcntl");
	return RPC_ERROR;
    }

    /* flag settings from 'fopen' */
    if (mode[0] == 'r') {
	flags = O_RDONLY;
    } else if (mode[0] == 'w') {
	flags = O_TRUNC|O_CREAT|O_WRONLY;
    } else if (mode[0] == 'a') {
	flags = O_CREAT|O_WRONLY;
    } else {
	(void) fprintf(stderr, "RPC Error: bad file mode\n");
	(void) close(newsocket);
	return RPC_OK;
    }

    /* open the file */
    if ((RPCFileDescriptors[newsocket] = open(file, flags, 0666)) < 0) {
	perror("file open");
	(void) close(newsocket);
	return RPC_OK;
    }

#ifndef L_XTND
#define L_XTND 2
#endif /* L_XTND */

    /* if append mode, lseek to the end of the file */
    if (mode[0] == 'a') {
	(void) lseek(RPCFileDescriptors[newsocket], (long) 0, L_XTND);
    }

    /* set up the selection mask */
    RPC_FD_SET(newsocket, RPCFileRequest);
    RPCLargestFD = MAX(RPCLargestFD, newsocket);

    return RPC_OK;

}


rpcStatus
RPCdmMessage(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    char *title, *message;
    long formatp;
    
    RPCASSERT(RPCReceiveString(&title, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&message, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&formatp, receiveStream), RPC_ERROR);

    dmMessage(title, message, formatp);
    
    RPCFREE(title);
    RPCFREE(message);

    return RPC_OK;
}


int
RPCDemonFunction(app, cl)
int app;
octObject *cl;
{
    int state = RPCApplication[app].state;
    FILE *stream = RPCApplication[app].SendStream;

    if (state == RPC_BUSY) {
/* fprintf(stderr, "aborting out of demon function since vem is busy\n"); */
	return RPC_ERROR;
    }
    RPCApplication[app].state = RPC_BUSY;
/* fprintf(stderr, "going busy (demon function request)\n"); */
    RPCApplication[app].clonedBags = lsCreate();

    RPCASSERT(RPCSendLong((long) RPC_DEMON_FUNCTION, stream), RPC_ERROR);
    RPCASSERT(RPCSendLong((long) cl->objectId, stream), RPC_ERROR);
    RPCFLUSH(stream, RPC_ERROR);

    return RPC_OK;
}


rpcStatus
RPCbufLock(receiveStream, sendStream)
STREAM receiveStream, sendStream;
{
    long id;
#ifndef NOWAIT
    vemStatus status;
#endif

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
#ifndef NOWAIT
    status = bufLock((octId) id);
    RPCASSERT(RPCSendLong((long) status, sendStream), RPC_ERROR);
    RPCFLUSH(stream, RPC_ERROR);
#else
    (void) bufLock((octId) id);
#endif
    return RPC_OK;
}

/*
 * process a remote VEM request
 *
 * This routine receives the VEM function request from the application and
 * calls the proper VEM routine to deal with it.
 *
 * Arguments:
 *   application - number of the application
 *                 integer from 0 to RPCMaxApplications - 1
 *   functionNumber - number of the function (see rpcInternal.h)
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 *
 */
rpcStatus
RPCVEMRequest(application, functionNumber)
int application;
long functionNumber;
{
    rpcStatus status;
    STREAM receiveStream, sendStream;

    receiveStream = RPCApplication[application].ReceiveStream;	/* grab the streams */
    sendStream = RPCApplication[application].SendStream;

    switch (functionNumber) {
	
	case VEM_MESSAGE_FUNCTION:
	    status = RPCvemMessage(receiveStream, sendStream);
	    break;

	case VEM_PROMPT_FUNCTION:
	    status = RPCvemPrompt();
	    break;

	case VEM_INITIALIZE_APPLICATION_FUNCTION:
	    status = RPCVemInitializeApplication(application, receiveStream, sendStream);
	    break;

	case VEM_SEND_MENU_FUNCTION:
	    status = RPCvemSendMenu(application, receiveStream, sendStream);
	    break;

	case VEM_COMMAND_FUNCTION:
	    status = RPCvemCommand(application);
	    break;

	case VEM_OPEN_WINDOW_FUNCTION:
	    status = RPCvemOpenWindow(application, receiveStream, sendStream);
	    break;

	case VEM_OPEN_RPC_WINDOW_FUNCTION:
	    status = RPCvemOpenRPCWindow(application, receiveStream, sendStream);
	    break;
	    
	case VEM_FLUSH_TECH_INFO_FUNCTION:
	    status = RPCvemFlushTechInfo(receiveStream);
	    break;
	    
	case VEM_SET_DISPLAY_TYPE_FUNCTION:
	    status = RPCvemSetDisplayType(receiveStream);
	    break;
	    
	case VEM_GET_DISPLAY_TYPE_FUNCTION:
	    status = RPCvemGetDisplayType(sendStream);
	    break;
	    
	case WN_FLUSH_FUNCTION:
	    status = RPCvemWnFlush(sendStream);
	    break;

	case WN_QREGION_FUNCTION:
            status = RPCvemWnQRegion(receiveStream, sendStream);
	    break;
	    
	case WN_GET_OPTIONS_FUNCTION:
            status = RPCvemWnGetOptions(receiveStream, sendStream);
	    break;
	    
	case WN_SET_OPTIONS_FUNCTION:
            status = RPCvemWnSetOptions(receiveStream, sendStream);
	    break;
	    
	case WN_TURN_OFF_LAYER_FUNCTION:
            status = RPCvemWnTurnOffLayer(receiveStream, sendStream);
	    break;
	    
	case WN_TURN_ON_LAYER_FUNCTION:
            status = RPCvemWnTurnOnLayer(receiveStream, sendStream);
	    break;
	    
	case VEM_NEW_SEL_SET_FUNCTION:
	    status = RPCvemNewSelSet(receiveStream, sendStream);
	    break;
	    
	case VEM_FREE_SEL_SET_FUNCTION:
	    status = RPCvemFreeSelSet(receiveStream, sendStream);
	    break;
	    
	case VEM_CLEAR_SEL_SET_FUNCTION:
	    status = RPCvemClearSelSet(receiveStream, sendStream);
	    break;
	    
	case VEM_ADD_SEL_SET_FUNCTION:
	    status = RPCvemAddSelSet(receiveStream, sendStream);
	    break;
	    
	case VEM_DEL_SEL_SET_FUNCTION:
	    status = RPCvemDelSelSet(receiveStream, sendStream);
	    break;
	    
	case VEM_ZOOM_SEL_SET_FUNCTION:
	    status = RPCvemZoomSelSet(receiveStream, sendStream);
	    break;

	case REG_INIT_FUNCTION:
	    status = RPCvemRegInit(receiveStream, sendStream);
	    break;
	    
	case REG_NEXT_FUNCTION:
	    status = RPCvemRegNext(receiveStream, sendStream);
	    break;
	    
	case REG_END_FUNCTION:
	    status = RPCvemRegEnd(receiveStream, sendStream);
	    break;

        case REG_OBJ_START_FUNCTION:
            status = RPCregObjStart(receiveStream, sendStream);
            break;

        case REG_OBJ_NEXT_FUNCTION:
            status = RPCregObjNext(receiveStream, sendStream);
            break;

        case REG_OBJ_FINISH_FUNCTION:
            status = RPCregObjFinish(receiveStream, sendStream);
            break;

        case REG_FIND_ACTUAL_FUNCTION:
            status = RPCregFindActual(receiveStream, sendStream);
            break;

        case REG_FIND_IMPL_FUNCTION:
            status = RPCregFindImpl(receiveStream, sendStream);
            break;

        case REG_FIND_NET_FUNCTION:
            status = RPCregFindNet(receiveStream, sendStream);
            break;
	    
        case REG_ERROR_STRING_FUNCTION:
            status = RPCregErrorString(receiveStream, sendStream);
            break;
	    
        case DM_MESSAGE_FUNCTION:
	    status = RPCdmMessage(receiveStream, sendStream);
	    break;
	    
        case DM_MULTI_WHICH_FUNCTION:
	    status = RPCdmMultiWhich(receiveStream, sendStream);
	    break;
	    
	case DM_WHICH_ONE_FUNCTION:
	    status = RPCdmWhichOne(receiveStream, sendStream);
	    break;

	case DM_MULTI_TEXT_FUNCTION:
	    status = RPCdmMultiText(receiveStream, sendStream);
	    break;

	case DM_CONFIRM_FUNCTION:
	    status = RPCdmConfirm(receiveStream, sendStream);
	    break;

	case VEM_COMPLETE_FUNCTION:
	    status = RPCVemRemoteFunctionComplete(application, sendStream);
	    break;
	    
        case VU_FIND_SPOT_FUNCTION:
            status = RPCvuFindSpot(receiveStream, sendStream);
            break;
	    
        case VU_FIND_LAYER_FUNCTION:
            status = RPCvuFindLayer(receiveStream, sendStream);
            break;
	    
        case RPC_CLEAN_EXIT_FUNCTION:
            status = RPCcleanExit(application, receiveStream);
            break;

        case RPC_X_GET_DEFAULT_FUNCTION:
            status = RPCXGetDefault(receiveStream, sendStream);
            break;

        case RPC_EXIT_VEM_FUNCTION:
            status = RPCExitVem(receiveStream, sendStream);
            break;

        case RPC_REGISTER_DEMON_FUNCTION:
            status = RPCRegisterDemon(receiveStream, sendStream, application);
            break;

        case RPC_FILE_OPEN_FUNCTION:
            status = RPCfileOpen(receiveStream, sendStream);
            break;

        case VEM_BUF_LOCK_FUNCTION:
            status = RPCbufLock(receiveStream, sendStream);
            break;

	default:
	    (void) sprintf(RPCErrorBuffer,
			   "illegal VEM function number (%ld)\n",
			   functionNumber);
	    vemMessage(RPCErrorBuffer, MSG_DISP);
	    return RPC_ERROR;
    }

    RPCFLUSH(sendStream, RPC_ERROR);
    
    return status;
}
