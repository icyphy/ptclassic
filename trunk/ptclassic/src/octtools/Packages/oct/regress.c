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
#include "errtrap.h"
#include "st.h"
#include "oct.h"
#ifdef REGION
#include "region.h"
#endif

#ifdef SYSV
#include <unistd.h>
#endif

#if defined(MCC_DMS)
 /* Some changes were required in this regression test because of DMS. */
 /* Most of them to remove the references to directories and replace them */
 /* with workspaces. An example is where "getwd" is called. This was */
 /* replaced with ptcGetWorkingWorkspace, the equivalent call for a DMS */
 /* based object system. */

#include "dms.h"
#endif

static int errorCount;

static st_table *xid_table;

#define LOG(msg) \
(void) fprintf(stderr, "ERROR: %s (%s: %d)\n", msg, __FILE__, __LINE__); octError(""); errorCount++;

#define LOG2(msg) \
(void) fprintf(stderr, "ERROR: %s (%s: %d)\n", msg, __FILE__, __LINE__); errorCount++;

#define OKLOG(msg) (void) fprintf(stderr, "%s\n", msg);

#define NL (void) fprintf(stderr, "\n");

#define TEST(func,value) \
{ \
    octStatus last; \
    char *pkg, *msg; \
    int code; \
    char buffer[256]; \
\
    if (!setjmp(errJmpBuf)) { \
        errIgnPush(); \
        if ((last = func) != value) { \
	    (void) sprintf(buffer, "func returned %d, expected %d", last, value); \
            LOG(buffer); \
	    return; \
        } \
    } \
    errIgnPop(); \
    if (errStatus(&pkg, &code, &msg) && (code != value)) { \
	(void) sprintf(buffer, "func returned %d, expected %d (%s)", code, value, msg); \
        LOG(buffer); \
	return; \
    } \
}

/* Like TEST above, we return 0*/
#define TEST_RET(func,value) \
{ \
    octStatus last; \
    char *pkg, *msg; \
    int code; \
    char buffer[256]; \
\
    if (!setjmp(errJmpBuf)) { \
        errIgnPush(); \
        if ((last = func) != value) { \
	    (void) sprintf(buffer, "func returned %d, expected %d", last, value); \
            LOG(buffer); \
	    return 0; \
        } \
    } \
    errIgnPop(); \
    if (errStatus(&pkg, &code, &msg) && (code != value)) { \
	(void) sprintf(buffer, "func returned %d, expected %d (%s)", code, value, msg); \
        LOG(buffer); \
	return 0; \
    } \
}

#define TEST_OK(func)			TEST(func, OCT_OK)
#define TEST_RET_OK(func)		TEST_RET(func, OCT_OK)
#define TEST_ERROR(func)		TEST(func, OCT_ERROR)
#define TEST_NOT_ATTACHED(func)		TEST(func, OCT_NOT_ATTACHED)
#define TEST_ALREADY_ATTACHED(func)	TEST(func, OCT_ALREADY_ATTACHED)
#define TEST_GEN_DONE(func)		TEST(func, OCT_GEN_DONE)
#define TEST_NOT_FOUND(func)		TEST(func, OCT_NOT_FOUND)
#define TEST_CORRUPTED_OBJECT(func)	TEST(func, OCT_CORRUPTED_OBJECT)
#define TEST_NO_BB(func)		TEST(func, OCT_NO_BB)
#define TEST_TOO_SMALL(func)		TEST(func, OCT_TOO_SMALL)
#define TEST_ILL_OP(func)		TEST(func, OCT_ILL_OP)
#define TEST_NEW_FACET(func)		TEST(func, OCT_NEW_FACET)
#define TEST_OLD_FACET(func)		TEST(func, OCT_OLD_FACET)
#define TEST_ALREADY_OPEN(func)		TEST(func, OCT_ALREADY_OPEN)
#define TEST_NO_EXIST(func)		TEST(func, OCT_NO_EXIST)
#define TEST_NO_PERM(func)		TEST(func, OCT_NO_PERM)
#define TEST_INCONSISTENT(func)		TEST(func, OCT_INCONSISTENT)

#define TEST_ONE(func)			TEST(func, 1)
#define TEST_ZERO(func)			TEST(func, 0)

#define my_strsave(ptr) strcpy((char *) malloc((unsigned) strlen(ptr) + 1), ptr)

#define objectCompare(obj1, obj2, msg) \
{ \
    char buffer[256]; \
\
    if ((obj1)->objectId != (obj2)->objectId) { \
	(void) sprintf(buffer, "object compare failed (by objectId): %s", msg); \
	LOG2(buffer); \
	(void) octPrintObject(stderr, obj1, 0); NL; \
	(void) octPrintObject(stderr, obj2, 0); NL; \
    } \
    if ((obj1)->type != (obj2)->type) { \
	(void) sprintf(buffer, "object compare failed (by type): %s", msg); \
	LOG2(buffer); \
	(void) octPrintObject(stderr, obj1, 0); NL; \
	(void) octPrintObject(stderr, obj2, 0); NL; \
    } \
}


static int
setName(obj, name)
octObject *obj;
char *name;
{
    int named = 0;
    
    switch (obj->type) {
	case OCT_INSTANCE:
	obj->contents.instance.name = name;
	named = 1;
	break;
	
	case OCT_NET:
	obj->contents.net.name = name;
	named = 1;
	break;
	
	case OCT_TERM:
	obj->contents.term.name = name;
	named = 1;
	break;
	
	case OCT_BAG:
	obj->contents.bag.name = name;
	named = 1;
	break;
	
	case OCT_LABEL:
	obj->contents.label.label = name;
	named = 1;
	break;
	
	case OCT_PROP:
	obj->contents.prop.name = name;
	named = 1;
	break;
	
	case OCT_LAYER:
	obj->contents.layer.name = name;
	named = 1;
	break;
    }
    return named;
}


