/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/*  octIfc.c  edg
Version identification:
$Id$
Useful higher level OCT interface functions.
*/

/* Includes */
#include "local.h"
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
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

static boolean SetParamProp();
extern char* HashString();

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
    octObject rfacet;

    octGetFacet(i, &rfacet);
    t->type = OCT_FACET;
    t->objectId = 0;		/* though not used, it is sent, so
				   this is needed to shut up Purify */
    t->contents.facet.cell = i->contents.instance.master;
    t->contents.facet.view = i->contents.instance.view;
    t->contents.facet.facet = f;
    t->contents.facet.version = OCT_CURRENT_VERSION;
    t->contents.facet.mode = m;
    CK_OCT(octOpenRelative(&rfacet, t, OCT_SIBLING));
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
    octObject master, prop;
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
    octObject master, prop;
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
    octObject master, prop;
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
    int status;
    fterm.objectId = 0;		/* silence Purify */
    status = (octGenFirstContent(facetPtr, OCT_TERM_MASK, &fterm) == OCT_OK);
    if (status) FreeOctMembers(&fterm);
    return status;
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
    int status = FALSE;

    octInitGenContents(facetPtr, OCT_INSTANCE_MASK, &gen);
    while (octGenerate(&gen, &inst) == OCT_OK && !status) {
	status = IsCursor(&inst);
	FreeOctMembers(&inst);
    }
    octFreeGenerator(&gen);
    return status;
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
    status = CreateOrModifyPropStr(galFacetPtr, &prop, "params", pStr);
    free(pStr);
    if (status != OCT_OK) {
	return(FALSE);
    } else {
	FreeOctMembers(&prop);
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
    prop.objectId = 0;		/* silence Purify */
    if (GetByPropName(galFacetPtr, &prop, "params") == OCT_NOT_FOUND) {
	pListPtr->length = 0;
	pListPtr->array = (ParamType *) calloc(1, sizeof(ParamType));
    } else {
	ERR_IF1(!PStrToPList(prop.contents.prop.value.string, pListPtr));
	FreeOctMembers(&prop);
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

boolean
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
    status = CreateOrModifyPropStr(instPtr, &prop, propname, pStr);
    free(pStr);
    if (status != OCT_OK) {
	return(FALSE);
    } else {
	FreeOctMembers(&prop);
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
    return LoadTheStar(instPtr, FALSE, NULL);
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
    octObject mFacet;
    char *fullName;

    ERR_IF1(!MyOpenMaster(&mFacet, instPtr, "interface", "r"));
    octFullName(&mFacet, &fullName);
    FreeOctMembers(&mFacet);
    ERR_IF1(!KcLoad(fullName, permB, linkArgs));
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
    FreeOctMembers(&prop);
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
    FreeOctMembers(&prop);
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
boolean
GetStringizedProp(objPtr, name, dest, dlen)
octObject *objPtr;
char *name, *dest;
int dlen;
{
    octObject prop;
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
    exist (or has a NULL value).  The string is hashed.
*/
boolean
GetCommentProp(objPtr, commentPtr)
octObject *objPtr;
char **commentPtr;
{
    octObject prop;

    if (GetByPropName(objPtr, &prop, "comment") == OCT_NOT_FOUND)
	*commentPtr = NULL;
    else {
	*commentPtr = HashString(prop.contents.prop.value.string);
	FreeOctMembers(&prop);
    }
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

/* Get or create the domain property.  The return value is from HashString.
*/
boolean
GOCDomainProp(facetPtr, domainPtr, defaultDomain)
octObject *facetPtr;
char **domainPtr;
char *defaultDomain;
{
    octObject prop;
    CK_OCT(GetOrCreatePropStr(facetPtr, &prop, "domain", defaultDomain));
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
    octObject prop;
    prop.objectId = 0;		/* silence Purify */
    CK_OCT(CreateOrModifyPropStr(facetPtr, &prop, "domain", domain));
    FreeOctMembers(&prop);
    return (TRUE);
}

boolean
GOCTargetProp(facetPtr, targetPtr, defaultTarget)
octObject *facetPtr;
char **targetPtr;
char *defaultTarget;
{
    octObject prop;
    prop.objectId = 0;
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
    octObject prop;

    CK_OCT(CreateOrModifyPropStr(facetPtr, &prop, "target", target));
    FreeOctMembers(&prop);
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
    FreeOctMembers(&prop);
    return 0;
}

void
SetIterateProp(facetPtr, iterate)
octObject *facetPtr;
int iterate;
{
    octObject prop;

    CreateOrModifyPropInt(facetPtr, &prop, "iterate", iterate);
    FreeOctMembers(prop);
}
/***** End of iterate prop */

/* the following were macros */
boolean
CreateOrModifyPropInt(c, t, s, i)
octObject *c, *t;
char *s;
int i;
{
    t->type=OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name=s;
    t->contents.prop.type = OCT_INTEGER;
    t->contents.prop.value.integer = i;
    return octCreateOrModify(c,t);
}

boolean
CreateOrModifyPropStr(c, t, s, s1)
octObject *c,*t;
char *s, *s1;
{
    t->type=OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name=s;
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
    t->type=OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name=s;
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
    t->type=OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name=s;
    t->contents.prop.type = OCT_INTEGER;
    t->contents.prop.value.integer = i;
    return octGetOrCreate(c,t);
}

boolean
GetOrCreatePropStr(c, t, s, s1)
octObject *c,*t;
char *s, *s1;
{
    t->type=OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name=s;
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
    t->type=OCT_PROP;
    t->objectId = 0;
    t->contents.prop.name=s;
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

/* fns to free fields in an octObject.  There must be no other refs to
   them!  Assumed retrieved from one of the oct functions that puts
   everything on the heap.

   Warning: obj must have been received from the Oct interface, not
   filled in by the user.
*/

boolean
FreeOctMembers(o)
octObject *o;
{
	struct octFacet *f;
	struct octProp *p;
	struct octInstance *i;
	switch (o->type) {
	case OCT_FACET:
		f = &(o->contents.facet);
		free(f->cell);
		free(f->view);
		free(f->facet);
		free(f->version);
		free(f->mode);
		break;
	case OCT_INSTANCE:
		i = &(o->contents.instance);
		free(i->name);
		free(i->master);
		free(i->view);
		free(i->facet);
		free(i->version);
		break;
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
			/* nothing to free */
			break;
		default:
			fprintf(stderr,
			   "FreeOctMembers, OCT_PROP type %d not handled\n",
				p->type);
			break;
		}
		break;
	case OCT_TERM:
		free (o->contents.term.name);
		break;
	case OCT_NET:
		free (o->contents.net.name);
		break;
	default:
		fprintf (stderr, "FreeOctMembers, type %d not handled\n",
			 o->type);
		break;
	}
	return TRUE;
}
