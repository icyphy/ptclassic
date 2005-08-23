/* Version Identification:
 * @(#)oct.h	1.6	09/22/98
 */
/*
Copyright (c) 1990-1998 The Regents of the University of California.
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
#ifndef OCT_H
#define OCT_H

#define OCT_LEVEL 2

#ifndef OCT_1_NONCOMPAT
#ifndef OCT_1_COMPAT
#define OCT_1_COMPAT
#endif /* OCT_1_COMPAT */
#endif /* OCT_1_NONCOMPAT */

#ifndef PORT_H
#include "port.h"
#endif /* PORT_H */

#ifndef ANSI_H
#include "ansi.h"
#endif /* ANSI_H */

#ifndef FILE
#include <stdio.h>		/* Because some functions require FILE* args. */
#endif

#define OCT_PKG_NAME "oct"

typedef int32 octCoord; 
#define OCT_MAX_COORD LONG_MAX
#define OCT_MIN_COORD -(OCT_MAX_COORD)

#ifdef BIGID
typedef struct {
	int32 idmsb;
	int32 idlsb;
} octId;

extern octId oct_null_id;
#else
typedef int32 octId;
#define oct_null_id ((octId) 0)
#endif
#define OCT_NULL_ID oct_null_id

#define OCT_CURRENT_VERSION ""

#define OCT_INCONSISTENT_BAG "*OCT_INCONSISTENT*"
#define OCT_CHILD	0x01
#define OCT_SIBLING	0x02

#define octGenerate(generator,object) \
    (*(generator)->generator_func)((generator),(object))

    /* to be expanded as needed */

typedef int octStatus;
#define OCT_INCONSISTENT 8
#define OCT_ALREADY_ATTACHED 7
#define OCT_NOT_ATTACHED 6
#define OCT_ALREADY_OPEN 5
#define OCT_GEN_DONE  4
#define OCT_OLD_FACET 3
#define OCT_NEW_FACET 2
#define OCT_OK 1
#define OCT_ERROR -1
#define OCT_NO_EXIST -2
#define OCT_NO_PERM -3
#define OCT_CORRUPTED_OBJECT -4		/* errRaise - done */
#define OCT_NOT_FOUND -5
#define OCT_TOO_SMALL -6
#define OCT_ILL_OP -7			/* errRaise - done */
#define OCT_NO_BB -8

/* modes for octOpenFacet */

#define OCT_READ 1
#define OCT_OVER_WRITE 2
#define OCT_APPEND 3
#define OCT_REVERT 4

enum octPropType {
    OCT_NULL=0, OCT_INTEGER, OCT_REAL, OCT_STRING, OCT_ID, OCT_STRANGER,
    OCT_REAL_ARRAY, OCT_INTEGER_ARRAY
};
typedef enum octPropType octPropType;

struct octUserValue {
    int32 length;
    char *contents;
};

struct octProp {
    char *name;
    octPropType type;
    union octValue {
	int32 integer;
	double real;
	char *string;
	octId id;
	struct {
	    int32 length;
	    int32 *array;
	} integer_array;
	struct {
	    int32 length;
	    double *array;
	} real_array;
	struct octUserValue stranger;
    } value;
};

struct octPoint {
    octCoord x;
    octCoord y;
};

struct octEdge {
    struct octPoint start;
    struct octPoint end;
};

struct octBox {
    struct octPoint lowerLeft;
    struct octPoint upperRight;
};

struct octPolygon {
    int unused;    /* So that it is not an empty structure. */
};

struct octCircle {
    octCoord startingAngle;
    octCoord endingAngle;
    octCoord innerRadius;
    octCoord outerRadius;
    struct octPoint center;
};

struct octPath {
    octCoord width;
};

/* for vertical justification */
#define OCT_JUST_BOTTOM	0
#define OCT_JUST_CENTER	1
#define OCT_JUST_TOP	2

/* for horizontal justification -- also uses OCT_JUST_CENTER */
#define OCT_JUST_LEFT	0
#define OCT_JUST_RIGHT	2

struct octLabel {
    char *label;
    struct octBox region;
    octCoord textHeight;
    unsigned vertJust : 2;	/* vert position of text block in region */
    unsigned horizJust : 2;	/* horiz position of text block in region */
    unsigned lineJust : 2;	/* horiz position of each line in text block */
};

enum octTransformType {
    OCT_NO_TRANSFORM = 0,
    OCT_MIRROR_X = 1,
    OCT_MIRROR_Y = 2,
    OCT_ROT90 = 3,
    OCT_ROT180 = 4,
    OCT_ROT270 = 5,
    OCT_MX_ROT90 = 6,
    OCT_MY_ROT90 = 7,
    OCT_FULL_TRANSFORM = 8
};

