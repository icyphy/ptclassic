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
 * code that is used both by the server and the application
 *
 * Rick L Spickelmier, 3/17/86, last modified 3/2/87
 *
 */

#ifdef SERVER  
#include <X/Xlib.h>  
#include "list.h"
#include "message.h"
#include "selset.h"
#include "bindings.h"
#include "buffer.h"
#include "attributes.h"
#include "commands.h"
#include "region.h"
#include "rpcInternal.h"
char RPCErrorBuffer[MESSAGE_SIZE];
#else  
#include "rpcApp.h"
#endif /* SERVER */

rpcInternalStatus RPCReceiveStranger
	ARGS((struct octUserValue *stranger, STREAM stream));
rpcInternalStatus RPCSendStranger
	ARGS((struct octUserValue *stranger, STREAM stream));
rpcInternalStatus RPCReceiveSpot
	ARGS((RPCSpot *spot, STREAM stream));
rpcStatus RPCReceiveVemArg
	ARGS((RPCArg *arg, STREAM stream));
rpcInternalStatus RPCSendSpot
	ARGS((RPCSpot *spot, STREAM stream));
rpcStatus RPCSendVemArg
	ARGS((RPCArg *arg, STREAM stream));

rpcInternalStatus
RPCReceiveOctObject(object, stream)
octObject *object;
STREAM stream;
{
    long dummy, dummy1, dummy2, dummy3;
    rpcInternalStatus status;
    long startingAngle, endingAngle, innerRadius, outerRadius;
    char temp;
    
    RPCASSERT(RPCReceiveLong(&dummy, stream));
    object->type = (int) dummy;
    RPCASSERT(RPCReceiveLong(&dummy, stream));
    object->objectId = (octId) dummy;

    switch (object->type) {
	
	case OCT_UNDEFINED_OBJECT:
	    return RPC_TRUE;

	case OCT_FACET:
	    return(RPCReceiveString(&object->contents.facet.cell, stream)
		&& RPCReceiveString(&object->contents.facet.view, stream)
		&& RPCReceiveString(&object->contents.facet.facet, stream)
		&& RPCReceiveString(&object->contents.facet.version, stream)
		&& RPCReceiveString(&object->contents.facet.mode, stream));

	case OCT_INSTANCE:
	    return(RPCReceiveOctTransform(&object->contents.instance.transform, stream)
		&& RPCReceiveString(&object->contents.instance.name, stream)
		&& RPCReceiveString(&object->contents.instance.master, stream)
		&& RPCReceiveString(&object->contents.instance.view, stream)
		&& RPCReceiveString(&object->contents.instance.facet, stream)
		&& RPCReceiveString(&object->contents.instance.version, stream));

	case OCT_PROP:
	    return RPCReceiveOctProp(&(object->contents.prop), stream);

	case OCT_TERM:
	    status = RPCReceiveLong(&dummy1, stream)
	          && RPCReceiveLong(&dummy2, stream)
	          && RPCReceiveLong(&dummy3, stream)
	          && RPCReceiveString(&object->contents.term.name, stream);
            object->contents.term.instanceId = (octId) dummy1;
            object->contents.term.formalExternalId = (int32) dummy2;
            object->contents.term.width = (int32) dummy3;
            return status;
    

	case OCT_NET:
	    status = RPCReceiveString(&object->contents.net.name, stream)
		  && RPCReceiveLong(&dummy, stream);
            object->contents.net.width = (int32) dummy;
	    return status;

	case OCT_BOX:
	    return RPCReceiveOctBox(&object->contents.box, stream);

	case OCT_CIRCLE:
	    status = RPCReceiveLong(&startingAngle, stream)
	          && RPCReceiveLong(&endingAngle, stream)
		  && RPCReceiveLong(&innerRadius, stream)
		  && RPCReceiveLong(&outerRadius, stream)
		  && RPCReceiveOctPoint(&object->contents.circle.center, stream);
            if (status) {
                object->contents.circle.startingAngle = (octCoord) startingAngle;
                object->contents.circle.endingAngle = (octCoord) endingAngle;
                object->contents.circle.innerRadius = (octCoord) innerRadius;
                object->contents.circle.outerRadius = (octCoord) outerRadius;
	    }
            return status;

	case OCT_PATH:
	    status = RPCReceiveLong(&dummy, stream);
            object->contents.path.width = (octCoord) dummy;
	    return status;
	    
	case OCT_POLYGON:
	    return RPC_TRUE;

	case OCT_LABEL:
	    status = RPCReceiveString(&object->contents.label.label, stream)
	      && RPCReceiveOctBox(&object->contents.label.region, stream)
		&& RPCReceiveLong(&dummy, stream);
	    if (!status) {
		return RPC_FALSE;
	    }
	    object->contents.label.textHeight = (octCoord) dummy;
	    if (!RPCReceiveByte(&temp, stream)) {
		return RPC_FALSE;
	    }
	    object->contents.label.vertJust = (unsigned) temp;
	    if (!RPCReceiveByte(&temp, stream)) {
		return RPC_FALSE;
	    }
	    object->contents.label.horizJust = (unsigned) temp;
	    if (!RPCReceiveByte(&temp, stream)) {
		return RPC_FALSE;
	    }
	    object->contents.label.lineJust = (unsigned) temp;
	    return RPC_TRUE;

	case OCT_BAG:
	    return RPCReceiveString(&object->contents.bag.name, stream);

	case OCT_LAYER:
	    return RPCReceiveString(&object->contents.layer.name, stream);

	case OCT_POINT:
	    return RPCReceiveOctPoint(&object->contents.point, stream);

	case OCT_EDGE:
	    return(RPCReceiveOctPoint(&object->contents.edge.start, stream)
		&& RPCReceiveOctPoint(&object->contents.edge.end, stream));

	case OCT_CHANGE_LIST:
	    status = RPCReceiveLong(&dummy1, stream)
	          && RPCReceiveLong(&dummy2, stream);
	    object->contents.changeList.objectMask = (octObjectMask) dummy1;
	    object->contents.changeList.functionMask = (octFunctionMask) dummy2;
	    return status;

	case OCT_CHANGE_RECORD:
	    return(RPCReceiveLong(&object->contents.changeRecord.changeType, stream)
	        && RPCReceiveLong(&object->contents.changeRecord.objectExternalId, stream)
	        && RPCReceiveLong(&object->contents.changeRecord.contentsExternalId, stream));

	default:
	    (void) fprintf(stderr, "RPC Error: illegal oct object type: %d\n", object->type);
	    return RPC_FALSE;
    }
}

