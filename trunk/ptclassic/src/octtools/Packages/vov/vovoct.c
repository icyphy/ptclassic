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
#include "vov.h"
#include "utility.h"
#include "oct.h"
#include "errtrap.h"

#include "vov_lib.h"

static char* facet_fullname(facet)
    octObject* facet;
/*
* A small utility to get the full name of a facet.
* protect it if the objectId is not valid.
*/
{
    char* fullname = facet->contents.facet.cell; /* Default */
    octObject tmpFacet;
    tmpFacet.objectId = facet->objectId;
    if ( octGetById( &tmpFacet ) == OCT_OK ) {	 /* If ID is good,.. */
	if ( tmpFacet.type == OCT_FACET ) {	 /* Make sure it is a facet. */
	    if ( strcmp( fullname, tmpFacet.contents.facet.cell ) == 0 ) {
		octFullName( &tmpFacet, &fullname);	 /* Only now get the full name. */
	    }
	}
    } 
    return fullname;
}


int VOVinputFacet( facet )
    octObject* facet;
{
    if ( ! libActive(0) ) return 0;
    {
	char buf[1024];
	sprintf( buf, "%s:%s:%s", 
		facet_fullname(facet),
		facet->contents.facet.view ,
		facet->contents.facet.facet );
	return VOVinput( VOV_OCT_FACET, buf );
    }
}

int VOVoutputFacet( facet )
    octObject* facet;
{
    if ( ! libActive(0) ) return 0;
    {
	char buf[1024];
	sprintf( buf, "%s:%s:%s", 
		facet_fullname(facet),
		facet->contents.facet.view ,
		facet->contents.facet.facet );
	return VOVoutput( VOV_OCT_FACET, buf );
    }
}





