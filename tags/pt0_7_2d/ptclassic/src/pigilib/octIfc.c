/* 
Copyright (c) 1990-1999 The Regents of the University of California.
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
/*  octIfc.c  edg
Version identification:
@(#)octIfc.c	1.64	01/28/99
Useful higher level OCT interface functions.
*/

/* Includes */

/* Standard include files */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Octtools include files */
#include "oct.h"
#include "rpc.h"
#include "oh.h"
#include "region.h"

/* Pigilib include files */
#include "octIfc.h"
#include "paramStructs.h"
#include "err.h"
#include "util.h"
#include "octMacros.h"
#include "mkIcon.h"
#include "mkTerm.h"
#include "vemInterface.h"
#include "pigiLoader.h"
#include "kernelCalls.h"


/* 
returns a pointer to the name of the passed Oct Object in "namePtr"
written by Xavier Warzee 12/94 - installed into Ptolemy by Alan Kamas 3/95
*/
boolean GetStarName(instPtr, namePtr)
octObject *instPtr;
char** namePtr;
{
    char *starName;
    starName = AkoName(instPtr->contents.instance.master);
    *namePtr = strdup(starName);

    return TRUE;
}


/* 8/14/89
Opens the master of an instance, but allows you to choose which facet.
i is the instance pointer, f is the facet, and m is the mode.  t points
to the returned master octObject.
*/
boolean
MyOpenMaster(t, i, f, m)
octObject *t, *i;
char *f, *m;
{
    octObject rfacet = {OCT_UNDEFINED_OBJECT, 0};

    octGetFacet(i, &rfacet);
    t->type = OCT_FACET;
    t->objectId = 0;		    /* set only to silence Purify warnings */
    t->contents.facet.cell = i->contents.instance.master;
    t->contents.facet.view = i->contents.instance.view;
    t->contents.facet.facet = f;
    t->contents.facet.version = OCT_CURRENT_VERSION;
    t->contents.facet.mode = m;
    CK_OCT(octOpenRelative(&rfacet, t, OCT_SIBLING));  /* FIXME: Memory leak */

    FreeOctMembers(&rfacet);

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
	PT_PERCENT "p", strlen(PT_PERCENT "p")) == 0);
}

boolean 
IsInputPort(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master), 
            PT_PERCENT "pInput") == 0);
}

boolean 
IsDelay(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master),
            PT_PERCENT "dDelay") == 0);
}

boolean 
IsDelay2(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master),
            PT_PERCENT "dDelay2") == 0);
}

boolean
IsBus(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master),
            PT_PERCENT "dBus") == 0);
}

boolean
IsMarker(instPtr)
octObject *instPtr;
{
    return(strncmp(BaseName(instPtr->contents.instance.master),
            PT_PERCENT "d", strlen(PT_PERCENT "d")) == 0);
}

boolean 
IsGal(instPtr)
octObject *instPtr;
{
    octObject master = {OCT_UNDEFINED_OBJECT, 0},
	      prop = {OCT_UNDEFINED_OBJECT, 0};
    int status;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    status = (GetByPropName(&master, &prop, "galaxy") != OCT_NOT_FOUND);
    FreeOctMembers(&master);
    if (status) FreeOctMembers(&prop);
    return status;
}

boolean 
IsStar(instPtr)
octObject *instPtr;
{
    octObject master = {OCT_UNDEFINED_OBJECT, 0},
	      prop  = {OCT_UNDEFINED_OBJECT, 0};
    int status;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    status = (GetByPropName(&master, &prop, "star") != OCT_NOT_FOUND);
    FreeOctMembers(&master);
    if (status) FreeOctMembers(&prop);
    return status;
}

boolean 
IsUniv(instPtr)
octObject *instPtr;
{
    octObject master = {OCT_UNDEFINED_OBJECT, 0},
	      prop = {OCT_UNDEFINED_OBJECT, 0};
    int status;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    status = (GetByPropName(&master, &prop, "universe") != OCT_NOT_FOUND);
    FreeOctMembers(&master);
    if (status) FreeOctMembers(&prop);
    return status;
}

boolean 
IsPal(instPtr)
octObject *instPtr;
{
    octObject master = {OCT_UNDEFINED_OBJECT, 0},
	      prop = {OCT_UNDEFINED_OBJECT, 0};
    int status;

    ERR_IF1(!MyOpenMaster(&master, instPtr, "interface", "r"));
    status = (GetByPropName(&master, &prop, "palette") != OCT_NOT_FOUND);
    FreeOctMembers(&master);
    if (status) FreeOctMembers(&prop);
    return status;
}