rpcInternalStatus
RPCSendOctObject(object, stream)
octObject *object;
STREAM stream;
{
    rpcInternalStatus status;
    
    RPCASSERT(RPCSendLong((long) object->type, stream));
    RPCASSERT(RPCSendLong((long) object->objectId, stream));

    switch (object->type) {

	case OCT_UNDEFINED_OBJECT:
	    return RPC_TRUE;

	case OCT_FACET:
	    return(RPCSendString(object->contents.facet.cell, stream)
		&& RPCSendString(object->contents.facet.view, stream)
		&& RPCSendString(object->contents.facet.facet, stream)
		&& RPCSendString(object->contents.facet.version, stream)
		&& RPCSendString(object->contents.facet.mode, stream));

	case OCT_INSTANCE:
	    return(RPCSendOctTransform(&object->contents.instance.transform, stream)
		&& RPCSendString(object->contents.instance.name, stream)
		&& RPCSendString(object->contents.instance.master, stream)
		&& RPCSendString(object->contents.instance.view, stream)
		&& RPCSendString(object->contents.instance.facet, stream)
		&& RPCSendString(object->contents.instance.version, stream));

	case OCT_PROP:
	    return RPCSendOctProp(&(object->contents.prop), stream);

	case OCT_TERM:
	    return(RPCSendLong((long) object->contents.term.instanceId, stream)
	        && RPCSendLong((long) object->contents.term.formalExternalId, stream)
	        && RPCSendLong((long) object->contents.term.width, stream)
		&& RPCSendString(object->contents.term.name, stream));

	case OCT_NET:
	    return RPCSendString(object->contents.net.name, stream)
		&& RPCSendLong((long) object->contents.net.width, stream);

	case OCT_BOX:
	    return RPCSendOctBox(&(object->contents.box), stream);

	case OCT_CIRCLE:
	    return(RPCSendLong((long) object->contents.circle.startingAngle, stream)
		&& RPCSendLong((long) object->contents.circle.endingAngle, stream)
		&& RPCSendLong((long) object->contents.circle.innerRadius, stream)
		&& RPCSendLong((long) object->contents.circle.outerRadius, stream)
		&& RPCSendOctPoint(&(object->contents.circle.center), stream));

	case OCT_PATH:
	    return RPCSendLong((long) object->contents.path.width, stream);

	case OCT_POLYGON:
	    return RPC_TRUE;

	case OCT_LABEL:
	    status = RPCSendString(object->contents.label.label, stream)
	      && RPCSendOctBox(&(object->contents.label.region), stream)
		&& RPCSendLong((long) object->contents.label.textHeight, stream);
	    if (!status) {
		return RPC_FALSE;
	    }
	    if (!RPCSendByte((char) object->contents.label.vertJust, stream)) {
		return RPC_FALSE;
	    }
	    if (!RPCSendByte((char) object->contents.label.horizJust, stream)) {
		return RPC_FALSE;
	    }
	    if (!RPCSendByte((char) object->contents.label.lineJust, stream)) {
		return RPC_FALSE;
	    }
	    return RPC_TRUE;

	case OCT_BAG:
	    return RPCSendString(object->contents.bag.name, stream);

	case OCT_LAYER:
	    return RPCSendString(object->contents.layer.name, stream);

	case OCT_POINT:
	    return RPCSendOctPoint(&(object->contents.point), stream);

	case OCT_EDGE:
	    return(RPCSendOctPoint(&(object->contents.edge.start), stream)
		&& RPCSendOctPoint(&(object->contents.edge.end), stream));

	case OCT_CHANGE_LIST:
	    return(RPCSendLong((long) object->contents.changeList.objectMask, stream)
	        && RPCSendLong((long) object->contents.changeList.functionMask, stream));

	case OCT_CHANGE_RECORD:
	    return(RPCSendLong((long) object->contents.changeRecord.changeType, stream)
	        && RPCSendLong((long) object->contents.changeRecord.objectExternalId, stream)
	        && RPCSendLong((long) object->contents.changeRecord.contentsExternalId, stream));

	default:
	    (void) fprintf(stderr, "RPC Error: illegal oct object type: %d\n", object->type);
	    return RPC_FALSE;
    }
}

