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
/* compile.c  edg

Version identification:
$Id$

There are two representations of a design: the Oct one and the internal
kernel one.  The compile routines keep these representations consistent.
The name we give to a kernel object is just the base name of its facet,
so it is possible for more than one facet to map to the same kernel object.
To avoid confusion, we use the kernel's "definition source" information
to store the full pathname of the source facet for any compiled galaxy
or universe.  A facet must be recompiled if it is not known to the kernel,
or if the definition source does not match, or if the Oct facet has been
changed since the last transfer.  (Oct change lists in each facet keep
track of the latter condition.)

We also keep a "traverse list" of galaxy facets visited during the current
scan of the hierarchical Oct design.  (The Oct design can be represented as
a directed acyclic graph (DAG), with each node in the DAG corresponding to
an Oct facet.)  traverse prevents visiting a node twice.  This saves duplicate
compilations when a subgalaxy occurs multiple times, and allows us to detect
two kinds of error conditions:
 1. Recursive references (cycles in the DAG), which would otherwise cause
    infinite recursion and coredump.
 2. Two different facets used in the same design that map to the same
    kernel object name.  This cannot be supported without using more
    complicated kernel object names (ie, the full path instead of only
    the base facet name); the feature is not deemed worth the extra
    verbosity that would ensue.
Case 1 is detected when a facet is visited that is already in the traverse
list, but the kernel does not have a matching object (hence we have not
finished traversing the facet).
Case 2 is detected when a facet is visited that is already in the traverse
list, and the kernel has a matching object, but the kernel object has
the wrong definition source.
Note: it is possible to generate the wrong error message, if a recursive
loop occurs in a facet for which a conflicting definition was previously
compiled.  Then we will detect case 2 although it's really case 1.
It's still an error, so this is not very critical.

At first glance it might seem sufficient to store full facet pathnames
in the traverse list.  But the same galaxy facet can map to multiple
kernel objects depending on the domain of the referencing facet, because
we compile a wormhole belonging to the outer domain if it's different.
So the traverse list is indexed by pathname and outer domain.
 
Universes need not be entered in the traverse list at all, since a
universe facet cannot be visited recursively.  If we did not maintain
a strict distinction between universes and galaxies in the Oct representation,
this would not be so.
*/

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* Octtools include */
#include "oct.h"		/* define octObject */
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define remote procedure calls */
#include "oh.h"

/* Pigilib includes */
#include "compile.h"
#include "paramStructs.h"
#include "vemInterface.h"
#include "util.h"
#include "err.h"
#include "main.h"
#include "octIfc.h"
#include "octMacros.h"
#include "ptk.h"
#include "kernelCalls.h"
#include "handle.h"

/* maximum number of actual terms allowed on a net */
#define TERMS_MAX 50

/* Scratch string for constructing error messages */
static char msg[1024];

/* used to traverse design hierarchy which is a DAG (directed acyclic graph) */
static DupSheet traverse;


/***** Begin Error Select Set (Ess) routines */
/* 8/20/89
Caveat: Ess routines create a select set in only one facet, so all
objects added to the select set must be in the same facet!
*/
static vemSelSet essSelSet;		/* for highlighting errors */
static boolean essExist = FALSE;

boolean
EssClear()
{
    if (essExist) {
	ERR_IF2(vemFreeSelSet(essSelSet) != VEM_OK,
	    "EssClear: Cannot free error select set");
	essExist = FALSE;
    }
    return (TRUE);
}

boolean
EssAddObj(obj)
octObject *obj;
{
    octObject facet = {OCT_UNDEFINED_OBJECT, 0};
    if (!essExist) {
	octGetFacet(obj, &facet);
	essSelSet = vemNewSelSet(facet.objectId, 65535, 0, 0, 2, 3, 1, 1, "0");
	essExist = TRUE;
    }
    ERR_IF2(vemAddSelSet(essSelSet, obj->objectId) != VEM_OK,
	"EssAddObj: Cannot put object in error select set");
    FreeOctMembers(&facet);
    return (TRUE);
}
/***** End Ess routines (Error Select Set) */


static boolean
ProcessFormalParams(galFacetPtr)
octObject *galFacetPtr;
{
    ParamListType pList = {0, 0, 0, FALSE};
    ParamType *p;
    int i;
    int retval = TRUE;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));
    for (i = 0, p = pList.array; i < pList.length; i++, p++) {
	if (!p->type || p->type[0] == 0)
	    p->type = "FLOAT"; /* backward compatibility */
	if (!KcMakeState(p->name, p->type, p->value)) {
	   retval = FALSE;
	   break;
	}
    }
    FreeFlatPList(&pList);
    return(retval);
}

static boolean
ProcessTargetParams(targName, galFacetPtr)
char* targName;
octObject *galFacetPtr;
{
    ParamListType pList = {0, 0, 0, FALSE};
    int retval = TRUE;
    if (targName[0] == '<') return TRUE;
    if (!GetTargetParams(targName, galFacetPtr, &pList)) {
	retval = FALSE;
    }
    else if (!KcModTargetParams(&pList)) {
	retval = FALSE;
    }
    FreeFlatPList(&pList);
    return(retval);
}