boolean 
IsCursor(instPtr)
octObject *instPtr;
{
    return(strcmp(BaseName(instPtr->contents.instance.master),
            PT_PERCENT "cCursor") == 0);
}

/***** end of Isxxx functions */

/* see if facetPtr has any items of the type designated by the mask argument.
   (e.g. OCT_TERM_MASK, OCT_INSTANCE_MASK, ...)
 */

static boolean
FacetHasItem(facetPtr,mask)
octObject *facetPtr;
octObjectMask mask;
{
    octObject obj = {OCT_UNDEFINED_OBJECT, 0};
    int status;

    status = (octGenFirstContent(facetPtr, mask, &obj) == OCT_OK);
    if (status) FreeOctMembers(&obj);
    return status;
}

/* 1/30/89
Test to see if facet is a galaxy by looking for a formal term.
*/
boolean 
IsGalFacet(facetPtr)
octObject *facetPtr;
{
    return FacetHasItem(facetPtr,OCT_TERM_MASK);
}

/*
 Predicate returns true if there is an instance that is a universe,
 palette, or cursor instance.  Used in IsPalFacet and IsUnivFacet.
 */
static boolean
HasUPC(facetPtr)
octObject *facetPtr;
{
    octObject inst = {OCT_UNDEFINED_OBJECT, 0};
    octGenerator gen;
    int status = FALSE;

    octInitGenContentsSpecial(facetPtr, OCT_INSTANCE_MASK, &gen);
    while (!status && octGenerate(&gen, &inst) == OCT_OK) {
	status = IsCursor(&inst) || IsUniv(&inst) || IsPal(&inst);
	FreeOctMembers(&inst);
    }
    FreeOctMembers(&inst);
    octFreeGenerator(&gen);

    return status;
}

/* 8/8/89
Test to see if facet is a palette by looking for absense of nets, and
either a cursor or universe instance.
*/
boolean 
IsPalFacet(facetPtr)
octObject *facetPtr;
{
    return (!FacetHasItem(facetPtr,OCT_NET_MASK) &&
	    HasUPC(facetPtr));
}

/* 8/8/89
 A universe may not have terminals, and cannot have universe,
 cursor, or palette instances in it.
*/
boolean 
IsUnivFacet(facetPtr)
octObject *facetPtr;
{
    return (!FacetHasItem(facetPtr,OCT_TERM_MASK) &&
	    !HasUPC(facetPtr));
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

/* SetParamProp
Basis of SetFormalParams, SetSogParams, and SetTargetParams.
 */

boolean
SetParamProp(instPtr, pListPtr, propname)
octObject *instPtr;
ParamListType *pListPtr;
char* propname;
{
    char *pStr;
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};
    octStatus status;
    pStr = PListToPStr(pListPtr);
    status = CreateOrModifyPropStr(instPtr, &prop, propname, pStr);
    free(pStr);
    if (status != OCT_OK) {
	return(FALSE);
    } else {
	FreeOctMembers(&prop);
	return(TRUE);
    }
}

/* 3/19/89
Set formal params of a galaxy schematic.
*/
boolean
SetFormalParams(galFacetPtr, pListPtr)
octObject *galFacetPtr;
ParamListType *pListPtr;
{
    return SetParamProp(galFacetPtr, pListPtr, "params");
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
    int retval = TRUE;
    octObject prop = {OCT_PROP, 0};

    if (GetByPropName(galFacetPtr, &prop, "params") == OCT_NOT_FOUND) {
	pListPtr->length = 0;
	pListPtr->array = (ParamType *) calloc(1, sizeof(ParamType));
	pListPtr->dynamic_memory = 0;
	pListPtr->flat_plist_flag = TRUE;
    }
    else {
	if (!PStrToPList(prop.contents.prop.value.string, pListPtr)) {
	    retval = FALSE;
	}
	FreeOctMembers(&prop);
    }

    return(retval);
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
	return SetParamProp(instPtr, pListPtr, "params");
}

boolean
SetTargetParams(instPtr, pListPtr)
octObject *instPtr;
ParamListType *pListPtr;
{
	return SetParamProp(instPtr, pListPtr, "targetparams");
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
    (void)sprintf(buf, "Unknown star '%s' in domain %s, trying to load it...",
		  akoName, curDomainName());
    PrintCon(buf);
    return LoadTheStar(instPtr, FALSE, (char *)NULL);
}

/* used to be part of AutoLoadCk; this always loads the star,
   whether known or not.
 */
