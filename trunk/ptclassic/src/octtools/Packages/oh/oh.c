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
#include "errtrap.h"
#include "oct.h"
#include "utility.h"
#include "st.h"
#include "oh.h"
#if defined(MCC_DMS)
#include "wfs.h"
#include "wfd.h"
#endif /* defined(MCC_DMS) */

octStatus ohLastStatus;

/* LINTLIBRARY */

void
ohOctFailed(file, line, why, offending_object)
char *file;
int line;
char *why;
octObject *offending_object;
{
    errRaise(OH_PKG_NAME, ohLastStatus,
	     "Oct assertion failed (%d): file %s at line %d\n%s %s\n%s", 
	     ohLastStatus, file, line, why,
	     (offending_object == NIL(octObject) ? "" : ohFormatName(offending_object)),
	     octErrorString());
}


void
ohCreateSegment(c, t, p1x, p1y, p2x, p2y, width)
octObject *c, *t;
octCoord p1x, p1y, p2x, p2y;
octCoord width;
{
    struct octPoint p[2];

    t->type = OCT_PATH;
    t->contents.path.width = width;
    OH_ASSERT(octCreate(c, t));
    p[0].x = p1x;
    p[0].y = p1y;
    p[1].x = p2x;
    p[1].y = p2y;
    OH_ASSERT(octPutPoints(t, 2, p));
    return;
}


/*
 *  ohBB: Find the bounding box of the object or terminal implementation
 */

octStatus
ohBB(obj, bb)
octObject *obj;
struct octBox *bb;
{
    switch ( obj->type ) {
    case OCT_TERM:
	return ohTerminalBB(obj, bb);
    case OCT_NET:
    case OCT_LAYER:
	{
	    octGenerator gen;
	    octObject o;
	    struct octBox tmpbb;
	    int count = 0;
    
	    octInitGenContents( obj, OCT_GEO_MASK | OCT_TERM_MASK, &gen );

	    bb->lowerLeft.x = 0;
	    bb->lowerLeft.y = 0;
    
	    bb->upperRight.x = 0;
	    bb->upperRight.y = 0;
	    while ( octGenerate( &gen, &o ) == OCT_OK ) {
		if ( ohBB( &o, &tmpbb ) == OCT_NO_BB ) {
		    errRaise( OH_PKG_NAME, 1, "ohBB: %s in %s has no BB.",
			     ohFormatName( &o ), ohFormatName( obj ) );
		}
		if ( count++ == 0 ) {
		    *bb = tmpbb;
		} else {
		    bb->lowerLeft.x = MIN(bb->lowerLeft.x, tmpbb.lowerLeft.x );
		    bb->lowerLeft.y = MIN(bb->lowerLeft.y, tmpbb.lowerLeft.y );

		    bb->upperRight.x = MAX(bb->upperRight.x, tmpbb.upperRight.x );
		    bb->upperRight.y = MAX(bb->upperRight.y, tmpbb.upperRight.y );
		}
	    }
	    return count ? OCT_OK: OCT_NO_BB;
	}
    default:
	return octBB(obj, bb);
    }
}


/*
 *  ohActualTerminalBB: Find the bounding box of an actual terminal
 */

octStatus
ohActualTerminalBB(aterm, bb)
octObject *aterm;
struct octBox *bb;
{
    octObject instance, fterm, box;
    octStatus status;

    /* Get the instance from the terminal */
    status = ohGetById(&instance, aterm->contents.term.instanceId);
    if (status != OCT_OK) return status;

    /* Get the corresponding formal terminal */
    status = ohFindFormal(&fterm, aterm);
    if (status < OCT_OK) return status;

    /* Get the formal terminal's BB */
    status = ohFormalTerminalBB(&fterm, &box.contents.box);
    if (status < OCT_OK) return status;

    /* transform the BB */
    box.type = OCT_BOX;
    octTransformGeo(&box, &instance.contents.instance.transform);
    *bb = box.contents.box;
    return OCT_OK;
}


/*
 *  ohFormalTerminalBB: Find the bounding box of a formal terminal
 */

#define MIN_ASSIGN(dst, src)	if ((src) < (dst)) (dst) = (src)
#define MAX_ASSIGN(dst, src)	if ((src) > (dst)) (dst) = (src)

