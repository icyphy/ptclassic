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
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "symlib.h"
#include "symlib_int.h"


void
symOpenFacet(facet, mode, technology)
octObject *facet;
char *mode;
char *technology;
{
    octObject prop;

    facet->type = OCT_FACET;
    facet->contents.facet.mode = mode;
    OCT_CHECK(octOpenFacet(facet));
    OCT_CHECK(ohGetOrCreatePropStr(facet, &prop, "EDITSTYLE", "SYMBOLIC"));
    OCT_CHECK(ohGetOrCreatePropStr(facet, &prop, "VIEWTYPE", "SYMBOLIC"));
    OCT_CHECK(ohGetOrCreatePropStr(facet, &prop, "TECHNOLOGY", technology));
    return;
}

    
