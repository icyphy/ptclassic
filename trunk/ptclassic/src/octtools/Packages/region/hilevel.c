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
/*
 * Region Package High Level Interface
 *
 * David Harrison
 * University of California, Berkeley
 * 1986
 *
 * This file contains routines for presenting a high level
 * interface to the region package.  It contains generators
 * for generating geometry and instances in a region regardless
 * of the layers involved and some high level symbolic policy
 * routines for querying certain information that requires
 * complex region searching.
 */

#include "copyright.h"
#include "port.h"
#include "oct.h"		/* OCT interface        */
#include "tr.h"			/* Transform package    */
#include "region.h"		/* Region package       */
#include "reg_internal.h"	/* Internal definitions */
#include "errtrap.h"		/* Error handling       */
#include "oh.h"

/* regObjGen is actually a pointer to this structure: */

typedef struct obj_gen {
    int insideFlag;
    octObject genFacet;
    octObjectMask theMask;
    struct octBox extent;
    octGenerator *lyrGenPtr;
    octGenerator layerGen;
    regGenerator regionGen;
} regRealObjGen;

static void find_aterms();
static regStatus findFrml();
static regStatus findActl();

regStatus regObjStart(theFacet, region, mask, theGen, allInside)
octObject *theFacet;		/* Facet to look in            */
struct octBox *region;		/* Region to search            */
octObjectMask mask;		/* Geometry mask               */
regObjGen *theGen;		/* Generator handle            */
int allInside;			/* If set, only those enclosed */
/*
 * This routine initializes the generation of geometry in
 * the given region.  The difference between this generator
 * and the region package is this generator automatically
 * generates across all layers in the view to find the geometries.
 * Only items which have bounding boxes will be found using
 * this routine.  If the flag 'allInside' is set,  only geometries
 * entirely inside the region will be returned (normally all
 * objects intersecting the region are returned.  NOTE: the routine
 * may return immediately with REG_NOMORE if there are no objects
 * to generate.
 */
{
    octObject firstLayer;
    octStatus retCode;
    regRealObjGen *newGen;

    REG_PRE("regObjStart");
    newGen = REG_ALLOC(regRealObjGen);
    (*theGen) = (obj_gen_dummy *) newGen;
    if (mask & (~(OCT_GEO_MASK|OCT_INSTANCE_MASK))) {
	(void) reg_fault(REG_BA, "inappropriate mask");
	/* NOTREACHED */
    }
    newGen->theMask = mask;
    newGen->extent = *region;
    newGen->insideFlag = allInside;
    newGen->genFacet = *theFacet;
    newGen->lyrGenPtr = (octGenerator *) 0;
    if (mask & OCT_GEO_MASK) {
	newGen->lyrGenPtr = &(newGen->layerGen);
	REG_OA(octInitGenContents(theFacet, OCT_LAYER_MASK, newGen->lyrGenPtr));
	if ((retCode = octGenerate(newGen->lyrGenPtr, &firstLayer))
	    == OCT_GEN_DONE)
	  {
	      /* No layers to check -- is the instance mask on? */
	      if (newGen->theMask & OCT_INSTANCE_MASK) {
		  /* Yes - initialize region package to get instances */
		  regInit(&(newGen->genFacet), &(newGen->extent),
			  OCT_INSTANCE_MASK, &(newGen->regionGen));
		  /* Turn off the instance mask */
		  newGen->theMask &= ~OCT_INSTANCE_MASK;
		  REG_RETURN( REG_OK );
	      } else {
		  /* All done generating everything */
		  newGen->regionGen.state = (reg_generic) 0;
		  REG_RETURN( REG_NOMORE );
	      }
	  }
	else if (retCode == OCT_OK) {
	    regInit(&firstLayer, region, mask & OCT_GEO_MASK,
		    &(newGen->regionGen));
	    REG_RETURN( REG_OK );
	} else {
	    (void) reg_fault(REG_OCT, "cannot generate layers");
	    /* NOTREACHED */
	}
    } else {
	/* Is instance mask on? */
	if (newGen->theMask & OCT_INSTANCE_MASK) {
	    /* Yes - initialize region package to get instances */
	    regInit(&(newGen->genFacet), &(newGen->extent),
		    OCT_INSTANCE_MASK, &(newGen->regionGen));
	    /* Turn off the instance mask */
	    newGen->theMask &= ~OCT_INSTANCE_MASK;
	    REG_RETURN( REG_OK );
	} else {
	    /* All done generating everything */
	    newGen->regionGen.state = (reg_generic) 0;
	    REG_RETURN( REG_NOMORE );
	}
    }
    /* NOTREACHED */
    REG_RETURN( REG_OK );
}