/* This function will be called recursively if there's hierarchy */
/* Only free local octObject variable inst in the while loop */
/* Do not free galFacet */
static boolean
ProcessSubGals(facetPtr)
octObject *facetPtr;
{
    int retval = TRUE;
    octGenerator genInst;
    octObject inst = {OCT_UNDEFINED_OBJECT, 0},
	      galFacet = {OCT_UNDEFINED_OBJECT, 0};

    (void) octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	if (IsVemConnector(&inst) || IsIoPort(&inst) || IsMarker(&inst)) {
	    /* skip */
	    ;
	} else if (IsGal(&inst)) {
	    if ( !MyOpenMaster(&galFacet, &inst, "contents", "r") ||
		 !CompileGal(&galFacet) ) {
		FreeOctMembers(&inst);
		retval = FALSE;
		break;
	    }
	} else {			/* assume inst is a star... */
	    if (!AutoLoadCk(&inst)) {
		EssAddObj(&inst);
		FreeOctMembers(&inst);
		retval = FALSE;
		break;
	    }
	}
	FreeOctMembers(&inst);
    }

    octFreeGenerator(&genInst);

    return(retval);
}

/* DetachDelaysFromNets  5/29/88
Detach any delay or bus props from nets in facet.
*/
static void
DetachDelaysFromNets(facetPtr)
octObject *facetPtr;
{
    octObject net = {OCT_UNDEFINED_OBJECT, 0};
    octGenerator netGen;
    octObject prop;		/* do not free prop: will crash Ptolemy */

    (void) octInitGenContentsSpecial(facetPtr, OCT_NET_MASK, &netGen);
    while (octGenerate(&netGen, &net) == OCT_OK) {
	prop.type = OCT_PROP;
	prop.objectId = 0;
	prop.contents.prop.name = "delay";
	prop.contents.prop.type = OCT_NULL;
	if (octGetByName(&net, &prop) == OCT_NOT_FOUND) {
	    prop.contents.prop.name = "delay2";
	    if (octGetByName(&net, &prop) == OCT_NOT_FOUND) {
	        prop.contents.prop.name = "buswidth";
		if (octGetByName(&net, &prop) == OCT_NOT_FOUND) continue;
	    }
	}
	(void) octDetach(&net, &prop);
        FreeOctMembers(&prop);
        FreeOctMembers(&net);
    }
    FreeOctMembers(&net);
    octFreeGenerator(&netGen);
}

/* ProcessMarker  5/28/88
Process delay instance by attaching its delay prop to the net associated
with the delay instance.  Same for buses and buswidth property.
Inputs: facetPtr, instPtr
Outputs: return = FALSE if error
*/
static boolean
ProcessMarker(facetPtr, instPtr, propname)
octObject *facetPtr, *instPtr;
char *propname;
{
    int retval = TRUE;
    octObject path = {OCT_UNDEFINED_OBJECT, 0},
	      dummy = {OCT_UNDEFINED_OBJECT, 0},
	      net = {OCT_UNDEFINED_OBJECT, 0};
    struct octBox box;
    regObjGen rGen;

    if (octBB(instPtr, &box) != OCT_OK) {
	ErrAdd("ProcessMarker: Instance has no bounding box");
	return(FALSE);
    }

    (void) regObjStart(facetPtr, &box, OCT_PATH_MASK, &rGen, 0);
    if (regObjNext(rGen, &path) == REG_NOMORE) {
	ErrAdd("Dangling delay or bus marker");
	EssAddObj(instPtr);
	(void) regObjFinish(rGen);
	retval = FALSE;
    }

    else if (regObjNext(rGen, &dummy) != REG_NOMORE) {
	ErrAdd("Delay or bus marker intersects more than one path");
	EssAddObj(instPtr);
	(void) regObjFinish(rGen);
	retval = FALSE;
    }

    else if (octGenFirstContainer(&path, OCT_NET_MASK, &net) != OCT_OK) {
	ErrAdd("ProcessMarker: Path not contained in a net");
	EssAddObj(&path);
	retval = FALSE;
    }

    else {
	octObject prop = {OCT_PROP, 0}; /* don't deallocate: has static data */
	prop.contents.prop.name = propname;

	if (octGetByName(&net, &prop) != OCT_NOT_FOUND) {
	    char buf[128];
	    sprintf(buf, "Net has more than one %s instance on top of it",
		    propname);
	    ErrAdd(buf);
	    EssAddObj(&net);
	    FreeOctMembers(&prop);
	    retval = FALSE;
	}

	/* fill in default values, and get or create the property. */
	else {
	    int newpropmemory;
	    if ( IsBus(instPtr) ) {
		prop.contents.prop.type = OCT_INTEGER;
		prop.contents.prop.value.integer = 1;
	    }
	    else if ( IsDelay(instPtr) ) {
		prop.contents.prop.type = OCT_STRING;
		prop.contents.prop.value.string = "1";
	    }
	    else {					/* IsDelay2(instPtr) */
		prop.contents.prop.type = OCT_STRING;
		prop.contents.prop.value.string = "0";
	    }
            newpropmemory = octGetOrCreate(instPtr, &prop);
	    if ( octAttach(&net, &prop) != OCT_OK ) {
		ErrAdd(octErrorString());
		retval = FALSE;
	    }
	    if ( newpropmemory ) {
		FreeOctMembers(&prop);		/* created by octGetOrCreate */
	    }
	}
    }

    /* Clean up dynamic memory */
    FreeOctMembers(&path);
    FreeOctMembers(&dummy);
    FreeOctMembers(&net);

    return(retval);
}

