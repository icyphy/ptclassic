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
symCreateInstance(facet, inst)
octObject *facet, *inst;
{
    octObject bag;

    inst->type = OCT_INSTANCE;
    inst->contents.instance.name = NIL(char);
    OCT_CHECK(octCreate(facet, inst));
    OCT_CHECK(ohGetOrCreateBag(facet, &bag, (symConnectorp(inst) ? "CONNECTORS" : "INSTANCES")));
    OCT_CHECK(octAttach(&bag, inst));
    return;
}

    
int
symConnectorp(object)
octObject *object;
{
    octObject facet, master, prop, instance, ibag, cbag;

    octGetFacet(object, &facet);
    OCT_CHECK(ohGetOrCreateBag(&facet, &ibag, "INSTANCES"));
    OCT_CHECK(ohGetOrCreateBag(&facet, &cbag, "CONNECTORS"));
    
    switch (object->type) {
	case OCT_TERM:
	if (SYM_FTERMP(object)) {
	    return 0;
	}
	instance.objectId = object->contents.term.instanceId;
	OCT_CHECK(octGetById(&instance));
	break;

	case OCT_INSTANCE:
	instance = *object;
	break;

	default:
	return 0;
    }
    
    if (octIsAttached(&cbag, &instance) == OCT_OK) {
	return 1;
    }
    if (octIsAttached(&ibag, &instance) == OCT_OK) {
	return 0;
    }
    OCT_CHECK(ohOpenMaster(&master, &instance, "interface", "r"));

    if (ohGetByPropName(&master, &prop, "CELLTYPE") < OCT_OK) {
	return 0;
    }

    if ((prop.contents.prop.type != OCT_STRING) ||
	(prop.contents.prop.value.string == NIL(char))) {
	return 0;
    }

    return (strcmp(prop.contents.prop.value.string, "CONTACT") == 0) ? 1 : 0;
}


void
symInitInstanceGen(facet, type, gen)
octObject *facet;
int type;
octGenerator *gen;
{
    octObject bag;
    
    OCT_CHECK(ohGetByBagName(facet, &bag, ((type == SYM_INSTANCES) ? "INSTANCES" : "CONNECTORS")));
    OCT_CHECK(octInitGenContents(&bag, OCT_INSTANCE_MASK, gen));
    return;
}

    
void
symReplaceInstance(old, new)
octObject *old, *new;
{
    octObject facet, oterm, nterm;
    octGenerator tgen;
    octStatus status;

    if ((strcmp(old->contents.instance.master, new->contents.instance.master) == 0)
	&& (strcmp(old->contents.instance.view, new->contents.instance.view) == 0)) {
	*new = *old;
	return;
    }

    octGetFacet(old, &facet);
    new->contents.instance.transform.translation = old->contents.instance.transform.translation;
    OCT_CHECK(octCreate(&facet, new));

    /* fix terminal attachments */
    OCT_CHECK(octInitGenContents(new, OCT_TERM_MASK, &tgen));
	
    while ((status = octGenerate(&tgen, &nterm)) == OCT_OK) {
	/* XXX fix this (XID) */
	status = ohGetByTermName(old, &oterm, nterm.contents.term.name);
	switch (status) {
	    case OCT_OK:
	    break;

	    case OCT_NOT_FOUND:
	    errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "Replacing %s:\n\tmissing terminal correspondence (%s)",
		     ohFormatName( old ),
		     nterm.contents.term.name);

	    default:
	    RAISE_ERROR;
	}
	    
	ohAttachContainers(&oterm, &nterm, OCT_ALL_MASK&~OCT_INSTANCE_MASK);
	ohAttachContents(&oterm, &nterm, OCT_ALL_MASK);
    }

    if (status != OCT_GEN_DONE) {
	RAISE_ERROR;
    }

    ohAttachContainers(old, new, OCT_ALL_MASK&~OCT_FACET_MASK);
    ohAttachContents(old, new, OCT_ALL_MASK&~OCT_TERM_MASK);

    OCT_CHECK(octDelete(old));

    return;
}