octStatus
ohFormalTerminalBB(fterm, bb)
octObject *fterm;
struct octBox *bb;
{
    octStatus status;
    octGenerator implGen;
    octObject implementation;
    int count = 0;
    struct octBox tempBB;

    /* Run through all the implementing terminals and geometries */
    status = octInitGenContents(fterm, OCT_TERM_MASK|OCT_GEO_MASK, &implGen);
    if (status != OCT_OK) return status;
    while ((status = octGenerate(&implGen, &implementation)) == OCT_OK) {
	/* Find the BB of this implementation */
	if (implementation.type == OCT_TERM) {
	    status = ohTerminalBB(&implementation, &tempBB);
	} else {
	    status = octBB(&implementation, &tempBB);
	}
	if (status == OCT_NO_BB) continue;
	if (status != OCT_OK) return status;

	/* If this is the first implementation, use its BB; else add it in */
	if (count++ == 0) {
	    *bb = tempBB;
	} else {
	    MIN_ASSIGN(bb->lowerLeft.x, tempBB.lowerLeft.x);
	    MIN_ASSIGN(bb->lowerLeft.y, tempBB.lowerLeft.y);
	    MAX_ASSIGN(bb->upperRight.x, tempBB.upperRight.x);
	    MAX_ASSIGN(bb->upperRight.y, tempBB.upperRight.y);
	}
    }
    if (status < OCT_OK) return status;

    if (count == 0) {
	return OCT_NO_BB;
    } else {
	return OCT_OK;
    }
}


/*
 *  ohTerminalBB: Find the bounding box of the implementation of a terminal
 */

octStatus
ohTerminalBB(term, bb)
octObject *term;
struct octBox *bb;
{
    if (octIdIsNull(term->contents.term.instanceId)) {
	return ohFormalTerminalBB(term, bb);
    } else {
	return ohActualTerminalBB(term, bb);
    }
}


/*
 *  ohGetByNameContainer: find the container of an object by name
 *
 *  Returns:
 *	OCT_OK if object was found
 *	OCT_NOT_FOUND if container was not found
 */

octStatus
ohGetByNameContainer(contents, obj, type, container_name)
octObject *contents;
octObject *obj;
int type;
char *container_name;
{
    octObjectMask mask;
    octStatus status;
    octGenerator gen;
    char *name;

    /* Start a generator on the containers */
    mask = 1 << type;
    OH_ASSERT(octInitGenContainers(contents, mask, &gen));

    while ((status = octGenerate(&gen, obj)) == OCT_OK) {
	if ((name = ohGetName(obj)) == NIL(char)) {
	    status = OCT_ERROR;
	} else if (strcmp(name, container_name) == 0) {
	    return OCT_OK;
	}
    }
    if (status != OCT_GEN_DONE) return status;
    return OCT_NOT_FOUND;
}


/*
 *  ohGetName: return the name field of an object, or NIL(char) if the
 *  object has no name.
 */
char *
ohGetName(object)
octObject *object;
{
    char *name;

    switch (object->type) {
    case OCT_TERM: 
	name = object->contents.term.name; 
	break;
    case OCT_NET: 
	name = object->contents.net.name; 
	break;
    case OCT_INSTANCE: 
	name = object->contents.instance.name; 
	break;
    case OCT_PROP: 
	name = object->contents.prop.name; 
	break;
    case OCT_LAYER: 
	name = object->contents.layer.name; 
	break;
    case OCT_BAG: 
	name = object->contents.bag.name; 
	break;
    case OCT_LABEL: 
	name = object->contents.label.label; 
	break;
    default:
	return(NIL(char));
    }

    /* just to be sure... (someone other than oct may have filled this in) */
    if (name == NIL(char)) {
	return "";
    } else {
	return name;
    }
}


/*
 *  ohPutName -- set the name field of an object to 'name'.  Returns 
 *  OCT_ERROR if the object does not have a name field.
 */
octStatus
ohPutName(object, name)
octObject *object;
char *name;
{
    switch (object->type) {
    case OCT_TERM: 
	object->contents.term.name = name; 
	break;
    case OCT_NET: 
	object->contents.net.name = name; 
	break;
    case OCT_INSTANCE: 
	object->contents.instance.name = name; 
	break;
    case OCT_PROP: 
	object->contents.prop.name = name; 
	break;
    case OCT_LAYER: 
	object->contents.layer.name = name; 
	break;
    case OCT_BAG: 
	object->contents.bag.name = name; 
	break;
    case OCT_LABEL: 
	object->contents.label.label = name; 
	break;
    default:
	return OCT_ERROR;
    }
    return OCT_OK;
}


