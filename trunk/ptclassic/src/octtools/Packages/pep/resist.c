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
#include "port.h"
#include "copyright.h"
#include "utility.h"
#include "oct.h"
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "tap.h"
#include "th.h"
#include "errtrap.h"
#include "ansi.h"
#include "pepint.h"

#define MIN_TERM_NUM 10
#define MAXSTRLEN 256

#define   pepReduceTerm(t)    {octObject ttt;      \
      if(octGenFirstContent(t, OCT_TERM_MASK, &ttt) == OCT_OK) \
	*t = ttt; }

int isTermAbutted
    ARGS((octObject *termP));

static octId *idPath;             /*  Stack with the objects already met */
static int pathSize = 0;          /*  Its size.  */
static octId *visitedIdPath;      /*  Stack with the paths already met */
static int visitedSize = 0;       /*  Its size.  */
static octStatus nextNewTermInPath();
/*static octStatus chooseTermCouple(); */
static octId *scanNetBranches();

static double
  getCoordSize()
{
    static double coord_size = (-1.0);
    if( coord_size < 0 ) coord_size = thCoordSize();
    return coord_size;
}

octStatus
pepPathFollow(obj, foll)
    octObject  *obj;
    octObject  *foll;
{
    octGenerator  Gener;
    int           i;

    if (obj == NULL)
      return OCT_NOT_FOUND;

    switch (obj->type) {
    case OCT_PATH:
	OH_ASSERT(octInitGenContents(obj, OCT_TERM_MASK, &Gener));
	break;

    case OCT_TERM:
	pepReduceTerm(obj);
	OH_ASSERT(octInitGenContainers(obj, OCT_PATH_MASK, &Gener));
	break;

    default:
	return OCT_NOT_FOUND;
    }

    while(octGenerate(&Gener, foll) == OCT_OK) {
	int found = 0;
	for( i = 0; !found ; i++ ) {
	    if ( octIdIsNull(idPath[i]) ) { /* End of array: */
		octFreeGenerator(&Gener); /* Object is good. */
		return OCT_OK;
	    }
	    if ( octIdsEqual(foll->objectId, idPath[i] ) ) {
		found = 1;
	    }
	}
	/* Object already in array, try the next one */
    }
    octFreeGenerator(&Gener);
    return OCT_NOT_FOUND;
}

static double
pepTermRes(term, path1, path2)
    octObject *term;
    octObject *path1, *path2;
