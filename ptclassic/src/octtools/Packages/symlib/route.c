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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"
#include "oct.h"
#include "tap.h"
#include "symlib.h"
#include "symlib_int.h"


void
symRoute(array, count)
symSegment *array;
int count;
{
    int i;
    octObject term1, term2;

    term1.objectId = oct_null_id;
    term2.objectId = oct_null_id;
    symCreateSegment(&(array[0].layer),
		     &(array[0].pt1), &term1,
		     SYM_USE_EXISTING_OR_CREATE,
		     &(array[0].pt2), &term2,
		     SYM_USE_EXISTING_OR_CREATE,
		     array[0].width,
		     array[0].direction);
    for (i = 1; i < count; i++) {
	switch (i % 2) {
	    case 0:
	    term2.objectId = oct_null_id;
	    symCreateSegment(&(array[i].layer),
			     &(array[i].pt1), &term1,
			     SYM_USE_EXISTING,
			     &(array[i].pt2), &term2,
			     SYM_USE_EXISTING_OR_CREATE,
			     array[i].width,
			     array[i].direction);
	    break;
	    
	    case 1:
	    term1.objectId = oct_null_id;
	    symCreateSegment(&(array[i].layer),
			     &(array[i].pt1), &term2,
			     SYM_USE_EXISTING,
			     &(array[i].pt2), &term1,
			     SYM_USE_EXISTING_OR_CREATE,
			     array[i].width,
			     array[i].direction);
	    break;
	}
	
    }
    return;
}