/*
 * ohTypeName -- return the name of an Oct type
 */
char *
ohTypeName(object)
octObject *object;
{
    switch(object->type) {
    case OCT_BAG: return "OCT_BAG";
    case OCT_BOX: return "OCT_BOX";
    case OCT_CIRCLE: return "OCT_CIRCLE";
    case OCT_EDGE: return "OCT_EDGE";
    case OCT_FACET: return "OCT_FACET";
    case OCT_INSTANCE: return "OCT_INSTANCE";
    case OCT_LABEL: return "OCT_LABEL";
    case OCT_LAYER: return "OCT_LAYER";
    case OCT_NET: return "OCT_NET";
    case OCT_PATH: return "OCT_PATH";
    case OCT_POINT: return "OCT_POINT";
    case OCT_POLYGON: return "OCT_POLYGON";
    case OCT_PROP: return "OCT_PROP";
    case OCT_TERM: return "OCT_TERM";
    case OCT_CHANGE_LIST: return "OCT_CHANGE_LIST";
    case OCT_CHANGE_RECORD: return "OCT_CHANGE_RECORD";
    default:
	return "unknown-object-type";
    }
}

/*
 *  ohFormatName -- provide a reasonable 'name' description of an
 *  object, suitable for error messages.
 */
char *
ohFormatName(object)
octObject *object;
{
    static char s[8][1024];
    static int count = 0;
    octStatus status;
    octObject inst;

    if (++count > 7) {
	count = 0;
    }

    switch(object->type) {
	case OCT_BAG:
	case OCT_LABEL:
	case OCT_LAYER:
	case OCT_NET:
	    (void) sprintf(s[count], "%s %s", ohTypeName(object), ohGetName(object));
	    break;
	case OCT_PROP:
	    switch ( object->contents.prop.type ) {
	    case OCT_STRING:
		(void)sprintf(s[count], "%s=\"%s\"",ohGetName(object),object->contents.prop.value.string); break;
	    case OCT_REAL:
		{
		    double v = object->contents.prop.value.real;
		    if ( ABS(v) < 1e-4 || ABS(v) > 1e8 ) {
			(void)sprintf(s[count], "%s=%.4e",ohGetName(object),v );
		    } else {
			(void)sprintf(s[count], "%s=%.4f",ohGetName(object),v );
		    }
		}
		break;
	    case OCT_INTEGER:
		(void)sprintf(s[count], "%s=%ld",ohGetName(object),object->contents.prop.value.integer); break;
	    default:
		(void) sprintf(s[count], "%s %s", ohTypeName(object), ohGetName(object));
		break;
	    }
	    break;
	case OCT_TERM:
	    if (octIdIsNull(object->contents.term.instanceId)) {
		(void) sprintf(s[count], "formal terminal \"%s\"",
		    object->contents.term.name);
	    } else {
		status = ohGetById(&inst, object->contents.term.instanceId);
		(void) sprintf(s[count], "actual terminal \"%s\" of instance \"%s\"",
		    object->contents.term.name, 
		    status==OCT_OK ? inst.contents.instance.name : "--ERROR--");
	    }
	    break;
		    
	case OCT_INSTANCE:
	    (void) sprintf(s[count], "instance \"%s\" of \"%s:%s:%s;%s\"",
		object->contents.instance.name, 
		object->contents.instance.master, 
		object->contents.instance.view, 
		object->contents.instance.facet, 
		object->contents.instance.version);
	    break;
	
	case OCT_FACET:
	    (void) sprintf(s[count], "\"%s:%s:%s;%s\"",
		object->contents.facet.cell, object->contents.facet.view,
		object->contents.facet.facet, object->contents.facet.version);
	    break;

	default:
	    (void) strcpy(s[count], ohTypeName(object));
	    break;
    }
    return s[count];
}