/*
 * This function returns the resistance of a connector. If the connector
 * links two segments on the same layer and with the same direction,
 * resistance is computed as
 *     Rs * length / width
 * where Rs is the sheet resistance, length and width are the dimensions
 * of the connector. If the two segments are on the same layer, but in
 * different directions, resistance is given by 1/2 of the previous case.
 * If they are in different layers, the connector is a via.
 */
{
    int32  n;
    octObject  layer;
    octObject  ConnI;		/*  Connector instance  */
    octObject  ConnF;		/*  Connector Facet     */
    octGenerator Gener;
    struct octPoint pts1[2], pts2[2], *pts; /*  Path extreme points */
    struct octBox    bb;
    double  coord_size;
    double  ratio,		/*  Connector aspect ratio  */
    R;				/*  Resistance  */

    ohTerminalBB(term, &bb);
    ratio = ((double)  (bb.upperRight.y - bb.lowerLeft.y)) /
      (bb.upperRight.x - bb.lowerLeft.x);
    ohGetPoints(path1, &n, &pts);
    pts1[0] = pts[0]; pts1[1] = pts[1];
    ohGetPoints(path2, &n, &pts);
    pts2[0] = pts[0]; pts2[1] = pts[1];

    OH_ASSERT(octGenFirstContainer(path1, OCT_LAYER_MASK, &layer));
    if(octIsAttached(&layer, path2) == OCT_OK) {
	if(pts1[0].x == pts1[1].x) {
	    /*  First segment is vertical */
	    if(pts2[0].x == pts2[1].x) {
		return ((double) thGetSheetResistance(&layer) * ratio);
	    } else {
		return ((double) thGetSheetResistance(&layer) * ratio / 2.0);
	    }
	} else {
	    /*  First segment is supposed to be horizontal */
	    if(pts2[0].y == pts2[1].y) {
		return ((double) thGetSheetResistance(&layer) / ratio);
	    } else {
		return ((double) thGetSheetResistance(&layer) / ratio / 2.0);
	    }
	}
    } else {
	/*  Different layers ... this is a via  */
	OH_ASSERT(octGenFirstContainer(term, OCT_INSTANCE_MASK, &ConnI));
	OH_ASSERT(ohOpenMaster(&ConnF, &ConnI, "contents", "r"));
	R = 0;
	OH_ASSERT(octInitGenContents(&ConnF, OCT_LAYER_MASK, &Gener));
	coord_size = getCoordSize();
	while(octGenerate(&Gener, &layer) == OCT_OK) {
	    if(strcmp(thGetLayerClass(&layer), "CONTACT") == 0) {
		ohBB(&layer, &bb);
		R = thGetContactResistance(&layer);
		R /= ((bb.upperRight.x - bb.lowerLeft.x) * coord_size *
		      (bb.upperRight.y - bb.lowerLeft.y) * coord_size);
		break;
	    }
	}
	octCloseFacet(&ConnF);
	return R;
    }
}

double
pepPathRes(path, term1, term2)
    octObject *path;
    octObject *term1, *term2;
/*
 *  This function returns the resistance of a segment, given by
 *      Rs * length / width
 *  where Rs is the sheet resistance. Only the segment portion that
 *  is outside of the terminals is considered.
 */
{
    int32  n;
    struct octPoint  *pts;	/*  Path extreme points */
    struct octBox bb1, bb2;	/*  Terms Bounding boxes */
    octObject  layer;		/*  Path layer  */
    octCoord  l, w;		/*  Final path length and width */

    w = path->contents.path.width;

    ohGetPoints(path, &n, &pts);
    if(n != 2)
      errRaise(PEP_PKG_NAME, 1, "Wrong number of points found in path");

    ohTerminalBB(term1, &bb1);
    ohTerminalBB(term2, &bb2);
    if(pts[0].x == pts[1].x) {
	/*  Vertical segment  */
	if(bb1.lowerLeft.y > bb2.lowerLeft.y)
	  SWAP(struct octBox, bb1, bb2);
	l = bb2.lowerLeft.y - bb1.upperRight.y;
	if(l <= 0) return (double) 0;
    } else {
	/*  Supposedly horizontal segment  */
	if(bb1.lowerLeft.x > bb2.lowerLeft.x)
	  SWAP(struct octBox, bb1, bb2);
	l = bb2.lowerLeft.x - bb1.upperRight.x;
	if(l <= 0) return (double) 0;
    }
  
    octGenFirstContainer(path, OCT_LAYER_MASK, &layer );

    return ((double) l * thGetSheetResistance(&layer) / w);
}

static double
pepSingleResistance(t1, obj, t2)
    octObject *t1, *obj, *t2;
/*  
 *  No checks nor terminal reduction are performed here !!
 */
{
    if(t1->objectId == obj->objectId)
      return ((double) 0.0);

    switch (t1->type ) {
    case OCT_BOX:
    case OCT_POLYGON:
	return ((double) 0.0);

    case OCT_PATH:
	return pepTermRes(obj, t1, t2);

    case OCT_TERM:
	return pepPathRes(obj, t1, t2);
	
    default:
	errRaise(PEP_PKG_NAME, 1, "Cannot measure res on non-geometric objects");
    }
    return 0.0;
}

static double
pepResCalculation(prev, src, tgt)
    octObject  *prev,     /*  Previous object in the path  */
               *src,      /*  Source   */
               *tgt;      /*  Target   */