regStatus regObjNext(theGen, theObj)
regObjGen theGen;		/* Generator handle */
octObject *theObj;		/* Returned object  */
/*
 * This routine generates the next object in the sequence
 * initialized by regObjStart.  If there are no more objects,
 * the routine returns REG_NOMORE and leaves 'theObj' unchanged.
 */
{
    octObject retObj, layerObj;
    regRealObjGen *realGen = (regRealObjGen *) theGen;
    struct octBox bb;
    struct octPoint *retPoints;
    int length;
    regStatus rc;

    REG_PRE("regObjNext");
    /* First,  try to generate object as it sits */
    if (realGen->regionGen.state == (reg_generic) 0) {
	REG_RETURN( REG_NOMORE );
    }
    if (regNext(&(realGen->regionGen), &retObj, &length, &retPoints, &bb) == REG_OK) {
	/* Test to see if inside region */
	if (realGen->insideFlag) {
	    if (!REGBOXINSIDE(bb, realGen->extent)) {
		/* Call recursively */
		rc = regObjNext(theGen, theObj);
		REG_RETURN( rc );
	    }
	}
	*theObj = retObj;
	REG_RETURN( REG_OK );
    } else {
	/* No more objects on this layer.  Increment layer. */
	regEnd(&(realGen->regionGen));
	realGen->regionGen.state = (reg_generic) 0;
	if ((realGen->lyrGenPtr) &&
	    (octGenerate(realGen->lyrGenPtr, &layerObj) == OCT_OK)) {
	    /* Ok,  initialize region package again with the new layer */
	    regInit(&layerObj, &(realGen->extent),
		    (realGen->theMask & OCT_GEO_MASK),
		    &(realGen->regionGen));
	    /* Call recursively to get the next object */
	    rc = regObjNext(theGen, theObj);
	    REG_RETURN( rc );
	} else {
	    /* No more layers to check -- is the instace mask on? */
	    realGen->lyrGenPtr = (octGenerator *) 0;
	    if (realGen->theMask & OCT_INSTANCE_MASK) {
		/* Yes - initialize region package to get instances */
		regInit(&(realGen->genFacet), &(realGen->extent),
			OCT_INSTANCE_MASK, &(realGen->regionGen));
		/* Turn off the instance mask */
		realGen->theMask &= ~OCT_INSTANCE_MASK;
		/* Recursive call to get next object */
		rc = regObjNext(theGen, theObj);
		REG_RETURN( rc );
	    } else {
		/* All done generating everything */
		REG_RETURN( REG_NOMORE );
	    }
	}
    }
    /*NOTREACHED*/
}

void regObjFinish(theGen)
regObjGen theGen;
/*
 * This routine ends the generation of geometry initialized by
 * regObjStart.  This routine should be called after the
 * caller has gotten all information desired from the generation
 * sequence.  NOTE:  this routine is NOT called automatically
 * once all items have been generated.
 */
{
    regRealObjGen *realGen = (regRealObjGen *) theGen;

    REG_PRE("regObjFinish");
    if (realGen->regionGen.state) regEnd(&(realGen->regionGen));
    REG_FREE(realGen);
    REG_POST;
}




regStatus regMultiActual(theFacet, extent, n_ok, ary)
octObject *theFacet;		/* Facet of the point        */
struct octBox *extent;		/* Area to look for terminal */
int *n_ok;			/* Number of terminals found */
regTermInfo **ary;		/* Returned info             */
/*
 * This routine finds all actual terminals which intersect
 * the box `extent' in facet `theFacet'.  It returns
 * all of the terminals and their associated bounding boxes
 * in the array `ary'.  The number of terminals returned is
 * given by `n_ok'.  The space for the array is allocated
 * in this routine and should be freed once the caller is
 * finished with it.  It will return REG_FAIL if no terminals
 * are found.
 */
{
    regGenerator theGen;
    octObject theObj;
    struct octPoint *retPoints;
    int ary_size, failed, retLength;

    REG_PRE("regMultiActual");

    /* Set up initial array */
    ary_size = 3;
    *ary = REG_ARYALLOC(regTermInfo, ary_size);
    *n_ok = 0;
    
    /* Find intersecting instances */
    regInit(theFacet, extent, OCT_INSTANCE_MASK, &theGen);
    while (regNext(&theGen, &theObj, &retLength, &retPoints, (struct octBox *) 0) == REG_OK) {
	find_aterms(&theObj, (octObject *) 0, extent, &ary_size, n_ok, ary,
		    &failed);
    }
    regEnd(&theGen);
    if (*n_ok) {
	REG_RETURN( REG_OK );
    } else {
	REG_RETURN( REG_FAIL );
    }
}


