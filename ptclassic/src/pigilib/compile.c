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
kernel one.  The compile routines keep these representations
consistent.  There are two lists: traverse and xfered.  The traverse
list is used to traverse the heirarchical design in Oct which can be
represented as a directed acyclic graph (DAG), with each node in the
DAG corresponding to a facet in the design.  (traverse prevents
visiting a node twice.)

The xfered list keeps track of which nodes in the DAG have been
transfered to the kernel so far.  Change lists in each facet keep track of
any changes.  A facet (node in the DAG) is recompiled if it is not on
the xfered list or if it has been changed since the last transfer.

The difference is simply that traverse is re-initialized each time
CompileFacet is called.  xfered never gets re-initialized.
*/

/* Includes */
#include "local.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "oct.h"
#include "oh.h"
#include "rpc.h"
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
#include "compile.h"

extern void KcLog();
extern void KcFlushLog();
extern void KcSetDesc();
static boolean RunAll();

#define TERMS_MAX 50  /* maximum number of actual terms allowed on a net */

/* Scratch string for constructing error messages */
static char msg[1024];

static DupSheet traverse;  /* used to traverse design heriarchy which is a DAG */
    /* DAG = directed acyclic graph */
static DupSheet xfered;  /* list of galaxies that have been transfered to the kernel */


/***** Begin Ess routines (Error Select Set) */
/* 8/20/89
Caveat: Ess routines create a select set in only one facet, so all
objects added to the select set must be in the same facet!
*/
static vemSelSet essSelSet;  /* for highlighting errors */
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
    octObject facet;
    if (!essExist) {
	octGetFacet(obj, &facet);
	essSelSet = vemNewSelSet(facet.objectId, 65535, 0, 0, 2, 3, 1, 1, "0");
	essExist = TRUE;
    }
    FreeOctMembers(&facet);
    ERR_IF2(vemAddSelSet(essSelSet, obj->objectId) != VEM_OK,
	"EssAddObj: Cannot put object in error select set");
    return (TRUE);
}
/***** End Ess routines (Error Select Set) */


static boolean
ProcessFormalParams(galFacetPtr)
octObject *galFacetPtr;
{
    ParamListType pList;
    ParamType *p;
    int i;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));
    for (i = 0, p = pList.array; i < pList.length; i++, p++) {
	if (!p->type || p->type[0] == 0)
	    p->type = "FLOAT"; /* backward compatibility */
	ERR_IF1(!KcMakeState(p->name, p->type, p->value));
    }
    return(TRUE);
}

static boolean
ProcessTargetParams(targName, galFacetPtr)
char* targName;
octObject *galFacetPtr;
{
    ParamListType pList;
    if (targName[0] == '<') return TRUE;
    ERR_IF1(!GetTargetParams(targName, galFacetPtr, &pList));
    ERR_IF1(!KcModTargetParams(&pList));
    return(TRUE);
}

static boolean
ProcessSubGals(facetPtr)
octObject *facetPtr;
{
    octGenerator genInst;
    octObject inst, galFacet;

    (void) octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	if (IsVemConnector(&inst) || IsIoPort(&inst) || IsMarker(&inst)) {
	    /* skip */
	    ;
	} else if (IsGal(&inst)) {
	    if (!MyOpenMaster(&galFacet, &inst, "contents", "r")
	      || !CompileGal(&galFacet)) {
		octFreeGenerator(&genInst);
		FreeOctMembers(&galFacet);
		FreeOctMembers(&inst);
		return FALSE;
	    }
	} else {
	    /* assume inst is a star... */
	    if (!AutoLoadCk(&inst)) {
		EssAddObj(&inst);
		octFreeGenerator(&genInst);
		FreeOctMembers(&inst);
		return (FALSE);
	    }
	}
    }
    FreeOctMembers(&inst);
    octFreeGenerator(&genInst);
    return(TRUE);
}