static void
tstAlot(facet, obj1, obj2)
octObject *facet, *obj1, *obj2;
{
    octObject fct, obj;
    octGenerator gen;
    struct octBox bb;
    static struct octPoint points[] = {{0,0}, {10,0}, {10, 10}};
    struct octPoint gpoints[4];
    int32 id, nid, num, named, count, fid;
    char buffer[256];
    extern char *oct_type_names[];
    struct octFacetInfo info, info2;

    TEST_OK(octGetFacetInfo(facet, &info));
    
    /*
     * XXX there is a serious problem with this routine, if
     * anything fails before the deletes, the objects will still
     * be in the facet and thus will cause generation problems...
     *
     * should reopen the facet each time through
     */

    (void) sprintf(buffer, "  checking %s -> %s",
		   oct_type_names[obj1->type],
		   oct_type_names[obj2->type]);
    OKLOG(buffer);

    TEST_OK(octCreate(facet, obj1));
    TEST_OK(octCreate(facet, obj2));

    /*
     * tst temporary and xids
     */
    octMarkTemporary(obj1);
    TEST_ONE(octIsTemporary(obj1));
    octUnmarkTemporary(obj1);
    TEST_ZERO(octIsTemporary(obj1));

    octExternalId(obj1, &id);
    if (st_insert(xid_table, (char *) id, (char *) 0)) {
	LOG2("duplicate xid");
    }
    octExternalId(obj1, &nid);
    if (id != nid) {
	LOG2("two xids for the same object");
    }

    /*
     * tst get by xid
     */
    TEST_OK(octGetByExternalId(facet, id, &obj));
    objectCompare(obj1, &obj, "get by id");

    /*
     * tst bug in facet xid
     */
    octExternalId(facet, &fid);
    TEST_OK(octGetByExternalId(facet, fid, &obj));

    /*
     * test get by id
     */
    obj.objectId = obj1->objectId;
    TEST_OK(octGetById(&obj));
    objectCompare(obj1, &obj, "get by id");

    /*
     * test bounding box
     */
    switch (obj1->type) {
	case OCT_TERM:
	case OCT_NET:
	case OCT_PROP:
	case OCT_BAG:
	case OCT_LAYER:
	case OCT_CHANGE_LIST:
	TEST_NO_BB(octBB(obj1, &bb));
	break;

	case OCT_INSTANCE:
	case OCT_BOX:
	case OCT_CIRCLE:
	case OCT_EDGE:
	case OCT_LABEL:
	TEST_OK(octBB(obj1, &bb));
	if ((bb.lowerLeft.x == bb.upperRight.x) &&
	    (bb.lowerLeft.y == bb.upperRight.y)) {
	    LOG2("bogus bounding box");
	}
	break;

	case OCT_POINT:
	TEST_OK(octBB(obj1, &bb));
	break;
	
	case OCT_POLYGON:
	case OCT_PATH:
	TEST_NO_BB(octBB(obj1, &bb));	/* verify no bb without points */
	TEST_OK(octPutPoints(obj1, 3, points));
	num = 2;	/* verify too small size check */
	TEST_TOO_SMALL(octGetPoints(obj1, &num, gpoints));
	num = 3;	/* verify correct size check */
	TEST_OK(octGetPoints(obj1, &num, gpoints));
	num = 4;	/* verify the size check is not equality */
	TEST_OK(octGetPoints(obj1, &num, gpoints));
	TEST_OK(octBB(obj1, &bb));
	break;

	default:
	LOG2("not handled object type");
    }

    {
	/*
	 * test transform operations
	 */
	struct octTransform trans;

	TEST_OK(octGetById(obj1));		/* get to a good state */
	if (obj1->type != OCT_CHANGE_LIST) {
	    trans.transformType = OCT_ROT90;
	    trans.translation.x = 100;
	    trans.translation.y = 100;
	    octTransformGeo(obj1, &trans);
	    octScaleGeo(obj1, 2.0);
	    TEST_OK(octTransformAndModifyGeo(obj1, &trans));
	    TEST_OK(octScaleAndModifyGeo(obj1, 2.0));
	}
    }
    
    /*
     * test get facet
     */
    octGetFacet(obj1, &fct);
    objectCompare(facet, &fct, "get facet");

    /*
     * test attachment (attach, detach, isattached, attachonce)
     */
    TEST_OK(octAttach(obj1, obj2));
    TEST_OK(octIsAttached(obj1, obj2));
    TEST_ALREADY_ATTACHED(octAttachOnce(obj1, obj2));	/* detect attach */

    TEST_OK(octDetach(obj1, obj2));
    TEST_NOT_ATTACHED(octIsAttached(obj1, obj2));
    TEST_OK(octAttachOnce(obj1, obj2));		/* make sure it can attach */
    TEST_OK(octIsAttached(obj1, obj2));

    /*
     * start generator tests (masks are not tested)
     */
    if (obj1->type != OCT_CHANGE_LIST) {
	
	obj.objectId = 0;
	if (obj1->type == OCT_INSTANCE) {
	    TEST_OK(octGenFirstContent(obj1, OCT_ALL_MASK&~OCT_TERM_MASK, &obj));
	} else {
	    TEST_OK(octGenFirstContent(obj1, OCT_ALL_MASK, &obj));
	}
	if (obj2->objectId != obj.objectId) {
	    LOG2("octGenFirstContent failed");
	}
	obj.objectId = 0;
	TEST_OK(octGenFirstContainer(obj2, OCT_ALL_MASK&~OCT_FACET_MASK, &obj));
	if (obj1->objectId != obj.objectId) {
	    LOG2("octGenFirstContainer failed");
	}

	/*
	 * test terminals on the instance
	 */
	if (obj1->type == OCT_INSTANCE) {
	    TEST_OK(octInitGenContents(obj1, OCT_TERM_MASK, &gen));
	    count = 0;
	    while(octGenerate(&gen, &obj) == OCT_OK) {
		count++;
	    }
	    TEST_OK(octFreeGenerator(&gen));
	    if (count != 6) {
		LOG2("wrong number of terminals");
	    }
	}
	
	obj.objectId = 0;
	if (obj1->type == OCT_INSTANCE) {
	    TEST_OK(octInitGenContents(obj1, OCT_ALL_MASK&~OCT_TERM_MASK, &gen));
	} else {
	    TEST_OK(octInitGenContents(obj1, OCT_ALL_MASK, &gen));
	}
	TEST_OK(octGenerate(&gen, &obj));
	if (obj.objectId != obj2->objectId) {
	    LOG2("octGenerate Contents failed");
	}
	TEST_GEN_DONE(octGenerate(&gen, &obj));
	TEST_GEN_DONE(octGenerate(&gen, &obj));
	TEST_OK(octFreeGenerator(&gen));

	obj.objectId = 0;
	TEST_OK(octInitGenContainers(obj2, OCT_ALL_MASK&~OCT_FACET_MASK, &gen));
	TEST_OK(octGenerate(&gen, &obj));
	if (obj.objectId != obj1->objectId) {
	    LOG("octGenerate Containers failed");
	}
	TEST_GEN_DONE(octGenerate(&gen, &obj));
	TEST_GEN_DONE(octGenerate(&gen, &obj));
    }
    
    /*
     * modification and getbyname tests
     */
    obj = *obj1;
    TEST_OK(octGetById(&obj));		/* get a clean copy */

    named = setName(&obj, my_strsave("new-name"));

    if (obj.type == OCT_CHANGE_LIST) {
	TEST_ERROR(octModify(&obj));
    } else {
	TEST_OK(octModify(&obj));
    }	

    if (named) {
	obj.objectId = oct_null_id;
	TEST_OK(octGetByName(facet, &obj));
	TEST_OK(octGetById(obj1));
	objectCompare(&obj, obj1, "get by name");
	TEST_OK(octGetContainerByName(obj2, &obj));
	objectCompare(&obj, obj1, "get container by name");
    }

    if (setName(&obj, "old-name")) {
	obj.objectId = oct_null_id;
	TEST_NOT_FOUND(octGetByName(facet, &obj));
	TEST_NOT_FOUND(octGetContainerByName(obj2, &obj));
    }

    
    /*
     * bad instance modification test
     */
    if (obj1->type == OCT_INSTANCE) {
	obj = *obj1;

	TEST_OK(octGetById(&obj));		/* get a clean copy */
	obj.contents.instance.name = "willy";
	TEST_OK(octModify(&obj));		/* good modify */

	TEST_OK(octGetById(&obj));		/* get a clean copy */
	obj.contents.instance.view = "bob";
#ifdef OLD_STUFF
	TEST_ERROR(octModify(&obj));		/* bad modify */
#else
	TEST_OK(octModify(&obj));		/* good modify */

	TEST_OK(octGetById(&obj));		/* get a clean copy */
	obj.contents.instance.view = "fredwashere";
	TEST_ERROR(octModify(&obj));		/* bad modify */
	TEST_OK(octGetById(&obj));		/* get a clean copy */
#endif
    }
    
    /*
     * start bogus argument testing
     */
    TEST_OK(octDelete(obj1));
    TEST_OK(octDelete(obj2));

    TEST_CORRUPTED_OBJECT(octAttach(obj1, obj2));
    TEST_CORRUPTED_OBJECT(octBB(obj1, &bb));
    TEST_CORRUPTED_OBJECT(octDelete(obj1));
    TEST_CORRUPTED_OBJECT(octDetach(obj1, obj2));
    TEST_NOT_FOUND(octGetById(obj1));
    TEST_NOT_FOUND(octGetByExternalId(facet, id, &obj));

    TEST_OK(octGetFacetInfo(facet, &info2));

    if (info.timeStamp == info2.timeStamp) {
	LOG2("timeStamp mismatch");
    }
    if (info.createDate == info2.createDate) {
	LOG2("createDate mismatch");
    }
    if (info.deleteDate == info2.deleteDate) {
	LOG2("deleteDate mismatch");
    }
    if (info.modifyDate == info2.modifyDate) {
	LOG2("modifyDate mismatch");
    }
    if (info.attachDate == info2.attachDate) {
	LOG2("attachDate mismatch");
    }
    if (info.detachDate == info2.detachDate) {
	LOG2("detachDate mismatch");
    }

    return;
}