/*
 *  Service, unexportable  procedure. See pepResistance for the
 *  description.
 */
{
    octObject    next;		/*  Next object in the path  */
    double       Res=(-1.0);	/*  Total resistance to the target */
    double 	 fRes;		/*  Resistance of an edge (parallel)  */
    int          thisPathSize;	/*  Temporary stack size  */


    while(pepPathFollow(src, &next) != OCT_NOT_FOUND) {
    
	idPath[pathSize++] = next.objectId;
	idPath = REALLOC( octId, idPath, pathSize+1 );
	idPath[pathSize] = oct_null_id;
	thisPathSize = pathSize;

	if(next.objectId == tgt->objectId)
	  return pepSingleResistance(prev, src, tgt);

	if((fRes=pepResCalculation(src, &next, tgt)) >= 0) {
	    fRes += pepSingleResistance(prev, src, &next);
	    if(Res < 0)
	      Res = fRes;
	    else
	      Res = (Res * fRes) / (Res + fRes);
	}
	pathSize = thisPathSize;

	idPath[pathSize] = oct_null_id;
    }

    return Res;
}


double
pepResistance(src, tgt)
    octObject *src, *tgt;
/*
 *  Compute the resistance between two given objects, by means of a
 *  dynamic programming approach.
 *  <src> and <tgt> must be of the same type, segments or terminals.
 *  Serial and parallel current flows are considered.
 */
{
    octObject int_src,int_tgt;
    double Res;

    if(src->type != tgt->type)
      errRaise(PEP_PKG_NAME, 1, "Objects must be the same type");

    int_src = *src;		/* Copy objects for manipulation */
    int_tgt = *tgt;		/*  */

    if(int_src.type == OCT_TERM)      pepReduceTerm(&int_src);
    if(int_tgt.type == OCT_TERM)      pepReduceTerm(&int_tgt);

    pathSize = 1;
    idPath = ALLOC( octId, 2 );
    idPath[0] = int_src.objectId;
    idPath[1] = oct_null_id;

    Res = pepResCalculation(&int_src, &int_src, &int_tgt);
    FREE(idPath);
    return Res;
}


/* scan net path and put their terms into an array if they are contained in more than 2 paths or
   if they are part of an instance */

static octId
*scanNetBranches(net1, branchNumber)
    octObject *net1;
    int *branchNumber;
{

    int abutNumber, tempAbutNumber;
    int tempNumber;  
    int idFound, foundTerm;
    octId *abutIdArray, *int_branchIdArray;
    octGenerator pathGen, termGen;
    octObject pathP, termP;
    int i, k;

    tempNumber = 0;
    tempAbutNumber = 0;
    *branchNumber = MIN_TERM_NUM;
    int_branchIdArray = ALLOC(octId, *branchNumber);
    abutNumber = MIN_TERM_NUM;
    abutIdArray = ALLOC(octId, abutNumber);
    OH_ASSERT(octInitGenContents(net1, OCT_PATH_MASK, &pathGen));
    while (octGenerate(&pathGen, &pathP) == OCT_OK) {
	OH_ASSERT(octInitGenContents(&pathP, OCT_TERM_MASK, &termGen));
	while (octGenerate(&termGen, &termP) == OCT_OK) {
	    
	    /* skip it since it is of no interest */
	    if ((ohCountContainers(&termP, OCT_PATH_MASK) <= 2) &&
		strcmp(termP.contents.term.name, "t") == 0) 
	      continue; /* skip operation */ 

	    /* skip it since it is abutted with another one */
	    if (isTermAbutted(&termP)) {
		if (tempAbutNumber + 1 > abutNumber) {
		    abutNumber += MIN_TERM_NUM;
		    abutIdArray = REALLOC(octId, abutIdArray, abutNumber);
		}
		abutIdArray[tempAbutNumber] = termP.objectId;
		tempAbutNumber++;
	    }
	    foundTerm = 0;
	    for (k = 0; k < tempAbutNumber; k++) {
		if (abutIdArray[k] == termP.objectId) foundTerm++;
	    }
	    if (foundTerm > 1) continue; /* skip operation */

	    /* regular case */
	    idFound = 0;
	    for (i = 0; i < tempNumber; i++) {
		if (octIdsEqual(termP.objectId, int_branchIdArray[i])) {
		    idFound = 1;
		    break;
		}
	    }
	    if (idFound) continue;
	    if ( tempNumber + 1 > *branchNumber) {
		*branchNumber += MIN_TERM_NUM;
		int_branchIdArray = REALLOC(octId, int_branchIdArray, *branchNumber);
	    } 
	    int_branchIdArray[tempNumber] = termP.objectId;
	    tempNumber++;
	}
	octFreeGenerator(&termGen);
    }
    octFreeGenerator(&pathGen);
    *branchNumber = tempNumber;
    return int_branchIdArray;

}