/* DetachDelaysFromNets  5/29/88
Detach any delay or bus props from nets in facet.
*/
static void
DetachDelaysFromNets(facetPtr)
octObject *facetPtr;
{
    octObject net;
    octGenerator netGen;
    octObject prop;		/* most data members are not dynamic */

    (void) octInitGenContentsSpecial(facetPtr, OCT_NET_MASK, &netGen);
    while (octGenerate(&netGen, &net) == OCT_OK) {
	prop.type = OCT_PROP;
	prop.contents.prop.name = "delay";
	if (octGetByName(&net, &prop) == OCT_NOT_FOUND) {
	    prop.contents.prop.name = "delay2";
	    if (octGetByName(&net, &prop) == OCT_NOT_FOUND) {
	        prop.contents.prop.name = "buswidth";
		if (octGetByName(&net, &prop) == OCT_NOT_FOUND) continue;
	    }
	}
	(void) octDetach(&net, &prop);
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
    octObject path, dummy, net;
    octObject prop;			/* some data members are not dynamic */
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
	FreeOctMembers(&path);
	return(FALSE);
    }
    if (regObjNext(rGen, &dummy) != REG_NOMORE) {
	ErrAdd("Delay or bus marker intersects more than one path");
	EssAddObj(instPtr);
	(void) regObjFinish(rGen);
	FreeOctMembers(&path);
	FreeOctMembers(&dummy);
	return(FALSE);
    }

    if (octGenFirstContainer(&path, OCT_NET_MASK, &net) != OCT_OK) {
	ErrAdd("ProcessMarker: Path not contained in a net");
	EssAddObj(&path);
	FreeOctMembers(&net);
	FreeOctMembers(&path);
	FreeOctMembers(&dummy);
	return(FALSE);
    }

    prop.type = OCT_PROP;
    prop.contents.prop.name = propname;
    if (octGetByName(&net, &prop) != OCT_NOT_FOUND) {
	char buf[128];
	sprintf (buf, "Net has more than one %s instance on top of it",
		 propname);
	ErrAdd(buf);
	EssAddObj(&net);
	FreeOctMembers(&net);
	FreeOctMembers(&path);
	FreeOctMembers(&dummy);
	return(FALSE);
    }

    /* fill in default values, and get or create the property. */
    if( IsBus(instPtr) ) {
        prop.contents.prop.type = OCT_INTEGER;
        prop.contents.prop.value.integer = 1;
    }
    else if ( IsDelay(instPtr) ) {
        prop.contents.prop.type = OCT_STRING;
        prop.contents.prop.value.string = "1";
    }
    else {
        /* IsDelay2(instPtr) */
        prop.contents.prop.type = OCT_STRING;
        prop.contents.prop.value.string = "0";
    }
    FreeOctMembers(&net);
    FreeOctMembers(&path);
    FreeOctMembers(&dummy);
    prop.objectId = 0;				/* silence Purify */
    octGetOrCreate (instPtr, &prop);
    ERR_IF2(octAttach(&net, &prop) != OCT_OK, octErrorString());
    return(TRUE);
}