rpcInternalStatus
RPCReceiveOctProp(prop, stream)
struct octProp *prop;
STREAM stream;
{
    long dummy;
    rpcInternalStatus status;

    
    RPCASSERT(RPCReceiveString(&(prop->name), stream));
    RPCASSERT(RPCReceiveLong(&dummy, stream));
    prop->type = (octPropType) dummy;

    switch(prop->type) {

	case OCT_NULL:
	    return RPC_TRUE;

	case OCT_INTEGER:
	    status = RPCReceiveLong(&dummy, stream);
	    prop->value.integer = (int) dummy;
	    return status;

	case OCT_REAL:
	    return RPCReceiveFloat(&(prop->value.real), stream);

	case OCT_STRING:
	    return RPCReceiveString(&(prop->value.string), stream);

	case OCT_ID:
	    status = RPCReceiveLong(&dummy, stream);
	    prop->value.id = (octId) dummy;
	    return status;

	case OCT_STRANGER:
	    return RPCReceiveStranger(&(prop->value.stranger), stream);

	case OCT_INTEGER_ARRAY:
	    return RPCReceiveIntegerArray((struct octIntegerArray *) &(prop->value.integer_array), stream);

	case OCT_REAL_ARRAY:
	    return RPCReceiveRealArray((struct octRealArray *) &(prop->value.real_array), stream);

	default:
	    (void) fprintf(stderr, "RPC Error: illegal property type: %d\n", prop->type);
	    return RPC_FALSE;
    }
}

rpcInternalStatus
RPCSendOctProp(prop, stream)
struct octProp *prop;
STREAM stream;
{
    RPCASSERT(RPCSendString(prop->name, stream));
    RPCASSERT(RPCSendLong((long) prop->type, stream));

    switch(prop->type) {

	case OCT_NULL:
	    return RPC_TRUE;

	case OCT_INTEGER:
	    return RPCSendLong((long) prop->value.integer, stream);

	case OCT_REAL:
	    return RPCSendFloat(prop->value.real, stream);

	case OCT_STRING:
	    return RPCSendString(prop->value.string, stream);

	case OCT_ID:
	    return RPCSendLong((long) prop->value.id, stream);

	case OCT_STRANGER:
	    return RPCSendStranger(&(prop->value.stranger), stream);

	case OCT_INTEGER_ARRAY:
	    return RPCSendIntegerArray((struct octIntegerArray *) &(prop->value.integer_array), stream);

	case OCT_REAL_ARRAY:
	    return RPCSendRealArray((struct octRealArray *) &(prop->value.real_array), stream);

	default:
	    (void) fprintf(stderr, "RPC Error: illegal property type: %d\n", prop->type);
	    return RPC_FALSE;
	    
    }
}

rpcInternalStatus
RPCSendIntegerArray(iarray, stream)
struct octIntegerArray *iarray;
STREAM stream;
{
    int i;

    RPCASSERT(RPCSendLong((long) iarray->length, stream));

    if (!RPCByteSwapped) {
	if (fwrite((char *) iarray->array, sizeof(int),
		   (int) iarray->length, stream) != iarray->length) {
	    (void) fprintf(stderr, "RPC Error: fwrite of irray->array failed\n");
	    return RPC_FALSE;
	}
    } else {
	for (i = 0; i < iarray->length; i++) {
	    RPCASSERT(RPCSendLong((long) iarray->array[i], stream));
	}
    }
    
    return RPC_TRUE;
}


rpcInternalStatus
RPCSendRealArray(rarray, stream)
struct octRealArray *rarray;
STREAM stream;
{
    int i;

    RPCASSERT(RPCSendLong((long) rarray->length, stream));

    for (i = 0; i < rarray->length; i++) {
	RPCASSERT(RPCSendFloat(rarray->array[i], stream));
    }
    return RPC_TRUE;
}


rpcInternalStatus
RPCReceiveIntegerArray(iarray, stream)
struct octIntegerArray *iarray;
STREAM stream;
{
    int i;
    long dummy;
    

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    iarray->length = (int) dummy;

    if (!RPCByteSwapped) {
	iarray->array = RPCARRAYALLOC(int, iarray->length);
	if (fread((char *) iarray->array, sizeof(int),
		  (int) iarray->length, stream) != iarray->length) {
	    (void) fprintf(stderr, "RPC Error: fread of irray->array failed\n");
	    return RPC_FALSE;
	}
    } else {
	for (i = 0; i < iarray->length; i++) {
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    iarray->array[i] = (int) dummy;
	}
    }
    
    return RPC_TRUE;
}

rpcInternalStatus
RPCReceiveRealArray(rarray, stream)
struct octRealArray *rarray;
STREAM stream;
{
    int i;
    long dummy;
    

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    rarray->length = (int) dummy;

    rarray->array = RPCARRAYALLOC(double, rarray->length);

    for (i = 0; i < rarray->length; i++) {
	RPCASSERT(RPCReceiveFloat(&rarray->array[i], stream));
    }
    return RPC_TRUE;
}


