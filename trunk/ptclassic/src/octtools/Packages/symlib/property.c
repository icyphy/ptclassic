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
#include "st.h"
#include "oct.h"
#include "oh.h"
#include "symlib.h"
#include "symlib_int.h"


octStatus
symGetProperty(object, prop, name)
octObject *object, *prop;
char *name;
{
    octStatus status;
    octObject inst, master, obj;
    
    status = ohGetByPropName(object, prop, name);
    switch (status) {
	case OCT_OK:
	return OCT_OK;

	case OCT_NOT_FOUND:
	if (object->type == OCT_TERM) {
	    inst.objectId = object->contents.term.instanceId;
	    OCT_CHECK(octGetById(&inst));
	} else {
	    inst = *object;
	}
	OCT_CHECK(ohOpenMaster(&master, &inst, "interface", "r"));
	if (object->type == OCT_TERM) {
	    status = ohGetTerminal(&master, object->contents.term.formalExternalId, &obj);
	    switch (status) {
		case OCT_NOT_FOUND:
		errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "master/instance inconsistency: terminal %s is not in the master",
			 object->contents.term.name);

		case OCT_OK:
		break;

		default:
		RAISE_ERROR;
	    }
	} else {
	    obj = master;
	}

	status = ohGetByPropName(&obj, prop, name);
	switch (status) {
	    case OCT_OK:
	    return OCT_OK;
	    
	    case OCT_NOT_FOUND:
	    return OCT_NOT_FOUND;

	    default:
	    RAISE_ERROR;
	}
	
	default:
	RAISE_ERROR;
    }
    return OCT_OK;
}


static octStatus
sym_prop_gen(state_p, obj)
char **state_p;
octObject *obj;
{
    st_generator *stgen = (st_generator *) *state_p;
    int status;
    char *key, *value;
    
    status = st_gen(stgen, &key, &value);
    if (status == 0) {
	return OCT_GEN_DONE;
    }
    *obj = *((octObject *) value);
    return OCT_OK;
}


static octStatus
sym_prop_free(state)
char *state;
{
    (void) st_free_gen((st_generator *) state);
    return OCT_OK;
}


void
symInitPropertyGen(object, gen)
octObject *object;
octGenerator *gen;
{
    octObject master, instance, obj, *prop;
    octGenerator pgen;
    octStatus status;
    st_table *tbl = st_init_table(strcmp, st_strhash);
    st_generator *stgen;

    if (object->type == OCT_TERM) {
	instance.objectId = object->contents.term.instanceId;
	OCT_CHECK(octGetById(&instance));
    } else {
	instance = *object;
    }
    OCT_CHECK(ohOpenMaster(&master, &instance, "interface", "r"));

    if (object->type == OCT_TERM) {
	status = ohGetTerminal(&master, object->contents.term.formalExternalId, &obj);
	switch (status) {
	    case OCT_NOT_FOUND:
	    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "master/instance inconsistency: terminal %s is not in the master",
		     object->contents.term.name);

	    case OCT_OK:
	    break;

	    default:
	    RAISE_ERROR;
	}
    } else {
	obj = master;
    }

    OCT_CHECK(octInitGenContents(&obj, OCT_PROP_MASK, &pgen));
    prop = ALLOC(octObject, 1);
    while ((status = octGenerate(&pgen, prop)) == OCT_OK) {
	(void) st_insert(tbl, prop->contents.prop.name, (char *) prop);
	prop = ALLOC(octObject, 1);
    }
    (void) free((char *) prop);

    if (status != OCT_GEN_DONE) {
	RAISE_ERROR;
    }
    
    OCT_CHECK(octInitGenContents(object, OCT_PROP_MASK, &pgen));
    prop = ALLOC(octObject, 1);
    while ((status = octGenerate(&pgen, prop)) == OCT_OK) {
	(void) st_insert(tbl, prop->contents.prop.name, (char *) prop);
	prop = ALLOC(octObject, 1);
    }
    (void) free((char *) prop);

    if (status != OCT_GEN_DONE) {
	RAISE_ERROR;
    }

    stgen = st_init_gen(tbl);
    OCT_CHECK(octInitUserGen((char *) stgen, sym_prop_gen, sym_prop_free, gen));
    
    return;
}    