/* scan terminals in net "net1" and put them into an array */

int 
scanNetTerminals(net1, termIdArray)
    octObject *net1;
    octId *termIdArray;
{
    int tempNumber, termNumber;
    octGenerator termGen;
    octObject termP;
    
    tempNumber = 0;
    termNumber = MIN_TERM_NUM;
    termIdArray = ALLOC(octId, termNumber);
    OH_ASSERT(octInitGenContents(net1, OCT_TERM_MASK, &termGen));
    while (octGenerate(&termGen, &termP) == OCT_OK) {
	if (octIdIsNull(termP.objectId)) continue;
	if ( tempNumber + 1 > termNumber) {
	    termNumber += MIN_TERM_NUM;
	    termIdArray = REALLOC(octId, termIdArray, termNumber);
	} 
	termIdArray[tempNumber] = termP.objectId;
	tempNumber++;
    }
    octFreeGenerator(&termGen);
    return tempNumber;
}


/* checks if term "termP" is abutted with another term */

int
isTermAbutted(termP)
    octObject *termP;
{
    octObject fProp;

    if ( (ohGetByPropName(termP, &fProp, "TERMAUTO") == OCT_OK) &&
	(fProp.contents.prop.type == OCT_STRING) &&
	(strcmp (fProp.contents.prop.value.string, "ABUTTED") == 0) ) {
	return 1;
    } else {
	return 0;
    }
}

#ifdef NEED_CHOOSETERMCOUPLE
/* chooses a couple of terms in termIdArray wich is not abutted */

static octStatus
chooseTermCouple(f, e, termIdArray, termNumber)
    octObject *f;
    octObject *e;
    octId *termIdArray;
    int termNumber;
{
    int i;
    octObject fProp, eProp;
    octId fPropId, ePropId;

    f->objectId = termIdArray[0];
    octGetById(f);
    if (ohGetByPropName(f, &fProp, "TERMAUTO") == OCT_OK && fProp.contents.prop.type == OCT_STRING &&
	strcmp (fProp.contents.prop.value.string, "ABUTTED") == 0) {
	fPropId = fProp.objectId;
    } else {
	fPropId = oct_null_id;
    } 

    for (i = 1; i < termNumber; i++) {
	e->objectId = termIdArray[i];
	octGetById(e);
	if (ohGetByPropName(e, &eProp, "TERMAUTO") == OCT_OK && eProp.contents.prop.type == OCT_STRING &&
	    strcmp (eProp.contents.prop.value.string, "ABUTTED") == 0) {
	    ePropId = eProp.objectId;
	} else {
	    ePropId = oct_null_id;
	} 
	if ((octIdIsNull (fPropId)) || octIdIsNull (ePropId) ||
	    (!octIdsEqual (fPropId, ePropId))) {
	    return OCT_OK; 
	} else {
	    continue;
	}
    }
    return OCT_NOT_FOUND;

}
#endif