rpcInternalStatus
RPCReceiveOctTransform(transform, stream)
struct octTransform *transform;
STREAM stream;
{
    long dummy;

    
    RPCASSERT(RPCReceiveOctPoint(&transform->translation, stream));

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    transform->transformType = (octTransformType) dummy;

    if (transform->transformType == OCT_FULL_TRANSFORM) {
	return(RPCReceiveFloat(&transform->generalTransform[0][0], stream)
	    && RPCReceiveFloat(&transform->generalTransform[0][1], stream)
	    && RPCReceiveFloat(&transform->generalTransform[1][0], stream)
	    && RPCReceiveFloat(&transform->generalTransform[1][1], stream));
    } else {
	return RPC_TRUE;
    }
}

rpcInternalStatus
RPCSendOctTransform(transform, stream)
struct octTransform *transform;
STREAM stream;
{
    if (transform->transformType == OCT_FULL_TRANSFORM) {
	return(RPCSendOctPoint(&(transform->translation), stream)
	    && RPCSendLong((long) transform->transformType, stream)
	    && RPCSendFloat(transform->generalTransform[0][0], stream)
	    && RPCSendFloat(transform->generalTransform[0][1], stream)
	    && RPCSendFloat(transform->generalTransform[1][0], stream)
	    && RPCSendFloat(transform->generalTransform[1][1], stream));
    } else {
	return(RPCSendOctPoint(&(transform->translation), stream)
	    && RPCSendLong((long) transform->transformType, stream));
    }
}

