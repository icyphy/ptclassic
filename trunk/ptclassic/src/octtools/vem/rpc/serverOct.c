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
 * OCT part of RPC server
 *
 * Rick L Spickelmier, 3/20/86, modified 7/24/88
 *
 */

#include <X11/Xlib.h>
#include "general.h"
#include "list.h"
#include "message.h"
#include "attributes.h"
#include "rpcServer.h"
#include "errtrap.h"
#include "rpcInternal.h"

extern struct RPCApplication *RPCApplication;


void
RPCTrappedError(status)
octStatus status;
{
    if (status < OCT_OK) {
	char buffer[1024];
		       
	vemMessage("An OCT error occured and the application was not informed\n",
			 MSG_DISP);
	(void) sprintf(buffer, "%s\n", octErrorString());
	vemMessage(buffer, MSG_DISP);
	return;
    }
    return;
}
static jmp_buf  rpcFailureJump;

void vemRpcErrorHandler( name, code, message)
    STR name;			/* Package name      */
    int code;			/* Error code number */
    STR message;		/* Error message     */
    /*
     *
     */
{
    if ( strcmp( name, OCT_PKG_NAME ) == 0 ) {
	char buffer[1024];
	vemMessage("An Serious OCT error occured processing an RPC request:\n",
		   MSG_DISP);
	(void) sprintf(buffer, "%s\n", octErrorString());
	vemMessage(buffer, MSG_DISP);
	longjmp( rpcFailureJump, 1 );
    }
    /* Fall through */
}


