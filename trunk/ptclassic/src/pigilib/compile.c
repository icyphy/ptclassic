/* compile.c  edg
Version identification:
$Id$
Updates:
    4/16/90 = added Oct change lists to keep track of changes so that only
    facets that the user changes get compiled.

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
*/

/* Includes */
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include "local.h"
#include "rpc.h"
#include "paramStructs.h"
#include "kernelCalls.h"
#include "vemInterface.h"
#include "util.h"
#include "err.h"
#include "main.h"
#include "octIfc.h"
#include "octMacros.h"

#define TERMS_MAX 50  /* maximum number of actual terms allowed on a net */

DupSheet traverse;  /* used to traverse design heriarchy which is a DAG */
    /* DAG = directed acyclic graph */
DupSheet xfered;  /* list of galaxies that have been transfered to the kernel */


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
    ERR_IF2(vemAddSelSet(essSelSet, obj->objectId) != VEM_OK,
	"EssAddObj: Cannot put object in error select set");
    return (TRUE);
}
/***** End Ess routines (Error Select Set) */


/* currently, all galaxy parameters are type "float".  We need to fix
   this.
 */
static boolean
ProcessFormalParams(galFacetPtr)
octObject *galFacetPtr;
{
    ParamListType pList;
    ParamType *p;
    int i;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));
    for (i = 0, p = pList.array; i < pList.length; i++, p++) {
	ERR_IF1(!KcMakeState(p->name, "float", p->value));
    }
    return(TRUE);
}

static boolean
ProcessSubGals(facetPtr)
octObject *facetPtr;
{
    octGenerator genInst;
    octObject inst, galFacet;

    (void) octInitGenContents(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	if (IsVemConnector(&inst) || IsIoPort(&inst) || IsDelay(&inst)) {
	    /* skip */
	    continue;
	} else if (IsGal(&inst)) {
	    ERR_IF1(!MyOpenMaster(&galFacet, &inst, "contents", "r"));
	    ERR_IF1(!CompileGal(&galFacet));
	} else {
	    /* assume inst is a star... */
	    if (!AutoLoadCk(&inst)) {
		EssAddObj(&inst);
		return (FALSE);
	    }
	}
    }
    return(TRUE);
}

/* DetachDelaysFromNets  5/29/88
Detach any delay props from nets in facet.
*/
static void
DetachDelaysFromNets(facetPtr)
octObject *facetPtr;
{
    octObject net, prop;
    octGenerator netGen;

    (void) octInitGenContents(facetPtr, OCT_NET_MASK, &netGen);
    while (octGenerate(&netGen, &net) == OCT_OK) {
	prop.type = OCT_PROP;
	prop.contents.prop.name = "delay";
	if (octGetByName(&net, &prop) == OCT_NOT_FOUND) {
	    continue;
	}
	(void) octDetach(&net, &prop);
    }
}

/* ProcessDelay  5/28/88
Process delay instance by attaching its delay prop to the net associated
with the delay instance.
Inputs: facetPtr, instPtr
Outputs: return = FALSE if error
*/
static boolean
ProcessDelay(facetPtr, instPtr)
octObject *facetPtr, *instPtr;
{
    octObject path, dummy, net, prop;
    struct octBox box;
    regObjGen rGen;

    if (octBB(instPtr, &box) != OCT_OK) {
	ErrAdd("ProcessDelay: Instance has no bounding box");
	return(FALSE);
    }

    (void) regObjStart(facetPtr, &box, OCT_PATH_MASK, &rGen, 0);
    if (regObjNext(rGen, &path) == REG_NOMORE) {
	ErrAdd("Dangling delay");
	EssAddObj(instPtr);
	(void) regObjFinish(rGen);
	return(FALSE);
    }
    if (regObjNext(rGen, &dummy) != REG_NOMORE) {
	ErrAdd("Delay intersects more than one path");
	EssAddObj(instPtr);
	(void) regObjFinish(rGen);
	return(FALSE);
    }

    if (octGenFirstContainer(&path, OCT_NET_MASK, &net) != OCT_OK) {
	ErrAdd("ProcessDelay: Path not contained in a net");
	EssAddObj(&path);
	return(FALSE);
    }
    prop.type = OCT_PROP;
    prop.contents.prop.name = "delay";
    if (octGetByName(&net, &prop) != OCT_NOT_FOUND) {
	ErrAdd("Net has more than one delay instance on top of it");
	EssAddObj(&net);
	return(FALSE);
    }
    GetOrInitDelayProp(instPtr, &prop);
    ERR_IF2(octAttach(&net, &prop) != OCT_OK, octErrorString());
    return(TRUE);
}

