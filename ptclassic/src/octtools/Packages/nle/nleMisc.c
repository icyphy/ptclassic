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
/*
 *  FILE: nleMisc.c
 *
 *  DESCRIPTION: miscellaneous routines
 *
 *
 *  CREATION DATE: 2/27/87        AUTHOR: A. Casotto
 *
 *  FUNCTIONS:
 *    octStatus InstTermBB(InstTermP,instTermBBox)
 *    boxCenter(boxP , centerP)
 */
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "tr.h"
#include "nle.h"
#include "nleCommon.h"
#include "nleMisc.h"

int
countObjectsInContainer(ContainerP,mask)
octObject *ContainerP;
int    mask;
{
    octGenerator gen;
    octObject Object;
    int    counter = 0;

    OH_ASSERT(octInitGenContents(ContainerP,mask,&gen));
    while(octGenerate(&gen,&Object)==OCT_OK)  counter++;
    return(counter);
}



octObject    LayerPlace; 
octObject    PlacementClass;
octObject    Mobility;
octObject    Direction;
octObject    FloorplanBag;
octObject    ModuleGenerator;
octObject    MinHeightOverWidth;
octObject    MaxHeightOverWidth;
octObject    FloatingEdge;
octObject       TermEdge;
octObject    TermType;
octObject    JoinedTerms;
octObject    Joined;
octObject    OrderTerms;
octObject    PermTerms;
octObject    NetType;
octObject    Criticality;

/* OSP required properties */
octObject    EditStyle;
octObject    ViewType;
octObject    Technology;

/*
 *   initPlacementObjects()
 *   Initializes some useful oct objects
 */
void
initPlacementObjects()
{
    LayerPlace.type = OCT_LAYER;
    LayerPlace.contents.layer.name = "PLACE";

    PlacementClass.type= OCT_PROP;
    PlacementClass.contents.prop.name = "PLACEMENT_CLASS";
    PlacementClass.contents.prop.type = OCT_STRING;

    TermType.type = OCT_PROP;
    TermType.contents.prop.name = "TERMTYPE";
    TermType.contents.prop.type = OCT_STRING;

    Direction.type = OCT_PROP;
    Direction.contents.prop.name = "DIRECTION";
    Direction.contents.prop.type = OCT_STRING;


    JoinedTerms.type = OCT_BAG;
    JoinedTerms.contents.bag.name = "JOINEDTERMS";

    Joined.type = OCT_BAG;
    Joined.contents.bag.name = "JOINED";

    OrderTerms.type = OCT_BAG;
    OrderTerms.contents.bag.name = "ORDERTERMS";

    PermTerms.type = OCT_BAG;
    PermTerms.contents.bag.name = "PERMTERMS";

    NetType.type = OCT_PROP;
    NetType.contents.prop.name = "NETTYPE";
    NetType.contents.prop.type = OCT_STRING;

    Criticality.type = OCT_PROP;
    Criticality.contents.prop.name = "CRITICALITY";
    Criticality.contents.prop.type = OCT_INTEGER;

    Mobility.type = OCT_PROP;
    Mobility.contents.prop.name = "MOBILITY";
    Mobility.contents.prop.type = OCT_STRING;

    FloorplanBag.type = OCT_BAG;
    FloorplanBag.contents.bag.name = "FLOORPLAN";


    ModuleGenerator.type = OCT_PROP;
    ModuleGenerator.contents.prop.name = "MODULE_GENERATOR";
    ModuleGenerator.contents.prop.type = OCT_STRING;

    MinHeightOverWidth.type = OCT_PROP;
    MinHeightOverWidth.contents.prop.name = "MIN_Y_OVER_X";
    MinHeightOverWidth.contents.prop.type = OCT_REAL;

    MaxHeightOverWidth.type = OCT_PROP;
    MaxHeightOverWidth.contents.prop.name = "MAX_Y_OVER_X";
    MaxHeightOverWidth.contents.prop.type = OCT_REAL;

    FloatingEdge.type = OCT_PROP;
    FloatingEdge.contents.prop.name = "TERM_FLOATING_EDGES";
    FloatingEdge.contents.prop.type = OCT_STRING;

    TermEdge.type = OCT_PROP;
    TermEdge.contents.prop.name = "TERM_EDGE";
    TermEdge.contents.prop.type = OCT_STRING;

    FloatingEdge.type = OCT_PROP;
    FloatingEdge.contents.prop.name = "TERM_FLOATING_EDGES";
    FloatingEdge.contents.prop.type = OCT_STRING;

    ViewType.type = OCT_PROP;
    ViewType.contents.prop.name = "VIEWTYPE";
    ViewType.contents.prop.type = OCT_STRING;

    EditStyle.type = OCT_PROP;
    EditStyle.contents.prop.name = "EDITSTYLE";
    EditStyle.contents.prop.type = OCT_STRING;

    Technology.type = OCT_PROP;
    Technology.contents.prop.name = "TECHNOLOGY";

    return;
}