rpcInternalStatus
RPCReceiveOctBox(box, stream)
struct octBox *box;
STREAM stream;
{
    if (!RPCByteSwapped) {
	if (fread((char *) box, sizeof(struct octBox), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC Error: fread of octBox failed\n");
	    return RPC_FALSE;
	}
	return RPC_TRUE;
    } else {
	return(RPCReceiveOctPoint(&box->lowerLeft, stream)
	       && RPCReceiveOctPoint(&box->upperRight, stream));
    }
}


rpcInternalStatus
RPCSendOctBox(box, stream)
struct octBox *box;
STREAM stream;
{
    if (!RPCByteSwapped) {
	if (fwrite((char *) box, sizeof(struct octBox), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC Error: fwrite of octBox failed\n");
	    return RPC_FALSE;
	}
	return RPC_TRUE;
    } else {
	return(RPCSendOctPoint(&(box->lowerLeft), stream)
	       && RPCSendOctPoint(&(box->upperRight), stream));
    }
}


rpcInternalStatus
RPCReceiveOctPoint(point, stream)
struct octPoint *point;
STREAM stream;
{
    long x, y;
    rpcInternalStatus status;

    
    if (!RPCByteSwapped) {
	if (fread((char *) point, sizeof(struct octPoint), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC Error: fread of octPoint failed\n");
	    return RPC_FALSE;
	}
	return RPC_TRUE;
    } else {
	status = RPCReceiveLong(&x, stream)
	      && RPCReceiveLong(&y, stream);
	if (status) {
	    point->x = (octCoord) x;
	    point->y = (octCoord) y;
	}
	return status;
    }
}


rpcInternalStatus
RPCSendOctPoint(point, stream)
struct octPoint *point;
STREAM stream;
{
    if (!RPCByteSwapped) {
	if (fwrite((char *) point, sizeof(struct octPoint), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC Error: fwrite of octPoint failed\n");
	    return RPC_FALSE;
	}
	return RPC_TRUE;
    } else {
	return(RPCSendLong((long) point->x, stream)
	       && RPCSendLong((long) point->y, stream));
    }
}


rpcInternalStatus
RPCReceiveRegionGenerator(generator, stream)
regGenerator *generator;
STREAM stream;
{
    rpcInternalStatus status;
    long gen_func, end_func, state;

    status = RPCReceiveLong(&gen_func, stream)
	     && RPCReceiveLong(&end_func, stream)
	     && RPCReceiveLong(&state, stream);

    generator->gen_func = (regStatus (*)()) gen_func;
    generator->end_func = (void (*)()) end_func;
    generator->state = (reg_generic) state;
    
    return status;
}


rpcInternalStatus
RPCReceiveOctGenerator(generator, stream)
octGenerator *generator;
STREAM stream;
{
    rpcInternalStatus status;
    long generator_func, state, free_func;

    
    status = RPCReceiveLong(&generator_func, stream)
          && RPCReceiveLong(&state, stream)
	  && RPCReceiveLong(&free_func, stream);
    
    if (status) {
	generator->generator_func = (octStatus (*)()) generator_func;
	generator->state = (char *) state;
	generator->free_func = (octStatus (*)()) free_func;
    }
    return status;
}


rpcInternalStatus
RPCSendRegionGenerator(generator, stream)
regGenerator *generator;
STREAM stream;
{
    long gen_func, end_func, state;

    gen_func = (long) generator->gen_func;
    end_func = (long) generator->end_func;
    state = (long) generator->state;
    return RPCSendLong(gen_func, stream)
        && RPCSendLong(end_func, stream)
	&& RPCSendLong(state, stream);
}


rpcInternalStatus
RPCSendOctGenerator(generator, stream)
octGenerator *generator;
STREAM stream;
{
    long gen_func, state, free_func;

    gen_func = (long) generator->generator_func;
    state = (long) generator->state;
    free_func = (long) generator->free_func;
    return(RPCSendLong(gen_func, stream) && RPCSendLong(state, stream)
	   && RPCSendLong(free_func, stream));
}

double RPCatof();

rpcInternalStatus
RPCReceiveFloat(real, stream)
double *real;
STREAM stream;
{
    char *charArray;

    union u_tag {
      long lngs[2];
      double db;
    } u;

    double db;

    if (!RPCFloatingPointSame) {
	charArray = RPCARRAYALLOC(char, 20);

	RPCASSERT(RPCReceiveString(&charArray, stream));

	db = RPCatof(charArray);
	*real = db;

	(void) RPCFREE(charArray);
	
    } else {
  	RPCASSERT(RPCReceiveLong(&u.lngs[0], stream));
  	RPCASSERT(RPCReceiveLong(&u.lngs[1], stream));
 	*real = u.db;
    }
	
    return RPC_TRUE;
}

rpcInternalStatus
RPCSendFloat(real, stream)
double real;
STREAM stream;
{
    char charArray[20];
    double db;
    long *lngs;
    
    if (!RPCFloatingPointSame) {
	(void) sprintf(charArray, "%g", real);
	return RPCSendString(charArray, stream);
    } else {
	db = real;
	lngs = (long *) &db;
	return RPCSendLong(lngs[0], stream) && RPCSendLong(lngs[1], stream);
    }
}

rpcInternalStatus
RPCReceiveStranger(stranger, stream)
struct octUserValue *stranger;
STREAM stream;
{
    long length;
    char *array;
    int i;

    RPCASSERT(RPCReceiveLong(&length, stream));

    stranger->length = length;
    array = RPCARRAYALLOC(char, length);

    if (!RPCByteSwapped) {
	if (fread((char *) array, sizeof(char), (int) length, stream) != length) {
	    (void) fprintf(stderr, "RPC Error: fread of stranger->contents failed\n");
	    return RPC_FALSE;
	}
    } else {
	for (i = 0; i < length; i++) {
	    RPCASSERT(RPCReceiveByte(&array[i], stream));
	}
    }
    
    stranger->contents = array;

    return RPC_TRUE;
}

rpcInternalStatus
RPCSendStranger(stranger, stream)
struct octUserValue *stranger;
STREAM stream;
{
    int i;

    RPCASSERT(RPCSendLong((long) stranger->length, stream));

    if (!RPCByteSwapped) {
	if (fwrite((char *) stranger->contents, sizeof(char), (int) stranger->length, stream) != stranger->length) {
	    (void) fprintf(stderr, "RPC Error: fwrite of stranger->contents failed\n");
	    return RPC_FALSE;
	}
    } else {
	for (i = 0; i < stranger->length; i++) {
	    RPCASSERT(RPCSendByte(stranger->contents[i], stream));
	}
    }
    
    return RPC_TRUE;
}


rpcInternalStatus
RPCReceiveString(userArray, stream)
char **userArray;
STREAM stream;
{
    long len, i;
    char *charArray;

    RPCASSERT(RPCReceiveLong(&len, stream));

    if (len == -1) {
	*userArray = RPCNIL(char);
	return RPC_TRUE;
    }

    charArray = RPCARRAYALLOC(char, (len + 1));
    
    if (len > 0) {
	/* can't send chunks bigger than 1024 across the network... */
	int numChunks = len / 1024;
	int leftOver = len % 1024;

	for (i = 0; i < numChunks; i++) {
	    if (fread(&charArray[i * 1024], sizeof(char), (int) 1024, stream) != 1024) {
		(void) fprintf(stderr, "RPC Error: fread of charArray failed\n");
		return RPC_FALSE;
	    }
        }
        if (leftOver != 0) {
	    if (fread(&charArray[numChunks * 1024], sizeof(char), (int) leftOver, stream) != leftOver) {
	        (void) fprintf(stderr, "RPC Error: fread of charArray failed\n");
	        return RPC_FALSE;
	    }
        }
    }
    
    charArray[len] = '\0';
    *userArray = charArray;

    return RPC_TRUE;
}

rpcInternalStatus
RPCSendString(charArray, stream)
char *charArray;
STREAM stream;
{
    long len, i;
    
    if (charArray == RPCNIL(char)) {
	RPCASSERT(RPCSendLong((long) -1, stream));
	return RPC_TRUE;
    }

    len = strlen(charArray);
    RPCASSERT(RPCSendLong(len, stream));

    if (len > 0) {
        /* can't send chunks bigger than 1024 across the network ... */
        int numChunks = len / 1024;
        int leftOver = len % 1024;

        for (i = 0; i < numChunks; i++) {
	    if (fwrite(&charArray[i * 1024], sizeof(char), (int) 1024, stream) != 1024) {
	        (void) fprintf(stderr, "RPC Error: fwrite of charArray failed\n");
	        return RPC_FALSE;
	    }
	    fflush(stream);
	}
	if (leftOver != 0) {
	    if (fwrite(&charArray[numChunks * 1024], sizeof(char), (int) leftOver, stream) != leftOver) {
	        (void) fprintf(stderr, "RPC Error: fwrite of charArray failed\n");
	        return RPC_FALSE;
	    }
	}
    }
    return RPC_TRUE;
}


rpcInternalStatus
RPCReceiveLong(value, stream)
long *value;
STREAM stream;
{
    char *buf, temp;
    
    if (!RPCByteSwapped) {
	if (fread((char *) value, sizeof(long), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "Ptolemy failed to receive response from Vem!\n");
	    return RPC_FALSE;
	} else {
	    return RPC_TRUE;
	}
    } else {
	if (fread((char *) value, sizeof(long), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "Ptolemy failed to receive response from Vem!\n");
	    return RPC_FALSE;
	} else {
	    buf = (char *) value;
	    temp = buf[0];
	    buf[0] = buf[3];
	    buf[3] = temp;
	    temp = buf[1];
	    buf[1] = buf[2];
	    buf[2] = temp;
	    return RPC_TRUE;
	}    
    }
}


rpcInternalStatus
RPCReceiveFacetInfo(info, stream)
struct octFacetInfo *info;
STREAM stream;
{
    long dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->timeStamp = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->bbDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->formalDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->createDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->deleteDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->modifyDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->attachDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->detachDate = (int32) dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    info->numObjects = (int) dummy;

    RPCASSERT(RPCReceiveOctBox(&info->bbox, stream));
    RPCASSERT(RPCReceiveString(&info->fullName, stream));

    return RPC_TRUE;
}


rpcInternalStatus
RPCSendFacetInfo(info, stream)
struct octFacetInfo *info;
STREAM stream;
{
    RPCASSERT(RPCSendLong((long) info->timeStamp, stream));
    RPCASSERT(RPCSendLong((long) info->bbDate, stream));
    RPCASSERT(RPCSendLong((long) info->formalDate, stream));
    RPCASSERT(RPCSendLong((long) info->createDate, stream));
    RPCASSERT(RPCSendLong((long) info->deleteDate, stream));
    RPCASSERT(RPCSendLong((long) info->modifyDate, stream));
    RPCASSERT(RPCSendLong((long) info->attachDate, stream));
    RPCASSERT(RPCSendLong((long) info->detachDate, stream));
    RPCASSERT(RPCSendLong((long) info->numObjects, stream));
    RPCASSERT(RPCSendOctBox(&info->bbox, stream));
    RPCASSERT(RPCSendString(info->fullName, stream));
    return RPC_TRUE;
}


#ifdef RPCLIBRARY
rpcInternalStatus
RPCReceiveFunctionToken(value, stream)
long *value;
STREAM stream;
/* this routine gives a better message than RPCReceiveLong */
{
    char *buf, temp;
    
    if (!RPCByteSwapped) {
	if (fread((char *) value, sizeof(long), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC application reports that VEM has been terminated\n");
	    return RPC_FALSE;
	} else {
	    return RPC_TRUE;
	}
    } else {
	if (fread((char *) value, sizeof(long), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC application reports that VEM has been terminated\n");
	    return RPC_FALSE;
	} else {
	    buf = (char *) value;
	    temp = buf[0];
	    buf[0] = buf[3];
	    buf[3] = temp;
	    temp = buf[1];
	    buf[1] = buf[2];
	    buf[2] = temp;
	    return RPC_TRUE;
	}    
    }
}
#endif


rpcInternalStatus
RPCSendLong(value, stream)
long value;
STREAM stream;
{
    char *buf, temp;
    long dummy;
    
    if (!RPCByteSwapped) {
	if (fwrite((char *) &value, sizeof(long), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC Error: fwrite of long failed\n");
	    return RPC_FALSE;
	} else {
	    return RPC_TRUE;
	}
    } else {
	dummy = value;
	buf = (char *) &dummy;
	temp = buf[0];
	buf[0] = buf[3];
	buf[3] = temp;
	temp = buf[1];
	buf[1] = buf[2];
	buf[2] = temp;
	if (fwrite((char *) &dummy, sizeof(long), (int) 1, stream) != 1) {
	    (void) fprintf(stderr, "RPC Error: fwrite of long failed\n");
	    return RPC_FALSE;
	} else {
	    return RPC_TRUE;
	}
    }
}


rpcInternalStatus
RPCReceiveByte(value, stream)
char *value;
STREAM stream;
{
    return fread(value, sizeof(char), (int) 1, stream) == 1;
}

rpcInternalStatus
RPCSendByte(value, stream)
char value;
STREAM stream;
{
    return fwrite((char *) &value, sizeof(char), (int) 1, stream) == 1;
}


char *
RPCstrsave(string)
char *string;
{
    char *p;

    p = RPCARRAYALLOC(char, (strlen(string) + 1));
    (void) strcpy(p, string);
    return p;
}


/*
 * receive the stripped down set of remote command arguments from VEM
 *
 * Arguments:
 *   spot	where the command was invoked (the facet id)
 *   argList	a list of arguments (points, boxes, lines, text, bags)
 *   userOptionWord	flags
 *
 * Returns:
 *   RPC_OK
 *   RPC_ERROR
 *
 */
rpcStatus
RPCReceiveVemArgs(spot, list, userOptionWord, stream)
RPCSpot *spot;
lsList *list;
long *userOptionWord;
STREAM stream;
{
    long count;			/* number of args to be sent */
    lsList tlist;		/* temporary list */
    RPCArg *arg;
    int i;

    /*
     * get the spot
     */

    RPCASSERT(RPCReceiveSpot(spot, stream));
    RPCASSERT(RPCReceiveLong(&count, stream));

    tlist = lsCreate();			/* create a temp list */
    for (i = 0; i < count; i++) {
	arg = RPCALLOC(struct RPCArg);
	RPCASSERT(RPCReceiveVemArg(arg, stream));
	lsNewEnd(tlist, (lsGeneric) arg, LS_NH);
    }
    *list = tlist;

    RPCASSERT(RPCReceiveLong(userOptionWord, stream));

    return RPC_OK;
}

/*
 * receive the spot
 */
rpcInternalStatus
RPCReceiveSpot(spot, stream)
RPCSpot *spot;
STREAM stream;
{
    long dummy;

    RPCASSERT(RPCReceiveLong(&dummy, stream));
    spot->theWin = dummy;
    RPCASSERT(RPCReceiveLong(&dummy, stream));
    spot->facet = (octId) dummy;
    RPCASSERT(RPCReceiveOctPoint(&spot->thePoint, stream));
    return RPC_TRUE;
}

/*
 * receive a single argument (which may be a list of arguments)
 */
rpcStatus
RPCReceiveVemArg(arg, stream)
RPCArg *arg;
STREAM stream;
{
    int i;
    long dummy;
    
    /* receive the type */
    RPCASSERT(RPCReceiveLong(&dummy, stream));
    arg->argType = (int) dummy;

    /*
     * receive the arg data
     *   only receive the length for the data items that need it
     */
    switch (arg->argType) {
	case VEM_POINT_ARG:		/* one or more points */
	    /* receive the length */
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    arg->length = (int) dummy;
	    
	    arg->argData.points = RPCARRAYALLOC(struct RPCPointArg, arg->length);
	    for (i = 0; i < arg->length; i++) {
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i].theWin = dummy;
		
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i].facet = (octId) dummy;
		
		RPCASSERT(RPCReceiveOctPoint(&arg->argData.points[i].thePoint, stream));
	    }
	    break;

	case VEM_BOX_ARG:		/* one or more boxes */
	    /* receive the length */
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    arg->length = (int) dummy;
	    
	    arg->argData.points = RPCARRAYALLOC(struct RPCPointArg, (arg->length * 2));
	    for (i = 0; i < arg->length * 2; i += 2) {
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i].theWin = dummy;
		
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i].facet = (octId) dummy;
		
		RPCASSERT(RPCReceiveOctPoint(&arg->argData.points[i].thePoint, stream));
		
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i+1].theWin = dummy;
		
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i+1].facet = (octId) dummy;
		
		RPCASSERT(RPCReceiveOctPoint(&arg->argData.points[i + 1].thePoint, stream));
	    }
	    break;

	case VEM_LINE_ARG:		/* one or more single segment lines */
	    /* receive the length */
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    arg->length = (int) dummy;
	    arg->argData.points = RPCARRAYALLOC(struct RPCPointArg, arg->length);
	    for (i = 0; i < arg->length; i++) {
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i].theWin = dummy;
		
		RPCASSERT(RPCReceiveLong(&dummy, stream));
		arg->argData.points[i].facet = (octId) dummy;
		
		RPCASSERT(RPCReceiveOctPoint(&arg->argData.points[i].thePoint, stream));
	    }
	    break;

	case VEM_TEXT_ARG:		/* text string */
	    RPCASSERT(RPCReceiveString(&arg->argData.string, stream));
	    break;

	case VEM_OBJ_ARG:		/* oct bag object id */
	    /* receive the length */
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    arg->length = (int) dummy;
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    arg->argData.objArg.facet = (octId) dummy;
	    
	    RPCASSERT(RPCReceiveLong(&dummy, stream));
	    arg->argData.objArg.theBag = (octId) dummy;
	    
	    RPCASSERT(RPCReceiveOctTransform(&arg->argData.objArg.tran, stream));
	    break;

	default:
	    (void) fprintf(stderr, "RPC Error: bad vem arg type");
	    return RPC_ERROR;
    }

    return RPC_OK;
}