regStatus regFindActual(theFacet, thePoint, termObject, implBB)
octObject *theFacet;		/* Facet of the point          */
struct octPoint *thePoint;	/* Point in OCT coordinates    */
octObject *termObject;		/* Returned terminal           */
struct octBox *implBB;		/* Implementation bounding box */
/*
 * This routine finds the actual terminal at 'thePoint'.  If there
 * is no such terminal or the terminal is ambiguous the routine
 * returns REG_FAIL.  The routine also returns a bounding
 * box which contains the implementation of the terminal.  Note it
 * assumes the terminals are implemented using boxes. Here are the steps:
 *   I.  For each instance under 'thePoint':
 *       A. Open up the interface of the instance.  This MUST exist
 *          (an error results if it doesn't).
 *       B. Translate 'thePoint' into the interface coordinate system.
 *       C. For each object under the translated point:
 *          1. If the object implements a formal terminal,  save it's name.
 *          2. If another name has already been saved,  the terminal
 *             is not unique.  This is an error.
 *       D. Save the terminal name at the instance level.  If another
 *          name is already saved,  the terminal is ambigious.  The
 *          routine barfs.
 *   II. Given the unique name of the terminal,  generate the
 *       actual terminal and return.
 */
{
    octObject theObj;
    octObject instMaster, finalInst, termImpl, oneTerm;
    regGenerator theGen;
    regObjGen subGen;
    octGenerator octGen;
    struct octBox extent, instSpot, localBB;
    struct octPoint *retPoints;
    char termName[120];
    int retLength;
    octCoord temp;

    REG_PRE("regFindActual");
    extent.lowerLeft = *thePoint;
    extent.upperRight = *thePoint;
    regInit(theFacet, &extent, OCT_INSTANCE_MASK, &theGen);
    finalInst.objectId = oct_null_id;
    termName[0] = '\0';
    implBB->lowerLeft.x = implBB->lowerLeft.y = REGMAXINT;
    implBB->upperRight.x = implBB->upperRight.y = REGMININT;
    /* (I) For each instance */
    while (regNext(&theGen, &theObj, &retLength, &retPoints,
		   (struct octBox *) 0) == REG_OK) {
	/* (A) Open up the interface */
	
	instMaster.type = OCT_FACET;
	instMaster.contents.facet.facet = "interface";
	instMaster.contents.facet.mode = "r";
	instMaster.contents.facet.version = OCT_CURRENT_VERSION;
	if (octOpenMaster(&theObj, &instMaster) < OCT_OK) {
	    (void) reg_fault(REG_OCT, 
			     "cannot open interface facet of instance %s:%s",
			     theObj.contents.instance.master,
			     theObj.contents.instance.view);
	    /* NOTREACHED */
	}
	
	/*
	 * (B) Master is ready.   We must transform the point to the
	 * instance coordinate system before trying to find actual
	 * terminal.
	 */
	
	instSpot = extent;
	tr_oct_inverse_transform(&(theObj.contents.instance.transform),
				 &(instSpot.lowerLeft.x),
				 &(instSpot.lowerLeft.y));
	instSpot.upperRight = instSpot.lowerLeft;
	/*
	 * (C) Generate objects beneath the extent in the
	 * interface facet.
	 */
	if (regObjStart(&instMaster, &instSpot, OCT_BOX_MASK, &subGen, 0)
	    != REG_OK)
	  {
	      /* Can't get geometry for this instance - skip on */
	      continue;
	  }
	while (regObjNext(subGen, &termImpl) == REG_OK) {
	    /* (1) See if the object is a formal terminal */
	    if (octInitGenContainers(&termImpl, OCT_TERM_MASK, &octGen) != OCT_OK)
	      continue;
	    while (octGenerate(&octGen, &oneTerm) == OCT_OK) {
		if ((termName[0] == '\0') ||
		    (strcmp(termName, oneTerm.contents.term.name) == 0))
		  {
		      (void) strcpy(termName, oneTerm.contents.term.name);
		      finalInst = theObj;
		      /* Compute bounding box */
		      (void) octBB(&termImpl, &localBB);
		      tr_oct_transform(&(finalInst.contents.instance.transform),
				       &(localBB.lowerLeft.x),
				       &(localBB.lowerLeft.y));
		      tr_oct_transform(&(finalInst.contents.instance.transform),
				       &(localBB.upperRight.x),
				       &(localBB.upperRight.y));
		      REGBOXNORM(localBB, temp);
		      /* Merge box with final box */
		      implBB->lowerLeft.x = REGMIN(implBB->lowerLeft.x,
						   localBB.lowerLeft.x);
		      implBB->lowerLeft.y = REGMIN(implBB->lowerLeft.y,
						   localBB.lowerLeft.y);
		      implBB->upperRight.x = REGMAX(implBB->upperRight.x,
						    localBB.upperRight.x);
		      implBB->upperRight.y = REGMAX(implBB->upperRight.y,
						    localBB.upperRight.y);
		  } else {
		      /* Here it is:  ambiguious terminal */
		      REG_RETURN( reg_error(REG_FAIL,
		 "Terminal is ambiguous:\n  both %s and %s are located there\n",
		  termName, oneTerm.contents.term.name) );
		  }
	    }
	}
	regObjFinish(subGen);
    }
    /* Ok - we are out of the hairy code.  Lets try to find the actual */
    if (termName[0] == '\0') {
	REG_RETURN( reg_error(REG_FAIL, "No formal terminal found") );
    } else {
	termObject->type = OCT_TERM;
	termObject->contents.term.name = termName;
	if (octGetByName(&finalInst, termObject) != OCT_OK) {
	    (void) reg_fault(REG_OCT, "No corresponding actual terminal `%s'",
			     termName);
	    /* NOTREACHED */
	}
    }
    REG_RETURN( REG_OK );
}





