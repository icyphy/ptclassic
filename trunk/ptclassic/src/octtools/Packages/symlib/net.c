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
symCreateNet(fct, net)
octObject *fct, *net;
{
    net->type = OCT_NET;
    net->contents.net.name = NIL(char);
    net->contents.net.width = 1;
    OCT_CHECK(octCreate(fct, net));
    return;
}


void
sym_actual_formal_merge(term)
octObject *term;
{
    octObject net1, term2, net2;
    octStatus status;

    if (SYM_FTERMP(term)) {
	status = octGenFirstContent(term, OCT_TERM_MASK, &term2);
    } else {
	status = octGenFirstContainer(term, OCT_TERM_MASK, &term2);
    }
    switch (status) {
	case OCT_GEN_DONE:
	return;

	case OCT_OK:
	symGetNet(term, &net1);
	symGetNet(&term2, &net2);
	sym_merge_nets(&net1, &net2);
	return;

	default:
	RAISE_ERROR;
    }
    return;
}


static void
sym_handle_joined_bag(term)
octObject *term;
{
    octObject inst, master, fterm, bag, aterm, anet, net;
    octGenerator gen;
    octStatus status;

    if (SYM_FTERMP(term)) {
	return;
    }

    OCT_CHECK(octGenFirstContainer(term, OCT_NET_MASK, &net));
    
    inst.objectId = term->contents.term.instanceId;
    OCT_CHECK(octGetById(&inst));
    OCT_CHECK(ohOpenMaster(&master, &inst, "interface", "r"));
    status = ohGetTerminal(&master, term->contents.term.formalExternalId, &fterm);
    switch (status) {
	case OCT_NOT_FOUND:
	errRaise(SYMLIB_PKG_NAME, SYM_ERROR, "master/instance inconsistency: terminal %s is not in the master",
		       term->contents.term.name);

	case OCT_OK:
	break;

	default:
	RAISE_ERROR;
    }
    
    bag.type = OCT_BAG;
    bag.contents.bag.name = "JOINED";
    status = octGetContainerByName(&fterm, &bag);
    switch (status) {
	case OCT_NOT_FOUND:
	break;
	
	case OCT_OK:
	OCT_CHECK(octInitGenContents(&bag, OCT_TERM_MASK, &gen));
	while ((status = octGenerate(&gen, &fterm)) == OCT_OK) {
	    OCT_CHECK(octGenFirstContainer(term, OCT_NET_MASK, &net));
	    OCT_CHECK(ohGetByTermName(&inst, &aterm, fterm.contents.term.name));
	    status = octGenFirstContainer(&aterm, OCT_NET_MASK, &anet);
	    switch (status) {
		case OCT_OK:
		sym_merge_nets(&anet, &net);
		break;

		case OCT_GEN_DONE:
		OCT_CHECK(octAttach(&net, &aterm));
		break;

		default:
		RAISE_ERROR;
		break;
	    }
	    sym_actual_formal_merge(&aterm);
	}

	if (status != OCT_GEN_DONE) {
	    RAISE_ERROR;
	}
	break;

	default:
	RAISE_ERROR;
    }
    return;
}


void
symGetNet(term, net)
octObject *term, *net;
{
    octObject facet, path, fterm;
    octStatus status;

    octGetFacet(term, &facet);

    /* check for connector instance */
    if (symConnectorp(term)) {
	/* does it implement a formal terminal */
	status = octGenFirstContainer(term, OCT_TERM_MASK, &fterm);
	switch (status) {
	    case OCT_GEN_DONE:
	    /* nope */
	    break;

	    case OCT_OK:
	    /* yep */
	    status = octGenFirstContainer(&fterm, OCT_NET_MASK, net);
	    switch (status) {
		case OCT_GEN_DONE:
		/* no net on the formal teminal - error?  merge? XXX */
		symCreateNet(&facet, net);
		symAddTermToNet(&fterm, net);
		return;

		case OCT_OK:
		return;

		default:
		RAISE_ERROR;
	    }		

	    default:
	    RAISE_ERROR;
	}		

	status = octGenFirstContainer(term, OCT_PATH_MASK, &path);
	switch (status) {
	    case OCT_GEN_DONE:
	    net->objectId = oct_null_id;
	    return;

	    case OCT_OK:
	    status = octGenFirstContainer(&path, OCT_NET_MASK, net);
	    switch (status) {
		case OCT_GEN_DONE:
		symCreateNet(&facet, net);
		OCT_CHECK(octAttach(net, &path));
		return;

		case OCT_OK:
		return;

		default:
		RAISE_ERROR;
	    }		

	    default:
	    RAISE_ERROR;
	}
    }
    
    status = octGenFirstContainer(term, OCT_NET_MASK, net);
    switch (status) {
	case OCT_GEN_DONE:
	symCreateNet(&facet, net);
	symAddTermToNet(term, net);
	return;

	case OCT_OK:
	return;

	default:
	RAISE_ERROR;
    }
    return;
}


void
symAddTermToNet(term, net)
octObject *term, *net;
{
    octObject nnet, facet;
    octStatus status;
    
    /* check for connector instance */
    if (symConnectorp(term)) {
	return;
    }

    status = octGenFirstContainer(term, OCT_NET_MASK, &nnet);
    switch (status) {
	case OCT_OK:
	sym_merge_nets(net, &nnet);
	break;

	case OCT_GEN_DONE:
	if (octIdIsNull(net->objectId)) {
	    octGetFacet(term, &facet);
	    symCreateNet(&facet, net);
	}
	OCT_CHECK(octAttach(net, term));
	break;

	default:
	RAISE_ERROR;
    }

    sym_actual_formal_merge(term);
    sym_handle_joined_bag(term);
    symGetNet(term, net);
    return;
}


#define SYM_NIL_NET_NAME(netobject) \
    (((netobject)->contents.net.name == NIL(char)) \
     || (strcmp((netobject)->contents.net.name, "") == 0))

static void
sym_merge_function_default(net1, net2, keep, merge)
octObject *net1, *net2, *keep, *merge;
{
    /* keep named net, if both named, keep the shortest name */
    
    if (SYM_NIL_NET_NAME(net1)) {
	*keep = *net2;
	*merge = *net1;
	return;
    }
    if (SYM_NIL_NET_NAME(net2)) {
	*keep = *net1;
	*merge = *net2;
	return;
    }
    if (strlen(net1->contents.net.name) < strlen(net2->contents.net.name)) {
	*keep = *net1;
	*merge = *net2;
    } else {
	*keep = *net2;
	*merge = *net1;
    }	
    return;
}


static void (*sym_merge_function)() = sym_merge_function_default;

void
symSetMergeFunction(mergeNetsFunction)
void (*mergeNetsFunction)();
{
    if (mergeNetsFunction == 0) {
	sym_merge_function = sym_merge_function_default;
    }
    sym_merge_function = mergeNetsFunction;
    return;
}
    

void
sym_merge_nets(net1, net2)
octObject *net1, *net2;
{
    octObject keepNet, mergeNet;

    if ((octIdIsNull(net1->objectId)) ||
	(octIdIsNull(net2->objectId))) {
	return;
    }
    
    if (octIdsEqual(net1->objectId, net2->objectId)) {
	return;
    }

    (*sym_merge_function)(net1, net2, &keepNet, &mergeNet);
    ohAttachContents(&mergeNet, &keepNet, OCT_ALL_MASK);
    ohAttachContainers(&mergeNet, &keepNet, OCT_ALL_MASK&~OCT_FACET_MASK);
    OCT_CHECK(octDelete(&mergeNet));

    return;
}
