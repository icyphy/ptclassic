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

/*
 * routines for translating to and from VEM and RPC arglists
 */

#include <X11/Xlib.h>
#include "list.h"
#include "message.h"
#include "buffer.h"
#include "commands.h"
#include "rpcServer.h"

#include "serverUtils.h"

rpcStatus
RPCTranslateArgsToRPC(inList, outList, facetId, application)
lsList inList;				/* VEM arglist		*/
lsList *outList;			/* RPC arglist		*/
octId facetId;				/* facet		*/
int application;			/* application number	*/
/*
 * translate a VEM arglist to a RPC arglist
 */
{
    lsList cmdList;
    lsGen gen;
    vemOneArg *arg;
    RPCArg *sarg;
    octObject *newBag, facet, item;
    struct octGenerator contgen;
    int i;
    Pointer pointer;
    
    
    cmdList = lsCreate();
    gen = lsStart(inList);

    while (lsNext(gen, &pointer, (lsHandle *) 0) == LS_OK) {
	arg = (vemOneArg *) pointer;
	sarg = RPCALLOC(struct RPCArg);
	sarg->argType = arg->argType;
	sarg->length = arg->length;
	switch (sarg->argType) {
	    case VEM_POINT_ARG:
		sarg->argData.points =
		  RPCARRAYALLOC(struct RPCPointArg, sarg->length);
		for (i = 0; i < sarg->length; i++) {
		    sarg->argData.points[i].theWin =
		      arg->argData.points[i].theWin;
		    sarg->argData.points[i].facet =
		      arg->argData.points[i].theFct;
		    sarg->argData.points[i].thePoint =
		      arg->argData.points[i].thePoint;
		}
		break;

	    case VEM_BOX_ARG:
		sarg->argData.points =
		  RPCARRAYALLOC(struct RPCPointArg, (sarg->length * 2));
		for (i = 0; i < sarg->length * 2; i++) {
		    sarg->argData.points[i].theWin =
		      arg->argData.points[i].theWin;
		    sarg->argData.points[i].facet =
		      arg->argData.points[i].theFct;
		    sarg->argData.points[i].thePoint =
		      arg->argData.points[i].thePoint;
		}
		break;

	    case VEM_LINE_ARG:
		sarg->argData.points = RPCARRAYALLOC(struct RPCPointArg, sarg->length);
		for (i = 0; i < sarg->length; i++) {
		    sarg->argData.points[i].theWin =
		      arg->argData.points[i].theWin;
		    sarg->argData.points[i].facet =
		      arg->argData.points[i].theFct;
		    sarg->argData.points[i].thePoint =
		      arg->argData.points[i].thePoint;
		}
		break;

	    case VEM_TEXT_ARG:
		sarg->argData.string = RPCstrsave(arg->argData.vemTextArg);
		break;

	    case VEM_OBJ_ARG:
		/* clone the bag */
		newBag = RPCALLOC(struct octObject);
		newBag->objectId = arg->argData.vemObjArg.theBag;
		octGetById(newBag);

		/* create the attachments generator */
		octInitGenContents(newBag, OCT_ALL_MASK, &contgen);

		/* get the facet */
		octGetFacet(newBag, &facet);
		octCreate(&facet, newBag);
		octMarkTemporary(newBag);

		/* generate and attach */
		while (octGenerate(&contgen, &item) == OCT_OK) {
		    octAttach(newBag, &item);
		}

		/* add to the cloned list */
		lsNewBegin(RPCApplication[application].clonedBags,
			   (Pointer) newBag, (lsHandle *) 0);

		sarg->argData.objArg.facet = arg->argData.vemObjArg.theFct;
		sarg->argData.objArg.theBag = newBag->objectId;
		sarg->argData.objArg.tran = arg->argData.vemObjArg.tran;
		break;
	}

	lsNewEnd(cmdList, (Pointer) sarg, (lsHandle *) 0);
    }

    lsFinish(gen);

    *outList = cmdList;

    return(RPC_OK);
}