void
ohReplaceInstance(newInst, oldInst)
octObject *newInst, *oldInst;
{
    octObject new_term, term, facet;
    octGenerator gen;
    octStatus status;

    octGetFacet(oldInst, &facet);
    OH_ASSERT(octCreate(&facet, newInst));

    /*
     *  Copy attachements for objects attached to the instance
     */
    ohAttachContents(oldInst, newInst, OCT_ALL_MASK &~ OCT_TERM_MASK);
    ohAttachContainers(oldInst, newInst, OCT_ALL_MASK &~ OCT_FACET_MASK);
    /*
     *  Copy all objects attached to each terminal
     */
    OH_ASSERT(octInitGenContents(oldInst, OCT_TERM_MASK, &gen));
    while ((status = octGenerate(&gen, &term)) == OCT_OK) {
	/* Find corresponding terminal on the new cell */
	new_term = term;
	status = octGetByName(newInst, &new_term);
	if (status == OCT_OK) {
	    ohAttachContents(&term, &new_term, OCT_ALL_MASK);
	    ohAttachContainers(&term, &new_term, OCT_ALL_MASK&~OCT_INSTANCE_MASK);
	} else if (status == OCT_NOT_FOUND) {
	    errRaise(OH_PKG_NAME, OCT_ERROR, "Formal/Actual Terminal mismatch: %s",
				  ohFormatName(&term));
	} else {
	    OH_ASSERT(status);
	}
    }
    OH_ASSERT(status);
    OH_ASSERT(octDelete(oldInst));		/* delete the old instance */
    return;
}