typedef enum octTransformType octTransformType;

struct octTransform {
    struct octPoint translation;
    octTransformType transformType;
    double generalTransform[2][2];
};

struct octInstance {
    char *name;
    char *master;
    char *view;
    char *facet;
    char *version;
    struct octTransform transform;
};

struct octNet {
    char *name;
    int32 width;
};

struct octTerm  {
    char *name;
    octId instanceId;
    int32 formalExternalId;
    int32 width;
};

struct octBag {
    char *name;
};

struct octLayer {
    char *name;
};

typedef int32 octObjectMask;
typedef int32 octFunctionMask;

struct octChangeList {
    octObjectMask objectMask;
    octFunctionMask functionMask;
};

struct octChangeRecord {
    int32 changeType;
    int32 objectExternalId;
    int32 contentsExternalId;
};

#define OCT_UNDEFINED_OBJECT 0
#define OCT_FACET 1
#define OCT_TERM 2
#define OCT_NET 3
#define OCT_INSTANCE 4
#define OCT_POLYGON 5
#define OCT_BOX 6
#define OCT_CIRCLE 7
#define OCT_PATH 8
#define OCT_LABEL 9
#define OCT_PROP 10
#define OCT_BAG 11
#define OCT_LAYER 12
#define OCT_POINT 13
#define OCT_EDGE 14
#define OCT_FORMAL 15
#define OCT_MASTER 16
#define OCT_CHANGE_LIST 17
#define OCT_CHANGE_RECORD 18
#define OCT_MAX_TYPE 18

#define OCT_FACET_MASK ((octObjectMask) (1<<OCT_FACET))
#define OCT_TERM_MASK ((octObjectMask) (1<<OCT_TERM))
#define OCT_NET_MASK ((octObjectMask) (1<<OCT_NET))
#define OCT_INSTANCE_MASK ((octObjectMask) (1<<OCT_INSTANCE))
#define OCT_PROP_MASK ((octObjectMask) (1<<OCT_PROP))
#define OCT_BAG_MASK ((octObjectMask) (1<<OCT_BAG))
#define OCT_POLYGON_MASK ((octObjectMask) (1<<OCT_POLYGON))
#define OCT_BOX_MASK ((octObjectMask) (1<<OCT_BOX))
#define OCT_CIRCLE_MASK ((octObjectMask) (1<<OCT_CIRCLE))
#define OCT_PATH_MASK ((octObjectMask) (1<<OCT_PATH))
#define OCT_LABEL_MASK ((octObjectMask) (1<<OCT_LABEL))
#define OCT_LAYER_MASK ((octObjectMask) (1<<OCT_LAYER))
#define OCT_POINT_MASK ((octObjectMask) (1<<OCT_POINT))
#define OCT_EDGE_MASK ((octObjectMask) (1<<OCT_EDGE))
#define OCT_FORMAL_MASK ((octObjectMask) (1<<OCT_FORMAL))
#define OCT_CHANGE_LIST_MASK ((octObjectMask) (1<<OCT_CHANGE_LIST))
#define OCT_CHANGE_RECORD_MASK ((octObjectMask) (1<<OCT_CHANGE_RECORD))

#define OCT_MARK -1
#define OCT_CREATE 1
#define OCT_DELETE 2
#define OCT_MODIFY 3
#define OCT_PUT_POINTS 4
#define OCT_ATTACH_CONTENT 5
#define OCT_ATTACH_CONTAINER 6
#define OCT_DETACH_CONTENT 7
#define OCT_DETACH_CONTAINER 8
#define OCT_MARKER 9

#define OCT_CREATE_MASK ((octFunctionMask) (1<<OCT_CREATE))
#define OCT_DELETE_MASK ((octFunctionMask) (1<<OCT_DELETE))
#define OCT_MODIFY_MASK ((octFunctionMask) (1<<OCT_MODIFY))
#define OCT_PUT_POINTS_MASK ((octFunctionMask) (1<<OCT_PUT_POINTS))
#define OCT_DETACH_CONTENT_MASK ((octFunctionMask) (1<<OCT_DETACH_CONTENT))
#define OCT_DETACH_CONTAINER_MASK ((octFunctionMask) (1<<OCT_DETACH_CONTAINER))
#define OCT_ATTACH_CONTENT_MASK ((octFunctionMask) (1<<OCT_ATTACH_CONTENT))
#define OCT_ATTACH_CONTAINER_MASK ((octFunctionMask) (1<<OCT_ATTACH_CONTAINER))
#define OCT_ATTACH_MASK ((octFunctionMask)(OCT_ATTACH_CONTENT_MASK|OCT_ATTACH_CONTAINER_MASK))
#define OCT_DETACH_MASK ((octFunctionMask)(OCT_DETACH_CONTENT_MASK|OCT_DETACH_CONTAINER_MASK))
#define OCT_MARKER_MASK ((octFunctionMask) (1<<OCT_MARKER))
#define OCT_ALL_FUNCTION_MASK ((octFunctionMask)\
  (OCT_CREATE_MASK|OCT_DELETE_MASK|OCT_MODIFY_MASK|OCT_ATTACH_MASK|OCT_DETACH_MASK|OCT_PUT_POINTS_MASK))

