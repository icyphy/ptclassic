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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"
#include "ansi.h"  

/*
 * application routines for oct requests
 *
 * Rick L Spickelmier, 3/20/86, last modified 10/25/88
 */

#include "rpcApp.h"
#define MAXCACHESIZE 1000

STREAM RPCSendStream;
STREAM RPCReceiveStream;
int RPCByteSwapped = 0;

octStatus	RPCOctGenerate();
octStatus	RPCOctGenerateDone();
octStatus	RPCOctGenerateError();
octStatus	RPCOctGenerateSpecial();

rpcInternalStatus RPCReceiveFacetInfo
	ARGS((struct octFacetInfo *info, STREAM stream)); 


/*
 * handle OCT functions that create generators
 */
static octStatus
RPCoctInitGen(type, container, mask, generator, sendStream, receiveStream)
long type;
octObject *container;
octObjectMask mask;
octGenerator *generator;
STREAM sendStream;
STREAM receiveStream;
{
    long returnCode;
    octGenerator *realGenerator;

    realGenerator = RPCALLOC(octGenerator);

    RPCASSERT(RPCSendLong(type, sendStream));
    RPCASSERT(RPCSendLong((long) container->objectId, sendStream));
    RPCASSERT(RPCSendLong((long) mask, sendStream));

    RPCFLUSH(sendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, receiveStream));
    RPCASSERT(RPCReceiveOctGenerator(realGenerator, receiveStream));

    generator->generator_func = RPCOctGenerate;
    generator->state = (char *) realGenerator;

    return (octStatus) returnCode;
}


struct rpcGenerator {
    octGenerator *generator;
    octObject object[MAXCACHESIZE];
    octStatus status[MAXCACHESIZE];
    long count;
};

    
static octStatus
RPCoctInitGenSpecial(type, container, mask, generator, sendStream, receiveStream)
long type;
octObject *container;
octObjectMask mask;
octGenerator *generator;
STREAM sendStream;
STREAM receiveStream;
{
    long returnCode;
    octGenerator *realGenerator;
    struct rpcGenerator *rpcGenerator;

    realGenerator = RPCALLOC(octGenerator);
            
    RPCASSERT(RPCSendLong(type, sendStream));
    RPCASSERT(RPCSendLong((long) container->objectId, sendStream));
    RPCASSERT(RPCSendLong((long) mask, sendStream));

    RPCFLUSH(sendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, receiveStream));
    RPCASSERT(RPCReceiveOctGenerator(realGenerator, receiveStream));

    if (returnCode >= OCT_OK) {
	rpcGenerator = RPCALLOC(struct rpcGenerator);
	rpcGenerator->count = MAXCACHESIZE;
	rpcGenerator->generator = realGenerator;
	generator->generator_func = RPCOctGenerateSpecial;
	generator->state = (char *) rpcGenerator;
    }
    return (octStatus) returnCode;
}



/*
 * basic OCT Calls
 *
 * all work as follows:
 *
 *   send OCT RPC function code
 *   send arguments
 *   wait for return code
 *   wait for return arguments (modified versions of the original arguments)
 */