void
ohAttachOnceContents(src, dst, mask)
octObject *src;
octObject *dst;
octObjectMask mask;
{
    octGenerator gen;
    octObject obj;
    octStatus status;

    OH_ASSERT(octInitGenContents(src, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	OH_ASSERT(octAttachOnce(dst, &obj));
    }
    OH_ASSERT(status);
    return;
}

void
ohAttachContents(src, dst, mask)
octObject *src;
octObject *dst;
octObjectMask mask;
{
    octGenerator gen;
    octObject obj;
    octStatus status;

    OH_ASSERT(octInitGenContents(src, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	OH_ASSERT(octAttach(dst, &obj));
    }
    OH_ASSERT(status);
    return;
}


void
ohAttachContainers(src, dst, mask)
octObject *src;
octObject *dst;
octObjectMask mask;
{
    octGenerator gen;
    octObject obj;
    octStatus status;

    OH_ASSERT(octInitGenContainers(src, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	OH_ASSERT(octAttach(&obj, dst));
    }
    OH_ASSERT(status);
    return;
}


void
ohAttachOnceContainers(src, dst, mask)
octObject *src;
octObject *dst;
octObjectMask mask;
{
    octGenerator gen;
    octObject obj;
    octStatus status;

    OH_ASSERT(octInitGenContainers(src, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	OH_ASSERT(octAttachOnce(&obj, dst));
    }
    OH_ASSERT(status);
    return;
}


void
ohDeleteContents(container, mask)
octObject *container;
octObjectMask mask;
{
    octGenerator gen;
    octObject obj;
    octStatus status;

    OH_ASSERT(octInitGenContents(container, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	OH_ASSERT(octDelete(&obj));
    }
    OH_ASSERT(status);
    return;
}


void
ohRecursiveDelete(object, mask)
octObject *object;
octObjectMask mask;
{
    octGenerator gen;
    octObject obj;
    octStatus status;

    OH_ASSERT(octInitGenContents(object, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	ohRecursiveDelete(&obj, mask);
    }
    OH_ASSERT(status);
    OH_ASSERT(octDelete(object));
    return;
}


octStatus
ohTerminalNet(fterm, net)
octObject *fterm;
octObject *net;
{
    octStatus status;
    status = octGenFirstContainer(fterm, OCT_NET_MASK, net);
    if( status == OCT_GEN_DONE ) {
	octObject aterm;
	if ( octIdIsNull(fterm->contents.term.instanceId) &&
	    octGenFirstContent( fterm, OCT_TERM_MASK, &aterm ) == OCT_OK ) {
	    return ohTerminalNet( &aterm, net );
	} 
	return OCT_NOT_FOUND;
    }
    return status;
}


octStatus
ohFindFormal(fterm, aterm)
octObject *fterm, *aterm;
{
    octObject inst, master;
    octStatus status;

    if ((status = ohGetById(&inst, aterm->contents.term.instanceId)) != OCT_OK) {
	return status;
    }
    if ((status = ohOpenMaster(&master, &inst, "interface", "r")) < OCT_OK) {
	return status;
    }
#if OCT_LEVEL == 1
    return ohGetByTermName(&master, fterm, aterm->contents.term.name);
#else
    return ohGetTerminal(&master, aterm->contents.term.formalExternalId, fterm);
#endif
}


/*
 *  ohGetPoints -- get the points from a PATH or POLYGON
 *
 *  A pointer to a static structure is returned; please do not free it
 */

void
ohGetPoints(object, num, arr)
octObject *object;
int32 *num;
struct octPoint **arr;
{
    int status;
    int32 real_num_points;
    static int32 num_points = 0;
    static struct octPoint *points;

    if (num_points == 0) {
	num_points = 50;
	points = ALLOC(struct octPoint, num_points);
    }
    
    real_num_points = num_points;
    status = octGetPoints(object, &real_num_points, points);
    if (status != OCT_OK) {
	if (status != OCT_TOO_SMALL) {
	    OH_ASSERT(status);
	}
	num_points = real_num_points;
	points = REALLOC(struct octPoint, points, num_points);
	real_num_points = num_points;
	OH_ASSERT(octGetPoints(object, &real_num_points, points));
    }
    *num = real_num_points;
    *arr = points;
    return;
}


octStatus
ohUnpackFacetName(facet, name)
octObject *facet;
char *name;
{
    char *s;
#if defined(MCC_DMS)
    char *wfn;		/* cell component pointing to dms static */
    int nlen;		/* Full Name length */
    char *cp;		/* Scratch char ptr */
    char *typ;		/* DMS type spec */
#endif

    /* Check for silly input */
    if (name == NIL(char) || strcmp(name, "") == 0) return OCT_ERROR;
    if (strchr(name, ' ') != NIL(char)) return OCT_ERROR;
	
#if defined(MCC_DMS)
    /* MCC Modification: Exapnd path and isolate wpn component with a DMS */
    /* routine because of potential occurrences TYPE spec and inability */
    /* of handling relative path names. */
    if (!(cp = ptcExpandWFPN(name, "r"))) cp = name;
    if (typ = wfpnExtractType(cp)) {
	/* There is a type specification */
	char *p;
	
	/* If it isn't OCT, die. */
	if (strcmp(typ, "OCT")) return OCT_ERROR;
	/* If can't find the comma before OCT, die. */
	if (!(p = strrchr(cp, ','))) return OCT_ERROR;
	*p = '\0';	/* NULL out comma (ie. Type spec at end) */
    }
    nlen = strlen(cp);
    s = memcpy(ALLOC(char,  nlen + 1), cp, nlen);
    s[nlen] = '\0';
#else
    /* Make a copy of the input string (because we will tear it apart) */ 
    s = strcpy(ALLOC(char, strlen(name)+1), name);
#endif
    if (s[0] != ':') facet->contents.facet.cell = s;
    if ((s = strchr(s, ':')) != NIL(char)) {
	*s++ = '\0';
	if (s[0] != ':') facet->contents.facet.view = s;
	if ((s = strchr(s, ':')) != NIL(char)) {
	    *s++ = '\0';
	    if (s[0] != ':') facet->contents.facet.facet = s;
	    if ((s = strchr(s, ':')) != NIL(char)) {
		*s++ = '\0';
		if (strchr(s, ':') != NIL(char)) return OCT_ERROR;
		if (*s == '\0') {
		    facet->contents.facet.version = OCT_CURRENT_VERSION;
		} else {
		    facet->contents.facet.version = s;
		}
	    }
	}
    }

    if (facet->contents.facet.cell == NIL(char) ||
        facet->contents.facet.view == NIL(char) ||
        facet->contents.facet.facet == NIL(char)) {
	FREE(s);
	return OCT_ERROR;
    }        

    return OCT_OK;
}


/*
 * uniquify all objects of a given type (or set of types) that are
 * contained by 'container'
 */

void
ohUniqNames(container, mask)
octObject *container;
octObjectMask mask;
{
    st_table *table = st_init_table(strcmp, st_strhash);
    octGenerator gen;
    octObject object;
    char *name, *dummy;
    char head[1024], newname[1024];
    int count = 0, temp, modifyp;

    mask &= (OCT_TERM_MASK|OCT_NET_MASK|OCT_INSTANCE_MASK|OCT_PROP_MASK|
	     OCT_LAYER_MASK|OCT_BAG_MASK|OCT_LABEL_MASK);

    OH_ASSERT(octInitGenContents(container, mask, &gen));

    while (octGenerate(&gen, &object) == OCT_OK) {
	modifyp = 0;
	name = ohGetName(&object);
	if ((name == NIL(char)) || (*name == '\0')) {
	    (void) sprintf(newname, "oh-%d", count++);
	    name = newname;
	    OH_ASSERT(ohPutName(&object, name));
	    modifyp = 1;
	}
	
	while (st_lookup(table, name, &dummy)) {
	    modifyp = 1;
	    /* found something, need to generate a unique name */
	    if (sscanf(name, "%[^-]-%d", head, &temp) == 2) {
		name = head;
		temp++;
	    } else {
		temp = count++;
	    }
	    (void) sprintf(newname, "%s-%d", name, temp);
	    name = newname;
	    OH_ASSERT(ohPutName(&object, name));
	}
	if (modifyp) {
	    OH_ASSERT(octModify(&object));
	    OH_ASSERT(octGetById(&object));
	}
	/* insert oct's copy of the string into the table */
	(void) st_insert(table, ohGetName(&object), (char *) 0);
    }
    st_free_table(table);
    return;
}

int
ohCountContents(object, mask)
octObject *object;
octObjectMask mask;
{
    octObject obj;
    octGenerator gen;
    octStatus status;
    int count = 0;

    OH_ASSERT(octInitGenContents(object, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	count++;
    }
    OH_ASSERT(status);
    return count;
}

int
ohCountContainers(object, mask)
    octObject *object;
    octObjectMask mask;
{
    octObject obj;
    octGenerator gen;
    octStatus status;
    int count = 0;

    OH_ASSERT(octInitGenContainers(object, mask, &gen));
    while ((status = octGenerate(&gen, &obj)) == OCT_OK) {
	count++;
    }
    OH_ASSERT(status);
    return count;
}

void
ohContentsToArray(object, mask, array, array_n)
octObject *object;
octObjectMask mask;
octId **array;
int *array_n;
{
    octGenerator gen;
    octObject obj;
    octStatus s;
    int size;

    OH_ASSERT(octInitGenContents(object, mask, &gen));
    size = 3;
    *array = ALLOC(octId, size);
    *array_n = 0;

    while ((s = octGenerate(&gen, &obj)) == OCT_OK) {
	if (*array_n >= size) {
	    size *= 2;
	    *array = REALLOC(octId, *array, size);
	}
	(*array)[*array_n] = obj.objectId;
	(*array_n)++;
    }
    OH_ASSERT(s);
    return;
}


/*
 * ohFast routines to make octGetByName faster (use hash tables)
 *
 *   why: octGetByName does a linear search to find an object,
 *   as the number of objects increases, the search times becomes
 *   dominant.  The ohFast functions surround octGetByName and
 *   hash information as it is found/created.
 */

static st_table *ohNetTable = NIL(st_table);

static void
initialize_net_table()
{
    ohNetTable = st_init_table(strcmp, st_strhash);
    return;
}


octStatus
ohFastGetByNetName(facet, net, name)
octObject *facet, *net;
char *name;
{
    char *ptr;
    octStatus status;

    if (!ohNetTable) {
        initialize_net_table();
    }
    
    if (st_lookup(ohNetTable, name, &ptr)) {
	net->objectId = *(octId *) ptr;
	(void) octGetById(net);
	return OCT_OK;
    } else {
	if ((status = ohGetByNetName(facet, net, name)) == OCT_OK) {
            octId *id = ALLOC(octId, 1);
	    *id = net->objectId;
	    (void) st_insert(ohNetTable, name, (char *) id);
	    return OCT_OK;
	}
	return status;
    }
}


octStatus
ohFastCreateNet(facet, net, name)
octObject *facet, *net;
char *name;
{
    octStatus status;
    octId *id;

    if (!ohNetTable) {
        initialize_net_table();
    }

    if (st_lookup(ohNetTable, name, NIL(char *))) {
	errRaise(OH_PKG_NAME, OCT_ERROR,
		 "Trying to create a net with a name that already exists: %s",
		 name);
    }

    if ((status = ohCreateNet(facet, net, name)) != OCT_OK) {
        return status;
    }
    id = ALLOC(octId, 1);
    *id = net->objectId;
    (void) st_insert(ohNetTable, name, (char *) id);
    return OCT_OK;
}


octStatus
ohFastGetOrCreateNet(facet, net, name)
octObject *facet, *net;
char *name;
{
    if (ohFastGetByNetName(facet, net, name) == OCT_OK) {
	return OCT_OK;
    }
    return ohFastCreateNet(facet, net, name);
}


void
ohFastReleaseStorage()
{
    st_free_table(ohNetTable);
    return;
}


#if OCT_LEVEL == 2
octStatus
ohGetTerminal(container, fxid, term)
octObject *container;
int32 fxid;
octObject *term;
{
    octGenerator gen;
    octObject rterm;

    /* 
     * Oct 2 has changed abit, early versions did not force
     * the same formal terminal in all facets to have the same
     * xid
     */

    if ((fxid < 0) 
	&& (container->type == OCT_FACET)
        && (octGetByExternalId(container, fxid, &rterm) == OCT_OK)) {
	*term = rterm;
	return OCT_OK;
    }

    OH_ASSERT(octInitGenContents(container, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &rterm) == OCT_OK) {
	if (rterm.contents.term.formalExternalId == fxid) {
	    *term = rterm;
	    octFreeGenerator(&gen);
	    return OCT_OK;
	}
    }
    return OCT_NOT_FOUND;
}


void
ohPrintInconsistent(facet, stream)
octObject *facet;
FILE *stream;
{
    octObject bag, object;
    octGenerator gen;
    int status;

    (void) fprintf(stream, "\nWarning, the following objects in %s are inconsistent:\n\n",
		   ohFormatName(facet));

    bag.type = OCT_BAG;
    bag.contents.bag.name = OCT_INCONSISTENT_BAG;

    if (octGetByName(facet, &bag) != OCT_OK) {
	octError("Getting the error bag");
	exit(-1);
    }
    if (octInitGenContents(&bag, OCT_ALL_MASK, &gen) != OCT_OK) {
	octError("generating inconsistent objects");
	exit(-1);
    }

    while ((status = octGenerate(&gen, &object)) == OCT_OK) {
	(void) fprintf(stream, "\t%s\n", ohFormatName(&object));
    }
    
    if (status != OCT_GEN_DONE) {
	octError("inconsistent-bag generator");
    }

    (void) fprintf(stream, "\nEnd of inconsistent objects\n\n");

    return;
}


octStatus
ohBestName(facet, inst, givenMaster)
octObject *facet, *inst, *givenMaster;
{
    octObject master;
    char *name, *fname, *ptr;

    if (facet->type != OCT_FACET) {
	errRaise(OH_PKG_NAME, -1, "non-facet passed to ohBestName as the facet argument");
    }
    if (inst->type != OCT_INSTANCE) {
	errRaise(OH_PKG_NAME, -1, "non-instance passed to ohBestName as the instance argument");
    }

    switch (inst->contents.instance.master[0]) {
    case '~':
    case '$':
	/* case '/': */
	return OCT_OK;
    }
    if (givenMaster == NIL(octObject)) {
	if (ohOpenMaster(&master, inst, "contents", "r") < OCT_OK) {
	    octError("can not open the master of the instance");
	    return OCT_ERROR;
	}
	octFullName(&master, &name);
    } else {
	octFullName(givenMaster, &name);
    }

    octFullName(facet, &fname);

    if ((ptr = strrchr(fname, '/')) != NIL(char)) {
	*ptr = '\0';
    }
    if ((ptr = strrchr(name, '/')) != NIL(char)) {
	*ptr = '\0';
    }

    /* fprintf(stderr, "ohBestName:\n\t%s\n\t%s\n", name, fname); */

    if (strncmp(fname, name, strlen(fname)) == 0) {
	if (ptr != NIL(char)) {
	    *ptr = '/';
	}
	inst->contents.instance.master = util_strsav(&name[strlen(fname)+1]);
    } else {
	if (ptr != NIL(char)) {
	    *ptr = '/';
	}
	inst->contents.instance.master = util_strsav(name);
    }

    /* fprintf(stderr, "\t\t%s\n\n", inst->contents.instance.master); */

    FREE(fname);
    FREE(name);

    return OCT_OK;
}
#endif