#define OCT_GEO_MASK ((octObjectMask)\
  (OCT_POLYGON_MASK|OCT_CIRCLE_MASK|OCT_PATH_MASK|OCT_LABEL_MASK|OCT_BOX_MASK|\
   OCT_POINT_MASK|OCT_EDGE_MASK))

#define OCT_ALL_MASK ((octObjectMask)\
  (OCT_GEO_MASK|OCT_TERM_MASK|OCT_NET_MASK|OCT_INSTANCE_MASK|OCT_PROP_MASK|\
   OCT_LAYER_MASK|OCT_BAG_MASK|OCT_FACET_MASK|OCT_FORMAL_MASK|OCT_CHANGE_LIST_MASK|\
   OCT_CHANGE_RECORD_MASK))
   
struct octFacet {
    char *cell;
    char *view;
    char *facet;
    char *version;
    char *mode;
};

struct octFacetInfo {
    int32 timeStamp;
    int32 bbDate;
    int32 formalDate;
    int32 createDate;
    int32 deleteDate;
    int32 modifyDate;
    int32 attachDate;
    int32 detachDate;
    int numObjects;
    struct octBox bbox;
    char *fullName;
};

struct octObject {
    int type;
#if defined(PTALPHA) && defined(__GNUC__)
    /* Set up alignment for 64 bit archs.  If objectId is not aligned
       on 8 bytes on the Alpha, then vem will produce lots of
       alignment faults.  The problem is that bufChanges() has a cast
       that passes the objectId to st_gen().
     */
    octId objectId __attribute__((aligned(8)));
#else
    octId objectId;
#endif
    union {
	struct octFacet facet;
	struct octInstance instance;
	struct octProp prop;
	struct octTerm term;
	struct octNet net;
	struct octBox box;
	struct octPolygon polygon;
	struct octCircle circle;
	struct octPath path;
	struct octLabel label;
	struct octBag bag;
	struct octLayer layer;
	struct octPoint point;
	struct octEdge edge;
	struct octChangeRecord changeRecord;
	struct octChangeList changeList;
    } contents;
};


struct octGenerator {
    octStatus (*generator_func)
	ARGS((struct octGenerator *generator, struct octObject *object));
    char *state;
    octStatus (*free_func)
	ARGS((struct octGenerator *generator));
};

typedef struct octGenerator octGenerator;

typedef struct octObject octObject;
typedef struct octBox octBox;
typedef struct octPolygon octPolygon; /* NEW (Aug 7 1991)*/
typedef struct octPoint octPoint;
typedef struct octTransform octTransform;

#ifdef OCT_1_COMPAT
#define octOpenMaster compatOctOpenMaster
#endif

OCT_EXTERN void octBegin
	NULLARGS;
OCT_EXTERN void octEnd
	NULLARGS;
OCT_EXTERN void octError
	ARGS((char *));
OCT_EXTERN char *octErrorString
	NULLARGS;

OCT_EXTERN octStatus octBB
	ARGS((octObject *object, octBox *box));
OCT_EXTERN octStatus octOpenFacet
	ARGS((octObject *facet));
OCT_EXTERN octStatus octOpenMaster
	ARGS((octObject *instance, octObject *master));
OCT_EXTERN octStatus octWriteFacet
	ARGS((octObject *newf, octObject *old));
OCT_EXTERN octStatus octCopyFacet
	ARGS((octObject *newf, octObject *old));
OCT_EXTERN octStatus octCloseFacet
	ARGS((octObject *facet));
OCT_EXTERN octStatus octFlushFacet
	ARGS((octObject *facet));
OCT_EXTERN octStatus octFreeFacet
	ARGS((octObject *facet));
OCT_EXTERN octStatus octCreateOrModify
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octGetById
	ARGS((octObject *object));
OCT_EXTERN octStatus octGetByName
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octGetContainerByName
	ARGS((octObject *container, octObject *object));
