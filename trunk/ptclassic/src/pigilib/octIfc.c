/*  octIfc.c  edg
Version identification:
$Id$
Useful higher level OCT interface functions.
*/

/* Includes */
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
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
IsBus(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master), "%dBus") == 0);
}

boolean
IsMarker(instPtr)
octObject *instPtr;
{
    return(strncmp(BaseName(instPtr->contents.instance.master), "%d", 2) == 0);
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
	    octFreeGenerator(&gen);
	    return (TRUE);
	}
    }
    octFreeGenerator(&gen);
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
	return SetParamProp(instPtr,pListPtr,"params");
}

SetTargetParams(instPtr, pListPtr)
octObject *instPtr;
ParamListType *pListPtr;
{
	return SetParamProp(instPtr,pListPtr,"targetparams");
}

static boolean
SetParamProp(instPtr, pListPtr, propname)
octObject *instPtr;
ParamListType *pListPtr;
char* propname;
{
    char *pStr;
    octObject prop;
    octStatus status;

    pStr = PListToPStr(pListPtr);
    prop.type = OCT_PROP;
    prop.contents.prop.name = propname;
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
    (void)sprintf(buf,
		  "Unknown star '%s' in current domain, trying to load it...",
		  akoName);
    PrintCon(buf);
    return LoadTheStar(instPtr);
}

/* used to be part of AutoLoadCk; this always loads the star,
   whether known or not.
 */
boolean
LoadTheStar(instPtr)
octObject *instPtr;
{
    octObject mFacet;
    char *fullName;

    ERR_IF1(!MyOpenMaster(&mFacet, instPtr, "interface", "r"));
    octFullName(&mFacet, &fullName);
    ERR_IF1(!KcLoad(fullName));
    PrintCon("Load completed");
    return (TRUE);
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

/* GetOrInitTargetParams
Get targetparams property if it exists or else init them first
then return targetparams.
Inputs:
    targName = name of target
    facetPtr = adr of facet to define targetparams for
    pListPtr = adr of an empty ParamList 
Outputs: return = TRUE if successful, else FALSE.
*/
boolean
GetTargetParams(targName, facetPtr, pListPtr)
char* targName;
octObject *facetPtr;
ParamListType *pListPtr;
{
    octObject prop;
    ParamListType tempList;
    void MergeParams();

    ERR_IF1(!KcGetTargetParams(targName, pListPtr));
    prop.type = OCT_PROP;
    prop.contents.prop.name = "targetparams";
    if (octGetByName(facetPtr, &prop) == OCT_NOT_FOUND) {
	/* no parameters: return default list */
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

/* same, for bus width */
void
GetOrInitBusProp(instPtr, propPtr)
octObject *instPtr, *propPtr;
{
    propPtr->type = OCT_PROP;
    propPtr->contents.prop.name = "buswidth";
    propPtr->contents.prop.type = OCT_INTEGER;
    propPtr->contents.prop.value.integer = 1;
    (void) octGetOrCreate(instPtr, propPtr);
}

/* given a property, make a string out of it and return it. */
boolean
StringizeProp(propPtr, buf, len)
octObject *propPtr;
char* buf;
int len;
{
	struct octProp* p = &(propPtr->contents.prop);
	switch (p->type) {
	case OCT_INTEGER:
		(void)sprintf (buf, "%ld", (long)p->value.integer);
		return TRUE;
	case OCT_REAL:
		(void)sprintf (buf, "%g", p->value.real);
		return TRUE;
	case OCT_STRING:
		if (strlen (p->value.string) >= len) return FALSE;
		(void)strcpy (buf, p->value.string);
		return TRUE;
	default:
		return FALSE;
	}
}

/* get delay, buswidth, or some other property as a string using
 * Stringize -- empty string and return of false if not set.
 */
GetStringizedProp(objPtr, name, dest, dlen)
octObject *objPtr;
char *name, *dest;
int dlen;
{
    octObject prop;
    if (GetByPropName(objPtr, &prop, name) == OCT_NOT_FOUND) {
	*dest = 0;
	return FALSE;
    }
    return StringizeProp(&prop, dest, dlen);
}

/* given a property, if it is OCT_STRING but its value corresponds to
   an integer, convert it to an integer property (OCT_INTEGER)
 */
void
IntizeProp(propPtr)
octObject *propPtr;
{
	char* s;
	int n;
	struct octProp* p = &(propPtr->contents.prop);
	if (p->type != OCT_STRING) return;
	s = p->value.string;
	/* skip initial whitespace */
	while (*s && isspace(*s)) s++;
	/* return if this is not a number */
	if (*s == 0 || !isdigit(*s)) return;
	/* skip digits */
	while (*s && isdigit(*s)) s++;
	/* skip trailing whitespace */
	while (*s && isspace(*s)) s++;
	/* if not at end-of-string, not a number */
	if (*s != 0) return;
	/* OK, convert to a number */
	n = atoi (p->value.string);
	p->type = OCT_INTEGER;
	p->value.integer = n;
	return;
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

boolean
GOCTargetProp(facetPtr, targetPtr, defaultTarget)
octObject *facetPtr;
char **targetPtr;
char *defaultTarget;
{
    octObject prop;

/* don't use ohGetOrCreatePropStr, so we avoid problems with the user
   changing the domain and needing to do edit-target as well.
 */
    if (GetByPropName(facetPtr, &prop, "target") == OCT_NOT_FOUND)
	*targetPtr = defaultTarget;
    else
	*targetPtr = prop.contents.prop.value.string;
    return (TRUE);
}

/* 1/28/91, EAL
*/
boolean
SetTargetProp(facetPtr, target)
octObject *facetPtr;
char *target;
{
    octObject prop;

    CK_OCT(ohCreateOrModifyPropStr(facetPtr, &prop, "target", target));
    return (TRUE);
}

/* 1/28/91, EAL

/*****  5/2/90 10/23/89
Iterate prop stores the number of iterations for a universe.
*/

int
GetIterateProp(facetPtr, pIterate)
octObject *facetPtr;
int *pIterate;
{
    octObject prop;

    *pIterate = 1;
    if ( GetByPropName(facetPtr, &prop, "iterate") == OCT_NOT_FOUND )
	return -1;
    *pIterate = prop.contents.prop.value.integer;
    return 0;
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