static int
bbCompare(bb1, bb2)
struct octBox *bb1, *bb2;
{
    
    if (((bb1->lowerLeft.x - bb1->upperRight.x) ==
	 (bb2->lowerLeft.x - bb2->upperRight.x)) &&
	((bb1->lowerLeft.y - bb1->upperRight.y) ==
	 (bb2->lowerLeft.y - bb2->upperRight.y))) {
	return 1;
    }
    return 0;
}


static int
countAttachments(obj, mask)
octObject *obj;
octObjectMask mask;
{
    int count = 0;
    octGenerator gen;
    octObject dummy;

    TEST_RET_OK(octInitGenContents(obj, mask, &gen));
    while (octGenerate(&gen, &dummy) == OCT_OK) {
	count++;
    }
    TEST_RET_OK(octFreeGenerator(&gen));
    return count;
}


static int
reset(objs)
octObject objs[];
{
    int i = 0;
    
    objs[i].type = OCT_LAYER;
    objs[i].contents.layer.name = "";
    i++;
    objs[i].type = OCT_LAYER;
    objs[i].contents.layer.name = "";
    i++;

    objs[i].type = OCT_NET;
    objs[i].contents.net.name = "";
    i++;
    objs[i].type = OCT_NET;
    objs[i].contents.net.name = "";
    i++;

    objs[i].type = OCT_BAG;
    objs[i].contents.bag.name = "";
    i++;
    objs[i].type = OCT_BAG;
    objs[i].contents.bag.name = "";
    i++;

    objs[i].type = OCT_PROP;
    objs[i].contents.prop.name = "";
    objs[i].contents.prop.type = OCT_NULL;
    i++;
    objs[i] = objs[i - 1];
    objs[i].contents.prop.name = "";
    i++;

    objs[i].type = OCT_LABEL;
    objs[i].contents.label.label = "label";
    objs[i].contents.label.region.lowerLeft.x = 0;
    objs[i].contents.label.region.lowerLeft.y = 0;
    objs[i].contents.label.region.upperRight.x = 100;
    objs[i].contents.label.region.upperRight.y = 40;
    objs[i].contents.label.textHeight = 10;
    objs[i].contents.label.vertJust = OCT_JUST_CENTER;
    objs[i].contents.label.horizJust = OCT_JUST_LEFT;
    objs[i].contents.label.lineJust = OCT_JUST_RIGHT;
    i++;

    objs[i] = objs[i - 1];
    objs[i].contents.label.label = "label";
    i++;

    objs[i].type = OCT_BOX;
    objs[i].contents.box.lowerLeft.x = 0;
    objs[i].contents.box.lowerLeft.y = 0;
    objs[i].contents.box.upperRight.x = 10;
    objs[i].contents.box.upperRight.y = 10;
    i++;
    objs[i] = objs[i - 1]; i++;

    objs[i].type = OCT_CIRCLE;
    objs[i].contents.circle.startingAngle = 0;
    objs[i].contents.circle.endingAngle = 3600;
    objs[i].contents.circle.innerRadius = 0;
    objs[i].contents.circle.outerRadius = 100;
    objs[i].contents.circle.center.x = 0;
    objs[i].contents.circle.center.y = 0;
    i++;
    objs[i] = objs[i - 1]; i++;

    objs[i].type = OCT_EDGE;
    objs[i].contents.edge.start.x = 100;
    objs[i].contents.edge.start.y = 100;
    objs[i].contents.edge.end.x = 200;
    objs[i].contents.edge.end.y = 200;
    i++;
    objs[i] = objs[i - 1]; i++;

    objs[i].type = OCT_POINT;
    objs[i].contents.point.x = 100;
    objs[i].contents.point.y = 100;
    i++;
    objs[i] = objs[i - 1]; i++;

    objs[i].type = OCT_POLYGON;
    i++;
    objs[i] = objs[i - 1]; i++;

    objs[i].type = OCT_PATH;
    objs[i].contents.path.width = 100;
    i++;
    objs[i] = objs[i - 1]; i++;

    objs[i].type = OCT_INSTANCE;
    objs[i].contents.instance.name = "";
    objs[i].contents.instance.master = "sample";
    objs[i].contents.instance.view = "physical";
    objs[i].contents.instance.facet = "contents";
    objs[i].contents.instance.version = OCT_CURRENT_VERSION;
    objs[i].contents.instance.transform.translation.x = 100;
    objs[i].contents.instance.transform.translation.y = 200;
    objs[i].contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    i++;

    objs[i] = objs[i - 1];
    objs[i].contents.instance.name = "";
    objs[i].contents.instance.master = "sample";
    objs[i].contents.instance.view = "physical";
    objs[i].contents.instance.facet = "contents";
    objs[i].contents.instance.version = OCT_CURRENT_VERSION;
    i++;

#ifdef notdef
    objs[i].type = OCT_CHANGE_LIST;
    objs[i].contents.changeList.objectMask = OCT_ALL_MASK;
    objs[i].contents.changeList.functionMask = OCT_ALL_FUNCTION_MASK;
    i++;

    objs[i] = objs[i - 1]; i++;
#endif

    return i;
}


static void
tstInconsistent()
{
    /*
     * test oct-inconsistent-bag
     */
    octObject facet1, bag;

    OKLOG("test inconsistent facets");

    facet1.type = OCT_FACET;
    facet1.contents.facet.cell = "inconsistent";
    facet1.contents.facet.view = "physical";
    facet1.contents.facet.facet = "contents";
    facet1.contents.facet.version = OCT_CURRENT_VERSION;
    facet1.contents.facet.mode = "r";
    TEST_INCONSISTENT(octOpenFacet(&facet1));

    bag.type = OCT_BAG;
    bag.contents.bag.name = OCT_INCONSISTENT_BAG;
    TEST_OK(octGetByName(&facet1, &bag));
    if (countAttachments(&bag, OCT_ALL_MASK) != 1) {
	LOG2("   incorrect number of inconsistencies");
    } else {
	OKLOG("  inconsistent facet passed");
    }
    TEST_OK(octCloseFacet(&facet1));
    
    NL;
    return;
}


static void
tstTransforms()
{
    octObject facet, inst;
	
    OKLOG("testing instance transforms");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));

    inst.type = OCT_INSTANCE;
    inst.contents.instance.name = "";
    inst.contents.instance.master = "sample";
    inst.contents.instance.view = "physical";
    inst.contents.instance.facet = "contents";
    inst.contents.instance.version = OCT_CURRENT_VERSION;
    inst.contents.instance.transform.translation.x = 100;
    inst.contents.instance.transform.translation.y = 200;
    inst.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_MIRROR_X;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_MIRROR_Y;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_ROT90;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_ROT180;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_ROT270;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_MX_ROT90;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_MY_ROT90;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = OCT_FULL_TRANSFORM;
    inst.contents.instance.transform.generalTransform[0][0] = 1.0;
    inst.contents.instance.transform.generalTransform[0][1] = 0.0;
    inst.contents.instance.transform.generalTransform[1][0] = 0.0;
    inst.contents.instance.transform.generalTransform[1][1] = 1.0;
    TEST_OK(octCreate(&facet, &inst));
    inst.contents.instance.transform.transformType = (octTransformType) 164;
    TEST_ERROR(octCreate(&facet, &inst));
}