boolean
LoadTheStar(instPtr, permB, linkArgs)
    octObject*	instPtr;
    int		permB;
    char*	linkArgs;
{
    octObject mFacet = {OCT_UNDEFINED_OBJECT, 0};
    char *fullName;

    ERR_IF1(!MyOpenMaster(&mFacet, instPtr, "interface", "r"));
    octFullName(&mFacet, &fullName);
    FreeOctMembers(&mFacet);
    ERR_IF1(!KcLoad(fullName, permB, linkArgs));
    (void) PrintCon("Load completed");
    return (TRUE);
}

/* find a parameter name in a ParamListType. */
ParamType*
findParam(name, pListp)
const char* name;
ParamListType *pListp;
{
    int i;
    ParamType* curr = pListp->array;
    for (i = 0; i < pListp->length; i++) {
	if (strcmp (name, curr->name) == 0) return curr;
	curr++;
    }
    return 0;
}

/*
 * merge a set of instance parameters (2nd arg) with a set of default
 * parameters.  return the "official" ones in the 1st argument (default
 * parameters).  function return value indicates whether or not any
 * of the pListInst parameters were used. 
 */
static
boolean
MergeParams(pListDef, pListInst)
ParamListType *pListDef, *pListInst;
{
    boolean retval = FALSE;
    int i;
    ParamType* match;
    ParamType* curr = pListDef->array;
    for (i = 0; i < pListDef->length; i++) {
	match = findParam(curr->name, pListInst);
	if (match) {
	    curr->value = match->value;
	    retval = TRUE;
	}
	curr++;
    }
    return (retval);
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
    octObject prop = {OCT_PROP, 0};
    ParamListType tempList = {0, 0, 0, FALSE};
    int retval = TRUE;

    ERR_IF1(!GetDefaultParams(instPtr, pListPtr));

    /* If no parameters, return default list */
    prop.contents.prop.name = "params";
    if (octGetByName(instPtr, &prop) == OCT_NOT_FOUND) {
	return SetSogParams(instPtr, pListPtr);
    }

    /* Convert the parameter string to a parameter data structure */
    if (!PStrToPList(prop.contents.prop.value.string, &tempList)) {
	retval = FALSE;
    }
    /* Merge parameters: if true, strings from tempList were used */
    else if (MergeParams(pListPtr, &tempList)) {
	CopyFlatPList(pListPtr, pListPtr);
    }

    /* Free dynamic memory */
    FreeFlatPList(&tempList);		/* allocated by PStrToPList */
    FreeOctMembers(&prop);		/* allocated by octGetByName */

    return(retval);
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
    octObject prop = {OCT_PROP, 0};
    ParamListType tempList = {0, 0, 0, FALSE};
    int retval = TRUE;

    ERR_IF1(!KcGetTargetParams(targName, pListPtr));

    /* If no parameters, return default list */
    /* Do not free prop using FreeOctMembers since it contains static memory */
    prop.contents.prop.name = "targetparams";
    if (octGetByName(facetPtr, &prop) == OCT_NOT_FOUND) {
	return TRUE;
    }

    /* Convert the parameter string to a parameter data structure */
    if (!PStrToPList(prop.contents.prop.value.string, &tempList)) {
	retval = FALSE;
    }
    /* Merge parameters: if true, strings from tempList were used */
    else if (MergeParams(pListPtr, &tempList)) {
	CopyFlatPList(pListPtr, pListPtr);
    }

    /* Deallocate dynamic memory */
    FreeFlatPList(&tempList);		/* allocated by PStrToPList */
    FreeOctMembers(&prop);		/* allocated by octGetByName */

    return(retval);
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
	octObject galFacet = {OCT_UNDEFINED_OBJECT, 0};
	ERR_IF1(!MyOpenMaster(&galFacet, instPtr, "contents", "r"));
	ERR_IF1(!GetFormalParams(&galFacet, pListPtr));
	FreeOctMembers(&galFacet);
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
    if( IsDelay(instPtr) ) {
      propPtr->contents.prop.name = "delay";
      propPtr->contents.prop.value.string = "1";
    } else {
      propPtr->contents.prop.name = "delay2";
      propPtr->contents.prop.value.string = "0";
    }
    propPtr->contents.prop.type = OCT_STRING;
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
		if ((int)strlen (p->value.string) >= len) return FALSE;
		(void)strcpy (buf, p->value.string);
		return TRUE;
	default:
		return FALSE;
	}
}

/* get delay, buswidth, or some other property as a string using
 * Stringize -- empty string and return of false if not set.
 */