static void find_aterms(inst, lyr, ext, ary_size, num, ary, failed)
octObject *inst;		/* Instance object            */
octObject *lyr;			/* Conditional layer          */
struct octBox *ext;		/* Extent to check against    */
int *ary_size;			/* Size of dynamic array      */
int *num;			/* Number of objects in array */
regTermInfo **ary;		/* Array of term info         */
int *failed;			/* Total terms found          */
/*
 * This routine finds all actual terminals of instance `inst'
 * whose implementation contains geometry on layer `lyr' that
 * intersect the region `ext' and places them into the
 * array `ary'.  This is done by checking all implementations
 * of all formal terminals of the instance master against
 * the region.  The returned terminal bounding box is the extent
 * of all implementations found to intersect `ext'.  The total
 * number of terminals found including those rejected for being
 * on the wrong layer is given in `failed'.
 */
{
    octObject master, term, geo, geoLyr;
    octGenerator termGen, geoGen;
    struct octBox local_ext, bb, bigbb;
    octCoord temp;
    int found_flag, reject_flag;

    /* Open instance master */
    master.type = OCT_FACET;
    master.contents.facet.facet = "interface";
    master.contents.facet.mode = "r";
    master.contents.facet.version = OCT_CURRENT_VERSION;
    if (octOpenMaster(inst, &master) < OCT_OK) {
	(void) reg_fault(REG_NIF);
	/* NOTREACHED */
    }

    /* Inverse transform region */
    local_ext = *ext;
    tr_oct_inverse_transform(&(inst->contents.instance.transform),
			     &(local_ext.lowerLeft.x),
			     &(local_ext.lowerLeft.y));
    tr_oct_inverse_transform(&(inst->contents.instance.transform),
			     &(local_ext.upperRight.x),
			     &(local_ext.upperRight.y));
    REGBOXNORM(local_ext, temp);

    /* Start generation of all formal terminals */
    REG_OA(octInitGenContents(&master, OCT_TERM_MASK, &termGen));
    while (octGenerate(&termGen, &term) == OCT_OK) {
	/* See if its implementation intersects region */
	REG_OA(octInitGenContents(&term, OCT_GEO_MASK, &geoGen));
	found_flag = reject_flag = 0;
	bigbb.lowerLeft.x = bigbb.lowerLeft.y = REGMAXINT;
	bigbb.upperRight.x = bigbb.upperRight.y = REGMININT;
	while (octGenerate(&geoGen, &geo) == OCT_OK) {
	    if (octBB(&geo, &bb) != OCT_OK) continue;
	    if (REGBOXINTERSECT(bb, local_ext) &&
		(octGenFirstContainer(&geo, OCT_LAYER_MASK, &geoLyr) == OCT_OK)) {
		if (!lyr || (strcmp(geoLyr.contents.layer.name,
				    lyr->contents.layer.name) == 0)) {
		    found_flag = 1;
		    bigbb.lowerLeft.x = REGMIN(bigbb.lowerLeft.x, bb.lowerLeft.x);
		    bigbb.lowerLeft.y = REGMIN(bigbb.lowerLeft.y, bb.lowerLeft.y);
		    bigbb.upperRight.x = REGMAX(bigbb.upperRight.x, bb.upperRight.x);
		    bigbb.upperRight.y = REGMAX(bigbb.upperRight.y, bb.upperRight.y);
		} else {
		    reject_flag = 1;
		}
	    }
	}
	if (found_flag) {
	    /* Add the terminal */
	    if (octGetByName(inst, &term) == OCT_OK) {
		if (*num >= *ary_size) {
		    /* Make more room */
		    *ary_size *= 2;
		    *ary = REG_REALLOC(regTermInfo, *ary, *ary_size);
		}
		(*ary)[*num].term = term;
		tr_oct_transform(&(inst->contents.instance.transform),
				 &(bigbb.lowerLeft.x), &(bigbb.lowerLeft.y));
		tr_oct_transform(&(inst->contents.instance.transform),
				 &(bigbb.upperRight.x), &(bigbb.upperRight.y));
		REGBOXNORM(bigbb, temp);
		(*ary)[*num].bb = bigbb;
		*num += 1;
	    }
	}
	if (found_flag || reject_flag) *failed += 1;
    }
}