static void
tstUpdating()
{
    /*
     * test oct bb/term updating on not in memory facets
     *
     *   create facet1 with 2 terms
     *   put down a 100x100 piece of geo
     *   close 'facet1'
     *   instantiate 'facet1' in 'facet2'
     *   close 'facet2'
     *   open 'facet1'
     *   delete 1 term in 'facet1'
     *   modify 100x100 geo to 10x10
     *   close 'facet1'
     *   open 'facet2'
     *   count terminals on the instance
     *   check bb of the instance
     *
     * threw in a net attachment to the instance term...
     *
     * XXX open up the interface and see what happens (do terms change)
     *   - before term delete (and save)
     *   - after term delete
     *
     */
    octObject facet1, facet2, term, box, inst;
#ifdef nets
    octObject net;
#endif
    struct octBox bb;

    OKLOG("test out of memory updating of bb and terms");

    facet1.type = OCT_FACET;			/* open the facet */
    facet1.contents.facet.cell = "cell";
    facet1.contents.facet.view = "view";
    facet1.contents.facet.facet = "contents";
    facet1.contents.facet.mode = "w";
    facet1.contents.facet.version = OCT_CURRENT_VERSION;
    TEST_NEW_FACET(octOpenFacet(&facet1));
    term.type = OCT_TERM;			/* add a terminal */
    term.contents.term.instanceId = oct_null_id;
    term.contents.term.name = "term1";
    term.contents.term.width = 1;
    TEST_OK(octCreate(&facet1, &term));
    term.contents.term.name = "term2";
    TEST_OK(octCreate(&facet1, &term));
    box.type = OCT_BOX;				/* add a box */
    box.contents.box.lowerLeft.x = 0;
    box.contents.box.lowerLeft.y = 0;
    box.contents.box.upperRight.x = 100;
    box.contents.box.upperRight.y = 100;
    TEST_OK(octCreate(&facet1, &box));
    TEST_OK(octCloseFacet(&facet1));		/* close the facet */
    
    facet2.type = OCT_FACET;
    facet2.contents.facet.cell = "facet2";	/* open another facet */
    facet2.contents.facet.view = "physical";
    facet2.contents.facet.facet = "contents";
    facet2.contents.facet.mode = "w";
    facet2.contents.facet.version = OCT_CURRENT_VERSION;
    TEST_NEW_FACET(octOpenFacet(&facet2));

    inst.type = OCT_INSTANCE;			/* instantiate the previous facet */
    inst.contents.instance.name = "";
    inst.contents.instance.master = "cell";
    inst.contents.instance.view = "view";
    inst.contents.instance.facet = "contents";
    inst.contents.instance.version = OCT_CURRENT_VERSION;
    inst.contents.instance.transform.translation.x = 0;
    inst.contents.instance.transform.translation.y = 0;
    inst.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    TEST_OK(octCreate(&facet2, &inst));
    /* XXX does this causes the term to stay around... */
#ifdef nets    
    net.type = OCT_NET;				/* attach a net to the actual terminal */
    net.contents.net.name = "net";
    net.contents.net.width = 1;
    TEST_OK(octCreate(&facet2, &net));
    TEST_OK(octGenFirstContent(&inst, OCT_TERM_MASK, &term));
    TEST_OK(octAttach(&net, &term));
#endif    
    TEST_OK(octCloseFacet(&facet2));		/* close the facet */
    
    facet1.contents.facet.mode = "a";		/* open the master */
    TEST_OLD_FACET(octOpenFacet(&facet1));
    TEST_OK(octGenFirstContent(&facet1, OCT_TERM_MASK, &term));
    TEST_OK(octDelete(&term));			/* delete the formal terminal */
    TEST_OK(octGenFirstContent(&facet1, OCT_BOX_MASK, &box));
    box.contents.box.upperRight.x = 10;		/* modify the bounding box */
    box.contents.box.upperRight.y = 10;
    TEST_OK(octModify(&box));
    TEST_OK(octCloseFacet(&facet1));		/* close the master */
    
    facet2.contents.facet.mode = "a";		/* reopen the top level facet */
    TEST_OLD_FACET(octOpenFacet(&facet2));

    						/* get the instance and see what has changed */
    TEST_OK(octGenFirstContent(&facet2, OCT_INSTANCE_MASK, &inst));
    if (countAttachments(&inst, OCT_TERM_MASK) != 1) {
	LOG2("  terminals not updated");	/* got the right number of terminals? */
    } else {
	OKLOG("  unloaded facet terminal updating passed");
    }

#ifdef nets    
    TEST_OK(octGenFirstContent(&facet2, OCT_NET_MASK, &net));
    if (countAttachments(&net, OCT_NET_MASK) != 0) {
	LOG2("  bad net/term attachment");	/* see if the net still thinks it has a terminal */
    } else {
	OKLOG("  term/net attachment passed");
    }
#endif    

    TEST_OK(octBB(&inst, &bb));			/* got the right bounding box? */

    if ((bb.upperRight.x != 10) || (bb.upperRight.y != 10)) {
	LOG2("  bb not updated");
    } else {
	OKLOG("  unloaded facet bounding box updating passed");
    }
    
    TEST_OK(octFreeFacet(&facet2));
    
    NL;
    return;
}

    
static void
tstPathBB()
{
    /*
     * test zero bounding box of path problem
     */
    octObject pfacet, obj;
    struct octPoint points[2];
    struct octBox bb;

    OKLOG("checking path bb on create and readin");
    
    pfacet.type = OCT_FACET;			/* open up a facet */
    pfacet.contents.facet.cell = "path";
    pfacet.contents.facet.view = "physical";
    pfacet.contents.facet.facet = "contents";
    pfacet.contents.facet.version = OCT_CURRENT_VERSION;
    pfacet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&pfacet));

    obj.type = OCT_PATH;			/* create a path */
    obj.contents.path.width = 100;
    TEST_OK(octCreate(&pfacet, &obj));
    points[0].x = -100;
    points[0].y = 0;
    points[1].x = 100;
    points[1].y = 0;
    TEST_OK(octPutPoints(&obj, 2, points));
    TEST_OK(octBB(&obj, &bb));
    if ((bb.lowerLeft.x != -100) || (bb.lowerLeft.y != -50) ||
	(bb.upperRight.x != 100) || (bb.upperRight.y != 50)) {
	LOG2("bad path bb on create");
    }
    TEST_OK(octCloseFacet(&pfacet));		/* close the facet */

    pfacet.contents.facet.mode = "r";
    TEST_OLD_FACET(octOpenFacet(&pfacet));	/* reopen the facet */

    TEST_OK(octGenFirstContent(&pfacet, OCT_PATH_MASK, &obj));
    TEST_OK(octBB(&obj, &bb));			/* is the bounding box correct? */
    if ((bb.lowerLeft.x != -100) || (bb.lowerLeft.y != -50) ||
	(bb.upperRight.x != 100) || (bb.upperRight.y != 50)) {
	LOG2("bad path bb on read");
    }

    TEST_OK(octFreeFacet(&pfacet));

    NL;
    return;
}