boolean
GetStringizedProp(objPtr, name, dest, dlen)
octObject *objPtr;
char *name, *dest;
int dlen;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};
    int status;

    if (GetByPropName(objPtr, &prop, name) == OCT_NOT_FOUND) {
	*dest = 0;
	return FALSE;
    }
    status = StringizeProp(&prop, dest, dlen);
    FreeOctMembers(&prop);
    return status;
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
	while (*s && isspace((int)*s)) s++;
	/* return if this is not a number */
	if (*s == 0 || !isdigit((int)*s)) return;
	/* skip digits */
	while (*s && isdigit((int)*s)) s++;
	/* skip trailing whitespace */
	while (*s && isspace((int)*s)) s++;
	/* if not at end-of-string, not a number */
	if (*s != 0) return;
	/* OK, convert to a number */
	n = atoi (p->value.string);
	p->type = OCT_INTEGER;
	p->value.integer = n;
	return;
}

/* GetStringProp
 * Get the value of a string property with the specified name 
 * associated with an oct object.
 * Output: valuePtr = returned string.  NULL means the prop doesn't
 * exist (or has a NULL value).  The string is hashed.
 */
boolean
GetStringProp(objPtr, propName, valuePtr)
octObject *objPtr;
char  *propName;
const char **valuePtr;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    if (GetByPropName(objPtr, &prop, propName) == OCT_NOT_FOUND)
	*valuePtr = NULL;
    else {
	*valuePtr = HashString(prop.contents.prop.value.string);
	FreeOctMembers(&prop);
    }
    return(TRUE);
}

/* SetStringProp
 * Set the value of a string property associated with the
 * specified oct object.
 * Input: 
 *    propName = string giving property name
 *    value = string, if string is NULL or == "" then the 
 *            named property is deleted, if it exists.
 */
boolean
SetStringProp(objPtr, propName, value)
octObject *objPtr;
char  *propName;
char  *value;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    if (value && *value) {
	ERR_IF2(CreateOrModifyPropStr(objPtr,&prop,propName,value) != OCT_OK,
		octErrorString());
    }
    else {		/* empty string: delete prop if it exists */
	if (GetByPropName(objPtr, &prop, propName) != OCT_NOT_FOUND) {
	    ERR_IF2(octDelete(&prop) != OCT_OK, octErrorString());
	    FreeOctMembers(&prop);
	}
    }
    return(TRUE);
}

/* Get or create the domain property.  The return value is from HashString.
*/
boolean
GOCDomainProp(facetPtr, domainPtr, defaultDomain)
octObject *facetPtr;
const char **domainPtr;
const char *defaultDomain;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};
    CK_OCT(GetOrCreatePropStr(facetPtr, &prop, "domain",
			      (char *) defaultDomain));
    *domainPtr = HashString(prop.contents.prop.value.string);
    FreeOctMembers(&prop);
    return (TRUE);
}

/* 8/7/89
*/
boolean
SetDomainProp(facetPtr, domain)
octObject *facetPtr;
char *domain;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    CK_OCT(CreateOrModifyPropStr(facetPtr, &prop, "domain", domain));
    FreeOctMembers(&prop);
    return (TRUE);
}

boolean
GOCTargetProp(facetPtr, targetPtr, defaultTarget)
octObject *facetPtr;
const char **targetPtr;
const char *defaultTarget;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

/* don't use ohGetOrCreatePropStr, so we avoid problems with the user
   changing the domain and needing to do edit-target as well.
 */
    if (GetByPropName(facetPtr, &prop, "target") == OCT_NOT_FOUND)
	*targetPtr = HashString(defaultTarget);
    else {
	*targetPtr = HashString(prop.contents.prop.value.string);
	FreeOctMembers(&prop);
    }
    return (TRUE);
}

/* 1/28/91, EAL
*/
boolean
SetTargetProp(facetPtr, target)
octObject *facetPtr;
char *target;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    CK_OCT(CreateOrModifyPropStr(facetPtr, &prop, "target", target));
    FreeOctMembers(&prop);
    return (TRUE);
}

/* 1/28/91, EAL

  5/2/90 10/23/89
Iterate prop stores the number of iterations for a universe.
*/

int
GetIterateProp(facetPtr, pIterate)
octObject *facetPtr;
int *pIterate;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    *pIterate = 1;
    if ( GetByPropName(facetPtr, &prop, "iterate") == OCT_NOT_FOUND )
	return -1;
    *pIterate = prop.contents.prop.value.integer;
    FreeOctMembers(&prop);
    return 0;
}

void
SetIterateProp(facetPtr, iterate)
octObject *facetPtr;
int iterate;
{
    octObject prop = {OCT_UNDEFINED_OBJECT, 0};

    CreateOrModifyPropInt(facetPtr, &prop, "iterate", iterate);
    FreeOctMembers(&prop);
}
/***** End of iterate prop */

