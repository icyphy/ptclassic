/*  octIfc.c  edg
Version identification:
$Id$
Useful higher level OCT interface functions.
*/

/* Includes */
#include <stdio.h>
#include <strings.h>
#include "local.h"
#include "rpc.h"
#include "paramStructs.h"
#include "err.h"
#include "util.h"
#include "octMacros.h"
#include "mkIcon.h"
#include "oh.h"
#include "region.h"
#include "oct.h"
#include "mkTerm.h"

/* 8/14/89
Opens the master of an instance, but allows you to choose which facet.
*/
boolean
MyOpenMaster(t, i, f, m)
octObject *t, *i;
char *f, *m;
{
    octObject rfacet;

    octGetFacet(i, &rfacet);
    t->type = OCT_FACET;
    t->contents.facet.cell = i->contents.instance.master;
    t->contents.facet.view = i->contents.instance.view;
    t->contents.facet.facet = f;
    t->contents.facet.version = OCT_CURRENT_VERSION;
    t->contents.facet.mode = m;
    CK_OCT(octOpenRelative(&rfacet, t, OCT_SIBLING));
    return (TRUE);
}

/*****  Isxxx  1/30/89
The following Isxxx functions test for types of instances in a facet.
*/
boolean 
IsVemConnector(instPtr)
octObject *instPtr;
{
    return(strcmp(instPtr->contents.instance.name, "*") == 0);
}

boolean 
IsIoPort(instPtr)
octObject *instPtr;
{
    return(strncmp(BaseName(instPtr->contents.instance.master), 
	"%p", 2) == 0);
}

boolean 
IsInputPort(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master), 
	"%pInput") == 0);
}

boolean 
IsDelay(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master), "%dDelay") == 0);
}

boolean 
IsGal(instPtr)
octObject *instPtr;
{
    octObject master, prop;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    return (GetByPropName(&master, &prop, "galaxy") != OCT_NOT_FOUND);
}

boolean 
IsStar(instPtr)
octObject *instPtr;
{
    octObject master, prop;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    return (GetByPropName(&master, &prop, "star") != OCT_NOT_FOUND);
}

boolean 
IsUniv(instPtr)
octObject *instPtr;
{
    octObject master, prop;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    return (GetByPropName(&master, &prop, "universe") != OCT_NOT_FOUND);
}

boolean 
IsPal(instPtr)
octObject *instPtr;
{
    octObject master, prop;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    return (GetByPropName(&master, &prop, "palette") != OCT_NOT_FOUND);
}

boolean 
IsCursor(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master),"%cCursor") == 0);
}

/***** end of Isxxx functions */


/* 1/30/89
Test to see if facet is a galaxy by looking for a formal term.
*/
boolean 
IsGalFacet(facetPtr)
octObject *facetPtr;
{
    octObject fterm;

    return(octGenFirstContent(facetPtr, OCT_TERM_MASK, &fterm) == OCT_OK);
}

/* 8/8/89
Test to see if facet is a palette by looking for a cursor.
*/
boolean 
IsPalFacet(facetPtr)
octObject *facetPtr;
{
    octObject inst;
    octGenerator gen;

    octInitGenContents(facetPtr, OCT_INSTANCE_MASK, &gen);
    while (octGenerate(&gen, &inst) == OCT_OK) {
	if (IsCursor(&inst)) {
	    return (TRUE);
	}
    }
    return (FALSE);
}

/* 8/8/89
*/
boolean 
IsUnivFacet(facetPtr)
octObject *facetPtr;
{
    return (!(IsGalFacet(facetPtr) || IsPalFacet(facetPtr)));
}

/* AkoName 2.0  8/29/88 7/1/88 4/17/88
Derive name of ako star from name of master facet.
Inputs: masterName = cell name of master facet
Outputs: return = pointer to part of masterName containting ako name
Updates:
    8/29/88 = cancel facet version idea so just use BaseName.
*/
char *
AkoName(masterName)
char *masterName;
{
    return(BaseName(masterName));
}