static void
tstTemporary()
{
    /*
     * test temporary
     */
    octObject pfacet, temp;

    OKLOG("checking temporary objects");
    
    pfacet.type = OCT_FACET;			/* open up a facet */
    pfacet.contents.facet.cell = "temp";
    pfacet.contents.facet.view = "physical";
    pfacet.contents.facet.facet = "contents";
    pfacet.contents.facet.version = OCT_CURRENT_VERSION;
    pfacet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&pfacet));
    temp.type = OCT_BOX;
    TEST_OK(octCreate(&pfacet, &temp));		/* create a an object and make it temporary */
    octMarkTemporary(&temp);
    TEST_OK(octCloseFacet(&pfacet));		/* close and reopen the facet */
    pfacet.contents.facet.mode = "a";
    TEST_OLD_FACET(octOpenFacet(&pfacet));
    						/* is the object still around? */
    TEST_GEN_DONE(octGenFirstContent(&pfacet, OCT_BOX_MASK, &temp));
    TEST_OK(octCloseFacet(&pfacet));
    OKLOG("  temporary object passed");

    TEST_OLD_FACET(octOpenFacet(&pfacet));	/* see if toggling the temp state does anything */
    TEST_OK(octCreate(&pfacet, &temp));
    octMarkTemporary(&temp);
    octUnmarkTemporary(&temp);
    TEST_OK(octCloseFacet(&pfacet));
    pfacet.contents.facet.mode = "a";
    TEST_OLD_FACET(octOpenFacet(&pfacet));
    TEST_OK(octGenFirstContent(&pfacet, OCT_BOX_MASK, &temp));
    TEST_OK(octCloseFacet(&pfacet));
    OKLOG("  temporary object made permanent passed");

    NL;
    return;
}


static void
tstOct1Cells()
{
    /*
     * test reading oct 1.0 facets
     */
    octObject pfacet;

    OKLOG("checking oct 1.0 facets");
    
    pfacet.type = OCT_FACET;			/* readin an oct 1.0 facet */
    pfacet.contents.facet.cell = "oct1.0";
    pfacet.contents.facet.view = "physical";
    pfacet.contents.facet.facet = "contents";
    pfacet.contents.facet.version = OCT_CURRENT_VERSION;
    pfacet.contents.facet.mode = "r";
    TEST_OLD_FACET(octOpenFacet(&pfacet));

    NL;
    return;
}


static void
tstChangeLists()
{
#ifdef REGION
    /* change list testing */
    octObject pfacet, obj;
    regObjGen gen;
    struct octBox region;
    int ocount = 0;
    int nicount = 0, nocount = 0;

    OKLOG("checking change lists");

    pfacet.type = OCT_FACET;
    pfacet.contents.facet.cell = "cl";
    pfacet.contents.facet.view = "symbolic";
    pfacet.contents.facet.facet = "contents";
    pfacet.contents.facet.version = OCT_CURRENT_VERSION;
    pfacet.contents.facet.mode = "r";
    TEST_OLD_FACET(octOpenFacet(&pfacet));

    OKLOG("  loaded the facet");

    region.lowerLeft.x = -100;
    region.lowerLeft.y = -100;
    region.upperRight.x = 1000;
    region.upperRight.y = 3000;
    
    if (regObjStart(&pfacet, &region, OCT_INSTANCE_MASK|OCT_GEO_MASK, &gen, 0) != REG_OK) {
	LOG2("bad regObjStart");
    }

    while (regObjNext(gen, &obj) == REG_OK) {
	if (obj.type == OCT_INSTANCE) {
	    TEST_OK(octDelete(&obj));
	} else {
	    ocount++;
	}
    }
    regObjFinish(gen);

    OKLOG("  first region search done");
    
    if (regObjStart(&pfacet, &region, OCT_INSTANCE_MASK|OCT_GEO_MASK, &gen, 0) != REG_OK) {
	LOG2("bad regObjStart");
    }

    while (regObjNext(gen, &obj) == REG_OK) {
	if (obj.type == OCT_INSTANCE) {
	    nicount++;
	} else {
	    nocount++;
	}
    }
    regObjFinish(gen);

    OKLOG("  second region search done");
    
    if (nicount != 0) {
	LOG2("still some instances left");
    }

    if (ocount != nocount) {
	LOG2("non-instance count changed");
    }
    
    TEST_OK(octCloseFacet(&pfacet));

    NL;
#endif
    return;
}


static void
tstReadWriteObjects()
{
    /*
     * make sure we can read and write all object types
     */
    int i;
    octObject facet, obj, new, new1, old;
    char buffer[2048];

    OKLOG("see if all object types can be stored and retrieved");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "all";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    
    for (i = OCT_TERM; i <= OCT_EDGE; i++) {
	(void) memset((char *) &obj, 0, sizeof(octObject));
	obj.type = i;
	if (obj.type == OCT_INSTANCE) {
#if !defined(MCC_DMS)
	/*    (void) sprintf(buffer, "%s/src/oct/tst/sample", CADROOT);*/
	    (void) sprintf(buffer, "%s/oct/tst/sample", CADROOT);
#else
	    (void) sprintf(buffer, "%s/sample", ptcGetWorkingWorkspace());
#endif
	    obj.contents.instance.master = buffer;
	    obj.contents.instance.view = "physical";
	    obj.contents.instance.facet = "contents";
	    obj.contents.instance.version = OCT_CURRENT_VERSION;
	}
	TEST_OK(octCreate(&facet, &obj));
    }

    new.type = OCT_FACET;
    new.contents.facet.cell = "/tmp/write1";
    new.contents.facet.view = "physical";
    new.contents.facet.facet = "contents";
    new.contents.facet.version  = OCT_CURRENT_VERSION;
    new.contents.facet.mode = "r";
    TEST_OK(octWriteFacet(&new, &facet));

    new.contents.facet.cell = "/tmp/write2";
    TEST_OK(octWriteFacet(&new, &facet));

    new.contents.facet.cell = "/tmp/write3";
    TEST_OK(octWriteFacet(&new, &facet));

    TEST_OLD_FACET(octOpenFacet(&new));

    new1 = new;
    new1.contents.facet.cell = "write1";
    TEST_OLD_FACET(octOpenRelative(&new, &new1, OCT_SIBLING));
    TEST_OK(octCloseFacet(&new1));

    TEST_ERROR(octCopyFacet(&new, &facet));
    TEST_OK(octCloseFacet(&new));
    TEST_OK(octCloseFacet(&facet));

    /* copy an already open facet */
    new1.contents.facet.cell = "/tmp/write4";
    TEST_OK(octCopyFacet(&new1, &facet));
    new1.contents.facet.mode = "r";
    TEST_OLD_FACET(octOpenFacet(&new1));

    TEST_OK(octCloseFacet(&facet));

    facet.contents.facet.cell = "all";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "r";
    facet.type = OCT_FACET;

    /* copy a closed facet */
    new1.contents.facet.cell = "/tmp/write5";
    TEST_OK(octCopyFacet(&new1, &facet));
    new1.contents.facet.mode = "r";
    TEST_OLD_FACET(octOpenFacet(&new1));

    TEST_OLD_FACET(octOpenFacet(&facet));
    TEST_OK(octCloseFacet(&facet));

    /* test copy to open and closed facet */

    old.type = OCT_FACET;
    old.contents.facet.cell = "/tmp/write1";
    old.contents.facet.view = "physical";
    old.contents.facet.facet = "contents";
    old.contents.facet.version = OCT_CURRENT_VERSION;
    old.contents.facet.mode = "r";

    new.type = OCT_FACET;
    new.contents.facet.cell = "/tmp/write1-copy";
    new.contents.facet.view = "physical";
    new.contents.facet.facet = "contents";
    new.contents.facet.version = OCT_CURRENT_VERSION;
    new.contents.facet.mode = "w";
    
    TEST_OLD_FACET(octOpenFacet(&old));
    TEST_NEW_FACET(octOpenFacet(&new));

    TEST_ERROR(octCopyFacet(&new, &old));

    TEST_OK(octCloseFacet(&new));

    TEST_OK(octCopyFacet(&new, &old));

    NL;
    return;
}