/*
 * send the vem args - spot, argList - to the remote function
 *
 * Arguments:
 *   application - number of the application
 *                 integer from 0 to RPCMaxApplications - 1
 *   spot - vem spot
 *   argList - vem arglist
 *   userOptionWord - flag word (help, undo, etc)
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 */

rpcStatus
RPCSendVemArgs(spot, argList, userOptionWord, stream)
RPCSpot *spot;
lsList argList;
long userOptionWord;
STREAM stream;
{
    lsGen listGen;
    Pointer arg;

    /* send spot */
    RPCASSERT(RPCSendSpot(spot, stream));

    /* send count */
    RPCASSERT(RPCSendLong((long) lsLength(argList), stream));

    /* send argList */
    listGen = lsStart(argList);
    while (lsNext(listGen, &arg, (lsHandle *) 0) == LS_OK) {
	RPCASSERT(RPCSendVemArg((RPCArg *) arg, stream));
    }
    (void) lsFinish(listGen);

    RPCASSERT(RPCSendLong(userOptionWord, stream));

    return RPC_OK;
}

/*
 * send the remote spot - a stripped down version of the tightly
 * bound one
 *
 * Arguments:
 *   spot - vem spot
 *   stream - stream to send the spot on
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 */
rpcStatus
RPCSendSpot(spot, stream)
RPCSpot *spot;
STREAM stream;
{
    RPCASSERT(RPCSendLong((long) spot->theWin, stream));
    RPCASSERT(RPCSendLong((long) spot->facet, stream));
    RPCASSERT(RPCSendOctPoint(&(spot->thePoint), stream));
    return RPC_OK;
}