/* 3/19/89
Set formal params of a galaxy schematic.
*/
boolean
SetFormalParams(galFacetPtr, pListPtr)
octObject *galFacetPtr;
ParamListType *pListPtr;
{
    char *pStr;
    octObject prop;
    octStatus status;

    pStr = PListToPStr(pListPtr);
    prop.type = OCT_PROP;
    prop.contents.prop.name = "params";
    prop.contents.prop.type = OCT_STRING;
    prop.contents.prop.value.string = pStr;
    status = octCreateOrModify(galFacetPtr, &prop);
    free(pStr);
    if (status != OCT_OK) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/* 7/26/90 3/19/89 1/31/89
Get formal params of a galaxy schematic.
There is always one extra slot in the returned pList.
*/
boolean
GetFormalParams(galFacetPtr, pListPtr)
octObject *galFacetPtr;
ParamListType *pListPtr;
{
    octObject prop;

    if (GetByPropName(galFacetPtr, &prop, "params") == OCT_NOT_FOUND) {
	pListPtr->length = 0;
	pListPtr->array = (ParamType *) calloc(1, sizeof(ParamType));
    } else {
	ERR_IF1(!PStrToPList(prop.contents.prop.value.string, pListPtr));
    }
    return(TRUE);
}

/* SetSogParams  1/31/89 4/18/88 4/16/88
Set instance params.
Inputs:
    instPtr = adr of instance of star
    pListPtr = adr of ParamList
Outputs:
    return = whether it was successful
Updates:
1/31/89 = name change
*/
boolean
SetSogParams(instPtr, pListPtr)
octObject *instPtr;
ParamListType *pListPtr;
{
    char *pStr;
    octObject prop;
    octStatus status;

    pStr = PListToPStr(pListPtr);
    prop.type = OCT_PROP;
    prop.contents.prop.name = "params";
    prop.contents.prop.type = OCT_STRING;
    prop.contents.prop.value.string = pStr;
    status = octCreateOrModify(instPtr, &prop);
    free(pStr);
    if (status != OCT_OK) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/* 
Check if star corresponding to instPtr, is known to the kernel and if not,
try to load it in.
*/
boolean
AutoLoadCk(instPtr)
octObject *instPtr;
{
    char buf[128];
    char *akoName;

    akoName = AkoName(instPtr->contents.instance.master);
    if (KcIsKnown(akoName)) {
	return (TRUE);
    }
    PrintCon(
	sprintf(buf,
	  "Unknown star '%s' in current domain, trying to load it...",
	  akoName));
    return LoadTheStar(instPtr);
}

/* used to be part of AutoLoadCk; this always loads the star,
   whether known or not.
 */
boolean
LoadTheStar(instPtr)
octObject *instPtr;
{
    char *akoName = AkoName(instPtr->contents.instance.master);
    octObject mFacet;
    char *fullName;

    ERR_IF1(!MyOpenMaster(&mFacet, instPtr, "interface", "r"));
    octFullName(&mFacet, &fullName);
    ERR_IF1(!KcLoad(fullName));
    if (KcIsKnown(akoName)) {
	PrintCon("Load completed");
	return (TRUE);
    }
    ErrAdd("Load failed.");
    return (FALSE);
}

/* GetOrInitSogParams
Get sog params if it exists or else init them first then return params.
Inputs:
    instPtr = adr of instance object to define params for
    pListPtr = adr of an empty ParamList 
Outputs: return = TRUE if successful, else FALSE.
*/
boolean
GetOrInitSogParams(instPtr, pListPtr)
octObject *instPtr;
ParamListType *pListPtr;
{
    octObject prop;
    ParamListType tempList;
    void MergeParams();

    ERR_IF1(!GetDefaultParams(instPtr, pListPtr));
    prop.type = OCT_PROP;
    prop.contents.prop.name = "params";
    if (octGetByName(instPtr, &prop) == OCT_NOT_FOUND) {
	/* no parameters: use default list */
	ERR_IF1(!SetSogParams(instPtr, pListPtr));
	return(TRUE);
    }
    if (!PStrToPList(prop.contents.prop.value.string, &tempList))
	return(FALSE);
    MergeParams(pListPtr,&tempList);
    return(TRUE);
}

/* find a parameter name in a ParamListType. */
ParamType*
findParam(name, pList)
char* name;
ParamListType *pList;
{
    int i;
    ParamType* curr = pList->array;
    for (i = 0; i < pList->length; i++) {
	if (strcmp (name, curr->name) == 0) return curr;
	curr++;
    }
    return 0;
}

/* merge a set of instance parameters (2nd arg) with a set of default
 * parameters.    return the "official" ones in the 1st argument (default
 * parameters).
 */
void
MergeParams(pListDef,pListInst)
ParamListType *pListDef, *pListInst;
{
    int i;
    ParamType* match;
    ParamType* curr = pListDef->array;
    for (i = 0; i < pListDef->length; i++) {
	match = findParam (curr->name, pListInst);
	if (match) curr->value = match->value;
	curr++;
    }
    return;
}

/* GetDefaultParams
Get default values of parameters.
Inputs, Outputs: same as for GetOrInitSogParams
*/
boolean
GetDefaultParams(instPtr, pListPtr)
octObject *instPtr;
ParamListType *pListPtr;
{
    if (IsGal(instPtr)) {
	octObject galFacet;
	ERR_IF1(!MyOpenMaster(&galFacet, instPtr, "contents", "r"));
	ERR_IF1(!GetFormalParams(&galFacet, pListPtr));
    }
    else {
	char *starName;
	ERR_IF1(!AutoLoadCk(instPtr));
	starName = AkoName(instPtr->contents.instance.master);
	ERR_IF1(!KcGetParams(starName, pListPtr));
    }
    return TRUE;
}



/* GetOrInitDelayProp  7/11/88 5/28/88 5/27/88
Get delay prop or if not exist, create default prop and return it.
Note: this should be the only place where the default delay value is set.
Inputs: instPtr, propPtr = ptr to empty prop
Outputs:
    propPtr = delay prop attached to delay instance
    return = TRUE
*/
void
GetOrInitDelayProp(instPtr, propPtr)
octObject *instPtr, *propPtr;
{
    propPtr->type = OCT_PROP;
    propPtr->contents.prop.name = "delay";
    propPtr->contents.prop.type = OCT_INTEGER;
    propPtr->contents.prop.value.integer = 1;
    (void) octGetOrCreate(instPtr, propPtr);
}

/* This function is used by GetGalTerms.  It will find the datatype of fTerm.
Right now I (Wan-teh) use the following scheme.  Each galaxy porthole is just
an alias for a porthole of a star or subgalaxy.  I will color the stem of fTerm
with the same color as the stem of the star or subgalaxy porthole it is an 
alias for. 
*/
static boolean
FindTermType(fTermPtr, aTermPtr, typeNamePtr)
octObject *fTermPtr, *aTermPtr;
char **typeNamePtr;
{
    octObject net, term, subInst, subfterm, subfacet, thePath, layer;
    octGenerator gen;
    struct octBox bb;
    regObjGen theGen;
    char *layerName;

    octGenFirstContainer(fTermPtr, OCT_NET_MASK, &net);
    octInitGenContents(&net, OCT_TERM_MASK, &gen);
    do
    {
        octGenerate(&gen, &term);
    } while (term.objectId == fTermPtr->objectId 
	     || term.objectId == aTermPtr->objectId);
    octGenFirstContainer(&term, OCT_INSTANCE_MASK, &subInst);
    ohFindFormal(&subfterm, &term);
    ohTerminalBB(&subfterm, &bb);
    subfacet.type = OCT_FACET;
    subfacet.contents.facet.cell = subInst.contents.instance.master;
    subfacet.contents.facet.view = "schematic";
    subfacet.contents.facet.facet = "interface";
    subfacet.contents.facet.version = OCT_CURRENT_VERSION;
    subfacet.contents.facet.mode = "r";
    octOpenFacet(&subfacet);
    regObjStart(&subfacet, &bb, OCT_PATH_MASK, &theGen, 0);
    regObjNext(theGen, &thePath);
    regObjFinish(theGen);
    octGenFirstContainer(&thePath, OCT_LAYER_MASK, &layer);
    octCloseFacet(&subfacet);
    layerName = layer.contents.layer.name;
    /* Note that icon stems used to be implemented on blueSolid, blueOutline,
       etc, so for backward compatibility before we re-implement all the
       icon stems on the new floatColor, intColor, ... layers, we still need
       the second test in the following if clauses. */
    if (strcmp(layerName, "floatColor") == 0 || 
	strncmp(layerName, FLOAT_COLOR, strlen(FLOAT_COLOR)) == 0) {
	StrDup(typeNamePtr, "float");
    } else if (strcmp(layerName, "intColor") == 0 || 
	       strncmp(layerName, INT_COLOR, strlen(INT_COLOR)) == 0) {
        StrDup(typeNamePtr, "int");
    } else if (strcmp(layerName, "complexColor") == 0 ||
	       strncmp(layerName, COMPLEX_COLOR, strlen(COMPLEX_COLOR)) == 0) {
	StrDup(typeNamePtr, "complex");
    } else if (strcmp(layerName, "anytypeColor") == 0 ||
	       strncmp(layerName, ANYTYPE_COLOR, strlen(ANYTYPE_COLOR)) == 0) {
	StrDup(typeNamePtr, "anytype");
    } else if (strcmp(layerName, "packetColor") == 0) {
	StrDup(typeNamePtr, "packet");
    } else { /* layer is nonstandard, i.e., not blueSolid, blueOutline, etc */
	/* return as default type "float" */
	StrDup(typeNamePtr, "float");
    }
}

/* GetGalTerms  2/4/89
Generates a TermList for a galaxy schematic facet.
Caveats: Allocates space but does not deallocate before exit.
*/
boolean
GetGalTerms(galFacetPtr, termsPtr)
octObject *galFacetPtr;
TermList *termsPtr;
{
    octObject fTerm, aTerm, inst;
    octGenerator genTerm;
    char *name;
    Term *inPtr, *outPtr;
    char *type;


    termsPtr->in_n = 0;
    termsPtr->out_n = 0;
    inPtr = termsPtr->in;
    outPtr = termsPtr->out;
    (void) octInitGenContents(galFacetPtr, OCT_TERM_MASK, &genTerm);
    while (octGenerate(&genTerm, &fTerm) == OCT_OK) {
	ERR_IF1(!StrDup(&name, fTerm.contents.term.name));
	ERR_IF2(octGenFirstContent(&fTerm, OCT_TERM_MASK, &aTerm) != OCT_OK,
	    octErrorString());;
	ERR_IF2(GetById(&inst, aTerm.contents.term.instanceId) != OCT_OK,
	    octErrorString());
	FindTermType(&fTerm, &aTerm, &type);
	if (IsInputPort(&inst)) {
	    ERR_IF2(termsPtr->in_n >= TERM_ARR_MAX,
		"GetGalTerms: too many input terminals");
	    inPtr->name = name;
	    inPtr->type = type;
	    inPtr->multiple = FALSE;
	    inPtr++;
	    termsPtr->in_n++;
	} else {
	    ERR_IF2(termsPtr->out_n >= TERM_ARR_MAX,
		"GetGalTerms: too many output terminals");
	    outPtr->name = name;
	    outPtr->type = type;
	    outPtr->multiple = FALSE;
	    outPtr++;
	    termsPtr->out_n++;
	}
    }
    octFreeGenerator(&genTerm);
    return(TRUE);
}

/* GetCommentProp  5/31/89
Outputs: commentPtr = returns comment string.  NULL means the prop doesn't
    exist (or has a NULL value).
*/
boolean
GetCommentProp(objPtr, commentPtr)
octObject *objPtr;
char **commentPtr;
{
    octObject prop;

    *commentPtr = (GetByPropName(objPtr, &prop, "comment") == OCT_NOT_FOUND)
	? NULL : prop.contents.prop.value.string;
    return(TRUE);
}

/* SetCommentProp  5/31/89
Inputs: comment = string, if string is NULL or == "" then the comment prop
    is deleted, if it exists.
*/
boolean
SetCommentProp(objPtr, comment)
octObject *objPtr;
char *comment;
{
    octObject prop;

    if (comment == NULL || *comment == '\0') {
	/* empty comment: delete prop if it exists */
	if (GetByPropName(objPtr, &prop, "comment") != OCT_NOT_FOUND) {
	    ERR_IF2(octDelete(&prop) != OCT_OK, octErrorString());
	}
    } else {
	ERR_IF2(CreateOrModifyPropStr(objPtr, &prop, "comment", comment) 
	    != OCT_OK, octErrorString());
    }
    return(TRUE);
}

/* 9/22/90, EAL
*/
boolean
GOCDomainProp(facetPtr, domainPtr, defaultDomain)
octObject *facetPtr;
char **domainPtr;
char *defaultDomain;
{
    octObject prop;

    CK_OCT(ohGetOrCreatePropStr(facetPtr, &prop, "domain", defaultDomain));
    *domainPtr = prop.contents.prop.value.string;
    return (TRUE);
}

/* 8/7/89
*/
boolean
SetDomainProp(facetPtr, domain)
octObject *facetPtr;
char *domain;
{
    octObject prop;

    CK_OCT(ohCreateOrModifyPropStr(facetPtr, &prop, "domain", domain));
    return (TRUE);
}

/* 1/28/91, EAL
*/
boolean
GOCArchProp(facetPtr, archPtr, defaultArch)
octObject *facetPtr;
char **archPtr;
char *defaultArch;
{
    octObject prop;

    CK_OCT(ohGetOrCreatePropStr(facetPtr, &prop, "architecture", defaultArch));
    *archPtr = prop.contents.prop.value.string;
    return (TRUE);
}

/* 1/28/91, EAL
*/
boolean
SetArchProp(facetPtr, arch)
octObject *facetPtr;
char *arch;
{
    octObject prop;

    CK_OCT(ohCreateOrModifyPropStr(facetPtr, &prop, "architecture", arch));
    return (TRUE);
}

/* 1/28/91, EAL

/*****  5/2/90 10/23/89
Iterate prop stores the number of iterations for a universe.
*/

int
GetIterateProp(facetPtr)
octObject *facetPtr;
{
    octObject prop;

    return (GetByPropName(facetPtr, &prop, "iterate") == OCT_NOT_FOUND)
	? -1 : (int) prop.contents.prop.value.integer;
}

void
SetIterateProp(facetPtr, iterate)
octObject *facetPtr;
int iterate;
{
    octObject prop;

    CreateOrModifyPropInt(facetPtr, &prop, "iterate", iterate);
}
/***** End of iterate prop */