rpcStatus
RPCTranslateArgsToVEM(inList, outList)
lsList inList;				/* RPC arglist		*/
lsList *outList;			/* VEM arglist		*/
/*
 * translate an RPC arglist to a VEM arglist
 */
{
    lsList cmdList;
    lsGen gen;
    vemOneArg *vemArg;
    RPCArg *rpcArg;
    int i;
    octObject facet;
    Pointer pointer;
    

    /* translate the RPC arg list to a VEM arglist */

    gen = lsStart(inList);
    cmdList = lsCreate();

    while (lsNext(gen, &pointer, (lsHandle *) 0) == LS_OK) {
	rpcArg = (RPCArg *) pointer;
	vemArg = RPCALLOC(vemOneArg);
	vemArg->argType = rpcArg->argType;
	vemArg->length = rpcArg->length;

	switch (rpcArg->argType) {
	    case VEM_POINT_ARG:
		vemArg->argData.points = RPCARRAYALLOC(vemPoint, rpcArg->length);
		vemArg->alloc_size = rpcArg->length;
		for (i = 0; i < rpcArg->length; i++) {
		    vemArg->argData.points[i].theWin = rpcArg->argData.points[i].theWin;
		    facet.objectId = rpcArg->argData.points[i].facet;
		    if (octGetById(&facet) < OCT_OK) {
	                octError("RPCTranslateArgsToVEM: can not get facet by id");
	                return(RPC_ERROR);
	            }
                    if (bufOpen(&facet, BUFSILENT) != VEM_OK) {
                        vemMessage("RPC Error: bufOpen failed\n", MSG_DISP);
                        return(RPC_ERROR);
                    }
		    vemArg->argData.points[i].theFct = facet.objectId;
		    vemArg->argData.points[i].thePoint = rpcArg->argData.points[i].thePoint;
		}
		break;

	    case VEM_BOX_ARG:
		vemArg->argData.points = RPCARRAYALLOC(vemPoint, rpcArg->length * 2);
		vemArg->alloc_size = rpcArg->length * 2;
		for (i = 0; i < rpcArg->length * 2; i++) {
		    vemArg->argData.points[i].theWin = rpcArg->argData.points[i].theWin;
		    facet.objectId = rpcArg->argData.points[i].facet;
		    if (octGetById(&facet) < OCT_OK) {
	                octError("RPCTranslateArgsToVem: can not get facet by id");
	                return(RPC_ERROR);
	            }
                    if (bufOpen(&facet, BUFSILENT) != VEM_OK) {
                        vemMessage("RPC Error: bufOpen failed\n", MSG_DISP);
                        return(RPC_ERROR);
                    }
		    vemArg->argData.points[i].theFct = facet.objectId;
		    vemArg->argData.points[i].thePoint = rpcArg->argData.points[i].thePoint;
		}
		break;

	    case VEM_LINE_ARG:
		vemArg->argData.points = RPCARRAYALLOC(vemPoint, rpcArg->length);
		vemArg->alloc_size = rpcArg->length;
		for (i = 0; i < rpcArg->length; i++) {
		    vemArg->argData.points[i].theWin = rpcArg->argData.points[i].theWin;
		    facet.objectId = rpcArg->argData.points[i].facet;
		    if (octGetById(&facet) < OCT_OK) {
	                octError("RPCTranslateArgsToVEM: can not get facet by id");
	                return(RPC_ERROR);
	            }
                    if (bufOpen(&facet, BUFSILENT) != VEM_OK) {
                        vemMessage("RPC Error: bufOpen failed\n", MSG_DISP);
                        return(RPC_ERROR);
                    }
		    vemArg->argData.points[i].theFct = facet.objectId;
		    vemArg->argData.points[i].thePoint = rpcArg->argData.points[i].thePoint;
		}
		break;

	    case VEM_TEXT_ARG:
		vemArg->argData.vemTextArg = RPCstrsave(rpcArg->argData.string);
		break;

	    case VEM_OBJ_ARG:
		facet.objectId = rpcArg->argData.objArg.facet;
		if (octGetById(&facet) < OCT_OK) {
	            octError("RPCTranslateArgsToVEM: can not get facet by id");
	            return(RPC_ERROR);
	        }
                if (bufOpen(&facet, BUFSILENT) != VEM_OK) {
                    vemMessage("RPC Error: bufOpen failed\n", MSG_DISP);
                    return(RPC_ERROR);
                }
		vemArg->argData.vemObjArg.theFct = facet.objectId;
		vemArg->argData.vemObjArg.theBag = rpcArg->argData.objArg.theBag;
		vemArg->argData.vemObjArg.tran = rpcArg->argData.objArg.tran;
		vemArg->argData.vemObjArg.attr = (atrHandle) 0;
		vemArg->argData.vemObjArg.selBag = 0;
		break;
	}

	lsNewEnd(cmdList, (Pointer) vemArg, (lsHandle *) 0);
    }

    lsFinish(gen);

    *outList = cmdList;

    return(RPC_OK);
}