/* follows the net to find the next term in the path */

static octStatus
nextNewTermInPath(obj, follTerm)
    octObject  *obj;
    octObject  *follTerm;
{
    octGenerator  pathGen, termGen;
    octObject     follPath;
    int           i, objFound;

    if (obj == NULL)
      return OCT_NOT_FOUND;
    follTerm->objectId = oct_null_id; /* reset follTerm, it will be filled out with a new one */

    pepReduceTerm(obj);
    OH_ASSERT(octInitGenContainers(obj, OCT_PATH_MASK, &pathGen));
    
    while(octGenerate(&pathGen, &follPath) == OCT_OK) {
	objFound = 0;
	for( i = 0; i < visitedSize ; i++ ) {
	    if ( octIdIsNull(visitedIdPath[i]) ) { /* End of array: */
		break;
	    }
	    if ( octIdsEqual(follPath.objectId, visitedIdPath[i] ) ) {
		objFound = 1;
		break;
	    }
	}
	if (!objFound) {
	    visitedIdPath = REALLOC (octId, visitedIdPath, i + 1);
	    visitedSize = i + 1;
	    visitedIdPath[i] = follPath.objectId;
#ifdef PEP_VERBOSE
	    fprintf(stderr, "%d: Visited path %d\n", i, follPath.objectId);
#endif
	    OH_ASSERT(octInitGenContents(&follPath, OCT_TERM_MASK, &termGen));
	    while(octGenerate(&termGen, follTerm) == OCT_OK) {
		if (octIdsEqual(follTerm->objectId, obj->objectId)) continue; /* original term */
		if (octIdIsNull(follTerm->objectId)) break; /* one of the two terms is zero */
		octFreeGenerator(&termGen);
		octFreeGenerator(&pathGen);
		return OCT_OK;
	    }
	    octFreeGenerator(&termGen);
	} 
    }	    
    octFreeGenerator(&pathGen);
    return OCT_NOT_FOUND;
}


/* computes the resistance for every branch of the net specified 
 * the value of the resistance is stored into an array of structures pepRes
 */
 
