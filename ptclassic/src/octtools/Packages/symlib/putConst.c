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
#include "utility.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "symlib.h"


static void constraintBag( facet, bag, direction )
    octObject* facet;
    octObject* bag;
    int    direction;
{
    char* name =  (direction == SYM_HORIZONTAL )  ? "XCONSTRAINTS" : "YCONSTRAINTS" ;
    ohGetOrCreateBag( facet, bag, name );
    return; 
}

static char* sideConstraintBag( facet, bag, side )
    octObject* facet;
    octObject* bag;
    int    side;
{
    switch ( side ) {
    case SYM_PUT_CONST_LEFT:
	constraintBag( facet, bag, SYM_HORIZONTAL );
	return "LEFT";
    case SYM_PUT_CONST_RIGHT:
	constraintBag( facet, bag, SYM_HORIZONTAL );
	return "RIGHT";
    case SYM_PUT_CONST_TOP:
	constraintBag( facet, bag, SYM_VERTICAL );
	return "TOP";
    case SYM_PUT_CONST_BOTTOM:
	constraintBag( facet, bag, SYM_VERTICAL );
	return "BOTTOM";
    }
    return "Error: sideContraintBag";
}


static int putBounds( constBag, min, max )
    octObject* constBag;
    int min;
    int max;
{
    octObject oProp;
    /*
     *  Check the min and max values.
     */
    if (min != SYM_PUT_CONST_UNDEFINED)  {
	ohCreatePropInt( constBag, &oProp, "MIN", min );
    }
    if (max != SYM_PUT_CONST_UNDEFINED)  {
	ohCreatePropInt( constBag, &oProp, "MAX", max );
    }
    return SYM_OK;
}


int symNormalConstraint( direction, oInst1, oInst2 , min, max )
    int direction;
    octObject *oInst1;
    octObject *oInst2;
    int min;
    int max;
{
    octObject facet;
    octObject firstBag;
    octObject constBag;

    octGetFacet( oInst1, &facet );
    constraintBag( &facet, &firstBag,  direction) ;

    ohCreateBag( &firstBag, &constBag, "CONSTRAINT" );
    if ( putBounds( &constBag, min, max ) == SYM_ERROR) {
	return SYM_ERROR;
    }
    octAttach(&constBag, oInst1);
    octAttach(&constBag, oInst2);

    return SYM_OK;
}


int symSideConstraint( firstInst, side, min, max )
    octObject* firstInst;
    int side;
    int min;
    int max;
{
    octObject facet;
    octObject firstBag;
    octObject secondBag;
    octObject constBag;
    char* name;
   
    octGetFacet( firstInst, &facet );

    name = sideConstraintBag( &facet, &firstBag, side );

    ohGetOrCreateBag(&firstBag, &secondBag, name );
    ohCreateBag( &secondBag, &constBag, "CONSTRAINT" );
    if ( putBounds( &constBag, min, max ) == SYM_ERROR ) {
	return SYM_ERROR;
    }
    octAttach( &constBag, firstInst);

    return SYM_OK;
}

int symCleanAllConstraints( facet )
    octObject *facet;
{
    octObject topbag;
    if (ohGetByBagName( facet, &topbag, "XCONSTRAINTS" ) == OCT_OK) {
	ohRecursiveDelete( &topbag, OCT_BAG_MASK | OCT_PROP_MASK );
    }
    if (ohGetByBagName( facet, &topbag, "YCONSTRAINTS" ) == OCT_OK) {
	ohRecursiveDelete( &topbag, OCT_BAG_MASK | OCT_PROP_MASK );
    }
    return SYM_OK;
}


static char* printBound( b , lambda )
    int b;
    int lambda;
{
    static char buf[64];
    if ( b == SYM_PUT_CONST_UNDEFINED)  {
	return "---";
    } else {
	sprintf( buf, "%d", b / lambda );
    }
    return buf;
}

static char* printName( inst )
    octObject* inst;
{
    static char buf[128];
    char* name;
    octObject term;
    if ( symConnectorp( inst ) ) {
	OH_ASSERT( octGenFirstContent( inst, OCT_TERM_MASK, &term ) );
	if ( octGenFirstContainer( &term, OCT_TERM_MASK, &term ) == OCT_OK ) {
	    strcpy( buf, term.contents.term.name );
	    return buf;
	}
    }
    name = inst->contents.instance.name;
    if ( strlen( name ) == 0 ) {
	return "instance_with_no_name";
    }
    strcpy( buf, name );
    return buf;
}