#define YVAL(xval)	(m * (xval)) + b
#define XVAL(yval)	((yval) - b) / m

static int box_meet_line(llx, lly, urx, ury, m, b)
double llx, lly, urx, ury;	/* Bounding box (in doubles) */
double m, b;			/* Line slope and intersect  */
/*
 * Returns a non-zero value if `bb' intersects the line described
 * by `m_num', `m_denom', and `b'.  Assumes bb is normalized.
 */
{
    double spot;

    /* Check lower point */
    spot = YVAL(llx);
    if ((spot >= lly) && (spot <= ury)) return 1;
    spot = XVAL(lly);
    if ((spot >= llx) && (spot <= urx)) return 1;
    /* Check upper point */
    spot = YVAL(urx);
    if ((spot >= lly) && (spot <= ury)) return 1;
    spot = XVAL(ury);
    if ((spot >= llx) && (spot <= urx)) return 1;
    return 0;
}

static void strip_nonintersect(p1, p2, num, ary)
struct octPoint *p1, *p2;	/* Segment points           */
int *num;			/* Number of terms in `ary' */
regTermInfo *ary;		/* Array of terminals       */
/*
 * For non-manhattan segments,  `ary' may contain some terminals
 * which intersect the bounding box of the segment but don't actually
 * intersect the segment itself.  This routine prunes off these
 * terminals.  I don't expect this to come into play often,  so
 * its calculations are done in floating point to avoid overflow
 * problems.
 */
{
    double m, b;
    int idx;

    m = ((double) (p2->y - p1->y)) / ((double) (p2->x - p1->x));
    b = ((double) p1->y) - m * ((double) p1->x);
    for (idx = 0;  idx < *num;  idx++) {
	if (!box_meet_line((double) ary[idx].bb.lowerLeft.x,
			   (double) ary[idx].bb.lowerLeft.y,
			   (double) ary[idx].bb.upperRight.x,
			   (double) ary[idx].bb.upperRight.y,
			   m, b))
	  {
	      /* Doesn't intersect - remove and check again */
	      (*num)--;
	      ary[idx] = ary[*num];
	      idx--;
	  }
    }
}



