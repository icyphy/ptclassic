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
#include "symlib.h"
#include "symlib_int.h"


void
symNameObject(object, name)
octObject *object;
char *name;
{
    octObject facet, obj;
    octStatus status;

    octGetFacet(object, &facet);

    obj.type = object->type;
    switch (object->type) {
	case OCT_TERM:
	if (!SYM_FTERMP(object)) {
	    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "can not name actual terminals");
	}
	object->contents.term.name = obj.contents.term.name = name;
	obj.contents.term.instanceId = oct_null_id;
	break;

	case OCT_NET:
	object->contents.net.name = obj.contents.net.name = name;
	break;

	case OCT_INSTANCE:
	object->contents.instance.name = obj.contents.instance.name = name;
	break;

	default:
	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "can only name nets, formal terminals, and instances");
    }
    
    if (name != NIL(char)) {
	status = octGetByName(&facet, &obj);
	switch (status) {
	    case OCT_OK:
	    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "object named %s already exists", name);

	    case OCT_NOT_FOUND:
	    break;

	    default:
	    RAISE_ERROR;
	}
    }
    OCT_CHECK(octModify(object));
    return;
}