/* ProcessInsts
 * This function is called recursively if there's hierarchy
 * Don't free octObject inst using FreeOctMembers because it would be
 * freed again during recursion.
 * It is okay to free the octGenerator. -BLE
 */
static boolean
ProcessInsts(facetPtr)
octObject *facetPtr;
{
    octGenerator genInst;
    octObject inst = {OCT_UNDEFINED_OBJECT, 0};
    ParamListType pList = {0, 0, 0, FALSE};
    char *name, *oldInstName, *akoName, *parentname;
    char instanceHandle[POCT_FACET_HANDLE_LEN];
    char facetHandle[POCT_FACET_HANDLE_LEN];
    int retval = TRUE;

    DetachDelaysFromNets(facetPtr);
    (void) octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &genInst);

    /* Iterate over all oct objects, overwriting inst at each iteration */
    /* This one line accounts for most of the memory leaks in pigilib */
    /* but you cannot deallocate inst using FreeOctMembers within while loop */
    /* FIXME: Memory leak in octGenerate not in pigilib -BLE */
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	if (IsVemConnector(&inst) || IsIoPort(&inst)) {
	    /* skip */
	}
	else if (IsDelay(&inst)) {
	    if (!ProcessMarker(facetPtr, &inst, "delay")) {
		retval = FALSE;
		break;
	    }
	}
	else if (IsDelay2(&inst)) {
	    if (!ProcessMarker(facetPtr, &inst, "delay2")) {
		retval = FALSE;
		break;
	    }
	}
	else if (IsBus(&inst)) {
	    if (!ProcessMarker(facetPtr, &inst, "buswidth")) {
		retval = FALSE;
		break;
	    }
	}
	/* test if it's a sog */
	/* Do not free inst: it will be read at a later time */
	else if (!GetOrInitSogParams(&inst, &pList)) {
	    retval = FALSE;
	    break;
	}
	else {
	    akoName = BaseName(inst.contents.instance.master); 
	    name = UniqNameGet(akoName);
	    if (name == NULL) {
		retval = FALSE;
		break;
	    }
	    /* put the unique instance name back in the facet,
	     * unless it is there already
	     */
	    oldInstName = inst.contents.instance.name;
	    if (!oldInstName || strcmp(name, oldInstName) != 0) {
		if (oldInstName) free(oldInstName);
		inst.contents.instance.name = name;
		if (octModify(&inst) != OCT_OK) {
		    ErrAdd(octErrorString());
		    retval = FALSE;
		    break;
		}
	    }
	    if (!KcInstance(name, akoName, &pList)) {
		free(name);
		retval = FALSE;
		break;
	    }
	    /* Process the pragmas list, if any */
	    ptkOctObj2Handle(&inst, instanceHandle);
	    ptkOctObj2Handle(facetPtr, facetHandle);
	    parentname = BaseName(facetPtr->contents.facet.cell);
	    if (Tcl_VarEval(ptkInterp, "ptkProcessPragmas ",
			    facetHandle, " ",
			    parentname, " ",
			    instanceHandle, " ",
			    name, " ",
			    (char*)NULL) == TCL_OK)
	      KcLog(ptkInterp->result);
	    free(name);
	}
	FreeFlatPList(&pList);
    }
    FreeFlatPList(&pList);
    octFreeGenerator(&genInst);
    return(retval);
}