/*ARGSUSED*/
rpcStatus
RPCoctAttach(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
        
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object2.objectId = (octId) id;
    
    returnCode = octAttach(&object1, &object2);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


rpcStatus
RPCoctAttachOnce(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
        
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object2.objectId = (octId) id;

    returnCode = octAttachOnce(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    return(RPC_OK);
}


rpcStatus
RPCoctIsAttached(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
        
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object2.objectId = (octId) id;

    returnCode = octIsAttached(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctDetach(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object2.objectId = (octId) id;

    returnCode = octDetach(&object1, &object2);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


rpcStatus
RPCoctCreate(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveOctObject(&object2, receiveStream), RPC_ERROR);
    
    returnCode = octCreate(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object2, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctDelete(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    returnCode = octDelete(&object);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


rpcStatus
RPCoctCopyFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    
    RPCASSERT(RPCReceiveOctObject(&object1, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveOctObject(&object2, receiveStream), RPC_ERROR);
    returnCode = octCopyFacet(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object1, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendOctObject(&object2, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


rpcStatus
RPCoctWriteFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
    
    RPCASSERT(RPCReceiveOctObject(&object1, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object2.objectId = (octId) id;
    octGetById(&object2);
    returnCode = octWriteFacet(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctError(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    char *error;
    char buffer[1024];
    
    RPCASSERT(RPCReceiveString(&error, receiveStream), RPC_ERROR);
    (void) sprintf(buffer, "%s: %s\n", error, octErrorString());
    vemMessage(buffer, MSG_DISP);
    return(RPC_OK);
}


rpcStatus
RPCoctGetByName(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id, type;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;

    RPCASSERT(RPCReceiveLong(&type, receiveStream), OCT_ERROR);
    object2.type = (int) type;

    switch (object2.type) {
	case OCT_INSTANCE:
	    RPCASSERT(RPCReceiveString(&object2.contents.instance.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_PROP:
	    RPCASSERT(RPCReceiveString(&object2.contents.prop.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_TERM:
	    RPCASSERT(RPCReceiveString(&object2.contents.term.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_NET:
	    RPCASSERT(RPCReceiveString(&object2.contents.net.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_LAYER:
	    RPCASSERT(RPCReceiveString(&object2.contents.layer.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_BAG:
	    RPCASSERT(RPCReceiveString(&object2.contents.bag.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_LABEL:
	    RPCASSERT(RPCReceiveString(&object2.contents.label.label, receiveStream), OCT_ERROR);
	    break;
    }
    returnCode = octGetByName(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object2, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


rpcStatus
RPCoctGetContainerByName(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id, type;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;

    RPCASSERT(RPCReceiveLong(&type, receiveStream), OCT_ERROR);
    object2.type = (int) type;

    switch (object2.type) {
	case OCT_INSTANCE:
	    RPCASSERT(RPCReceiveString(&object2.contents.instance.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_PROP:
	    RPCASSERT(RPCReceiveString(&object2.contents.prop.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_TERM:
	    RPCASSERT(RPCReceiveString(&object2.contents.term.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_NET:
	    RPCASSERT(RPCReceiveString(&object2.contents.net.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_LAYER:
	    RPCASSERT(RPCReceiveString(&object2.contents.layer.name, receiveStream), OCT_ERROR);
	    break;

	case OCT_BAG:
	    RPCASSERT(RPCReceiveString(&object2.contents.bag.name, receiveStream), OCT_ERROR);
	    break;
    }
    returnCode = octGetContainerByName(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object2, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}

/*ARGSUSED*/
rpcStatus
RPCoctErrorString(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    char *string;

    string = RPCstrsave(octErrorString());
    RPCASSERT(RPCSendString(string, sendStream), RPC_ERROR);
    (void) VEMFREE(string);

    return(RPC_OK);
}    


rpcStatus
RPCoctGetFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object, facet;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    octGetFacet(&object, &facet);
    RPCASSERT(RPCSendOctObject(&facet, sendStream), RPC_ERROR);
    return(RPC_OK);
}


rpcStatus
RPCoctGetById(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    returnCode = octGetById(&object);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


rpcStatus
RPCoctGetOrCreate(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveOctObject(&object2, receiveStream), RPC_ERROR);
    returnCode = octGetOrCreate(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object2, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


rpcStatus
RPCoctCreateOrModify(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object1, object2;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object1.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveOctObject(&object2, receiveStream), RPC_ERROR);
    returnCode = octCreateOrModify(&object1, &object2);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object2, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctModify(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    
    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    returnCode = octModify(&object);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctModifySpecial(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    (void) octGetById(&object);
    
    RPCASSERT(RPCReceiveString(&(object.contents.instance.name), receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveOctTransform(&(object.contents.instance.transform), receiveStream), RPC_ERROR);
    
    returnCode = octModify(&object);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


rpcStatus
RPCoctOpenFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    
    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    returnCode = octOpenFacet(&object);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctCloseFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    returnCode = octCloseFacet(&object);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctFlushFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;

    returnCode = octFlushFacet(&object);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctFreeFacet(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    returnCode = octFreeFacet(&object);
#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */
    return(RPC_OK);
}


rpcStatus
RPCoctOpenMaster(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject inst, facet;
    long id;
    octStatus returnCode;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    inst.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveString(&facet.contents.facet.facet, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveString(&facet.contents.facet.mode, receiveStream), RPC_ERROR);

    facet.type = OCT_FACET;

    returnCode = octOpenMaster(&inst, &facet);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&facet, sendStream), RPC_ERROR);
    }

    return(RPC_OK);
}


rpcStatus
RPCoctOpenRelative(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject facet, rfacet;
    long id, dummy;
    int location;
    octStatus returnCode;

    RPCASSERT(RPCReceiveOctObject(&facet, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    rfacet.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&dummy, receiveStream), RPC_ERROR);
    location = (int) dummy;
    
    returnCode = octOpenRelative(&rfacet, &facet, location);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&facet, sendStream), RPC_ERROR);
    }

    return(RPC_OK);
}

    
rpcStatus
RPCoctBB(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    struct octBox bbox;
    octStatus returnCode;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;

    returnCode = octBB(&object, &bbox);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctPoint(&(bbox.lowerLeft), sendStream), RPC_ERROR);
	RPCASSERT(RPCSendOctPoint(&(bbox.upperRight), sendStream), RPC_ERROR);
    }

    return(RPC_OK);
}


rpcStatus
RPCoctInitGenContainers(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long id, mask;
    octGenerator generator;
    octStatus returnCode;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveLong(&mask, receiveStream), RPC_ERROR);

    returnCode = octInitGenContainers(&object, (octObjectMask) mask, &generator);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    RPCASSERT(RPCSendOctGenerator(&generator, sendStream), RPC_ERROR);

    return(RPC_OK);
}


rpcStatus
RPCoctInitGenContents(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long id, mask;
    octGenerator generator;
    octStatus returnCode;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveLong(&mask, receiveStream), RPC_ERROR);

    returnCode = octInitGenContents(&object, (octObjectMask) mask, &generator);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    RPCASSERT(RPCSendOctGenerator(&generator, sendStream), RPC_ERROR);

    return(RPC_OK);
}


/* process a remote octGenerate request */
rpcStatus
RPCoctGenerate(sendStream, receiveStream)
STREAM sendStream;
STREAM receiveStream;
{
    octGenerator generator;
    octObject object;
    octStatus returnCode;

    RPCASSERT(RPCReceiveOctGenerator(&generator, receiveStream), RPC_ERROR);

    returnCode = octGenerate(&generator, &object);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    RPCASSERT(RPCSendOctGenerator(&generator, sendStream), RPC_ERROR);
    
    if (returnCode == OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
    }

    return(RPC_OK);
}


/* process a remote octGenerateSpecial request */
rpcStatus
RPCoctGenerateSpecial(sendStream, receiveStream)
STREAM sendStream;
STREAM receiveStream;
{
    octGenerator generator;
    octObject object;
    long maxcount, count;
    octStatus status;

    RPCASSERT(RPCReceiveOctGenerator(&generator, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&maxcount, receiveStream), RPC_ERROR);

    count = 0;

    while (count < maxcount) {
	
	status = octGenerate(&generator, &object);
	
	if (status != OCT_OK) {
	    RPCASSERT(RPCSendLong((long) status, sendStream), RPC_ERROR);
	    RPCASSERT(RPCSendOctGenerator(&generator, sendStream), RPC_ERROR);
	    return(RPC_OK);
	}

	/* XXX I can't remember why I put this in!!  Rick
	if (object.type == OCT_FACET) {
	    object.contents.facet.cell = NIL(char);
	    object.contents.facet.view = NIL(char);
	    object.contents.facet.facet = NIL(char);
	    object.contents.facet.version = NIL(char);
	    object.contents.facet.mode = NIL(char);
	}
	*/
	
	RPCASSERT(RPCSendLong((long) status, sendStream), RPC_ERROR);
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
	count++;
    }
    
    RPCASSERT(RPCSendOctGenerator(&generator, sendStream), RPC_ERROR);

    return(RPC_OK);
}


rpcStatus
RPCoctGenFirstContent(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object, content;
    long id, mask;
    octStatus returnCode;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveLong(&mask, receiveStream), RPC_ERROR);

    returnCode = octGenFirstContent(&object, (octObjectMask) mask, &content);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);

    /* OCT_GEN_DONE is > OCT_OK */
    if (returnCode == OCT_OK) {
	RPCASSERT(RPCSendOctObject(&content, sendStream), RPC_ERROR);
    }

    return(RPC_OK);
}
	

rpcStatus
RPCoctGenFirstContainer(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object, container;
    long id, mask;
    octStatus returnCode;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    RPCASSERT(RPCReceiveLong(&mask, receiveStream), RPC_ERROR);

    returnCode = octGenFirstContainer(&object, (octObjectMask) mask, &container);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);

    /* OCT_GEN_DONE is > OCT_OK */
    if (returnCode == OCT_OK) {
	RPCASSERT(RPCSendOctObject(&container, sendStream), RPC_ERROR);
    }

    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctFreeGenerator(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octGenerator generator;
    octStatus returnCode;

    RPCASSERT(RPCReceiveOctGenerator(&generator, receiveStream), RPC_ERROR);
    returnCode = octFreeGenerator(&generator);

#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */

    return(RPC_OK);
}


rpcStatus
RPCoctGetPoints(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long num_points;
    octStatus returnCode;
    long sendPoints;	/* used for the braindamaged function octGetPoints */
    struct octPoint *points;
    long id;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&num_points, receiveStream), RPC_ERROR);

    RPCASSERT(RPCReceiveLong(&sendPoints, receiveStream), RPC_ERROR);

    if (sendPoints == RPC_DO_NOT_SEND_POINTS) {
	points = NULL;
    } else {
	points = RPCARRAYALLOC(struct octPoint, num_points);
    }

    returnCode = octGetPoints(&object, (int32 *) &num_points, points);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    RPCASSERT(RPCSendLong(num_points, sendStream), RPC_ERROR);

    if ((sendPoints == RPC_SEND_POINTS) && (returnCode != OCT_TOO_SMALL)) {

	if (fwrite((char *) points, sizeof(struct octPoint), (int) num_points, sendStream) != num_points) {
	    return(RPC_ERROR);
	}
	(void) VEMFREE(points);
    }
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctPutPoints(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long num_points;
    octStatus returnCode;
    struct octPoint *points;

    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveLong(&num_points, receiveStream), RPC_ERROR);

    points = RPCARRAYALLOC(struct octPoint, num_points);

    if (fread((char *) points, sizeof(struct octPoint), (int) num_points, receiveStream) != num_points) {
	return(RPC_ERROR);
    }

    returnCode = octPutPoints(&object, (int) num_points, points);

#ifndef NOWAIT    
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
#else
    RPCTrappedError(returnCode);
#endif /* NOWAIT */

    (void) VEMFREE(points);
    return(RPC_OK);
}


rpcStatus
RPCoctTransformGeo(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    struct octTransform transform;

    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    
    RPCASSERT(RPCReceiveOctTransform(&transform, receiveStream), RPC_ERROR);

    octTransformGeo(&object, &transform);

    RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);

    return(RPC_OK);
}


rpcStatus
RPCoctTransformPoints(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    struct octTransform transform;
    long num_points;
    struct octPoint *points;
    int i;

    RPCASSERT(RPCReceiveLong(&num_points, receiveStream), RPC_ERROR);

    points = RPCARRAYALLOC(struct octPoint, num_points);

    for (i = 0; i < num_points; i++) {
	RPCASSERT(RPCReceiveOctPoint(&points[i], receiveStream), RPC_ERROR);
    }

    RPCASSERT(RPCReceiveOctTransform(&transform, receiveStream), RPC_ERROR);

    octTransformPoints(num_points, points, &transform);

    for (i = 0; i < num_points; i++) {
	RPCASSERT(RPCSendOctPoint(&(points[i]), sendStream), RPC_ERROR);
    }

    VEMFREE(points);
    return(RPC_OK);
}


rpcStatus
RPCoctTransformAndModifyGeo(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    struct octTransform transform;

    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveOctTransform(&transform, receiveStream), RPC_ERROR);

    returnCode = octTransformAndModifyGeo(&object, &transform);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
    }	

    return(RPC_OK);
}


rpcStatus
RPCoctScaleGeo(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    double scale;

    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveFloat(&scale, receiveStream), RPC_ERROR);

    octScaleGeo(&object, scale);

    RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);

    return(RPC_OK);
}


rpcStatus
RPCoctScaleAndModifyGeo(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject object;
    double scale;

    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);
    RPCASSERT(RPCReceiveFloat(&scale, receiveStream), RPC_ERROR);

    returnCode = octScaleAndModifyGeo(&object, scale);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
    }	

    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctMarkTemporary(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    octMarkTemporary(&object);
    return(RPC_OK);
}


/*ARGSUSED*/
rpcStatus
RPCoctUnmarkTemporary(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    octUnmarkTemporary(&object);
    return(RPC_OK);
}


rpcStatus
RPCoctIsTemporary(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    int returnCode;
    octObject object;
    long id;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    
    returnCode = octIsTemporary(&object);
    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    return(RPC_OK);
}


rpcStatus
RPCoctExternalId(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octObject object;
    long id;
    long xid = 0;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    octExternalId(&object, &xid);
    RPCASSERT(RPCSendLong((long) xid, sendStream), RPC_ERROR);
    return(RPC_OK);
}


rpcStatus
RPCoctGetByExternalId(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    octStatus returnCode;
    octObject facet, object;
    long id;
    int32 xid;
    
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    facet.objectId = (octId) id;
    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    xid = (int32) id;

    returnCode = octGetByExternalId(&facet, xid, &object);

    RPCASSERT(RPCSendLong((long) returnCode, sendStream), RPC_ERROR);
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendOctObject(&object, sendStream), RPC_ERROR);
    }
    return(RPC_OK);
}


rpcStatus
RPCoctFullName(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    char *string;
    octObject object;
    long id;

    RPCASSERT(RPCReceiveLong(&id, receiveStream), RPC_ERROR);
    object.objectId = (octId) id;
    octFullName(&object, &string);

    RPCASSERT(RPCSendString(string, sendStream), RPC_ERROR);
    (void) VEMFREE(string);
    return(RPC_OK);
}    


rpcStatus
RPCoctGetFacetInfo(sendStream, receiveStream)
STREAM sendStream, receiveStream;
{
    long returnCode;
    octObject object;
    struct octFacetInfo info;

    RPCASSERT(RPCReceiveOctObject(&object, receiveStream), RPC_ERROR);

    returnCode = octGetFacetInfo(&object, &info);

    RPCASSERT(RPCSendLong(returnCode, sendStream), RPC_ERROR);

    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCSendFacetInfo(&info, sendStream), RPC_ERROR);
    }

    return RPC_OK;
}

/*
 * process a remote OCT request
 *
 * get the OCT function number and then call the appropriate routine
 *
 * all remote OCT requests proceed as follows:
 *
 *   receive OCT function number
 *   receive OCT arguments
 *   (perform OCT function)
 *   send return code
 *   send modified OCT arguments
 */
rpcStatus
RPCOCTRequest(application, functionNumber)
int application;
long functionNumber;
{
    STREAM sendStream;
    STREAM receiveStream;
    rpcStatus status;

    sendStream = RPCApplication[application].SendStream;
    receiveStream = RPCApplication[application].ReceiveStream;

    
    if ( setjmp( rpcFailureJump ) ) {
	errPopHandler();
	return RPC_ERROR;
    } else {
	errPushHandler( vemRpcErrorHandler );
    }

    switch (functionNumber) {

	case OCT_DELETE_FUNCTION:
	    status = RPCoctDelete(sendStream, receiveStream);
	    break;
	
	case OCT_GET_BY_ID_FUNCTION:
	    status = RPCoctGetById(sendStream, receiveStream);
	    break;
	    
	case OCT_MODIFY_FUNCTION:
	    status = RPCoctModify(sendStream, receiveStream);
	    break;
	    
	case OCT_MODIFY_SPECIAL_FUNCTION:
	    status = RPCoctModifySpecial(sendStream, receiveStream);
	    break;
	    
        case OCT_OPEN_FACET_FUNCTION:
            status = RPCoctOpenFacet(sendStream, receiveStream);
	    break;
	    
	case OCT_FLUSH_FACET_FUNCTION:
            status = RPCoctFlushFacet(sendStream, receiveStream);
	    break;
	    
	case OCT_CLOSE_FACET_FUNCTION:
            status = RPCoctCloseFacet(sendStream, receiveStream);
	    break;
	    
	case OCT_FREE_FACET_FUNCTION:
            status = RPCoctFreeFacet(sendStream, receiveStream);
	    break;

	case OCT_OPEN_MASTER_FUNCTION:
            status = RPCoctOpenMaster(sendStream, receiveStream);
            break;

	case OCT_ATTACH_FUNCTION:
            status = RPCoctAttach(sendStream, receiveStream);
	    break;
	    
	case OCT_ATTACH_ONCE_FUNCTION:
            status = RPCoctAttachOnce(sendStream, receiveStream);
	    break;
	    
	case OCT_IS_ATTACHED_FUNCTION:
            status = RPCoctIsAttached(sendStream, receiveStream);
	    break;
	    
	case OCT_DETACH_FUNCTION:
            status = RPCoctDetach(sendStream, receiveStream);
	    break;
	    
	case OCT_COPY_FACET_FUNCTION:
            status = RPCoctCopyFacet(sendStream, receiveStream);
	    break;
	    
	case OCT_WRITE_FACET_FUNCTION:
            status = RPCoctWriteFacet(sendStream, receiveStream);
	    break;
	    
	case OCT_CREATE_FUNCTION:
            status = RPCoctCreate(sendStream, receiveStream);
	    break;
	    
	case OCT_GET_BY_NAME_FUNCTION:
            status = RPCoctGetByName(sendStream, receiveStream);
	    break;
	    
	case OCT_GET_CONTAINER_BY_NAME_FUNCTION:
            status = RPCoctGetContainerByName(sendStream, receiveStream);
	    break;
	    
	case OCT_GET_OR_CREATE_FUNCTION:
            status = RPCoctGetOrCreate(sendStream, receiveStream);
	    break;
	    
	case OCT_CREATE_OR_MODIFY_FUNCTION:
            status = RPCoctCreateOrModify(sendStream, receiveStream);
	    break;
	
	case OCT_INIT_GEN_CONTAINERS_FUNCTION:
            status = RPCoctInitGenContainers(sendStream, receiveStream);
	    break;
	    
	case OCT_INIT_GEN_CONTENTS_FUNCTION:
            status = RPCoctInitGenContents(sendStream, receiveStream);
	    break;

	case OCT_GENERATE_FUNCTION:
	    status = RPCoctGenerate(sendStream, receiveStream);
	    break;

	case OCT_GENERATE_SPECIAL_FUNCTION:
	    status = RPCoctGenerateSpecial(sendStream, receiveStream);
	    break;

	case OCT_GEN_FIRST_CONTENT_FUNCTION:
	    status = RPCoctGenFirstContent(sendStream, receiveStream);
	    break;

	case OCT_GEN_FIRST_CONTAINER_FUNCTION:
	    status = RPCoctGenFirstContainer(sendStream, receiveStream);
	    break;

	case OCT_FREE_GENERATOR_FUNCTION:
	    status = RPCoctFreeGenerator(sendStream, receiveStream);
	    break;

	case OCT_ERROR_FN_FUNCTION:
            status = RPCoctError(sendStream, receiveStream);
	    break;
	    
	case OCT_GET_POINTS_FUNCTION:
            status = RPCoctGetPoints(sendStream, receiveStream);
	    break;
	    
	case OCT_PUT_POINTS_FUNCTION:
            status = RPCoctPutPoints(sendStream, receiveStream);
	    break;
	    
	case OCT_BB_FUNCTION:
	    status = RPCoctBB(sendStream, receiveStream);
	    break;

        case OCT_GET_FACET_FUNCTION:
            status = RPCoctGetFacet(sendStream, receiveStream);
	    break;
	    
        case OCT_ERROR_STRING_FUNCTION:
            status = RPCoctErrorString(sendStream, receiveStream);
	    break;

        case OCT_TRANSFORM_GEO_FUNCTION:
            status = RPCoctTransformGeo(sendStream, receiveStream);
	    break;

        case OCT_TRANSFORM_MODIFY_GEO_FUNCTION:
            status = RPCoctTransformAndModifyGeo(sendStream, receiveStream);
	    break;

        case OCT_TRANSFORM_POINTS_FUNCTION:
            status = RPCoctTransformPoints(sendStream, receiveStream);
	    break;

        case OCT_SCALE_GEO_FUNCTION:
            status = RPCoctScaleGeo(sendStream, receiveStream);
	    break;

        case OCT_SCALE_MODIFY_GEO_FUNCTION:
            status = RPCoctScaleAndModifyGeo(sendStream, receiveStream);
	    break;

        case OCT_MARK_TEMPORARY_FUNCTION:
            status = RPCoctMarkTemporary(sendStream, receiveStream);
	    break;

        case OCT_UNMARK_TEMPORARY_FUNCTION:
            status = RPCoctUnmarkTemporary(sendStream, receiveStream);
	    break;

        case OCT_IS_TEMPORARY_FUNCTION:
            status = RPCoctIsTemporary(sendStream, receiveStream);
	    break;

        case OCT_XID_FUNCTION:
            status = RPCoctExternalId(sendStream, receiveStream);
	    break;

        case OCT_GET_BY_XID_FUNCTION:
            status = RPCoctGetByExternalId(sendStream, receiveStream);
	    break;

        case OCT_FULL_NAME_FUNCTION:
            status = RPCoctFullName(sendStream, receiveStream);
	    break;

        case OCT_GET_FACET_INFO_FUNCTION:
            status = RPCoctGetFacetInfo(sendStream, receiveStream);
	    break;

        case OCT_OPEN_RELATIVE_FUNCTION:
            status = RPCoctOpenRelative(sendStream, receiveStream);
	    break;

	default:
	    (void) sprintf(RPCErrorBuffer,
			   "RPC Error: illegal OCT function number (%ld)\n",
			   (long)functionNumber);
	    vemMessage(RPCErrorBuffer, 0);
	    return(RPC_ERROR);
    }

    errPopHandler();

    if (status == RPC_ERROR) {
	vemMessage("RPC Error: OCT function failure\n", 0);
	return(RPC_ERROR);
    }

    RPCFLUSH(sendStream, RPC_ERROR);
    return(RPC_OK);
}