regStatus regSegActuals(theFacet, lyr, p1, p2, n_ok, ary, n_fail)
octObject *theFacet;		/* Facet of the path   */
octObject *lyr;			/* Conditional layer   */
struct octPoint *p1;		/* First point         */
struct octPoint *p2;		/* Second point        */
int *n_ok;			/* # rtned terminals   */
regTermInfo **ary;		/* Returned info       */
int *n_fail;			/* Total terms found   */
/*
 * This routine finds the actual terminals which intersect a path drawn from
 * `p1' to `p2' (it assumes no width) on layer `lyr'.  It returns all of the
 * terminals and their associated bounding boxes in the array `ary'.  The
 * number of these terminals is returned in `n_ok'.  These are
 * dynamically allocated arrays which should be freed after the caller is
 * finished with them.  The mechanism for finding such terminals is VERY
 * involved and is summarized in the routine regFindActual.  This routine is
 * made more complex by the fact it must find all terminals intersecting the
 * path.  The total number of terminals found (including those rejected
 * due to the layer test) is given in `n_fail'.
 */
{
    regGenerator theGen;
    octObject theObj;
    struct octBox extent;
    struct octPoint *retPoints;
    int ary_size, num_failed, retLength;
    octCoord temp;

    REG_PRE("regSegActuals");
    /* Set up initial array of five items */
    ary_size = 5;
    *ary = REG_ARYALLOC(regTermInfo, ary_size);
    *n_ok = 0;
    num_failed = 0;

    /* See what instances intersect the path */
    extent.lowerLeft = *p1;
    extent.upperRight = *p2;
    REGBOXNORM(extent, temp);
    regInit(theFacet, &extent, OCT_INSTANCE_MASK, &theGen);
    while (regNext(&theGen, &theObj, &retLength, &retPoints,
		   (struct octBox *) 0) == REG_OK)
      {
	  find_aterms(&theObj, lyr, &extent, &ary_size, n_ok, ary, &num_failed);
      }
    regEnd(&theGen);
    /* Non-manhattan check and elimination */
    if ((p1->x != p2->x) && (p1->y != p2->y)) {
	strip_nonintersect(p1, p2, n_ok, *ary);
    }
    if (n_fail) *n_fail = num_failed;
    REG_RETURN( REG_OK );
}




regStatus regFindImpl(theTerm, implBB)
octObject *theTerm;		/* Input terminal          */
struct octBox *implBB;		/* Implementation geometry */
/*
 * This routine finds the bounding box of the implementation
 * of some terminal.  There are two cases:  formal and actual
 * terminals.  For formal terminals,  we look at its implementation
 * directly and return that.  If it is an actual terminal,  then
 * we hand that to the actual terminal finder which  opens up
 * master's interface and finds the geometry there.
 */
{
    regStatus stat;

    REG_PRE("regFindImpl");
    if (octIdIsNull(theTerm->contents.term.instanceId)) {
	/* Its a formal terminal */
	stat = findFrml(theTerm, implBB, (int *) 0, (char **) 0);
    } else {
	stat = findActl(theTerm, implBB, (int *) 0, (char **) 0);
    }
    REG_RETURN( stat );
}

#define LYRCOUNT	15
#define LYRSIZE		20
static char **theAry = (char **) 0;

regStatus regFindLyrImpl(theTerm, implBB, count, lyrAry)
octObject *theTerm;		/* Input terminal          */
struct octBox *implBB;		/* Implementation geometry */
int *count;			/* Return count of lyrs    */
char ***lyrAry;			/* Returned array          */
/*
 * This routine finds the bounding box of the implementation
 * of some terminal.  There are two cases:  formal and actual
 * terminals.  For formal terminals,  we look at its implementation
 * directly and return that.  If it is an actual terminal,  then
 * we hand that to the actual terminal finder which  opens up
 * master's interface and finds the geometry there.  The array
 * is maintained locally and should not be freed.
 */
{
    int idx;
    regStatus stat;

    REG_PRE("regFindLyrImpl");
    if (!theAry) {
	theAry = REG_ARYALLOC(char *, LYRCOUNT);
	for (idx = 0;  idx < LYRCOUNT;  idx++) {
	    theAry[idx] = REG_ARYALLOC(char, LYRSIZE);
	}
    }
    *lyrAry = theAry;
    if (octIdIsNull(theTerm->contents.term.instanceId)) {
	/* Its a formal terminal */
	stat = findFrml(theTerm, implBB, count, theAry);
    } else {
	stat = findActl(theTerm, implBB, count, theAry);
    }
    REG_RETURN( stat );
}