void symPrintConstraints( facet, dir)
    octObject* facet;
    char* dir;
{
    octObject topbag;
    octGenerator genbag, genbag2;
    octGenerator genInst;
    octObject bag, bag2;
    octObject min;
    octObject max;
    octObject inst1, inst2;
    char bagname[20];
    char* minStr;
    char* maxStr;
    char* name1;
    char* name2;
    int lambda = 1;		/* Default */
    
    if ( tapGetProp( facet, TAP_LAMBDA ) ) {
	lambda = TAP_LAMBDA->value.integer;
    }

    strcpy( bagname, dir );
    strcat( bagname, "CONSTRAINTS" );
    if (ohGetByBagName( facet, &topbag, bagname ) == OCT_OK) {
	octInitGenContents( &topbag, OCT_BAG_MASK, &genbag );
	
	while ( octGenerate( &genbag, &bag ) == OCT_OK ) {
	    /* If the props are missing, then the bound is undefined. */
	    min.contents.prop.value.integer = SYM_PUT_CONST_UNDEFINED;
	    max.contents.prop.value.integer = SYM_PUT_CONST_UNDEFINED;
	    if ( strcmp( bag.contents.bag.name, "CONSTRAINT" ) == 0 ) {
		ohGetByPropName( &bag, &min, "MIN" );
		ohGetByPropName( &bag, &max, "MAX" );
		octInitGenContents( &bag, OCT_INSTANCE_MASK, &genInst );
		octGenerate( &genInst, &inst1 );
		octGenerate( &genInst, &inst2 );
		minStr = util_strsav( printBound(min.contents.prop.value.integer, lambda ) );
		maxStr = util_strsav( printBound(max.contents.prop.value.integer, lambda ) );
		
		name1 = util_strsav( printName( &inst1 ) );
		name2 = util_strsav( printName( &inst2 ) );
		printf( "NORMAL %s %s %s %s %s\n",
		       dir, name1, name2, minStr, maxStr );
		free( minStr );
		free( maxStr );
		free( name1 );
		free( name2 );
	    } else {
		OH_ASSERT(octInitGenContents( &bag, OCT_BAG_MASK, &genbag2 ));
		while ( octGenerate( &genbag2, &bag2 ) == OCT_OK ) {
		    OH_ASSERT(ohGetByPropName( &bag2, &min, "MIN" ));
		    OH_ASSERT(ohGetByPropName( &bag2, &max, "MAX" ));
		    OH_ASSERT(octInitGenContents( &bag2, OCT_INSTANCE_MASK, &genInst ));
		    OH_ASSERT(octGenerate( &genInst, &inst1 ));
		    minStr = util_strsav( printBound(min.contents.prop.value.integer, lambda) );
		    maxStr = util_strsav( printBound(max.contents.prop.value.integer, lambda) );
		    name1 = util_strsav( printName( &inst1 ) );
		    printf( "SIDE %s %s %s %s\n",
			   bag.contents.bag.name,
			   name1, minStr, maxStr );
		    free( minStr );
		    free( maxStr );
		    free( name1 );
		}
	    }
	}
    }
}
    
int symActiveConstraint( direction, inst1, inst2, inst3, inst4  )
    int direction;
    octObject *inst1;
    octObject *inst2;
    octObject *inst3;
    octObject *inst4;
{
    octObject facet;
    octObject pairBag, directionBag, constBag;

    octGetFacet( inst1, &facet );
    constraintBag( &facet, &directionBag,  direction) ;
    /*
     *  Each pair is attached to a PAIR bag that hangs off of the
     *  ACONSTRAINT bag.
     */
    ohCreateBag( &directionBag, &constBag, "ACONSTRAINT" );

    ohCreateBag( &constBag, &pairBag, "PAIR" );
    octAttach(&pairBag, inst1);
    octAttach(&pairBag, inst2);

    ohCreateBag( &constBag, &pairBag, "PAIR" );
    octAttach(&pairBag, inst3);
    octAttach(&pairBag, inst4);

    return SYM_OK;
}


int symActiveSideConstraint( side, inst1, inst2  )
    int side;
    octObject *inst1;
    octObject *inst2;
{
    octObject facet;
    octObject edgeBag, directionBag, constBag;
    char* name;

    octGetFacet( inst1, &facet );

    name = sideConstraintBag( &facet, &directionBag, side );

    /*
     *  Each pair is attached to a PAIR bag that hangs off of the
     *  ACONSTRAINT bag.
     */
    ohCreateBag( &directionBag, &edgeBag, name );
    ohCreateBag( &directionBag, &constBag, "ACONSTRAINT" );
    octAttach( &constBag, inst1 );
    octAttach( &constBag, inst2 );

    return SYM_OK;
}


void symPrintAllConstraints( facet )
    octObject* facet;
{
    symPrintConstraints( facet, "X" );
    symPrintConstraints( facet, "Y" );
}