static void
tstOctError()
{  
    /*
     * test octErrorString and octError
     */
    OKLOG("checking oct error functions");
    (void) octErrorString();
    octError("  ignore this message, just a test");
    NL;
    return;
}


static void
tstFlushFree()
{
    /*
     * test for facet flush/free/close problems
     */
    octObject facet;

    OKLOG("checking flush/free/close/open facet");
    
    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    
    /* flush + free should be the same as close */
    TEST_OK(octCloseFacet(&facet));
    facet.contents.facet.mode = "a";
    TEST_OLD_FACET(octOpenFacet(&facet));

    TEST_OK(octFlushFacet(&facet));
    TEST_OK(octFreeFacet(&facet));
    TEST_OLD_FACET(octOpenFacet(&facet));

    /* test ref counting */
    TEST_OK(octCloseFacet(&facet));
    TEST_CORRUPTED_OBJECT(octCloseFacet(&facet));
    
    NL;
    return;
}


static void    
tstInterface()
{
    /*
     * test interfaces
     */
    octObject facet, term, ifacet, iifacet;
    struct octFacetInfo info, info2;
    int ftc;
	
    OKLOG("checking contents/interface consistency");
	
    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    TEST_OK(octGetFacetInfo(&facet, &info));
	
    /* open a new interface and see if the right number of terminals appear */
    term.type = OCT_TERM;
    term.contents.term.name = "bob";
    term.contents.term.instanceId = oct_null_id;
    term.contents.term.width = 1;
    TEST_OK(octCreate(&facet, &term));
    TEST_OK(octGetFacetInfo(&facet, &info2));
    if (info.formalDate == info2.formalDate) {
	LOG2("formalDate mismatch\n");
    }
    ftc = countAttachments(&facet, OCT_TERM_MASK);
    ifacet = facet;
    ifacet.contents.facet.mode = "w";
    ifacet.contents.facet.facet = "interface";
    TEST_NEW_FACET(octOpenFacet(&ifacet));
    if (ftc != countAttachments(&ifacet, OCT_TERM_MASK)) {
	LOG2("  differing numbers of terminals in contents and interface");
    } else {
	OKLOG("  terminals check");
    }

    /* delete a terminal from the contents, what does a new interface do */
    /* verify that detach fails */
    TEST_ILL_OP(octDetach(&facet, &term));
    TEST_OK(octDelete(&term));
    ftc = countAttachments(&facet, OCT_TERM_MASK);
    iifacet = facet;
    iifacet.contents.facet.mode = "w";
    iifacet.contents.facet.facet = "new-interface";
    TEST_NEW_FACET(octOpenFacet(&iifacet));
    if (ftc != countAttachments(&iifacet, OCT_TERM_MASK)) {
	LOG2("  differing numbers of terminals in contents and interface (detach)");
    } else {
	OKLOG("  deleted terminals check");
    }

    /* count terminals on the old interface - in memory test */
    if (ftc != countAttachments(&ifacet, OCT_TERM_MASK)) {
	LOG2("  differing numbers of terminals in contents and old interface");
    } else {
	OKLOG("  deleted terminals again check");
    }

    TEST_OK(octFreeFacet(&ifacet));
    TEST_OK(octFreeFacet(&iifacet));
    TEST_OK(octFreeFacet(&facet));
    
    NL;
    return;
}


static void
tstMoreInterface()
{
    octObject facet, ifacet;

    OKLOG("testing interfaces...");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));

    ifacet = facet;
    ifacet.contents.facet.facet = "interface";
    ifacet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&ifacet));
    TEST_OK(octCloseFacet(&ifacet));

    ifacet.contents.facet.mode = "r";
    TEST_OLD_FACET(octOpenFacet(&ifacet));
    TEST_OK(octCloseFacet(&ifacet));

    TEST_CORRUPTED_OBJECT(octCloseFacet(&ifacet));

    ifacet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&ifacet));

    TEST_OK(octFreeFacet(&facet));
    TEST_OK(octFreeFacet(&ifacet));

    NL;
}


static void
tstGeneration()
{
    /*
     * generation checks
     */
    int i;
    octObject facet, objs[10], obj;
    octGenerator gen, gen1, gen2, gen3, gen4;
	
    OKLOG("generation checks");
	
    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));

    /* deletion/modification in generator test */
    for (i = 0; i < 9; i++) {
	objs[i].type = OCT_POINT;
	objs[i].contents.point.x = i;
	objs[i].contents.point.y = i;
	TEST_OK(octCreate(&facet, &objs[i]));
    }
    TEST_OK(octInitGenContents(&facet, OCT_POINT_MASK, &gen));
    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[0], "order of generation");
    
    TEST_OK(octDelete(&objs[1]));
    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[2], "inline deletion");
    OKLOG("  inline deletion passed");
    
    objs[3].contents.point.x = 1000;
    TEST_OK(octModify(&objs[3]));
    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[3], "inline modification");
    if (obj.contents.point.x != 1000) {
	LOG2("inline modification value is incorrect");
    }
    OKLOG("  inline modification passed");

    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[4], "generate 4");
    
    TEST_OK(octDelete(&obj));
    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[5], "delete current object");
    OKLOG("  delete current item passed");
    
    TEST_OK(octDelete(&objs[8])); /* delete the last item */
    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[6], "generate 6");
    TEST_OK(octGenerate(&gen, &obj));
    objectCompare(&obj, &objs[7], "generate 7");
    TEST_GEN_DONE(octGenerate(&gen, &obj));
    OKLOG("  delete last object passed");

    /* test freegen */
    TEST_OK(octFreeGenerator(&gen)); /* redundant free gen */
    TEST_OK(octFreeGenerator(&gen));
    
    TEST_OK(octInitGenContents(&facet, OCT_POINT_MASK, &gen));
    TEST_OK(octGenerate(&gen, &obj));
    TEST_OK(octFreeGenerator(&gen));
    TEST_GEN_DONE(octGenerate(&gen, &obj));
    TEST_OK(octFreeGenerator(&gen));
    
    /* create during generation */
    obj.type = OCT_BOX;
    TEST_OK(octCreate(&facet, &obj));
    TEST_OK(octInitGenContents(&facet, OCT_BOX_MASK, &gen));
    TEST_OK(octGenerate(&gen, &obj));
    TEST_OK(octCreate(&facet, &obj));
    TEST_GEN_DONE(octGenerate(&gen, &obj));
    TEST_OK(octFreeGenerator(&gen));
    OKLOG("  create during generation passed");

    /* multiple generators */
    TEST_OK(octInitGenContents(&facet, OCT_BOX_MASK, &gen1));
    TEST_OK(octInitGenContents(&facet, OCT_BOX_MASK, &gen2));
    TEST_OK(octInitGenContents(&facet, OCT_BOX_MASK, &gen3));
    TEST_OK(octInitGenContents(&facet, OCT_BOX_MASK, &gen4));
    TEST_OK(octGenerate(&gen1, &obj));
    TEST_OK(octDetach(&facet, &obj));
    TEST_OK(octGenerate(&gen2, &obj));
    TEST_OK(octDetach(&facet, &obj));
    TEST_GEN_DONE(octGenerate(&gen3, &obj));
    TEST_GEN_DONE(octGenerate(&gen4, &obj));
    OKLOG("  simple multiple generator test passed");
    TEST_OK(octFreeGenerator(&gen1));
    TEST_OK(octFreeGenerator(&gen2));
    TEST_OK(octFreeGenerator(&gen3));
    TEST_OK(octFreeGenerator(&gen4));

    /* multiple attachment */
    obj.type = OCT_BOX;
    TEST_OK(octCreate(&facet, &obj));
    TEST_OK(octAttach(&facet, &obj));
    TEST_OK(octAttach(&facet, &obj));
    TEST_OK(octAttach(&facet, &obj));
    if (countAttachments(&facet, OCT_BOX_MASK) != 4) {
	LOG2("  multiple attachments failed");
    } else {
	OKLOG("  multiple attachments succeeded");
    }

    TEST_OK(octFreeFacet(&facet));
    
    NL;
    return;
}