static boolean
ProcessInsts(facetPtr)
octObject *facetPtr;
{
    octGenerator genInst;
    octObject inst;
    char *name;
    ParamListType pList;
    char *akoName;
    
    DetachDelaysFromNets(facetPtr);
    (void) octInitGenContents(facetPtr, OCT_INSTANCE_MASK, &genInst);
    while (octGenerate(&genInst, &inst) == OCT_OK) {
	if (IsVemConnector(&inst) || IsIoPort(&inst)) {
	    /* skip */
	    continue;
	} else if (IsDelay(&inst)) {
	    ERR_IF1(!ProcessDelay(facetPtr, &inst));
	} else {
	    /* assume it's a sog */
	    ERR_IF1(!GetOrInitSogParams(&inst, &pList));
	    if (IsGal(&inst)) {
		/* do defgalaxy ako... */
		akoName = BaseName(inst.contents.instance.master); 
		ERR_IF1((name = UniqNameGet(akoName)) == NULL);
		inst.contents.instance.name = name;
		ERR_IF2(octModify(&inst) != OCT_OK, octErrorString());
		ERR_IF1(!KcInstance(name, akoName, &pList));
	    } else {
		/* assume inst is a star... */
		akoName = AkoName(inst.contents.instance.master);
		ERR_IF1((name = UniqNameGet(akoName)) == NULL);
		inst.contents.instance.name = name;
		ERR_IF2(octModify(&inst) != OCT_OK, octErrorString());
		ERR_IF1(!KcInstance(name, akoName, &pList));
	    }
	}
    }
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
    char buf[1000];

    ERR_IF2(GetById(&inst, aTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    ERR_IF1(!MyOpenMaster(&master, &inst, "interface", "r"));
    ERR_IF2(GetByTermName(&master, &fTerm, aTermPtr->contents.term.name)
	!= OCT_OK, octErrorString());
    if (GetByPropName(&fTerm, &prop, "input") != OCT_NOT_FOUND) {
	*result = TRUE;
	return (TRUE);
    }
    if (GetByPropName(&fTerm, &prop, "output") != OCT_NOT_FOUND) {
	*result = FALSE;
	return (TRUE);
    }
    *result = FALSE;
    sprintf(buf, "IsInputTerm: terminal is of unknown type, '(%s %s)'",
	inst.contents.instance.name, aTermPtr->contents.term.name);
    ErrAdd(buf);
    EssAddObj(aTermPtr);
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
    (void) octInitGenContents(netPtr, OCT_TERM_MASK, &termGen);
    for (i = 0; i < TERMS_MAX; ) {
	if (octGenerate(&termGen, &term) != OCT_OK) {
	    break;
	}
	if (term.contents.term.instanceId != oct_null_id) {
	    /* actual term */
	    ERR_IF1(!IsInputTerm(&term, &result));
	    if (result) {
		in[(*inN)++] = term;
	    } else {
		out[(*outN)++] = term;
	    }
	    i++;
	}
    }
    if (i >= TERMS_MAX) {
	ErrAdd("CollectTerms: Too many actual terminals on net");
	EssAddObj(netPtr);
	return (FALSE);
    }
    return (TRUE);
}    

/* 7/31/90
   Insert a fork star and use it to connect things up.
   This function requires a star class in the current domain named
   Fork with terminals "input" and "output".
 */

static boolean
JoinWithFork(inTermPtr, outTermPtr, n)
octObject *inTermPtr, *outTermPtr;
int n;
{
    octObject inInst, outInst;
    char* forkname = UniqNameGet("!af");
    boolean outIsConn;
    int i;

    PrintDebug ("JoinWithFork: inserting a Fork star");
    ERR_IF1(!KcInstance (forkname, "Fork", (ParamListType *)0));
    ERR_IF2(GetById(&outInst, outTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    outIsConn = IsIoPort(&outInst);
    if (outIsConn) {
	ErrAdd ("JoinWithFork: output as alias not supported");
	return FALSE;
    }
    ERR_IF1(!KcConnect(outInst.contents.instance.name,
		       outTermPtr->contents.term.name,forkname,"input", 0));
    for (i = 0; i < n; i++) {
	ERR_IF2(GetById(&inInst, inTermPtr->contents.term.instanceId) != OCT_OK,
	    octErrorString());
	if (IsIoPort(&inInst)) {
	    ErrAdd ("JoinWithFork: input as alias not supported");
	    return FALSE;
	}
	ERR_IF1(!KcConnect(forkname,"output",inInst.contents.instance.name,
			   inTermPtr->contents.term.name, 0));
	inTermPtr++;
    }
    return (TRUE);
}

/* 6/14/89
*/
static boolean
JoinOrdinary(inTermPtr, outTermPtr, delay)
octObject *inTermPtr, *outTermPtr;
int delay;
{
    octObject inInst, outInst, fTerm;
    boolean inIsConn, outIsConn;

    ERR_IF2(GetById(&inInst, inTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    inIsConn = IsIoPort(&inInst);
    ERR_IF2(GetById(&outInst, outTermPtr->contents.term.instanceId) != OCT_OK,
	octErrorString());
    outIsConn = IsIoPort(&outInst);

    if (!inIsConn && !outIsConn) {
	/* 2 sogs connected */
	ERR_IF1(!KcConnect(
	    outInst.contents.instance.name, outTermPtr->contents.term.name, 
	    inInst.contents.instance.name, inTermPtr->contents.term.name, delay)
	);
    } else if (!inIsConn && outIsConn) {
	if (octGenFirstContainer(outTermPtr, OCT_TERM_MASK, &fTerm)
	    != OCT_OK) {
	    ErrAdd("JoinOrdinary: input port has no name");
	    EssAddObj(&outInst);
	    return (FALSE);
	}
	ERR_IF1(!KcAlias(fTerm.contents.term.name,
	    inInst.contents.instance.name, inTermPtr->contents.term.name));
    } else if (inIsConn && !outIsConn) {
	if (octGenFirstContainer(inTermPtr, OCT_TERM_MASK, &fTerm) !=OCT_OK) {
	    ErrAdd("JoinOrdinary: output port has no name");
	    EssAddObj(&inInst);
	    return (FALSE);
	}
	ERR_IF1(!KcAlias(fTerm.contents.term.name,
	    outInst.contents.instance.name, outTermPtr->contents.term.name));
    } else {
	ErrAdd("JoinOrdinary: cannot connect input port directly to output port");
	EssAddObj(&inInst);
	EssAddObj(&outInst);
	return (FALSE);
    }
    return (TRUE);
}

/* 6/14/89 7/31/90
Emits connect, input, and output statements.  Also handles connects to
multiportholes and auto-forks.
*/
static boolean
ConnectPass(facetPtr)
octObject *facetPtr;
{
    octObject net, in[TERMS_MAX], out[TERMS_MAX];
    octObject delayProp;
    octGenerator netGen;
    int inN, outN, totalN, i, delay;

    (void) octInitGenContents(facetPtr, OCT_NET_MASK, &netGen);
    while (octGenerate(&netGen, &net) == OCT_OK) {
	ERR_IF1(!CollectTerms(&net, in, &inN, out, &outN));
	totalN = inN + outN;
        delay = (GetByPropName(&net, &delayProp, "delay") == OCT_NOT_FOUND)
		? 0 : (int) delayProp.contents.prop.value.integer;
	if (totalN == 2) {
	    /* possible ordinary connection */
	    if (inN != 1 && outN != 1) {
		ErrAdd("ConnectPass: cannot match an input to an output");
		EssAddObj(&net);
		return (FALSE);
	    }
	    ERR_IF1(!JoinOrdinary(&in[0], &out[0], delay));
	}
	else if (totalN < 2) {
	    /* bad net, delete it */
	    ERR_IF2(octDelete(&net) != OCT_OK, octErrorString());
	    PrintDebug("Warning: bad net deleted");
	}
    /* totalN > 2: possible auto-fork or multiport connection */
	else if (inN == 1 && outN > 1) {
	    if (TermIsMulti(&in[0])) {
		for (i = 0; i < outN; i++) {
		    ERR_IF1(!JoinOrdinary(&in[0], &out[i], delay));
		}
	    }
	    else {
	        ErrAdd("ConnectPass: single input porthole has more than 1 output");
		EssAddObj(&net);
		return (FALSE);
	    }
	}
	else if (outN == 1 && inN > 1) {
	    if (TermIsMulti(&out[0])) {
		for (i = 0; i < inN; i++) {
		    ERR_IF1(!JoinOrdinary(&in[i], &out[0], delay));
		}
		return (TRUE);
	    }
	    /* auto-fork connection... */
	    if (delay > 0) {
		ErrAdd("ConnectPass: illegal delay on auto-fork net");
		EssAddObj(&net);
		return (FALSE);
	    }
	    /* create a fork star and add it */
	    ERR_IF1(!JoinWithFork(in,out,inN));
	}
	else {
	    ErrAdd("ConnectPass: multiple inputs and outputs");
	    EssAddObj(&net);
	    return (FALSE);
	}
    }
    return (TRUE);
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
Checks to see if facet has been changed.
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
    CK_OCT(octCreate(facetPtr, &cl));
    octMarkTemporary(&cl);
    return (TRUE);
}

static boolean
CompileGal(galFacetPtr)
octObject *galFacetPtr;
{
    char msg[1000], *name;
    boolean xferedBool;

    name = BaseName(galFacetPtr->contents.facet.cell);
    if (DupSheetIsDup(&traverse, name)) {
	return(TRUE);
    }
    ERR_IF2(!ProcessSubGals(galFacetPtr), msg);
    xferedBool = DupSheetIsDup(&xfered, name);
    if (xferedBool && !IsDirty(galFacetPtr)) {
	/* galaxy already xfered to kernel and is unchanged */
	return (TRUE);
    }
    sprintf(msg, "CompileGal: facet = %s", name);
    PrintDebug(msg);
    ERR_IF1(!KcDefgalaxy(name));
    ERR_IF2(!ProcessFormalParams(galFacetPtr), msg);
    ERR_IF2(!ProcessInsts(galFacetPtr), msg);
    ERR_IF2(!ConnectPass(galFacetPtr), msg);
    ERR_IF1(!KcEndDefgalaxy());
    ERR_IF2(!DupSheetAdd(&traverse, name), msg);
    ERR_IF2(!ClearDirty(galFacetPtr), msg);
    if (!xferedBool) {
	ERR_IF2(!DupSheetAdd(&xfered, name), msg);
    }
    return (TRUE);
}

static boolean
CompileUniv(facetPtr)
octObject *facetPtr;
{
    char *name;
    boolean xferedBool;

    name = BaseName(facetPtr->contents.facet.cell);
    ERR_IF1(!ProcessSubGals(facetPtr));
    xferedBool = DupSheetIsDup(&xfered, name);
    if (xferedBool && !IsDirty(facetPtr)) {
	/* universe already xfered to kernel and is unchanged */
	return (TRUE);
    }
    PrintDebug("CompileUniv");
    KcClearUniverse();
    ERR_IF1(!ProcessInsts(facetPtr));
    ERR_IF1(!ConnectPass(facetPtr));
    ERR_IF1(!ClearDirty(facetPtr));
    if (!xferedBool) {
	ERR_IF1(!DupSheetAdd(&xfered, name));
    }
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
    static octId lastFacetId = OCT_NULL_ID;

    /* if this is not the same facet as last time... */
    if (!octIdsEqual(lastFacetId, facetPtr->objectId)) {
	/* Different facet, so compile everything */
	DupSheetClear(&xfered);
	lastFacetId = OCT_NULL_ID;  /* forget last facet */
    }

    ERR_IF1(!UniqNameInit());
    DupSheetClear(&traverse);
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
    lastFacetId = facetPtr->objectId;  /* remember this facet */
    return (TRUE);
}

/* RpcCompileFacet  5/30/88 4/18/88
RPC function to compile facet
*/
int 
RpcCompileFacet(spot, cmdList, userOptionWord)
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
    if (!CompileFacet(&facet)) {
	PrintErr(ErrGet());
	ViDone();
    }
    ViDone();
}

void
CompileInit()
{
    DupSheetInit(&traverse);
    DupSheetInit(&xfered);
}

CompileEnd()
{
    /* clean up tmp file before exiting */
}