/* 6/13/89
Find matching formal terminal in master and check if it is an input.
Inputs: aTermPtr = actual terminal to check
*/
static boolean
IsInputTerm(aTermPtr, result)
octObject *aTermPtr;
boolean *result;
{
    octObject inst = {OCT_UNDEFINED_OBJECT, 0},
	      master = {OCT_UNDEFINED_OBJECT, 0},
	      fTerm = {OCT_UNDEFINED_OBJECT, 0},
	      prop = {OCT_PROP, 0};
    int retval = TRUE;

    ERR_IF2(GetById(&inst, aTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    ERR_IF1(!MyOpenMaster(&master, &inst, "interface", "r"));
    ERR_IF2(GetByTermName(&master, &fTerm, aTermPtr->contents.term.name) !=
	OCT_OK, octErrorString());

    if (GetByPropName(&fTerm, &prop, "input") != OCT_NOT_FOUND) {
	*result = TRUE;
	retval = TRUE;
	FreeOctMembers(&prop);
    }
    else if (GetByPropName(&fTerm, &prop, "output") != OCT_NOT_FOUND) {
	*result = FALSE;
	retval = TRUE;
	FreeOctMembers(&prop);
    }
    else {
	*result = FALSE;
	retval = FALSE;
	sprintf(msg, "IsInputTerm: terminal is of unknown type, '(%s %s)'",
		inst.contents.instance.name, aTermPtr->contents.term.name);
	ErrAdd(msg);
	EssAddObj(aTermPtr);
    }
    FreeOctMembers(&fTerm);
    FreeOctMembers(&inst);
    FreeOctMembers(&master);
    return (retval);
}

/* 6/13/89
Sort actual terminals in net into inputs and outputs.
*/
static boolean
CollectTerms(netPtr, in, inN, out, outN)
octObject *netPtr, *in, *out;
int *inN, *outN;
{
    octObject term = {OCT_UNDEFINED_OBJECT, 0};
    octGenerator termGen;
    int i;
    int retval = TRUE;
    boolean inputPortFlag;

    *inN = 0;				/* Set length of arrays to zero */
    *outN = 0;

    /* FIXME: Memory leak in octInitGenContentsSpecial not in pigilib */
    (void) octInitGenContentsSpecial(netPtr, OCT_TERM_MASK, &termGen);
    for (i = 0; i < TERMS_MAX; ) {
	if (octGenerate(&termGen, &term) != OCT_OK) {
	    break;				/* Do not free term here */
	}
	if (term.contents.term.instanceId != oct_null_id) {
	    /* actual term */
	    if (!IsInputTerm(&term, &inputPortFlag)) {
		FreeOctMembers(&term);
		retval = FALSE;
		break;
	    }
	    if (inputPortFlag) {
		in[(*inN)++] = term;
	    } else {
		out[(*outN)++] = term;
	    }
	    i++;
	}
	else {
	    FreeOctMembers(&term);
	}
    }

    if (i >= TERMS_MAX) {
	ErrAdd("CollectTerms: Too many actual terminals on net");
	EssAddObj(netPtr);
	retval = FALSE;
    }

    /* Clean up memory */
    octFreeGenerator(&termGen);
    if ( ! retval ) {
	for (i = 0; i < *inN; i++) {
	    FreeOctMembers( &in[i] );
	}
	*inN = 0;
	for (i = 0; i < *outN; i++) {
	    FreeOctMembers( &out[i] );
	}
	*outN = 0;
    }

    return (retval);
}

/* 6/14/89
*/
static boolean
JoinOrdinary(inTermPtr, outTermPtr, initDelayValues, width)
octObject *inTermPtr, *outTermPtr;
char *initDelayValues, *width;
{
    int retval = TRUE;
    octObject inInst = {OCT_UNDEFINED_OBJECT, 0},
	      outInst = {OCT_UNDEFINED_OBJECT, 0},
	      fTerm = {OCT_UNDEFINED_OBJECT, 0};
    boolean inIsGalPort, outIsGalPort;

    ERR_IF2(GetById(&inInst, inTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    inIsGalPort = IsIoPort(&inInst);
    ERR_IF2(GetById(&outInst, outTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    outIsGalPort = IsIoPort(&outInst);

    if (!inIsGalPort && !outIsGalPort) {
	/* 2 sogs connected - no Galaxy Ports involved */
	if (!KcConnect(outInst.contents.instance.name,
		       outTermPtr->contents.term.name, 
		       inInst.contents.instance.name,
		       inTermPtr->contents.term.name,
		       initDelayValues,
		       width)) {
	     retval = FALSE;
	}
    /* if one of the ports is a Galaxy Port, then width may not be defined */
    } else if ( (width != NULL) && *width ) {
	ErrAdd("Cannot make a bus connection between a galaxy port and its alias");
	EssAddObj(&inInst);
	EssAddObj(&outInst);
	retval = FALSE;
    } else if (!inIsGalPort && outIsGalPort) {
	/* Connect an input to a Galaxy Port */
	if (octGenFirstContainer(outTermPtr, OCT_TERM_MASK, &fTerm) != OCT_OK) {
	    ErrAdd("JoinOrdinary: input port has no name");
	    EssAddObj(&outInst);
	    retval = FALSE;
	}
	if (!KcAlias(fTerm.contents.term.name,
		     inInst.contents.instance.name,
		     inTermPtr->contents.term.name)) {
	    retval = FALSE;
	}
    } else if (inIsGalPort && !outIsGalPort) {
	/* Connect an output to a Galaxy Port */
	if (octGenFirstContainer(inTermPtr, OCT_TERM_MASK, &fTerm) !=OCT_OK) {
	    ErrAdd("JoinOrdinary: output port has no name");
	    EssAddObj(&inInst);
	    retval = FALSE;
	}
	if (!KcAlias(fTerm.contents.term.name,
		     outInst.contents.instance.name,
		     outTermPtr->contents.term.name)) {
	    retval = FALSE;
	}
    } else {
	ErrAdd("JoinOrdinary: cannot connect input galaxy port directly to output galaxy port");
	EssAddObj(&inInst);
	EssAddObj(&outInst);
	retval = FALSE;
    }

    FreeOctMembers(&inInst);
    FreeOctMembers(&outInst);
    FreeOctMembers(&fTerm);

    return (retval);
}

/* This function connects terminals to nodes; it is used in n-way connections*/
static boolean
JoinToNode(termPtr, nodename)
octObject *termPtr;
char *nodename;
{
    octObject inst = {OCT_UNDEFINED_OBJECT, 0};

    ERR_IF2(GetById(&inst, termPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    if (IsIoPort(&inst)) {
	ErrAdd("Galaxy portholes can be connected to only one port");
	EssAddObj(&inst);
	return (FALSE);
    }
    ERR_IF1(!KcNodeConnect(inst.contents.instance.name,
			   termPtr->contents.term.name, nodename));
    FreeOctMembers(&inst);
    return (TRUE);
}


/* 7/31/90
Checks to see if octObject is a MultiPortHole.
*/
static boolean
TermIsMulti(termPtr)
octObject *termPtr;
{
    int retval = FALSE;
    octObject inst = {OCT_UNDEFINED_OBJECT, 0};

    if (GetById(&inst, termPtr->contents.term.instanceId) != OCT_OK) {
	ErrAdd(octErrorString());
    }
    else {
	retval = KcIsMulti(inst.contents.instance.name,
			   termPtr->contents.term.name);
    }
    FreeOctMembers(&inst);

    return(retval);
}


/* 6/14/89 7/31/90
Emits connect, input, and output statements.
*/
#define BLEN 80

static boolean
ConnectPass(facetPtr)
octObject *facetPtr;
{
    octObject net = {OCT_UNDEFINED_OBJECT, 0}, in[TERMS_MAX], out[TERMS_MAX];
    octGenerator netGen;
    int inN = 0, outN = 0, totalN, i;
    char delay[BLEN], initDelayValues[BLEN], width[BLEN];
    char *errMsg = 0;    /* errMsg is initialized to 0.  If error occurs,
			    errMsg will be non-zero, since it will point
			    to an error message.  Note that the error message
			    could be the null string "". */

    (void) octInitGenContentsSpecial(facetPtr, OCT_NET_MASK, &netGen);
    while (octGenerate(&netGen, &net) == OCT_OK) {
	if (!CollectTerms(&net, in, &inN, out, &outN)) {
	    octFreeGenerator(&netGen);
	    FreeOctMembers(&net);
	    return FALSE;
	}
	totalN = inN + outN;
	if (GetStringizedProp(&net, "delay", delay, BLEN))
	    /* old type delay, mark it */
	    sprintf(initDelayValues, "*%s",delay);
	else
	    GetStringizedProp(&net, "delay2", initDelayValues, BLEN);
	GetStringizedProp(&net, "buswidth", width, BLEN);

	/* For each net, we require that there be at least one input
	   and at least one output. */
	if (totalN < 2) {
	    /* bad net, delete it */
	    if (octDelete(&net) != OCT_OK) {
		errMsg = octErrorString();	    /* static string */
		break;
	    }
	    PrintDebug("Warning: bad net deleted");	/* continue in while */
	} else if (inN == 0 || outN == 0) {
	    errMsg = "ConnectPass: cannot match an input to an output";
	    break;
	} else if (*initDelayValues && totalN > 2) {
	    /* We only allow delay on point-to-point connections */
	    errMsg = "ConnectPass: delay not allowed on multi-connections";
	    break;
	} else if (*width && totalN > 2) {
	    errMsg = "ConnectPass: bus not allowed on multi-connections";
	    break;
	} else if (totalN == 2) {
	    if (*width > 0 &&
		(!TermIsMulti(&in[0]) || !TermIsMulti(&out[0]))) {
		errMsg = "Bus connection only works with multiports";
		break;
	    } else if (!JoinOrdinary(&in[0], &out[0],
		     initDelayValues, width)) {
		errMsg = "";
		break;
	    }
	} else if (inN == 1 && outN > 1) {
	    /* Handle input multiporthole case.  Eventually this will be
	     * handled in the kernel (we will eliminate this else clause. */
	    if (TermIsMulti(&in[0])) {
		for (i = 0; i < outN; i++) {
		    if (!JoinOrdinary(&in[0], &out[i], (char*)0, (char*)0)) {
			errMsg = "";
			break;
		    }
		}
		if (errMsg) break;         /* Tom Lane's fix, 1/9/96 */
	    } else {
		/* error: multiple outputs connected to one ordinary input */
		strcpy(msg, "can't connect more than one output port:\n");
		for (i = 0; i < outN; i++) {
		    strcat(msg, "\t");
		    strcat(msg, ohFormatName(out + i));
		    strcat(msg, "\n");
		}
		strcat(msg, "to an ordinary input port:\n");
		strcat(msg, "\t");
		strcat(msg, ohFormatName(in));
		errMsg = msg;
		break;
	    }
	} else {
	    /* All other cases are handled by the kernel, including autofork */
	    char *nodename = UniqNameGet("node");
	    if (!KcNode(nodename)) {
		errMsg = "";
		break;
	    } else {
		for (i = 0; i < outN; i++) {
		    if (!JoinToNode(&out[i], nodename)) {
			errMsg = "";
			break;
		    }
		}
		if (errMsg) break;         /* Tom Lane's fix, 1/9/96 */
		for (i = 0; i < inN; i++) {
		    if (!JoinToNode(&in[i], nodename)) {
			errMsg = "";
			break;
		    }
		}
		if (errMsg) break;         /* Tom Lane's fix, 1/9/96 */
	    }
	    free(nodename);
	}
	FreeOctMembers(&net);
	for (i = 0; i < inN; i++) FreeOctMembers(&in[i]);
	inN = 0;
	for (i = 0; i < outN; i++) FreeOctMembers(&out[i]);
	outN = 0;
    }

    if (errMsg) {
	if (*errMsg) ErrAdd (errMsg);
	EssAddObj(&net);
    }

    /* Free dynamic memory */
    octFreeGenerator(&netGen);
    FreeOctMembers(&net);
    for (i = 0; i < inN; i++) FreeOctMembers(&in[i]);
    for (i = 0; i < outN; i++) FreeOctMembers(&out[i]);

    return errMsg ? FALSE : TRUE;
}

/*
Checks to see whether a galaxy facet has been changed since the last run.
Outputs: return = true if facet has been changed or if not sure.
*/
static boolean
IsDirty(facetPtr)
octObject *facetPtr;
{
    octObject cl = {OCT_UNDEFINED_OBJECT, 0},
	      cr = {OCT_UNDEFINED_OBJECT, 0};

    if (octGenFirstContent(facetPtr, OCT_CHANGE_LIST_MASK, &cl) != OCT_OK) {
	return (TRUE);
    }
    if (octGenFirstContent(&cl, OCT_CHANGE_RECORD_MASK, &cr) == OCT_OK) {
	return (TRUE);
    } else {
	return (FALSE);
    }
}

/*
Checks to see whether a universe facet has been changed since the last run,
or if an error occured in the last run; if the prior run failed then
the kernel image may be corrupted, and should be rebuilt.  (For example,
the user might have aborted the run partway through HOF star expansion.)
Outputs: return = true if facet has been changed or if not sure.
*/
static boolean
IsDirtyOrGone(facetPtr)
octObject *facetPtr;
{
    octObject cl = {OCT_UNDEFINED_OBJECT, 0},
	      cr = {OCT_UNDEFINED_OBJECT, 0};
    char* name;

    /* First check to see whether the last run was completed */
    name = BaseName(facetPtr->contents.facet.cell);
    TCL_CATCH_ERR2(
	Tcl_VarEval(ptkInterp, "ptkHasRun ", name, (char*) NULL),
	TRUE);
    if (*(ptkInterp->result) == '0') return (TRUE);

    if (octGenFirstContent(facetPtr, OCT_CHANGE_LIST_MASK, &cl) != OCT_OK) {
	return (TRUE);
    }
    if (octGenFirstContent(&cl, OCT_CHANGE_RECORD_MASK, &cr) == OCT_OK) {
	return (TRUE);
    } else {
	return (FALSE);
    }
}

/*
Clears the dirty condition of a facet.  In other words, clear all change
records and prepare to record new ones.
*/
static boolean
ClearDirty(facetPtr)
octObject *facetPtr;
{
    octObject cl = {OCT_CHANGE_LIST, 0};

    if (octGenFirstContent(facetPtr, OCT_CHANGE_LIST_MASK, &cl) == OCT_OK) {
	CK_OCT(octDelete(&cl));
    }
    cl.contents.changeList.objectMask = OCT_ALL_MASK &
	~OCT_CHANGE_LIST_MASK & ~OCT_CHANGE_RECORD_MASK;
    cl.contents.changeList.functionMask = OCT_ATTACH_CONTENT_MASK |
	OCT_DETACH_CONTENT_MASK | OCT_MODIFY_MASK | OCT_PUT_POINTS_MASK;
    CK_OCT(octCreate(facetPtr, &cl));
    octMarkTemporary(&cl);
    return (TRUE);
}

/* compile a galaxy instance in context.  If the instance is in a facet,
   this works like CompileGalStandalone.  Used in Profile.
 */
boolean
CompileGalInst(galInstPtr,parentFacetPtr)
octObject *galInstPtr, *parentFacetPtr;
{
    const char* galDomain;
    octObject galFacet = {OCT_UNDEFINED_OBJECT, 0};

    /* get the galaxy domain */
    if (!GOCDomainProp(galInstPtr, &galDomain, curDomainName())) {
        PrintErr(ErrGet());
        return (FALSE);
    }
    /* set outer domain */
    if (IsPalFacet(parentFacetPtr)) KcSetKBDomain(galDomain);
    else if (!setCurDomainF(parentFacetPtr)) {
	PrintErr("Invalid domain found");
	return FALSE;
    }

    /* now do the compile */
    ERR_IF1(!MyOpenMaster(&galFacet, galInstPtr, "contents", "r"));
    DupSheetClear(&traverse);
    return CompileGal(&galFacet);
}

/* standalone compilation of a galaxy, for icon-making.
   It makes the galaxy as a plain galaxy on its own domain's knownlist
   (i.e. not as a wormhole).
 */

boolean
CompileGalStandalone(galFacetPtr)
octObject *galFacetPtr;
{
    const char* galDomain;
    /* get the galaxy domain */
    if (!GOCDomainProp(galFacetPtr, &galDomain, curDomainName())) {
        PrintErr(ErrGet());
        return (FALSE);
    }
    /* Call Kernel function to set KnownBlock current domain */
    if (! KcSetKBDomain(galDomain)) {
        PrintErr("Domain error in galaxy or wormhole.");
        return (FALSE);
    }
    DupSheetClear(&traverse);
    return CompileGal(galFacetPtr);
}

/*
 * The following flag is used to determine whether any subgalaxy of
 * a universe has been recompiled.  If it has, the universe has to
 * be recompiled.
 */
static boolean recompileFlag;

/* compile a galaxy. */
/* this function will be called recursively if there's hierarchy */

boolean
CompileGal(galFacetPtr)
octObject *galFacetPtr;
{
    char *name;
    char *pathName;
    const char* defSource;
    const char *galDomain;
    const char *desc;
    const char *oldDomain;
    const char *galTarget;

    pathName = galFacetPtr->contents.facet.cell;
    name = BaseName(pathName);

    /* We will compile a block belonging to the outer domain. */
    oldDomain = curDomainName();

    /* get the definition source for any existing compilation */
    defSource = KcGalaxyDefSource(name, oldDomain);

    /* get the galaxy's own domain */
    if (!GOCDomainProp(galFacetPtr, &galDomain, oldDomain)) {
        PrintErr(ErrGet());
        return (FALSE);
    }

    /* Check to see if the galaxy has already been touched in this scan. */
    if (DupSheetIsDup(&traverse, pathName, oldDomain)) {
      /* Check for recursion or conflicting definitions. */
      if (defSource == NULL) {
	/* Must be a recursive reference. */
	PrintErr("Sorry, graphical recursion is not supported at this time");
	return(FALSE);
      }
      if (strcmp(defSource, pathName) != 0) {
	/* Conflicting definitions for same kernel object. */
	PrintErr("Multiple facets with same base name are not supported");
	return(FALSE);
      }
      /* OK, the galaxy has already been processed. */
      return(TRUE);
    }
    /*
     * mark the galaxy touched right away, in case there is a recursive
     * reference to it in the subgalaxies.
     */
    ERR_IF2(!DupSheetAdd(&traverse, pathName, oldDomain),
	    "DupSheetAdd failed! Out of memory?");

    /* Call Kernel function to set the current domain to the inner
     * domain.  It is important that we switch domains before processing
     * the subgalaxies.
     */
    if (! KcSetKBDomain(galDomain)) {
        PrintErr("Domain error in galaxy or wormhole.");
        return (FALSE);
    }

    /* get the galaxy target */
    if (!GOCTargetProp(galFacetPtr, &galTarget, "<parent>")) {
        PrintErr(ErrGet());
        return (FALSE);
    }

    ERR_IF2(!ProcessSubGals(galFacetPtr),
	    "Failed to process subgalaxies");

    /* Done if the galaxy has previously been compiled and
     * has not been modified since then.
     */
    if (defSource != NULL &&
	strcmp(defSource, pathName) == 0 &&
	!IsDirty(galFacetPtr)) {
      /* galaxy already xfered to kernel and is unchanged */
      /* restore old domain */
      KcSetKBDomain(oldDomain);
      return (TRUE);
    }

    /* We have to compile, so log it */
    if (strcmp(oldDomain, galDomain) != 0 ||
	(galTarget && strcmp(galTarget, "<parent>") != 0))
	sprintf(msg, "CompileGal: facet = %s, %s inside %s wormhole",
		name, galDomain, oldDomain);
    else sprintf(msg, "CompileGal: facet = %s, %s galaxy", name, galDomain);
    PrintDebug(msg);
    /*
     * So that KcDefgalaxy recognizes wormholes, we have to restore the
     * old domain.
     */
    KcSetKBDomain(oldDomain);
    /*
     * The following sets the new domain again.
     */
    ERR_IF1(!KcDefgalaxy(name,galDomain,galTarget));
    ERR_IF1(!GetStringProp(galFacetPtr,"comment",&desc));
    KcSetDesc(desc);
    ERR_IF1(!ProcessTargetParams(galTarget,galFacetPtr));
    ERR_IF2(!ProcessFormalParams(galFacetPtr),
	    "Failed to process formal parameters");
    /* Initialize instance name generation of blocks and nodes
       within the galaxy. */
    ERR_IF1(!UniqNameInit());
    ERR_IF2(!ProcessInsts(galFacetPtr), "Failed to process instances");
    ERR_IF2(!ConnectPass(galFacetPtr), "Connect pass failure");
    ERR_IF1(!KcEndDefgalaxy(pathName, oldDomain));
    /* After successful compilation, clear the Oct facet-modified flags */
    ERR_IF2(!ClearDirty(galFacetPtr), "Failed to mark clean!");
    /* Mark a subgalaxy has been recompiled, need to compile universe too */
    recompileFlag = TRUE;
    KcFlushLog();
    return (TRUE);
}

static boolean
CompileUniv(facetPtr)
octObject *facetPtr;
{
    char *name;
    char *pathName;
    const char* defSource;
    const char *domain;
    const char *target;

    pathName = facetPtr->contents.facet.cell;
    name = BaseName(pathName);

    /* get the definition source for any existing compilation */
    defSource = KcUniverseDefSource(name);

    if (!GOCDomainProp(facetPtr, &domain, DEFAULT_DOMAIN)) {
	PrintErr(ErrGet());
	sprintf(msg, "Domain error in facet %s", name);
	PrintErr(msg);
	return FALSE;
    }
    sprintf(msg, "reset ___empty___\ndomain %s\n", domain);
    KcLog(msg);
    TCL_CATCH_ERR1(Tcl_Eval(ptkInterp, msg));

    /* Process the subgalaxies */
    recompileFlag = 0;
    ERR_IF1(!ProcessSubGals(facetPtr));

    /* We need to recompile the universe if any subgalaxy changed,
     * or if the universe itself is unknown to the kernel or changed.
     */
    if (!recompileFlag &&
	defSource != NULL &&
	strcmp(defSource, pathName) == 0 &&
	!IsDirtyOrGone(facetPtr)) {
	/* universe already xfered to kernel and is unchanged */
	sprintf(msg, "curuniverse %s\n", name);
	KcLog(msg);
	Tcl_VarEval(ptkInterp, msg, (char *) NULL);
	return (TRUE);
    }

    sprintf(msg, "CompileUniv: facet = %s, %s universe", name, domain);
    PrintDebug(msg);
    sprintf(msg, "newuniverse %s %s\n", name, domain);
    KcLog(msg);
    TCL_CATCH_ERR1(Tcl_Eval(ptkInterp, msg));

    /* get the target */
    if (!GOCTargetProp(facetPtr, &target, KcDefTarget(domain))) {
        PrintErr(ErrGet());
        return (FALSE);
    }
    sprintf(msg, "target %s\n", target);
    KcLog(msg);
    TCL_CATCH_ERR1(Tcl_Eval(ptkInterp, msg));
    ERR_IF1(!ProcessTargetParams(target,facetPtr));
    ERR_IF1(!ProcessFormalParams(facetPtr));
    /* Initialize instance name generation of blocks and nodes
       within the universe. */
    ERR_IF1(!UniqNameInit());
    ERR_IF1(!ProcessInsts(facetPtr));
    ERR_IF1(!ConnectPass(facetPtr));
    /* Mark the finished universe with its definition source. */
    KcSetUniverseDefSource(pathName);
    /* After successful compilation, clear the Oct facet-modified flags */
    ERR_IF1(!ClearDirty(facetPtr));
    KcFlushLog();
    return (TRUE);
}

/* The following is defined in liboct.a but not librpc.a for some reason.
This is a quick fix.
*/
#define octIdsEqual(a, b) ((long) a == (long) b)

/* 4/16/90 5/16/89 2/20/89
*/
boolean
CompileFacet(facetPtr)
octObject *facetPtr;
{
    DupSheetClear(&traverse);
    ErrClear();
    if (IsGalFacet(facetPtr)) {
	if (!CompileGal(facetPtr)) {
	    return (FALSE);
	}
    } else if (IsPalFacet(facetPtr)) {
	ErrAdd("CompileFacet: palette facets cannot be compiled");
	return (FALSE);
    } else {
	if (!CompileUniv(facetPtr)) {
	    return (FALSE);
	}
    }
    return (TRUE);
}

/* RpcCompileFacet  5/30/88 4/18/88
RPC function to compile facet
*/
int 
RpcCompileFacet(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet = {OCT_UNDEFINED_OBJECT, 0};

    ViInit("compile-facet");
    ErrClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    }
    else if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
        PrintErr("Failed to set default domain.");
    }
    else if (!CompileFacet(&facet)) {
	PrintErr(ErrGet());
    }
    FreeOctMembers(&facet);
    ViDone();
}

/* Called once when pigi is started */
void
CompileInit()
{
    DupSheetInit(&traverse);
}

void
CompileEnd()
{
    /* clean up tmp file before exiting */
}

/* run everything in the facet, which is assumed to be a palette */
static boolean
RunAll(facetPtr)
octObject *facetPtr;
{
    int retval = TRUE;
    octGenerator genInst;
    octObject inst = {OCT_UNDEFINED_OBJECT, 0},
	      univFacet = {OCT_UNDEFINED_OBJECT, 0};

    octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	univFacet.type = OCT_UNDEFINED_OBJECT;
	univFacet.objectId = 0;

	if (!MyOpenMaster(&univFacet, &inst, "contents", "r")) {
	    PrintErr(octErrorString());
	    retval = FALSE;
	    break;
	}
	/* skip any non-universes */
	if (IsCursor(&inst)) {
	    /* nothing */;
	}
	else if (IsUnivFacet(&univFacet)) {
	    char *name = BaseName(univFacet.contents.facet.cell);
	    char octHandle[POCT_FACET_HANDLE_LEN];
	    char *flagVal;

	    sprintf(msg, "RunAllDemos: executing universe '%s'", name);
	    PrintDebug(msg);
	    ptkOctObj2Handle(&univFacet, octHandle);
	    /* ptkCompileRun is very similar to ptkGo, but assuming controlled
	       by run-all-demos panel instead of ordinary run control panel. */
	    retval = FALSE;
	    TCL_CATCH_ERR1_BREAK(Tcl_VarEval(ptkInterp, "ptkCompileRun ", name,
					     " ", octHandle, (char *) NULL));
	    /* delete the universe after run finishes. */
	    TCL_CATCH_ERR1_BREAK(Tcl_VarEval(ptkInterp, "ptkDelLite ", name,
					     " ", octHandle, (char *) NULL));
	    /* check abort-all-demos flag */
	    if ((flagVal = Tcl_GetVar(ptkInterp, "ptkRunAllFlag",
				      TCL_GLOBAL_ONLY)) != NULL &&
		*flagVal == '0')
	      break;
	    retval = TRUE;
	}
	else if (IsPalFacet(&univFacet)) {
	    if (! RunAll(&univFacet)) {
		retval = FALSE;
		break;
	    }
	}
	FreeOctMembers(&univFacet);
	FreeOctMembers(&inst);
    }
    FreeOctMembers(&univFacet);
    FreeOctMembers(&inst);
    octFreeGenerator(&genInst);
    return retval;
}

/* NEW FUNCTION */

int
RpcRunAllDemos(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet = {OCT_UNDEFINED_OBJECT, 0};
    char* name;
    char octHandle[POCT_FACET_HANDLE_LEN];

    ViInit("run-all-demos");
    ErrClear();
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    }
    else if (!IsPalFacet(&facet)) {
	PrintErr("This command must be run in a palette");
    }
    else {
	name = BaseName(facet.contents.facet.cell);
	ptkOctObj2Handle(&facet, octHandle);

	if (Tcl_VarEval(ptkInterp, "ptkRunAllDemos ", name, " ", octHandle,
			(char *) NULL) == TCL_ERROR) {
	    PrintErr("Run-all-demos control panel cannot be brought up");
	}
	else {
	    RunAll(&facet);
	    TCL_CATCH_ERR(Tcl_VarEval(ptkInterp, "ptkRunAllDemosDel ", name,
				      " ", octHandle, (char *) NULL));
	}
    }

    FreeOctMembers(&facet);
    ViDone();
}