static void
tstFacets()
{
    octObject facet, facet1;
    struct octFacetInfo info;
    struct octBox bb;
    char buffer[1024], cwd[256];
#ifndef SYSV
    char *getwd();
#endif    
    OKLOG("checking open facet return codes");
    
    facet.type = OCT_FACET;
    facet.contents.facet.cell = "corrupt";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "r";
    TEST_ERROR(octOpenFacet(&facet));
    OKLOG("  corrupt facet passed");

    
    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy-non-existent";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "r";
    TEST_NO_EXIST(octOpenFacet(&facet));
    OKLOG("  non-existent facet passed");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "/dummy-no-perm";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NO_PERM(octOpenFacet(&facet));
    OKLOG("  no permission facet passed");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    OKLOG("  new facet passed");

    TEST_OK(octGetFacetInfo(&facet, &info));
    fprintf(stderr, "  full name of %s:%s:%s is %s\n",
		    facet.contents.facet.cell,
		    facet.contents.facet.view,
		    facet.contents.facet.facet,
		    info.fullName);

    TEST_NO_BB(octBB(&facet, &bb));
    
    /* reopen, just to see */
    facet1 = facet;
    facet1.contents.facet.mode = "r";
    TEST_ALREADY_OPEN(octOpenFacet(&facet1));
    if (facet1.objectId != facet.objectId) {
	LOG2("facets have different object id's");
    }
    TEST_OK(octCloseFacet(&facet1));
    OKLOG("  same id's for same facet passed");

    /* reopen, with a different name */
#if !defined(MCC_DMS)
#ifdef SYSV
    if (getcwd(cwd, 256) == NULL) {
#else
    if (getwd(cwd) == NULL) {
#endif
	perror(cwd);
	exit(-1);
    }
    (void) sprintf(buffer, "%s/dummy", cwd);
#else
    (void) sprintf(buffer, "%s/dummy", ptcGetWorkingWorkspace());
#endif
    facet1.contents.facet.cell = buffer;
    TEST_ALREADY_OPEN(octOpenFacet(&facet1));
    if (facet1.objectId != facet.objectId) {
	LOG2("facets (different names - full/short) have different object id's");
    }

    TEST_OK(octCloseFacet(&facet1));
    OKLOG("  same id's for same facet passed again");

    TEST_OK(octFreeFacet(&facet));
    
    NL;
    return;
}


static void
tstMasterBB()  
{
    /*
     * change bb in master, see if instance bb changes
     */
    octObject facet, inst, master, layer, geo, obj;
    struct octBox oldmbb, oldibb, newmbb, newibb;
    octGenerator gen;
    int32 id;
	
    OKLOG("testing bounding box changes in masters and instances");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));

    inst.type = OCT_INSTANCE;
    inst.contents.instance.name = "";
    inst.contents.instance.master = "sample";
    inst.contents.instance.view = "physical";
    inst.contents.instance.facet = "contents";
    inst.contents.instance.version = OCT_CURRENT_VERSION;
    inst.contents.instance.transform.translation.x = 100;
    inst.contents.instance.transform.translation.y = 200;
    inst.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    TEST_OK(octCreate(&facet, &inst));
	
    master.type = OCT_FACET;
    master.contents.facet.facet = "contents";
    master.contents.facet.mode = "a";
    TEST_OLD_FACET(octOpenMaster(&inst, &master));

    /* bug check */
    octExternalId(&master, &id);
    TEST_OK(octGetByExternalId(&master, id, &obj));

	
    TEST_OK(octBB(&master, &oldmbb));
    TEST_OK(octBB(&inst, &oldibb));
    if (!bbCompare(&oldmbb, &oldibb)) {
	LOG2("old instance bb is not the same as the old master bb");
    }

    /* delete some of the geometry */
    TEST_OK(octGenFirstContent(&master, OCT_LAYER_MASK, &layer));
    TEST_OK(octInitGenContents(&layer, OCT_GEO_MASK, &gen));
    while (octGenerate(&gen, &geo) == OCT_OK) {
	TEST_OK(octDelete(&geo));
    }
    TEST_OK(octFreeGenerator(&gen));
    
    TEST_OK(octBB(&master, &newmbb));
    TEST_OK(octBB(&inst, &newibb));
    if (bbCompare(&newibb, &oldibb)) {
	LOG2("new instance bb is the same as the old instance bb");
    }

    if (bbCompare(&newmbb, &oldmbb)) {
	LOG2("new master bb is the same as the old master bb");
    }

    if (!bbCompare(&newmbb, &newibb)) {
	LOG2("new instance bb is not the same as the new master bb");
    }
    
    TEST_OK(octFreeFacet(&master));
    TEST_OK(octFreeFacet(&facet));
	
    NL;
    return;
}


static void
tstTerms()  
{
    /*
     * delete an ft in 'obj', see if the 'at' is still in objs[0]
     */
    octObject facet, inst, master, term, iterm;
    char buffer[1024];
    struct octFacetInfo info, info2;
	
    OKLOG("deleting formal term in master, verifying that actual term goes way");
    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    
    inst.type = OCT_INSTANCE;
    inst.contents.instance.name = "instance";
    inst.contents.instance.master = "sample";
    inst.contents.instance.view = "physical";
    inst.contents.instance.facet = "contents";
    inst.contents.instance.version = OCT_CURRENT_VERSION;
    inst.contents.instance.transform.translation.x = 100;
    inst.contents.instance.transform.translation.y = 200;
    inst.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    TEST_OK(octCreate(&facet, &inst));
	
    master.type = OCT_FACET;
    master.contents.facet.facet = "contents";
    master.contents.facet.mode = "a";
    TEST_OLD_FACET(octOpenMaster(&inst, &master));

    TEST_OK(octGetFacetInfo(&master, &info));
    
    TEST_OK(octGenFirstContent(&master, OCT_TERM_MASK, &term));
    (void) strcpy(buffer, term.contents.term.name);
    iterm.type = OCT_TERM;
    iterm.contents.term.name = buffer;
    TEST_OK(octGetByName(&inst, &iterm));
    
    TEST_OK(octDelete(&term));
    iterm.type = OCT_TERM;
    iterm.contents.term.name = buffer;
    TEST_NOT_FOUND(octGetByName(&inst, &iterm));

    TEST_OK(octGetFacetInfo(&master, &info2));
    if (info.formalDate == info2.formalDate) {
	LOG2("formalDate mismatch\n");
    }

    TEST_OK(octFreeFacet(&master));
    TEST_OK(octFreeFacet(&facet));

    NL;
    return;
}

int incompatible[20][20];

static void
tstFunctions()
{
    int i, j, k;
    octObject facet, objs[60];
    
    OKLOG("checking various combinations of objects for all functions");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));

    i = reset(objs);
    for (j = 0; j < i; j = j + 2) {
	for (k = 1; k < i; k = k + 2) {
	    if ((j == k) || (incompatible[objs[j].type][objs[k].type])) {
		continue;
	    }
	    tstAlot(&facet, &objs[j], &objs[k]);
	    i = reset(objs);
	}
    }
    TEST_OK(octFreeFacet(&facet));
    
    NL;
    return;
}