/* Routine to put all the correct properties to a new formal terminal */
void
OSPFormalTerminal(FTermP, type, direction)
octObject *FTermP;
char *type;
char *direction;
{
    if (type) {
        OH_ASSERT(octGetOrCreate(FTermP,&TermType));
        TermType.contents.prop.value.string = type;
        OH_ASSERT(octModify(&TermType));
    } 
    if (direction) {
        OH_ASSERT(octGetOrCreate(FTermP,&Direction));
        Direction.contents.prop.value.string = direction;
        OH_ASSERT(octModify(&Direction));
    }
    return;
}


void
getMasterTerm(it,mt)
octObject *it;        /* instance terminal */
octObject *mt;        /* master terminal */
{
    octObject Instance;
    octObject Master;


    Instance.objectId = it->contents.term.instanceId;
    OH_ASSERT(octGetById(&Instance));
    Master.type = OCT_FACET;
    Master.contents.facet.facet = "interface";
    Master.contents.facet.mode = "r";
    OH_ASSERT_DESCR(octOpenMaster(&Instance,&Master), "cannot open a master");

    mt->type = OCT_TERM;
    mt->contents.term.name = it->contents.term.name;
    OH_ASSERT(octGetByName(&Master,mt));
    return;
}


int
nleCopyProperties(oldcontainer, newcontainer)
octObject *oldcontainer, *newcontainer;
{
    octObject prop;
    octGenerator propgen;
    int    count = 0;

    OH_ASSERT(octInitGenContents(oldcontainer, OCT_PROP_MASK, &propgen));
    while (octGenerate(&propgen, &prop) == OCT_OK) {
        OH_ASSERT(octCreate(newcontainer, &prop));
        count++;
    }
    return(count);
}

#ifdef UNUSED
int
nleCopyContainedBags(oldcontainer, newcontainer, name)
octObject *oldcontainer, *newcontainer;
char *name;            /* Bag Name, can be NULL */
{
    octObject bag;
    octGenerator baggen;
    int    count = 0;

    OH_ASSERT(octInitGenContents(oldcontainer, OCT_BAG_MASK, &baggen));
    while (octGenerate(&baggen, &bag) == OCT_OK) {
        if (name == (char *) 0 || strcmp(name, bag.contents.bag.name)==0) {
            OH_ASSERT(octCreate(newcontainer, &bag));
            count++;
        }
    }
    return(count);
}
#endif

int
nleRecursiveCopyPropAndBags(oldcontainer, newcontainer)
octObject *oldcontainer, *newcontainer;
{
    octObject oldBP,newBP;    /* BP = Bag or Property */
    octGenerator BPgen;
    int    count = 0;

    OH_ASSERT(octInitGenContents(oldcontainer, OCT_PROP_MASK | OCT_BAG_MASK, &BPgen));
    while (octGenerate(&BPgen, &oldBP) == OCT_OK) {
        newBP.objectId = oldBP.objectId;
        OH_ASSERT(octGetById(&newBP));
        OH_ASSERT(octCreate(newcontainer, &newBP));
        nleRecursiveCopyPropAndBags(&oldBP , &newBP);
        count++;
    }
    return(count);
}