static regStatus findFrml(theTerm, implBB, lyrCount, lyrAry)
octObject *theTerm;		/* Input terminal          */
struct octBox *implBB;		/* Implementation geometry */
int *lyrCount;			/* Number of layers        */
char **lyrAry;			/* Layer array             */
/*
 * This routine finds the implementation bounding box of a formal
 * terminal.  It looks at the objects connected to the terminal.
 * If it is raw geometry,  the largest of them is returned.  If
 * it is an actual terminal,  the actual terminal lookup routine
 * is called.  It also finds the layers of the implementing geometry
 * and returns them in the preallocated array `lyrAry'.
 */
{
    octGenerator theGen;
    struct octBox localBB;
    octObject genObj, lyrObj;
    int geoFlag, idx;

    REG_OA(octInitGenContents(theTerm,
			      OCT_GEO_MASK|OCT_TERM_MASK, &theGen));
    implBB->lowerLeft.x = implBB->lowerLeft.y = REGMAXINT;
    implBB->upperRight.x = implBB->upperRight.y = REGMININT;
    geoFlag = 0;
    if (lyrCount) *lyrCount = 0;
    while (octGenerate(&theGen, &genObj) == OCT_OK) {
	/* If a terminal,  call recursively to find its implementation */
	if (genObj.type == OCT_TERM) {
	    /* Policy says its an actual terminal */
	    geoFlag = findActl(&genObj, implBB, lyrCount, lyrAry) == REG_OK;
	} else {
	    geoFlag = 1;
	    (void) octBB(&genObj, &localBB);
	    if (localBB.lowerLeft.x < implBB->lowerLeft.x)
	      implBB->lowerLeft.x = localBB.lowerLeft.x;
	    if (localBB.lowerLeft.y < implBB->lowerLeft.y)
	      implBB->lowerLeft.y = localBB.lowerLeft.y;
	    if (localBB.upperRight.x > implBB->upperRight.x)
	      implBB->upperRight.x = localBB.upperRight.x;
	    if (localBB.upperRight.y > implBB->upperRight.y)
	      implBB->upperRight.y = localBB.upperRight.y;
	    if (lyrCount &&
		(octGenFirstContainer(&genObj, OCT_LAYER_MASK,
				      &lyrObj)) == OCT_OK)
		{
		    for (idx = 0;  idx < *lyrCount;  idx++) {
			if (strcmp(lyrObj.contents.layer.name,lyrAry[idx])==0) {
			    break;
			}
		    }
		    if (idx >= (*lyrCount)) {
			(void) strcpy(lyrAry[(*lyrCount)++],
				      lyrObj.contents.layer.name);
		    }
		}
	}
    }
    if (geoFlag) {
	return REG_OK;
    } else {
	return reg_error(REG_FAIL, "No implementation found for terminal");
    }
}