static void
tstMasters()  
{
    /*
     * test masters
     */
    octObject facet, facet1, master1, master3, inst, term;
    char buffer[1024], cwd[256];
#ifndef SYSV
    char *getwd();
#endif

    OKLOG("checking master/facet");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "dummy";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    
    inst.type = OCT_INSTANCE;
    inst.contents.instance.name = "instance";
    inst.contents.instance.master = "sample";
    inst.contents.instance.view = "physical";
    inst.contents.instance.facet = "contents";
    inst.contents.instance.version = OCT_CURRENT_VERSION;
    inst.contents.instance.transform.translation.x = 100;
    inst.contents.instance.transform.translation.y = 200;
    inst.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
    TEST_OK(octCreate(&facet, &inst));
    
    master1.type = OCT_FACET;
    master1.contents.facet.facet = "contents";
    master1.contents.facet.mode = "a";
    TEST_OLD_FACET(octOpenMaster(&inst, &master1));

#if !defined(MCC_DMS)
#ifdef SYSV
    if (getcwd(cwd, 256) == NULL) {
#else
    if (getwd(cwd) == NULL) {
#endif
	perror(cwd);
	exit(-1);
    }
    (void) sprintf(buffer, "%s/sample", cwd);
#else
    (void) sprintf(buffer, "%s/sample", ptcGetWorkingWorkspace());
#endif
    inst.contents.instance.master = buffer;
    TEST_OK(octCreate(&facet, &inst));
    master3.type = OCT_FACET;
    master3.contents.facet.facet = "contents";
    master3.contents.facet.mode = "a";
    TEST_ALREADY_OPEN(octOpenMaster(&inst, &master3));
    if (master1.objectId != master3.objectId) {
	LOG2("open master with different names gets different object id's");
    } else {
	OKLOG("  master id passed");
    }	    
    
    facet1.type = OCT_FACET;
    facet1.contents.facet.cell = "sample";
    facet1.contents.facet.view = "physical";
    facet1.contents.facet.facet = "contents";
    facet1.contents.facet.version = OCT_CURRENT_VERSION;
    facet1.contents.facet.mode = "r";
    TEST_ALREADY_OPEN(octOpenFacet(&facet1));
    if (facet1.objectId != master1.objectId) {
	LOG2("open facet and open master get different object id's");
    } else {
	OKLOG("  master id passed again");
    }

    /* try to detach a formal ... (readin FORMAL check) */
    TEST_OK(octGenFirstContent(&facet1, OCT_TERM_MASK, &term));
    TEST_ILL_OP(octDetach(&facet1, &term));
	
    TEST_OK(octFreeFacet(&master1));
    TEST_OK(octFreeFacet(&master3));
    TEST_OK(octFreeFacet(&facet1));
    TEST_OK(octFreeFacet(&facet));

    NL;
    return;
}


static void
setUp()
{
    int k, j;

    for (k = 0; k < 20; k++) {
	for (j = 0; j < 20; j++) {
	    incompatible[k][j] = 0;
	}
    }

    incompatible[OCT_NET][OCT_LAYER] = 1;
    incompatible[OCT_NET][OCT_CHANGE_LIST] = 1;

    incompatible[OCT_PROP][OCT_NET] = 1;
    incompatible[OCT_PROP][OCT_LAYER] = 1;
    incompatible[OCT_PROP][OCT_INSTANCE] = 1;
    incompatible[OCT_PROP][OCT_LABEL] = 1;
    incompatible[OCT_PROP][OCT_BOX] = 1;
    incompatible[OCT_PROP][OCT_POLYGON] = 1;
    incompatible[OCT_PROP][OCT_PATH] = 1;
    incompatible[OCT_PROP][OCT_CIRCLE] = 1;
    incompatible[OCT_PROP][OCT_EDGE] = 1;
    incompatible[OCT_PROP][OCT_POINT] = 1;
    incompatible[OCT_PROP][OCT_CHANGE_LIST] = 1;


    incompatible[OCT_CHANGE_LIST][OCT_NET] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_LAYER] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_CHANGE_LIST] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_PROP] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_INSTANCE] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_LABEL] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_BOX] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_POLYGON] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_PATH] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_CIRCLE] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_EDGE] = 1;
    incompatible[OCT_CHANGE_LIST][OCT_POINT] = 1;
    
#define GEO(type) \
    incompatible[type][OCT_CHANGE_LIST] = 1; \
    incompatible[type][OCT_NET] = 1; \
    incompatible[type][OCT_LAYER] = 1; \
    incompatible[type][OCT_INSTANCE] = 1; \
    incompatible[type][OCT_LABEL] = 1; \
    incompatible[type][OCT_BOX] = 1; \
    incompatible[type][OCT_POLYGON] = 1; \
    incompatible[type][OCT_PATH] = 1; \
    incompatible[type][OCT_CIRCLE] = 1; \
    incompatible[type][OCT_EDGE] = 1; \
    incompatible[type][OCT_POINT] = 1;

    GEO(OCT_BOX);
    GEO(OCT_POLYGON);
    GEO(OCT_PATH);
    GEO(OCT_CIRCLE);
    GEO(OCT_EDGE);
    GEO(OCT_POINT);
    GEO(OCT_INSTANCE);
    GEO(OCT_LAYER);

    return;
}


static void
tstOffsetGen()
{
    int i, j;
    octObject facet, dummy, obj[10];
    octGenerator gen;

    OKLOG("see if offset gen works");

    facet.type = OCT_FACET;
    facet.contents.facet.cell = "offset";
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "w";
    TEST_NEW_FACET(octOpenFacet(&facet));
    
    for (i = 0; i < 10; i++) {
	obj[i].type = OCT_BOX;
	obj[i].contents.box.lowerLeft.x = i;
	obj[i].contents.box.lowerLeft.y = i;
	obj[i].contents.box.upperRight.x = i;
	obj[i].contents.box.upperRight.y = i;
	TEST_OK(octCreate(&facet, &obj[i]));
    }
    
    for (i = 0; i < 10; i++) {
        TEST_OK(octInitGenContentsWithOffset(&facet, OCT_ALL_MASK, &gen, &obj[i]));
        j = 0;
        while (octGenerate(&gen, &dummy) == OCT_OK) {
	    j++;
	}
        if (j != 10 - i) {
	    LOG2("Offset Generator failed");
	}
    }
    return;
}


/*ARGSUSED*/
int
main(argc, argv)
int argc;
char **argv;
{

    errProgramName(argv[0]);

    errorCount = 0;
    xid_table = st_init_table(st_numcmp, st_numhash);
    setUp();
    
    octBegin();

#if defined(MCC_DMS)
    /* Here we want to set the DMS current workspace to the current */
    /* directory. This will allow the regress test to be run from the */
    /* directory that contains the test facets. */
    {
	char cwd[1024];
	if (getwd(cwd) == NULL) {
	    perror(cwd);
	    exit(-1);
	}
	if (ptcSetWorkingWorkspace(cwd) != dmsNoError) {
	    fprintf(stderr, "Can not set current workspace: %s\n",
		    dmsErrorString());
	    exit(-1);
	}
    }
#endif /* MCC_DMS */

    fprintf(stderr, "OCT_MIN_COORD is %ld, OCT_MAX_COORD is %ld\n",
			OCT_MIN_COORD, OCT_MAX_COORD);
    tstFacets();
    tstInconsistent();
    tstMoreInterface();
    tstFunctions();
    tstOctError();
    tstGeneration();
    tstMasters();
    tstTerms();
    tstMasterBB();
    tstUpdating();
    tstInterface();
    tstFlushFree();
    tstPathBB();
    tstTransforms();
    tstTemporary();
    tstOct1Cells();
    tstChangeLists();
    tstReadWriteObjects();

    tstOffsetGen();
    
    octEnd();
    NL;
    
    (void) fprintf(stderr, "done: error count is %d\n", errorCount);
    
    /*exit((errorCount == 0) ? 0 : -1);*/
    return((errorCount == 0) ? 0 : -1);
}