OCT_EXTERN void octGetFacet
	ARGS((octObject *object, octObject *facet));
OCT_EXTERN octStatus octGetOrCreate
	ARGS((octObject *container, octObject *object));

OCT_EXTERN octStatus octGetPoints
	ARGS((octObject *object, int32 *num_points, struct octPoint *points));
OCT_EXTERN octStatus octPutPoints
	ARGS((octObject *object, int32 num_points, struct octPoint *points));

OCT_EXTERN octStatus octScaleAndModifyGeo
	ARGS((octObject *object, double scale));
OCT_EXTERN void octScaleGeo
	ARGS((octObject *object, double scale));
OCT_EXTERN octStatus octTransformAndModifyGeo
	ARGS((octObject *object, struct octTransform *transform));
OCT_EXTERN void octTransformGeo
	ARGS((octObject *object, struct octTransform *transform));
OCT_EXTERN void octTransformPoints
	ARGS((int num_points, struct octPoint *points, struct octTransform *transform));

OCT_EXTERN octStatus octPrintObject
	ARGS((FILE *outfile, octObject *object, int));

OCT_EXTERN octStatus octUnDetach
	ARGS((struct octObject *container, struct octObject *object));
OCT_EXTERN octStatus octAttach
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octAttachOnce
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octUnattach
	ARGS((struct octObject *container, struct octObject *object));
OCT_EXTERN octStatus octDetach
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octIsAttached
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octCreate
	ARGS((octObject *container, octObject *object));
OCT_EXTERN octStatus octUnCreate
	ARGS((octObject *object));
OCT_EXTERN int oct_do_undelete
	ARGS((octObject *container, octObject *object, int32 old_xid,
	      int32 old_id));
OCT_EXTERN octStatus octDelete
	ARGS((octObject *object));
OCT_EXTERN octStatus octModify
	ARGS((octObject *object));
OCT_EXTERN octStatus octUnModify
	ARGS((octObject *object));

OCT_EXTERN octStatus octGenFirstContainer
	ARGS((octObject *object, octObjectMask mask, octObject *container));
OCT_EXTERN octStatus octGenFirstContent
	ARGS((octObject *container, octObjectMask mask, octObject *object));
OCT_EXTERN octStatus octInitGenContainers
	ARGS((octObject *object, octObjectMask mask, octGenerator *gen));
OCT_EXTERN octStatus octInitGenContents
	ARGS((octObject *object, octObjectMask mask, octGenerator *gen));
OCT_EXTERN octStatus octInitGenContentsWithOffset
	ARGS((octObject *obj, octObjectMask mask, octGenerator *gen, octObject *offset));
OCT_EXTERN octStatus octFreeGenerator
	ARGS((octGenerator *gen));

OCT_EXTERN octStatus octGetByExternalId
	ARGS((octObject *container, int32 xid, octObject *object));
OCT_EXTERN void octExternalId
	ARGS((octObject *object, int32 *xid));
OCT_EXTERN octStatus octInitGenContents
	ARGS((octObject *object, octObjectMask mask, octGenerator *generator));
OCT_EXTERN octStatus octInitUserGen
        ARGS((char *user_state,octStatus (*gen_func)(),octStatus (*free_func)(),octGenerator *gen));
OCT_EXTERN octStatus octInitBackwardsGenContents
	ARGS((octObject *object, octGenerator *generator));

OCT_EXTERN void octWriteStats
	ARGS((octObject *object, FILE *file));
OCT_EXTERN octStatus octGetFacetInfo
	ARGS((octObject *object, struct octFacetInfo *info));
OCT_EXTERN int octIsTemporary
	ARGS((octObject *object));
OCT_EXTERN void octMarkTemporary
	ARGS((octObject *object));
OCT_EXTERN void octUnmarkTemporary
	ARGS((octObject *object));
OCT_EXTERN octStatus octOpenRelative
	ARGS((octObject *robj, octObject *fobj, int location));
OCT_EXTERN void octFullName
	ARGS((octObject *facet, char **name));

OCT_EXTERN int octIdsEqual
	ARGS((octId id1, octId id2));
OCT_EXTERN int octIdIsNull
	ARGS((octId id));
OCT_EXTERN char *octFormatId
	ARGS((octId id));

/* for use with the `st' package */

OCT_EXTERN int octIdCmp
	ARGS((const char *id1, const char *id2));
OCT_EXTERN int octIdHash
	ARGS((char *id1, int mod));

/* From change_record.c */
OCT_EXTERN octStatus oct_make_change_record_marker
	ARGS((octId clid));

/* From gen.c */
#endif /* OCT_H */
