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
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "symlib.h"
#include "symlib_int.h"


void
symCreateFormalTerm(fct, term, name)
octObject *fct, *term;
char *name;
/*
 * create a formal terminal, making sure that a formal terminal of the
 * same name does not already exist
 */
{
    octStatus status;
    
    term->type = OCT_TERM;
    term->contents.term.instanceId = oct_null_id;
    term->contents.term.width = 1;
    if (name != NIL(char)) {
	status = ohGetByTermName(fct, term, name);
	switch (status) {
	    case OCT_OK:
	    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "formal terminal %s already exists",
		     term->contents.term.name);

	    case OCT_NOT_FOUND:
	    OCT_CHECK(octCreate(fct, term));
	    break;

	    default:
	    RAISE_ERROR;
	}
    } else {
	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "formal terminal name can not be NIL");
    }
    
    return;
}


void
symImplementFormalTerm(ft, at)
octObject *ft, *at;
/*
 * implement a formal terminal, making sure that it is not already implemented,
 * and making sure that the actual terminal does not already implement a formal.
 */
{
    octObject dummy, net;
    octStatus status;

    status = octGenFirstContent(ft, OCT_TERM_MASK, &dummy);
    switch (status) {
	case OCT_OK:
	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "formal terminal %s already implemented",
		 ft->contents.term.name);

	case OCT_GEN_DONE:
	break;

	default:
	RAISE_ERROR;
    }

    status = octGenFirstContainer(at, OCT_TERM_MASK, &dummy);
    switch (status) {
	case OCT_OK:
	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "actual terminal %s already implements a formal (%s)",
		 at->contents.term.name, dummy.contents.term.name);

	case OCT_GEN_DONE:
	break;

	default:
	RAISE_ERROR;
    }

    /* to stop symGetNet from returning a bogus new net */
    symGetNet(at, &net);
    OCT_CHECK(octAttach(ft, at));
    symAddTermToNet(ft, &net);
    
    return;
}