/*
 * send a single argument from the argList (could be a list itself)
 *
 * Arguments:
 *   arg - vem arg
 *   stream - stream to send the spot on
 *
 * Returns:
 *   RPC_OK    - everything succeeded
 *   RPC_ERROR - something went wrong
 */
rpcStatus
RPCSendVemArg(arg, stream)
RPCArg *arg;
STREAM stream;
{
    int i;
    long length;

    /* send the type */
    RPCASSERT(RPCSendLong((long) arg->argType, stream));

    /*
     * send the arg data
     *   only send the length for the data items that need it
     */
    switch (arg->argType) {
	case VEM_POINT_ARG:
	    /* send the length */
	    length = (long) arg->length;
	    RPCASSERT(RPCSendLong(length, stream));
	    for (i = 0; i < length; i++) {
		RPCASSERT(RPCSendLong((long) arg->argData.points[i].theWin, stream));
		RPCASSERT(RPCSendLong((long) arg->argData.points[i].facet, stream));
		RPCASSERT(RPCSendOctPoint(&(arg->argData.points[i].thePoint), stream));
	    }
	    break;

	case VEM_BOX_ARG:
	    /* send the length */
	    length = (long) arg->length;
	    RPCASSERT(RPCSendLong(length, stream));
	    for (i = 0; i < length * 2; i += 2) {
		RPCASSERT(RPCSendLong((long) arg->argData.points[i].theWin, stream));
		RPCASSERT(RPCSendLong((long) arg->argData.points[i].facet, stream));
		RPCASSERT(RPCSendOctPoint(&(arg->argData.points[i].thePoint), stream));
		RPCASSERT(RPCSendLong((long) arg->argData.points[i + 1].theWin, stream));
		RPCASSERT(RPCSendLong((long) arg->argData.points[i + 1].facet, stream));
		RPCASSERT(RPCSendOctPoint(&(arg->argData.points[i + 1].thePoint), stream));
	    }
	    break;

	case VEM_LINE_ARG:
	    /* send the length */
	    length = (long) arg->length;
	    RPCASSERT(RPCSendLong(length, stream));
	    for (i = 0; i < length; i++) {
		RPCASSERT(RPCSendLong((long) arg->argData.points[i].theWin, stream));
		RPCASSERT(RPCSendLong((long) arg->argData.points[i].facet, stream));
		RPCASSERT(RPCSendOctPoint(&(arg->argData.points[i].thePoint), stream));
	    }
	    break;

	case VEM_TEXT_ARG:
	    RPCASSERT(RPCSendString(arg->argData.string, stream));
	    break;

	case VEM_OBJ_ARG:
	    /* send the length */
	    length = (long) arg->length;
	    RPCASSERT(RPCSendLong(length, stream));
	    RPCASSERT(RPCSendLong((long) arg->argData.objArg.facet, stream));
	    RPCASSERT(RPCSendLong((long) arg->argData.objArg.theBag, stream));
	    RPCASSERT(RPCSendOctTransform(&arg->argData.objArg.tran, stream));
	    break;

	default:
	    (void) fprintf(stderr, "RPC Error: bad vem arg type");
	    return RPC_ERROR;
    }

    return RPC_OK;
}