static regStatus findActl(theTerm, implBB, lyrCount, lyrAry)
octObject *theTerm;		/* Input terminal          */
struct octBox *implBB;		/* Implementation geometry */
int *lyrCount;			/* Return layer count      */
char **lyrAry;			/* Returned array of layers */
/*
 * This routine finds the implementation of an actual terminal.
 * It opens the interface of the master for the actual terminal,
 * gets the terminal by name,  and uses the findFormal routine
 * to get its implementation.  This implemenation is then transformed
 * back to get its implementation at this level.
 */
{
    octObject master, inst, masTerm;
    octCoord temp;
    regStatus rc;

    if (octIdIsNull(theTerm->contents.term.instanceId)) {
	/* Its not an actual terminal - try calling the formal stuff */
	return findFrml(theTerm, implBB, lyrCount, lyrAry);
    }
    /* Get the instance */
    inst.type = OCT_INSTANCE;
    inst.objectId = theTerm->contents.term.instanceId;
    if (octGetById(&inst) != OCT_OK) {
	(void) reg_fault(REG_OCT, "Can't get terminal's instance:\n  %s\n",
			 octErrorString());
	/* NOTREACHED */
    }
    /* Open up the master */
    master.type = OCT_FACET;
    master.contents.facet.cell = inst.contents.instance.master;
    master.contents.facet.view = inst.contents.instance.view;
    master.contents.facet.facet = "interface";
    master.contents.facet.version = inst.contents.instance.version;
    master.contents.facet.mode = "r";
    if (octOpenMaster(&inst, &master) < OCT_OK) {
	(void) reg_fault(REG_NIF);
	/* NOTREACHED */
    }
    /* Master's interface is open - find terminal by name */
    masTerm.type = OCT_TERM;
    masTerm.objectId = oct_null_id;
    masTerm.contents.term.instanceId = oct_null_id;
    masTerm.contents.term.name = theTerm->contents.term.name;
    if (octGetByName(&master, &masTerm) != OCT_OK) {
	char buf[1024];
	sprintf( buf, "Can't find %s\nin %s", ohFormatName( &masTerm ),
		ohFormatName( &master ));
	(void) reg_fault(REG_CON, buf);
	/* NOTREACHED */
    }
    /* Got him.  Now get his implementation using this formal */
    if ((rc = findFrml(&masTerm, implBB, lyrCount, lyrAry)) != REG_OK)
      return rc;

    /* Ok.  Now translate him back to normal. */
    tr_oct_transform(&(inst.contents.instance.transform),
		     &(implBB->lowerLeft.x), &(implBB->lowerLeft.y));
    tr_oct_transform(&(inst.contents.instance.transform),
		     &(implBB->upperRight.x), &(implBB->upperRight.y));
    REGBOXNORM((*implBB), temp);
    return REG_OK;
}



regStatus regFindNet(obj, netObj)
octObject *obj;			/* Object implementing net */
octObject *netObj;		/* Returned net            */
/*
 * This routine searches for the major net associated with
 * 'obj'.  The object can be a path or a terminal.
 * If it finds a unique net,  it copies it into 'netObj'.  
 * It will return REG_FAIL if it can't find the net for
 * some reason.  The reason is given in the region error string.
 */
{
    octGenerator theGen;
    octStatus retCode;
    octObject genObj, someNet, tempNet;
    regStatus netCode;
    int count;

    REG_PRE("regFindNet");
    if (obj->type == OCT_PATH) {
	/* Simply generate the nets */
	REG_OA(octInitGenContainers(obj, OCT_NET_MASK, &theGen));
	count = 0;
	while ((retCode = octGenerate(&theGen, netObj)) == OCT_OK) count++;
	if (retCode != OCT_GEN_DONE) {
	    (void) reg_fault(REG_OCT, "error generating nets");
	    /* NOTREACHED */
	}
	if (count > 1) {
	    REG_RETURN( reg_error(REG_FAIL, "Net is not unique") );
	}
	if (count == 0) {
	    REG_RETURN( reg_error(REG_FAIL, "No net found") );
	}
    } else if (obj->type == OCT_TERM) {
	/* Generate containing nets and paths and terminals */
	tempNet.objectId = oct_null_id;
	REG_OA(octInitGenContainers(obj, OCT_NET_MASK|OCT_PATH_MASK|OCT_TERM_MASK,
				    &theGen));
	while ((retCode = octGenerate(&theGen, &genObj)) == OCT_OK) {
	    switch (genObj.type) {
	    case OCT_PATH:
	    case OCT_TERM:
		/* Find the net recursively */
		netCode = regFindNet(&genObj, &someNet);
		if (netCode == REG_OK) {
		    if (octIdIsNull(tempNet.objectId)) {
			tempNet = someNet;
		    } else if (!octIdsEqual(tempNet.objectId, someNet.objectId)) {
			REG_RETURN( reg_error(REG_FAIL, "Net is not unique") );
		    }
		}
		break;
	    case OCT_NET:
		/* Its a net compare it */
		if (octIdIsNull(tempNet.objectId)) {
		    tempNet = genObj;
		} else if (!octIdsEqual(tempNet.objectId, genObj.objectId)) {
		    REG_RETURN( reg_error(REG_FAIL, "Net is not unique") );
		}
		break;
	    }
	}
	if (octIdIsNull(tempNet.objectId)) {
	    REG_RETURN( reg_error(REG_FAIL, "No net was found") );
	} else {
	    *netObj = tempNet;
	}
    }
    REG_RETURN( REG_OK );
}

