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
 * region package stubs
 *
 * Rick L Spickelmier, 7/24/88
 */

/* #include "region.h"  */
#include "rpcApp.h"

extern rpcInternalStatus        RPCSendRegionGenerator
	ARGS((regGenerator *generator, STREAM stream));
extern rpcInternalStatus        RPCReceiveRegionGenerator
	ARGS((regGenerator *generator, STREAM stream));

/*
 * region package
 */

/*ARGSUSED*/
regStatus
RPCregNextDone(generator, object)
regGenerator *generator;
octObject *object;
{
    return REG_NOMORE;
}


/*ARGSUSED*/
regStatus
RPCregErrorFunction(generator, object)
regGenerator *generator;
octObject *object;
{
    return REG_FAIL;
}


/*ARGSUSED*/
void
RPCregErrorEndFunction(generator, object)
regGenerator *generator;
octObject *object;
{
    return;
}


/* regNext is a macro in region.h */

regStatus
RPCregNext(generator, object, num_points, points, size)
regGenerator *generator;
octObject *object;
int *num_points;
struct octPoint **points;
struct octBox *size;
{
    long length, returnCode;
    struct octPoint *temp_points;
    regGenerator *realGenerator;
    int i;

    realGenerator = (regGenerator *) generator->state;
        
    RPCASSERT(RPCSendLong(REG_NEXT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendRegionGenerator(realGenerator, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    RPCASSERT(RPCReceiveRegionGenerator(realGenerator, RPCReceiveStream));

    switch (returnCode) {
	case (long) REG_OK:

	RPCASSERT(RPCReceiveOctObject(object, RPCReceiveStream));
	if ((object->type == OCT_POLYGON) || (object->type == OCT_PATH)) {
	    
	    /* receive the length */

	    RPCASSERT(RPCReceiveLong(&length, RPCReceiveStream));
	    *num_points = (int) length;

	    /* allocate and receive the points */
	    temp_points = RPCARRAYALLOC(struct octPoint, length);

	    for (i = 0; i < length; i++) {
		RPCASSERT(RPCReceiveOctPoint(&temp_points[i], RPCReceiveStream));
	    }
	    *points = temp_points;
	}
	RPCASSERT(RPCReceiveOctBox(size, RPCReceiveStream));
	generator->state = (char *) realGenerator;

	break;

	case (long) REG_FAIL:
	generator->gen_func = RPCregErrorFunction;
	break;
	
	case (long) REG_NOMORE:
	generator->gen_func = RPCregNextDone;
	break;
    }

    return (regStatus) returnCode;

}


void
RPCregEnd(generator)
regGenerator *generator;
{
    regGenerator *realGenerator;

    realGenerator = (regGenerator *) generator->state;

    (void) RPCSendLong(REG_END_FUNCTION, RPCSendStream);
    (void) RPCSendRegionGenerator(realGenerator, RPCSendStream);
    (void) fflush(RPCSendStream);

    RPCFREE(realGenerator);

    generator->gen_func = RPCregErrorFunction;
    generator->end_func = RPCregErrorEndFunction;
    
    return;
}


void
regInit(parentObj, someExtent, someMask, generator)
octObject *parentObj;
octBox *someExtent;
octObjectMask someMask;
regGenerator *generator;
{
    regGenerator *realGenerator;

    realGenerator = RPCALLOC(regGenerator);
    
    (void) RPCSendLong(REG_INIT_FUNCTION, RPCSendStream);
    (void) RPCSendOctObject(parentObj, RPCSendStream);
    (void) RPCSendOctBox(someExtent, RPCSendStream);
    (void) RPCSendLong((long) someMask, RPCSendStream);

    (void) fflush(RPCSendStream);

    (void) RPCReceiveRegionGenerator(realGenerator, RPCReceiveStream);
    generator->gen_func = RPCregNext;
    generator->end_func = RPCregEnd;
    generator->state = (char *) realGenerator;
    
    return;
}



regStatus
regObjStart(theFacet, region, mask, theGen, allInside)
octObject *theFacet;
struct octBox *region;
octObjectMask mask;
regObjGen *theGen;
int allInside;
{
    long returnCode, dummy;
    
    
    RPCASSERT(RPCSendLong(REG_OBJ_START_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) theFacet->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctBox(region, RPCSendStream));
    RPCASSERT(RPCSendLong((long) mask, RPCSendStream));
    RPCASSERT(RPCSendLong((long) allInside, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);
    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	*theGen = (regObjGen) dummy;
    }
    
    return (regStatus) returnCode;
}


regStatus
regObjNext(theGen, theObj)
regObjGen theGen;
octObject *theObj;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong(REG_OBJ_NEXT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) theGen, RPCSendStream));

    RPCFLUSH(RPCSendStream);
    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCReceiveOctObject(theObj, RPCReceiveStream));
    }
    
    return (regStatus) returnCode;
}


void
regObjFinish(theGen)
regObjGen theGen;
{
    (void) RPCSendLong(REG_OBJ_FINISH_FUNCTION, RPCSendStream);
    (void) RPCSendLong((long) theGen, RPCSendStream);
    (void) fflush(RPCSendStream);
    return;
}


regStatus
regFindActual(theFacet, thePoint, termObject, implBB)
octObject *theFacet;
struct octPoint *thePoint;
octObject *termObject;
struct octBox *implBB;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong(REG_FIND_ACTUAL_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) theFacet->objectId, RPCSendStream));
    RPCASSERT(RPCSendOctPoint(thePoint, RPCSendStream));

    RPCFLUSH(RPCSendStream);
    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCReceiveOctObject(termObject, RPCReceiveStream));
	RPCASSERT(RPCReceiveOctBox(implBB, RPCReceiveStream));
    }
    
    return (regStatus) returnCode;
}


regStatus
regFindImpl(theTerm, implBB)
octObject *theTerm;
struct octBox *implBB;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong(REG_FIND_IMPL_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) theTerm->objectId, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCReceiveOctBox(implBB, RPCReceiveStream));
    }
    
    return (regStatus) returnCode;
}


regStatus
regFindNet(obj, netObj)
octObject *obj, *netObj;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong(REG_FIND_NET_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) obj->objectId, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    if ((regStatus) returnCode == REG_OK) {
	RPCASSERT(RPCReceiveOctObject(netObj, RPCReceiveStream));
    }
    
    return (regStatus) returnCode;
}


char *
regErrorString()
{
    static char error_buffer[1024];
    char *ptr;

    RPCASSERT(RPCSendLong(REG_ERROR_STRING_FUNCTION, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&ptr, RPCReceiveStream));
    (void) strcpy(error_buffer, ptr);
    RPCFREE(ptr);
    return error_buffer;
}