pepRes
*pepBranchResistance(net, numberOfBranches)
    octObject *net;
    int *numberOfBranches;
{ 

    double coordSize = 20.0;    /* REMEMBER TO ADD th processing here */
    octObject branch, previousTerm, nextTerm, inst, finalTerm;
    octId *branchIdArray;
    int branchNumber = 0;
    int branchNum, nextBranchNum;
    octPoint centerBranch, centerNew;
    pepRes *res;
    char resName[MAXSTRLEN], node1Name[MAXSTRLEN], node2Name[MAXSTRLEN];
    octStatus test;

    int i, j;
    int flagNext = 0, flagBranch;
    int treeDepth;
    char *tmp1, *tmp2;

    *numberOfBranches = 0;
    branchIdArray = scanNetBranches(net, &branchNumber);
    res = ALLOC (pepRes, branchNumber);

    visitedIdPath = ALLOC (octId, 1);
    visitedSize = 1;
    
    if (branchNumber < 1) {
	fprintf (stderr, "WARNING: Net %s has only a terminal. Skip it\n", net->contents.net.name); 
    }

    for (i = 0; i < branchNumber; i++) {
	branch.objectId = branchIdArray[i];
	octGetById(&branch);
	centerBranch = pepCenterTerm(&branch, coordSize);
	flagBranch = strcmp(branch.contents.term.name, "t");

#ifdef PEP_VERBOSE
	fprintf(stderr, "*****branch node in position (%d,%d) \n", centerBranch.x,  centerBranch.y );
#endif
	branchNum = ohCountContainers(&branch, OCT_PATH_MASK);
	
	for (j = 0; j < branchNum; j++) {
	    previousTerm = branch;
	    treeDepth = 0;
	    while (nextNewTermInPath(&previousTerm, &nextTerm) != OCT_NOT_FOUND) {
		treeDepth++;		
		nextBranchNum = ohCountContainers(&nextTerm, OCT_PATH_MASK);
		centerNew = pepCenterTerm(&nextTerm, coordSize);
		flagNext = strcmp(nextTerm.contents.term.name, "t");
		if (nextBranchNum <= 0) {
		    fprintf (stderr,
			     "terminal (%ld,%ld) attached to no path\n", 
			     (long)centerNew.x, (long)centerNew.y);
		    exit(0);
		} else if (nextBranchNum == 1) {
		    /* final terminal. must be connected to a subcell */
		    break;
		} else if (nextBranchNum == 2) {
		    /* check the presence of an instance */
		    /* term connected to instance with two paths leaving from it */
		    if (flagNext != 0) break;
		    
 		    previousTerm = nextTerm;
		    continue;
		} else {
		    /* branch term */
		    break;
		}
	    }
	    if (!treeDepth) break; 
	    /* all branches of the tree have been extracted */
	    
	    /* check if branch term belongs to a formal term */
	    if (!flagBranch) {
		test = octGenFirstContainer(&branch, OCT_TERM_MASK, &finalTerm);
		if (test == OCT_OK && octIdIsNull(finalTerm.contents.term.instanceId)) { 
		    /*it does*/
		    sprintf(node1Name, "%s", finalTerm.contents.term.name);
		} else {	                  
		    /* it doesn't */
		    sprintf(node1Name, "%ld_%ld",
			    (long)centerBranch.x, (long)centerBranch.y);
		}
	    } else {
		OH_ASSERT(octGenFirstContainer(&branch, OCT_INSTANCE_MASK, &inst));
		sprintf(node1Name, "%s_%s", inst.contents.instance.name, 
			branch.contents.term.name);
	    }
	    
	    /* check if branch term belongs to a formal term */
	    if (!flagNext) {
		test = octGenFirstContainer(&nextTerm, OCT_TERM_MASK, &finalTerm);
		if (test == OCT_OK && octIdIsNull(finalTerm.contents.term.instanceId)) { 
		    /*it does*/
		    sprintf(node2Name, "%s", finalTerm.contents.term.name);
		} else {	                  
		    /* it doesn't */
		    sprintf(node2Name, "%ld_%ld",
			    (long)centerNew.x, (long)centerNew.y);
		}
	    } else {
		OH_ASSERT(octGenFirstContainer(&nextTerm, OCT_INSTANCE_MASK, &inst));
		sprintf(node2Name, "%s_%s", inst.contents.instance.name, 
			nextTerm.contents.term.name);
	    }

	    res[*numberOfBranches].value = pepResistance(&branch, &nextTerm);
	    if (strlen(node1Name) + strlen(node1Name) > MAXSTRLEN) {
		fprintf (stderr, "WARNING: names %s, %s too long. Cutting\n",
			 node1Name, node2Name);
		tmp1 = ALLOC (char, (int)(MAXSTRLEN/2));
		tmp2 = ALLOC (char, (int)(MAXSTRLEN/2));
		strncpy(tmp1, node1Name, (int)(MAXSTRLEN/2)-2);
		strncpy(tmp2, node2Name, (int)(MAXSTRLEN/2)-2);
		sprintf (resName, "%s_%s", tmp1, tmp2);
		res[*numberOfBranches].node1 = tmp1;
		res[*numberOfBranches].node2 = tmp2;
	    } else {
		sprintf (resName, "%s_%s", node1Name, node2Name);
		res[*numberOfBranches].node1 = util_strsav(node1Name);
		res[*numberOfBranches].node2 = util_strsav(node2Name);
	    }
	    res[*numberOfBranches].name = util_strsav (resName);

	    (*numberOfBranches)++;
	}
    }

    FREE (visitedIdPath);
    return res;

}