/*
 *	C library - ascii to floating
 */

double	twoe56	= 72057594037927936.; /*2^56*/
double	exp5[]	= {5.,25.,625.,390625.,152587890625.,23232710361480712890625.};

double
RPCatof(p)
register char *p;
{
	extern double ldexp();
	register c, exp = 0, eexp = 0;
	double fl = 0, flexp = 1.0;
	int bexp, neg = 1, negexp = 1;

	while((c = *p++) == ' ');
	if (c == '-') neg = -1;	else if (c == '+'); else --p;

	while ((c = *p++), isdigit(c))
		if (fl < twoe56) fl = 10*fl + (c-'0'); else exp++;
	if (c == '.')
	while ((c = *p++), isdigit(c))
		if (fl < twoe56)
		{
			fl = 10*fl + (c-'0');
			exp--;
		}
	if ((c == 'E') || (c == 'e'))
	{
		if ((c= *p++) == '+'); else if (c=='-') negexp = -1; else --p;
		while ((c = *p++), isdigit(c)) eexp = 10*eexp + (c-'0');
		if (negexp < 0) eexp = -eexp; exp += eexp;
	}
	bexp = exp;
	if (exp < 0) exp = -exp;

	for (c = 0; c < 6; c++)
	{
		if (exp & 01) flexp *= exp5[c];
		exp >>= 1; if (exp == 0) break;
	}

	if (bexp < 0) fl /= flexp; else fl *= flexp;
	fl = ldexp(fl, bexp);
	if (neg < 0) return -fl; else return fl;
}


#ifdef NOWAIT
/*
 * a version of fread that isn't buffers, but won't error on a partial
 * read
 */

int
RPCfread(ptr, size, count, stream)
register char *ptr;
int size;
int count;
FILE *stream;
{
    register int fd = fileno(stream);
    register int num_read, len = size * count;

    do {
	num_read = read(fd, ptr, len);
	if (num_read <= 0) {
	    if (num_read == 0) {
		(void) fprintf(stderr, "RPCfread: read returned 0!\n");
	    }
	    return (size * count - len) / size;
	}
	len -= num_read;
    } while (len);

    return count;
}
#endif