static boolean
ProcessInsts(facetPtr)
octObject *facetPtr;
{
    octGenerator genInst;
    octObject inst;
    char *name, *parentname;
    ParamListType pList;
    char *akoName, *oldInstName;
    char instanceHandle[16];
    char facetHandle[16];
    
    DetachDelaysFromNets(facetPtr);
    (void) octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	if (IsVemConnector(&inst) || IsIoPort(&inst)) {
	    /* skip */
	} else if (IsDelay(&inst)) {
	    if (!ProcessMarker(facetPtr, &inst, "delay")) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	} else if (IsDelay2(&inst)) {
	    if (!ProcessMarker(facetPtr, &inst, "delay2")) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	} else if (IsBus(&inst)) {
	    if (!ProcessMarker(facetPtr, &inst, "buswidth")) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	} else {
	    /* assume it's a sog */
	    if (!GetOrInitSogParams(&inst, &pList)) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	    akoName = BaseName(inst.contents.instance.master); 
	    if ((name = UniqNameGet(akoName)) == NULL) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	    /* put the unique instance name back in the facet,
	     * unless it is there already
	     */
	    oldInstName = inst.contents.instance.name;
	    if (!oldInstName || strcmp(name, oldInstName) != 0) {
		inst.contents.instance.name = name;
		if (octModify(&inst) != OCT_OK) {
		    octFreeGenerator(&genInst);
		    ErrAdd(octErrorString());
		    return FALSE;
		}
	    }
	    if (!KcInstance(name, akoName, &pList)) {
		octFreeGenerator(&genInst);
		return FALSE;
	    }
	    /* Process the pragmas list, if any */
	    ptkOctObj2Handle(&inst,instanceHandle);
	    ptkOctObj2Handle(facetPtr,facetHandle);
	    parentname = BaseName(facetPtr->contents.facet.cell);
	    Tcl_VarEval(ptkInterp, "ptkProcessPragmas ",
			facetHandle, " ",
			parentname, " ",
			instanceHandle, " ",
			name, " ",
			(char*)NULL);
	}
	FreeOctMembers(&inst);
    }
    octFreeGenerator(&genInst);
    return(TRUE);
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
    octObject inst, master, fTerm, prop;

    ERR_IF2(GetById(&inst, aTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    ERR_IF1(!MyOpenMaster(&master, &inst, "interface", "r"));
    ERR_IF2(GetByTermName(&master, &fTerm, aTermPtr->contents.term.name) !=
	OCT_OK, octErrorString());
    FreeOctMembers(&master);
    if (GetByPropName(&fTerm, &prop, "input") != OCT_NOT_FOUND) {
	*result = TRUE;
	FreeOctMembers(&prop);
	FreeOctMembers(&fTerm);
	FreeOctMembers(&inst);
	return (TRUE);
    }
    if (GetByPropName(&fTerm, &prop, "output") != OCT_NOT_FOUND) {
	*result = FALSE;
	FreeOctMembers(&prop);
	FreeOctMembers(&fTerm);
	FreeOctMembers(&inst);
	return (TRUE);
    }
    *result = FALSE;
    sprintf(msg, "IsInputTerm: terminal is of unknown type, '(%s %s)'",
	inst.contents.instance.name, aTermPtr->contents.term.name);
    ErrAdd(msg);
    EssAddObj(aTermPtr);
    FreeOctMembers(&prop);
    FreeOctMembers(&fTerm);
    FreeOctMembers(&inst);
    return (FALSE);
}

/* 6/13/89
Sort actual terminals in net into inputs and outputs.
*/
static boolean
CollectTerms(netPtr, in, inN, out, outN)
octObject *netPtr, *in, *out;
int *inN, *outN;
{
    octObject term;
    octGenerator termGen;
    int i;
    boolean result;

    *inN = 0;
    *outN = 0;
    (void) octInitGenContentsSpecial(netPtr, OCT_TERM_MASK, &termGen);
    for (i = 0; i < TERMS_MAX; ) {
	if (octGenerate(&termGen, &term) != OCT_OK) {
	    break;
	}
	if (term.contents.term.instanceId != oct_null_id) {
	    /* actual term */
	    if (!IsInputTerm(&term, &result)) {
		octFreeGenerator(&termGen);
		return FALSE;
	    }
	    if (result) {
		in[(*inN)++] = term;
	    } else {
		out[(*outN)++] = term;
	    }
	    i++;
	}
    }
    octFreeGenerator(&termGen);
    if (i >= TERMS_MAX) {
	ErrAdd("CollectTerms: Too many actual terminals on net");
	EssAddObj(netPtr);
	return (FALSE);
    }
    return (TRUE);
}    

/* 6/14/89
*/
static boolean
JoinOrdinary(inTermPtr, outTermPtr, initDelayValues, width)
octObject *inTermPtr, *outTermPtr;
char *initDelayValues, *width;
{
    octObject inInst, outInst, fTerm;
    boolean inIsGalPort, outIsGalPort;

    ERR_IF2(GetById(&inInst, inTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    inIsGalPort = IsIoPort(&inInst);
    ERR_IF2(GetById(&outInst, outTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    outIsGalPort = IsIoPort(&outInst);

    if (!inIsGalPort && !outIsGalPort) {
	/* 2 sogs connected - no Galaxy Ports involved */
	ERR_IF1(!KcConnect(
	    outInst.contents.instance.name, outTermPtr->contents.term.name, 
	    inInst.contents.instance.name, inTermPtr->contents.term.name,
			   initDelayValues, width)
	);
	
      /* if one of the ports is a Galaxy Port, then width may not be defined */
    } else if ( (width != NULL) && *width ) {
	ErrAdd("Cannot make a bus connection between a galaxy port and its alias");
	EssAddObj(&inInst);
	EssAddObj(&outInst);
	return (FALSE);
    } else if (!inIsGalPort && outIsGalPort) {
	/* Connect an input to a Galaxy Port */
	if (octGenFirstContainer(outTermPtr, OCT_TERM_MASK, &fTerm)
	    != OCT_OK) {
	    ErrAdd("JoinOrdinary: input port has no name");
	    EssAddObj(&outInst);
	    return (FALSE);
	}
	ERR_IF1(!KcAlias(fTerm.contents.term.name,
	    inInst.contents.instance.name, inTermPtr->contents.term.name));
    } else if (inIsGalPort && !outIsGalPort) {
	/* Connect an output to a Galaxy Port */
	if (octGenFirstContainer(inTermPtr, OCT_TERM_MASK, &fTerm) !=OCT_OK) {
	    ErrAdd("JoinOrdinary: output port has no name");
	    EssAddObj(&inInst);
	    return (FALSE);
	}
	ERR_IF1(!KcAlias(fTerm.contents.term.name,
	    outInst.contents.instance.name, outTermPtr->contents.term.name));
    } else {
	ErrAdd("JoinOrdinary: cannot connect input galaxy port directly to output galaxy port");
	EssAddObj(&inInst);
	EssAddObj(&outInst);
	return (FALSE);
    }
    FreeOctMembers(&inInst);
    FreeOctMembers(&outInst);
    return (TRUE);
}

/* This function connects terminals to nodes; it is used in n-way connections*/
static boolean
JoinToNode(termPtr, nodename)
octObject *termPtr;
char *nodename;
{
    octObject inst;

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



/* 6/14/89 7/31/90
Emits connect, input, and output statements.
*/
#define BLEN 80

static boolean TermIsMulti();

static boolean
ConnectPass(facetPtr)
octObject *facetPtr;
{
    octObject net, in[TERMS_MAX], out[TERMS_MAX];
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
	    return FALSE;
	}
	totalN = inN + outN;
	if(GetStringizedProp(&net, "delay", delay, BLEN))
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
		octFreeGenerator(&netGen);
		ErrAdd(octErrorString());
		return FALSE;
	    }
	    PrintDebug("Warning: bad net deleted");
	    continue;
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
	    } else {
		/* error: multiple outputs are connected to one ordinary
		   input */
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
		for (i = 0; i < inN; i++) {
		    if (!JoinToNode(&in[i], nodename)) {
			errMsg = "";
			break;
		    }
		}
	    }
	    free(nodename);
	}
    }
    if (errMsg) {
	if (*errMsg) ErrAdd (errMsg);
	EssAddObj(&net);
    }
    octFreeGenerator(&netGen);
    /* free up the terms */
    for (i = 0; i < inN; i++) FreeOctMembers(&in[i]);
    for (i = 0; i < outN; i++) FreeOctMembers(&out[i]);
    return errMsg ? FALSE : TRUE;
}

/* 7/31/90
Checks to see if octObject is a MultiPortHole.
*/
static boolean
TermIsMulti(termPtr)
octObject *termPtr;
{
    octObject inst;

    ERR_IF2(GetById(&inst, termPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    return KcIsMulti(inst.contents.instance.name,
		     termPtr->contents.term.name);
}

/*
Checks to see whether a galaxy facet has been changed since the last run.
Outputs: return = true if facet has been changed or if not sure.
*/
static boolean
IsDirty(facetPtr)
octObject *facetPtr;
{
    octObject cl, cr;

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
Checks to see whether a universe facet has been changed since the last run, or
if the Ptolemy kernel image of the facet has been deleted.  The latter
occurs whenever the run control window is dismissed.
Outputs: return = true if facet has been changed or if not sure.
*/
static boolean
IsDirtyOrGone(facetPtr)
octObject *facetPtr;
{
    octObject cl, cr;
    char* name;

    /* First check to see whether the Ptolemy image exists */
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
    octObject cl;

    if (octGenFirstContent(facetPtr, OCT_CHANGE_LIST_MASK, &cl) == OCT_OK) {
	CK_OCT(octDelete(&cl));
    }
    cl.type = OCT_CHANGE_LIST;
    cl.contents.changeList.objectMask = OCT_ALL_MASK &
	~OCT_CHANGE_LIST_MASK & ~OCT_CHANGE_RECORD_MASK;
    cl.contents.changeList.functionMask = OCT_ATTACH_CONTENT_MASK |
	OCT_DETACH_CONTENT_MASK | OCT_MODIFY_MASK | OCT_PUT_POINTS_MASK;
    cl.objectId = 0;		/* silence Purify */
    CK_OCT(octCreate(facetPtr, &cl));
    octMarkTemporary(&cl);
    return (TRUE);
}

extern char* curDomainName();

/* compile a galaxy instance in context.  If the instance is in a facet,
   this works like CompileGalStandalone.  Used in Profile.
 */
boolean
CompileGalInst(galInstPtr,parentFacetPtr)
octObject *galInstPtr, *parentFacetPtr;
{
    char* galDomain;
    octObject galFacet;

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
    char* galDomain;
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

boolean
CompileGal(galFacetPtr)
octObject *galFacetPtr;
{
    char *name;
    boolean xferedBool;
    char *oldDomain, *galDomain, *galTarget, *desc;

    name = BaseName(galFacetPtr->contents.facet.cell);

    oldDomain = curDomainName();

    /* get the galaxy domain */
    if (!GOCDomainProp(galFacetPtr, &galDomain, oldDomain)) {
        PrintErr(ErrGet());
        return (FALSE);
    }

    /*
     * quit if the galaxy has already been touched in this
     * compilation process and it was in the same domain at the time.
     */

    if (DupSheetIsDup2(&traverse, name, oldDomain)) {
      /*
       * Detect recursion here by checking to see whether the
       * objct is on the knownlist of the oldDomain.
       */
      if (!KcIsKnown(name)) {
	PrintErr("Sorry, graphical recursion is not supported at this time");
	return(FALSE);
      }
      return(TRUE);
    }
    /*
     * mark the galaxy touched right away, in case there is a recursive
     * reference to it in the subgalaxies.
     */
    ERR_IF2(!DupSheetAdd2(&traverse, name, oldDomain),
	    "DupSheetAdd2 failed! Out of memory?");

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

    /*
     * quit if the galaxy has been transferred to the Ptolemy kernel
     * at any time during this session, it has not been modified, and
     * it was sitting in the domain context when it was transferred.
     */
    xferedBool = DupSheetIsDup2(&xfered, name, oldDomain);
    if (xferedBool && !IsDirty(galFacetPtr)) {
	/* galaxy already xfered to kernel and is unchanged */
	/* restore old domain */
	KcSetKBDomain(oldDomain);
	return (TRUE);
    }
    if (strcmp(oldDomain,galDomain) != 0)
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
    ERR_IF1(!KcEndDefgalaxy(oldDomain));
    ERR_IF2(!ClearDirty(galFacetPtr), "Failed to mark clean!");
    if (!xferedBool) {
	ERR_IF2(!DupSheetAdd2(&xfered, name, oldDomain),
		"DupSheetAdd2 failure! Out of memory?");
    }
    recompileFlag = 1;
    KcFlushLog();
    return (TRUE);
}

char* KcDefTarget();

static boolean
CompileUniv(facetPtr)
octObject *facetPtr;
{
    char *name;
    boolean xferedBool;
    char *domain;
    char* target;
    
    name = BaseName(facetPtr->contents.facet.cell);
    if (!GOCDomainProp(facetPtr, &domain, DEFAULT_DOMAIN)) {
	PrintErr(ErrGet());
	sprintf(msg, "Domain error in facet %s", name);
	PrintErr(msg);
	return FALSE;
    }
    sprintf(msg, "reset\ndomain %s\n", domain);
    KcLog(msg);
    TCL_CATCH_ERR1(Tcl_Eval(ptkInterp, msg));

    recompileFlag = 0;
    ERR_IF1(!ProcessSubGals(facetPtr));
    xferedBool = DupSheetIsDup(&xfered, name);
    if (xferedBool && !IsDirtyOrGone(facetPtr) && !recompileFlag) {
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
    ERR_IF1(!ClearDirty(facetPtr));
    if (!xferedBool) {
	ERR_IF1(!DupSheetAdd(&xfered, name));
    }
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
    octObject facet;

    ViInit("compile-facet");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
	ViDone();
    }

    if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
        PrintErr("Failed to set default domain.");
        ViDone();
    }

    if (!CompileFacet(&facet)) {
	PrintErr(ErrGet());
	ViDone();
    }
    ViDone();
}

/* Called once when pigi is started */
void
CompileInit()
{
    DupSheetInit(&traverse);
    DupSheetInit(&xfered);
}

void
CompileEnd()
{
    /* clean up tmp file before exiting */
}

/* NEW FUNCTION */

int
RpcRunAllDemos(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet;
    char* name;
    char octHandle[16];

    ViInit("run-all-demos");
    ErrClear();
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
	ViDone();
    }
    if (!IsPalFacet(&facet)) {
	PrintErr("This command must be run in a palette");
	ViDone();
    }

    name = BaseName(facet.contents.facet.cell);
    ptkOctObj2Handle(&facet, octHandle);

    if (Tcl_VarEval(ptkInterp, "ptkRunAllDemos ", name, " ", octHandle,
      (char *) NULL) == TCL_ERROR) {
	PrintErr("Run-all-demos control panel cannot be brought up");
	ViDone();
    }
    RunAll(&facet);
    TCL_CATCH_ERR(Tcl_VarEval(ptkInterp, "ptkRunAllDemosDel ", name,
      " ", octHandle, (char *) NULL));
    FreeOctMembers(&facet);
    ViDone();
}

/* run everything in the facet, which is assumed to be a palette */
static boolean
RunAll(facetPtr)
octObject *facetPtr;
{
    octGenerator genInst;
    octObject inst;
    octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	octObject univFacet;

	if (!MyOpenMaster(&univFacet,&inst, "contents", "r")) {
	    octFreeGenerator(&genInst);
	    PrintErr(octErrorString());
	    return FALSE;
	}
	/* skip any non-universes */
	if (IsCursor(&inst)) {
	    /* nothing */;
	}
	else if (IsUnivFacet(&univFacet)) {
	    char * name = BaseName(univFacet.contents.facet.cell);
	    char octHandle[16];

	    sprintf(msg, "RunAllDemos: executing universe '%s'", name);
	    PrintDebug(msg);
	    ptkOctObj2Handle(&univFacet, octHandle);
	    /* ptkCompileRun is very similar to ptkGo, but assuming controlled
	       by the run-all-demos panel instead of ordinary run control
	       panel. */
	    TCL_CATCH_ERR(Tcl_VarEval(ptkInterp, "ptkCompileRun ", name,
	      " ", octHandle, (char *) NULL));
	    /* delete the universe after run finishes. */
	    TCL_CATCH_ERR1(Tcl_VarEval(ptkInterp, "ptkDelLite ", name,
	      " ", octHandle, (char *) NULL));
	}
	else if (IsPalFacet(&univFacet)) {
	    RunAll(&univFacet);
	}
	FreeOctMembers(&univFacet);
	FreeOctMembers(&inst);
    }
    octFreeGenerator(&genInst);
    return TRUE;
}
