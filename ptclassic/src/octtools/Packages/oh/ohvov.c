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
#include "oct.h"
#include "utility.h"
#include "oh.h"
#include "vov.h"

void ohWorkFacet( workFacetP, inName, outName )
    octObject* workFacetP;		/* The working facet. */
    char* inName;
    char* outName;
/*
 * Prepare facet to work on. If outName is NULL, then 
 * facet is the one specified by inName, and it is opened in "a" mode.
 * If outName is specified, the routine copies the facet specified
 * by inName into a facet specified by outName, and opens the
 * new facet with mode "a".
 */
{
    octStatus s;
    octObject f;		/* Allocate some space on stack */
    octObject* inputFacetP;	/* Use this just because the code looks more symmetric. */
    inputFacetP = &f;		/*  */

    inputFacetP->objectId = oct_null_id;	/* Initialize properly. */
    workFacetP->objectId = oct_null_id;	/* Initialize properly. */

    if ( inName == NIL(char) ) {
	errRaise( OH_PKG_NAME, 1, "ohWorkFacet: inName cannot be null" );
    }
    ohUnpackDefaults( inputFacetP, "a", "::contents" );
    ohUnpackFacetName( inputFacetP, inName );
    
    if ( outName ) {
	struct octFacetInfo info;
	*workFacetP = *inputFacetP;
        ohUnpackFacetName( workFacetP, outName );
	inputFacetP->contents.facet.facet = "interface";
	if ( octGetFacetInfo( inputFacetP, &info ) == OCT_OK ) {
	    workFacetP->contents.facet.facet = "interface";
	    VOVinputFacet( inputFacetP );
	    VOVoutputFacet( workFacetP );
	    OH_ASSERT( octCopyFacet( workFacetP, inputFacetP ) ) ;
	} 

	inputFacetP->contents.facet.facet = "contents";
	workFacetP->contents.facet.facet = "contents";
	VOVinputFacet( inputFacetP );
	VOVoutputFacet( workFacetP );
	OH_ASSERT( octCopyFacet( workFacetP, inputFacetP ) ) ;
    } else {
	*workFacetP = *inputFacetP;
	VOVinputFacet( workFacetP );	/* Both input and output (run in place)  */
	VOVoutputFacet( workFacetP );
    }
    s =  octOpenFacet( workFacetP );
    if ( s < OCT_OK ) {
	errRaise( OH_PKG_NAME, 1, "Cannot open %s", ohFormatName( workFacetP ) );
    } 
    if ( s == OCT_NEW_FACET ) {
	errRaise( OH_PKG_NAME, 1, "Facet does not exist %s", ohFormatName( workFacetP ) );
    }


}



void ohInputFacet( workFacetP, inName )
    octObject* workFacetP;		/* The working facet. */
    char* inName;
/*
 * Prepare facet to work on. 
 */
{
    octStatus s;

    workFacetP->objectId = oct_null_id;	/* Initialize properly. */

    if ( inName == NIL(char) ) {
	errRaise( OH_PKG_NAME, 1, "ohWorkFacet: inName cannot be null" );
    }

    ohUnpackDefaults( workFacetP, "r", "::contents" );
    ohUnpackFacetName( workFacetP, inName );
    
    VOVinputFacet( workFacetP );	/* Both input and output (run in place)  */

    s =  octOpenFacet( workFacetP );
    if ( s < OCT_OK ) {
	errRaise( OH_PKG_NAME, 1, "Cannot open %s", ohFormatName( workFacetP ) );
    } 
    if ( s == OCT_NEW_FACET ) {
	errRaise( OH_PKG_NAME, 1, "Facet does not exist %s", ohFormatName( workFacetP ) );
    }

}