/* the following were macros */
boolean
CreateOrModifyPropInt(c, t, s, i)
octObject *c, *t;
char *s;
int i;
{
    t->type = OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name = s;
    t->contents.prop.type = OCT_INTEGER;
    t->contents.prop.value.integer = i;
    return octCreateOrModify(c,t);
}

boolean
CreateOrModifyPropStr(c, t, s, s1)
octObject *c,*t;
char *s, *s1;
{
    t->type = OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name = s;
    t->contents.prop.type = OCT_STRING;
    t->contents.prop.value.string = s1;
    return octCreateOrModify(c, t);
}

boolean
CreateOrModifyPropReal(c, t, s, d)
octObject *c, *t;
char *s;
double d;
{
    t->type = OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name = s;
    t->contents.prop.type = OCT_REAL;
    t->contents.prop.value.real = d;
    return octCreateOrModify(c, t);
}

boolean
GetOrCreatePropInt(c, t, s, i)
octObject *c, *t;
char *s;
int i;
{
    t->type = OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name = s;
    t->contents.prop.type = OCT_INTEGER;
    t->contents.prop.value.integer = i;
    return octGetOrCreate(c,t);
}

boolean
GetOrCreatePropStr(c, t, s, s1)
octObject *c,*t;
char *s, *s1;
{
    t->type = OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name = s;
    t->contents.prop.type = OCT_STRING;
    t->contents.prop.value.string = s1;
    return octGetOrCreate(c, t);
}

boolean
GetOrCreatePropReal(c, t, s, d)
octObject *c, *t;
char *s;
double d;
{
    t->type = OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name = s;
    t->contents.prop.type = OCT_REAL;
    t->contents.prop.value.real = d;
    return octGetOrCreate(c, t);
}

boolean
OpenFacet(t, c, v, f, m)
octObject *t;
char *c, *v, *f, *m;
{
    t->objectId = 0;
    t->type = OCT_FACET;
    t->contents.facet.cell = c;
    t->contents.facet.view = v;
    t->contents.facet.facet = f;
    t->contents.facet.version = OCT_CURRENT_VERSION;
    t->contents.facet.mode = m;
    return octOpenFacet(t);
}

/*
   FreeOctMembers

   Frees fields in an octObject.  There must be no other references to
   them!  Assumed that the octObject was retrieved from one of the oct
   functions that puts everything on the heap.

   Warning: obj must have been received from the Oct interface, not
   filled in by the user.

   Calling FreeOctMembers multiple times on the same octObject * will
   not generate any new errors because upon successful memory
   deallocation, type field is set to OCT_UNDEFINED_OBJECT.
*/

boolean
FreeOctMembers(o)
octObject *o;
{
	struct octFacet *f;
	struct octProp *p;
	struct octInstance *i;
	int deleted = TRUE;

	switch (o->type) {
	  case OCT_UNDEFINED_OBJECT:
		break;
	  case OCT_FACET:
		f = &(o->contents.facet);
		free(f->cell);
		free(f->view);
		free(f->facet);
		free(f->version);
		free(f->mode);
		break;
	  case OCT_TERM:
		free (o->contents.term.name);
		break;
	  case OCT_NET:
		free (o->contents.net.name);
		break;
	  case OCT_INSTANCE:
		i = &(o->contents.instance);
		free(i->name);
		free(i->master);
		free(i->view);
		free(i->facet);
		free(i->version);
		break;
	  case OCT_POLYGON:
	  case OCT_BOX:
	  case OCT_CIRCLE:
	  case OCT_PATH:
		break;				/* nothing to free */
	  case OCT_PROP:
		p = &(o->contents.prop);
		free (p->name);
		switch (p->type) {
		  case OCT_STRING:
			free(p->value.string);
			break;
		  case OCT_NULL:
		  case OCT_INTEGER:
		  case OCT_REAL:
		  case OCT_ID:
			break;			/* nothing to free */
		  default:
			deleted = FALSE;
			fprintf(stderr,
			        "FreeOctMembers: OCT_PROP type %d not handled\n",
				p->type);
			break;
		}
		break;
	  case OCT_POINT:
	  case OCT_EDGE:
		break;
	  default:
		deleted = FALSE;
		fprintf(stderr,
			"FreeOctMembers: type %d not handled\n",
			o->type);
		break;
	}

	/* since it has been deallocated, reset type to unknown */
	if ( deleted ) {
		o->type = OCT_UNDEFINED_OBJECT;
		o->objectId = 0;
	}

	return TRUE;
}