octStatus
octAttach(object1, object2)
octObject *object1, *object2;
{
    long returnCode = OCT_OK;
    
    RPCASSERT(RPCSendLong((long) OCT_ATTACH_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object2->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octAttachOnce(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_ATTACH_ONCE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object2->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    return (octStatus) returnCode;
}


octStatus
octIsAttached(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_IS_ATTACHED_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object2->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    return (octStatus) returnCode;
}


octStatus
octDetach(object1, object2)
octObject *object1, *object2;
{
    long returnCode = OCT_OK;
    
    RPCASSERT(RPCSendLong((long) OCT_DETACH_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object2->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octCreate(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_CREATE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object2, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object2, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octDelete(object)
octObject *object;
{
    long returnCode = OCT_OK;
    
    RPCASSERT(RPCSendLong((long) OCT_DELETE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octCopyFacet(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_COPY_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object1, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object2, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object1, RPCReceiveStream));
	RPCASSERT(RPCReceiveOctObject(object2, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octWriteFacet(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_WRITE_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object1, RPCSendStream));
    RPCASSERT(RPCSendLong(object2->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    return (octStatus) returnCode;
}


void
octError(string)
char *string;
{
    RPCASSERT(RPCSendLong(OCT_ERROR_FN_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(string, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}


octStatus
octInitGenContents(container, mask, generator)
octObject *container;
octObjectMask mask;
octGenerator *generator;
{
    return(RPCoctInitGen(OCT_INIT_GEN_CONTENTS_FUNCTION, container, mask, generator,
				       RPCSendStream, RPCReceiveStream));
}


octStatus
octInitGenContainers(object, mask, generator)
octObject *object;
octObjectMask mask;
octGenerator *generator;
{
    return(RPCoctInitGen(OCT_INIT_GEN_CONTAINERS_FUNCTION, object, mask, generator,
				       RPCSendStream, RPCReceiveStream));
}


octStatus
octInitGenContentsSpecial(container, mask, generator)
octObject *container;
octObjectMask mask;
octGenerator *generator;
{
    return(RPCoctInitGenSpecial(OCT_INIT_GEN_CONTENTS_FUNCTION, container,
						       mask, generator,
				       RPCSendStream, RPCReceiveStream));
}


octStatus
octInitGenContainersSpecial(object, mask, generator)
octObject *object;
octObjectMask mask;
octGenerator *generator;
{
    return(RPCoctInitGenSpecial(OCT_INIT_GEN_CONTAINERS_FUNCTION, object,
							 mask, generator,
				       RPCSendStream, RPCReceiveStream));
}


/*ARGSUSED*/
octStatus
RPCOctGenerateDone(generator, object)
octGenerator *generator;
octObject *object;
{
    return OCT_GEN_DONE;
}


/*ARGSUSED*/
octStatus
RPCOctGenerateError(generator, object)
octGenerator *generator;
octObject *object;
{
    return OCT_ERROR;
}


octStatus
RPCOctGenerate(generator, object)
octGenerator *generator;
octObject *object;
{
    long returnCode;
    octGenerator *realGenerator;

    realGenerator = (octGenerator *) generator->state;

    RPCASSERT(RPCSendLong(OCT_GENERATE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctGenerator(realGenerator, RPCSendStream));

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    RPCASSERT(RPCReceiveOctGenerator(realGenerator, RPCReceiveStream));
    
    if (returnCode == OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
	generator->state = (char *) realGenerator;
    }
    
    if (returnCode == OCT_GEN_DONE) {
	generator->generator_func = RPCOctGenerateDone;
	(void) RPCFREE(generator->state);
        generator->state = RPCNIL(char);
    }

    return (octStatus) returnCode;
}


octStatus
RPCOctGenerateSpecial(generator, object)
octGenerator *generator;
octObject *object;
{
    struct rpcGenerator *rpcGenerator;
    octStatus status;
    long dummy;
    int count;
    octGenerator *realGenerator;

    rpcGenerator = (struct rpcGenerator *) generator->state;

    if (rpcGenerator->count < MAXCACHESIZE) {

	/* Still have oct objects on the remote side */
	status = rpcGenerator->status[rpcGenerator->count];
	if (status == OCT_GEN_DONE) {
	    generator->generator_func = RPCOctGenerateDone;
	    (void) RPCFREE(rpcGenerator);
	    generator->state = RPCNIL(char);
	    return OCT_GEN_DONE;
	}
	if (status != OCT_OK) {
	    generator->generator_func = RPCOctGenerateError;
	    (void) RPCFREE(rpcGenerator);
	    generator->state = RPCNIL(char);
	    return status;
	}
	*object = rpcGenerator->object[rpcGenerator->count++];
	return status;
	
    } else {

	/* Ask for more oct objects */
	realGenerator = rpcGenerator->generator;
	
	RPCASSERT(RPCSendLong(OCT_GENERATE_SPECIAL_FUNCTION, RPCSendStream));
	RPCASSERT(RPCSendOctGenerator(realGenerator, RPCSendStream));
	RPCASSERT(RPCSendLong((long) MAXCACHESIZE, RPCSendStream));
	RPCFLUSH(RPCSendStream);

	count = 0;
	
	while (count < MAXCACHESIZE) {
	    RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	    rpcGenerator->status[count] = (octStatus) dummy;
	    
	    if (rpcGenerator->status[count] != OCT_OK) {
		count++;
		break;
	    }
	    RPCASSERT(RPCReceiveOctObject(&(rpcGenerator->object[count++]), RPCReceiveStream));
	}
	RPCASSERT(RPCReceiveOctGenerator(realGenerator, RPCReceiveStream));
	rpcGenerator->count = 0;
	rpcGenerator->generator = realGenerator;
	generator->generator_func = RPCOctGenerateSpecial;
	generator->state = (char *) rpcGenerator;

	status = rpcGenerator->status[rpcGenerator->count];
	if (status == OCT_GEN_DONE) {
	    generator->generator_func = RPCOctGenerateDone;
	    (void) RPCFREE(rpcGenerator);
	    generator->state = RPCNIL(char);
	    return OCT_GEN_DONE;
	}
	if (status != OCT_OK) {
	    generator->generator_func = RPCOctGenerateError;
	    (void) RPCFREE(rpcGenerator);
	    generator->state = RPCNIL(char);
	    return status;
	}
	*object = rpcGenerator->object[rpcGenerator->count++];
	return status;
    }
}


octStatus
octGenFirstContent(object, mask, content)
octObject *object;
octObjectMask mask;
octObject *content;
{
    long returnCode;

    RPCASSERT(RPCSendLong((long) OCT_GEN_FIRST_CONTENT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));

    RPCASSERT(RPCSendLong((long) mask, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode == OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(content, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octGenFirstContainer(object, mask, container)
octObject *object;
octObjectMask mask;
octObject *container;
{
    long returnCode;

    RPCASSERT(RPCSendLong((long) OCT_GEN_FIRST_CONTAINER_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));

    RPCASSERT(RPCSendLong((long) mask, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode == OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(container, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


/* XXX should be fixed and call the vem side */

octStatus
octFreeGenerator(generator)
octGenerator *generator;
{
    struct rpcGenerator *rpcGenerator;

    rpcGenerator = (struct rpcGenerator *) generator->state;
    if (rpcGenerator != RPCNIL(struct rpcGenerator)) {
        (void) RPCFREE(rpcGenerator);
        generator->state = RPCNIL(char);
    }
    generator->generator_func = RPCOctGenerateError;

    return OCT_OK;
}


octStatus
octGetContainerByName(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_GET_CONTAINER_BY_NAME_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));

    RPCASSERT(RPCSendLong((long) object2->type, RPCSendStream));

    switch (object2->type) {
	case OCT_INSTANCE:
	    RPCASSERT(RPCSendString(object2->contents.instance.name, RPCSendStream));
	    break;

	case OCT_PROP:
	    RPCASSERT(RPCSendString(object2->contents.prop.name, RPCSendStream));
	    break;

	case OCT_TERM:
	    RPCASSERT(RPCSendString(object2->contents.term.name, RPCSendStream));
	    break;

	case OCT_NET:
	    RPCASSERT(RPCSendString(object2->contents.net.name, RPCSendStream));
	    break;

	case OCT_LAYER:
	    RPCASSERT(RPCSendString(object2->contents.layer.name, RPCSendStream));
	    break;

	case OCT_BAG:
	    RPCASSERT(RPCSendString(object2->contents.bag.name, RPCSendStream));
	    break;
    }

    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object2, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octGetByName(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_GET_BY_NAME_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));

    RPCASSERT(RPCSendLong((long) object2->type, RPCSendStream));

    switch (object2->type) {
	case OCT_INSTANCE:
	    RPCASSERT(RPCSendString(object2->contents.instance.name, RPCSendStream));
	    break;

	case OCT_PROP:
	    RPCASSERT(RPCSendString(object2->contents.prop.name, RPCSendStream));
	    break;

	case OCT_TERM:
	    RPCASSERT(RPCSendString(object2->contents.term.name, RPCSendStream));
	    break;

	case OCT_NET:
	    RPCASSERT(RPCSendString(object2->contents.net.name, RPCSendStream));
	    break;

	case OCT_LAYER:
	    RPCASSERT(RPCSendString(object2->contents.layer.name, RPCSendStream));
	    break;

	case OCT_BAG:
	    RPCASSERT(RPCSendString(object2->contents.bag.name, RPCSendStream));
	    break;

	case OCT_LABEL:
	    RPCASSERT(RPCSendString(object2->contents.label.label, RPCSendStream));
	    break;
    }

    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object2, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octGetById(object)
octObject *object;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_GET_BY_ID_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    }
    
    return (octStatus) returnCode;
}


octStatus
octGetOrCreate(object1, object2)
octObject *object1, *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_GET_OR_CREATE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object2, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object2, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octCreateOrModify(object1, object2)
octObject *object1;
octObject *object2;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_CREATE_OR_MODIFY_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object1->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object2, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object2, RPCReceiveStream));
    }
    return (octStatus) returnCode;

}


octStatus
octModify(object)
octObject *object;
{
    long returnCode = OCT_OK;
    
    if (object->type == OCT_INSTANCE) {
	RPCASSERT(RPCSendLong((long) OCT_MODIFY_SPECIAL_FUNCTION, RPCSendStream));
	RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
	RPCASSERT(RPCSendString(object->contents.instance.name, RPCSendStream));
	RPCASSERT(RPCSendOctTransform(&(object->contents.instance.transform), RPCSendStream));
	
    } else {
	RPCASSERT(RPCSendLong((long) OCT_MODIFY_FUNCTION, RPCSendStream));
	RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    }
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octOpenFacet(object)
octObject *object;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_OPEN_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


octStatus
octFlushFacet(object)
octObject *object;
{
    long returnCode = OCT_OK;
    
    RPCASSERT(RPCSendLong((long) OCT_FLUSH_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octCloseFacet(object)
octObject *object;
{
    long returnCode = OCT_OK;
    
    RPCASSERT(RPCSendLong((long) OCT_CLOSE_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octOpenMaster(inst, facet)
octObject *inst;
octObject *facet;
{
    long returnCode;

    RPCASSERT(RPCSendLong((long) OCT_OPEN_MASTER_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) inst->objectId, RPCSendStream));
    RPCASSERT(RPCSendString(facet->contents.facet.facet, RPCSendStream));
    RPCASSERT(RPCSendString(facet->contents.facet.mode, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(facet, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}

octStatus
octOpenRelative(rfacet, facet, location)
octObject *rfacet, *facet;
int location;
{
    long returnCode;

    RPCASSERT(RPCSendLong((long) OCT_OPEN_RELATIVE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(facet, RPCSendStream));
    RPCASSERT(RPCSendLong((long) rfacet->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) location, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(facet, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}
    

octStatus
octFreeFacet(object)
octObject *object;
{
    long returnCode = OCT_OK;
    
    RPCASSERT(RPCSendLong((long) OCT_FREE_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}


octStatus
octGetPoints(object, num_points, points)
octObject *object;
int32 *num_points;
struct octPoint *points;
{
    int i;
    long returnCode;
    long sendPoints, dummy;

    RPCASSERT(RPCSendLong(OCT_GET_POINTS_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) *num_points, RPCSendStream));

    if (points != RPCNIL(struct octPoint)) {
	sendPoints = RPC_SEND_POINTS;
    } else {
	sendPoints = RPC_DO_NOT_SEND_POINTS;
    }
    RPCASSERT(RPCSendLong(sendPoints, RPCSendStream));

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
    *num_points = (int) dummy;

    if ((sendPoints == RPC_SEND_POINTS) && (returnCode != OCT_TOO_SMALL)) {

	if (!RPCByteSwapped) {
	    if (fread((char *) points, sizeof(struct octPoint),
		      (int) *num_points, RPCReceiveStream) != *num_points) {
		return OCT_ERROR;
	    }
	} else {
	    for (i = 0; i < *num_points; i++) {
		RPCASSERT(RPCReceiveOctPoint(&points[i], RPCReceiveStream));
	    }
	}	    
    }
    return (octStatus) returnCode;

}


octStatus
octPutPoints(object, num_points, points)
octObject *object;
int32 num_points;
struct octPoint *points;
{
    int i;
    long returnCode = OCT_OK;

    RPCASSERT(RPCSendLong(OCT_PUT_POINTS_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCASSERT(RPCSendLong((long) num_points, RPCSendStream));

    if (!RPCByteSwapped) {
	if (fwrite((char *) points, sizeof(struct octPoint), (int) num_points, RPCSendStream) != num_points) {
	    return OCT_ERROR;
	}
    } else {
	for (i = 0; i < num_points; i++) {
	    RPCASSERT(RPCSendOctPoint(&(points[i]), RPCSendStream));
	}
    }
    
    RPCFLUSH(RPCSendStream);
#ifndef NOWAIT
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
#endif /* NOWAIT */    
    return (octStatus) returnCode;
}

octStatus
octBB(object, bbox)
octObject *object;
struct octBox *bbox;
{
    long returnCode;

    RPCASSERT(RPCSendLong(OCT_BB_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveOctBox(bbox, RPCReceiveStream));
    }

    return (octStatus) returnCode;
}


char *
octErrorString()
{
    char *string;
    static char rpcErrorMessageArea[1024];
    
    RPCASSERT(RPCSendLong(OCT_ERROR_STRING_FUNCTION, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveString(&string, RPCReceiveStream));
    (void) strcpy(rpcErrorMessageArea, string);
    (void) RPCFREE(string);
    return rpcErrorMessageArea;
}
			       

void
octGetFacet(object, facet)
octObject *object;
octObject *facet;
{
    RPCASSERT(RPCSendLong(OCT_GET_FACET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveOctObject(facet, RPCReceiveStream));
    return;
}

/*
 * dummy routines - octBegin and octEnd are called on the server side by VEM
 */

void
octBegin()
{
    return;
}

void
octEnd()
{
    return;
}


void
octTransformGeo(object, transform)
octObject *object;
struct octTransform *transform;
{
    RPCASSERT(RPCSendLong(OCT_TRANSFORM_GEO_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCASSERT(RPCSendOctTransform(transform, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    return;
}


octStatus
octTransformAndModifyGeo(object, transform)
octObject *object;
struct octTransform *transform;
{
    octStatus returnCode = OCT_OK;

    RPCASSERT(RPCSendLong(OCT_TRANSFORM_MODIFY_GEO_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCASSERT(RPCSendOctTransform(transform, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= (long) OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}

void
octTransformPoints(num_pts, pts, xform)
int num_pts;
struct octPoint *pts;
struct octTransform *xform;
{
    long n_pts = (long) num_pts;
    int i;

    RPCASSERT(RPCSendLong(OCT_TRANSFORM_POINTS_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong(n_pts, RPCSendStream));
    for (i = 0; i < n_pts; i++) {
	RPCASSERT(RPCSendOctPoint(&pts[i], RPCSendStream));
    }
    RPCASSERT(RPCSendOctTransform(xform, RPCSendStream));
    for (i = 0; i < n_pts; i++) {
	RPCASSERT(RPCReceiveOctPoint(&pts[i], RPCReceiveStream));
    }
    return;
}


void
octScaleGeo(object, scale)
octObject *object;
double scale;
{
    RPCASSERT(RPCSendLong(OCT_SCALE_GEO_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCASSERT(RPCSendFloat(scale, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    return;
}


octStatus
octScaleAndModifyGeo(object, scale)
octObject *object;
double scale;
{
    long returnCode = OCT_OK;

    RPCASSERT(RPCSendLong(OCT_SCALE_MODIFY_GEO_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCASSERT(RPCSendFloat(scale, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= (long) OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


void
octMarkTemporary(object)
octObject *object;
{
    RPCASSERT(RPCSendLong((long) OCT_MARK_TEMPORARY_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}


void
octUnmarkTemporary(object)
octObject *object;
{
    RPCASSERT(RPCSendLong((long) OCT_UNMARK_TEMPORARY_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}


int
octIsTemporary(object)
octObject *object;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_IS_TEMPORARY_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    return (int) returnCode;
}


void
octExternalId(object, xid)
octObject *object;
int32 *xid;
{
    long dummy;

    RPCASSERT(RPCSendLong((long) OCT_XID_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
    *xid = (int32) dummy;
    return;
}


octStatus
octGetByExternalId(facet, xid, object)
octObject *facet;
int32 xid;
octObject *object;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong((long) OCT_GET_BY_XID_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) facet->objectId, RPCSendStream));
    RPCASSERT(RPCSendLong((long) xid, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= (octStatus) OCT_OK) {
	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
    }
    return (octStatus) returnCode;
}


void
octFullName(object, name)
octObject *object;
char **name;
{
    RPCASSERT(RPCSendLong((long) OCT_FULL_NAME_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) object->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveString(name, RPCReceiveStream));
    return;
}


octStatus
octGetFacetInfo(object, info)
octObject *object;
struct octFacetInfo *info;
{
    long returnCode;

    RPCASSERT(RPCSendLong((long) OCT_GET_FACET_INFO_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendOctObject(object, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    if (returnCode >= OCT_OK) {
	RPCASSERT(RPCReceiveFacetInfo(info, RPCReceiveStream));
    }
    return returnCode;
}


/*ARGSUSED*/
octStatus
octPrintObject(stream, object, sub)
FILE *stream;
octObject *object;
int sub;
{
    vemMessage("octPrintObject is not implemented... yet\n", MSG_DISP);
    return OCT_OK;
}


int
octIdsEqual(id1, id2)
octId id1, id2;
{
    return id1 == id2;
}


int
octIdIsNull(id)
octId id;
{
    return id == oct_null_id;
}


/* for use with the `st' package */

int
octIdCmp(a, b)
CONST char *a, *b;
{
    return *((octId *) a) - *((octId *) b);
}


int
octIdHash(a, mod)
char *a;
int mod;
{
    char *dummy = (char *) *((octId *) a);
    return st_numhash(dummy, mod);
}


char *
octFormatId(id)
octId id;
{
    static char buffer[1024];
    (void) sprintf(buffer, "%d", (int) id);
    return buffer;
}

