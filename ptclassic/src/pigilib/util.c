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
/* util.c  edg
Version identification:
$Id$
Utility functions.
*/

/* Standard includes */
#include "local.h"
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include "tcl.h"

/* Octtools includes */
#include "oct.h"
#include "rpc.h"

/* Pigilib includes */
#include "err.h"
#include "util.h"
#include "octIfc.h"
#include "vemInterface.h"
#include "kernelCalls.h"
#include "icon.h"

/* UMalloc  12/10/88
Allocate using malloc.
Inputs: 
    outPtr = empty (for result)
    bytes = number of bytes to allocate
Outputs:
    return = success
    outPtr = places ptr to buffer at this adr
*/
boolean
UMalloc(outPtr, bytes)
char **outPtr;
int bytes;
{
    ERR_IF2((*outPtr = (char *) malloc(bytes)) == NULL, 
	"UMalloc: malloc() failed");
    return(TRUE);
}

/* StrDup  8/20/88
Duplicate string.
Inputs: 
    outPtr = empty (for result)
    in = string to duplicate
Outputs:
    return = success
    outPtr = places duplicate string at this adr
*/
boolean
StrDup(outPtr, in)
char **outPtr, *in;
{
    ERR_IF2((*outPtr = (char *) malloc(strlen(in) + 1)) == NULL, 
	"StrDup: malloc() failed");
    (void) strcpy(*outPtr, in);
    return(TRUE);
}

/* DupString  3/88
Don't use this anymore -- this is old.  Use StrDup().
Duplicates string and returns pointer to new string
*/
char *
DupString(string)
char *string;
{
    return(strcpy((char *) calloc(strlen(string)+1, sizeof(char)), string));
}

/* SkipWhite  4/28/88
*/
char *
SkipWhite(s)
char *s;
{
    while (*s == ' ' || *s == '\t') {
	s++;
    }
    return(s);
}

/* BaseName  5/26/88 4/17/88
Inputs: name
Outputs: return = part of name to the right of any '/'
Example: "/etc/passwd" -> "passwd"
*/
char *
BaseName(name)
char *name;
{
    char *base;

    if ((base = strrchr(name, '/')) != NULL) {
	return(++base);  /* skip over '/' */
    }
    return(name);
}

/* 11/20/89
Inputs: buffer containing pathname, this gets modified
Outputs: buffer contains dir name
*/
char *
DirName(buf)
char *buf;
{
    char *e;

    if ((e = strrchr(buf, '/')) != NULL) {
	*e = '\0';
	return(buf);
    }
    return(strcpy(buf, "."));
}

/*  4/14/89
Concat a directory name to a basename.
*/
boolean
ConcatDirName(dir, baseName, outPtr)
char *dir, *baseName, **outPtr;
{
    char *buf;

    if (strlen(SkipWhite(dir)) == 0) {
	ERR_IF1(!StrDup(outPtr, baseName));
    } else {
	ERR_IF1(!UMalloc(&buf, strlen(dir) + strlen(baseName) + 2));
	sprintf(buf, "%s/%s", dir, baseName);
	*outPtr = buf;
    }
    return(TRUE);
}

/* 5/17/90 7/28/89
Return full technology directory, that is the technology directory
with "ptolemy" appended to it.
*/
boolean
UGetFullTechDir(strPtr)
char **strPtr;
{
    static char *dir = NULL;
    char *tmp;

    if (dir == NULL) {
	ERR_IF2((tmp = RPCXGetDefault("vem", "technology")) == NULL,
	    "UGetFullTechDir: cannot read .Xdefaults 'vem.technology'");
	ERR_IF1(!UMalloc(&dir, strlen(tmp) + 32));
	sprintf(dir, "%s/ptolemy", tmp);
	free(tmp);
    }
    *strPtr = dir;
    return (TRUE);
}
    

/* YesNoToBoolean  9/29/89 5/30/88
If str starts with "y" or is "on", return TRUE
*/
boolean
YesNoToBoolean(str)
char *str;
{
    return (*str == 'y' || *str == 'Y'
	|| strcmp(str, "on") == 0 || strcmp(str, "On") == 0
	? TRUE : FALSE);
}

/* BooleanToYesNo  9/29/89 5/30/88
*/
char *
BooleanToYesNo(bit)
boolean bit;
{
    return (bit ? "on" : "off");
}


/***** UniqName routines
Uses Tcl's hash table library procedures.
*/

static Tcl_HashTable uniqNameTable;

/*
Clears all names
*/
boolean
UniqNameInit()
{
    static int UniqNameInitDone = 0;

    if (UniqNameInitDone)
	Tcl_DeleteHashTable(&uniqNameTable);
    else
	UniqNameInitDone = 1;
    Tcl_InitHashTable(&uniqNameTable, TCL_STRING_KEYS);
    return TRUE;
}

/*
Output: return = str with unique name or NULL if error.  Can free() str.
*/
char *
UniqNameGet(s)
char *s;
{
    int new;
    Tcl_HashEntry *item;
    int n;
    char buf[100];

    item = Tcl_CreateHashEntry(&uniqNameTable, s, &new);
    if (new) {
	n = 0;  /* instance number starts from 1 */
    } else {
	n = (int) Tcl_GetHashValue(item);
    }

    n++;
    Tcl_SetHashValue(item, (ClientData) n);
    sprintf(buf, "%s%d", s, n);
    return DupString(buf);
}
/***** End UniqName routines */

/***** DupSheet routines  1/24/89
Updates: 4/13/90 = allow multiple DupSheets
*/

void
DupSheetInit(ds)
DupSheet *ds;
{
    *ds = NULL;
}

void
DupSheetClear(ds)
DupSheet *ds;
{
    DupSheetNode *p, *hold;

    p = *ds;
    while (p != NULL) {
	hold = p->next;
  	UFree((char *) p);
	p = hold;
    }
    *ds = NULL;
}

/* It looks like DupSheetAdd is used to record compiling universes */
boolean
DupSheetAdd(ds, item)
DupSheet *ds;
const char *item;
{
    DupSheetNode *new;
    
    ERR_IF1(!UMalloc((char **)&new, sizeof(DupSheetNode)));
    new->info = item;
    new->moreinfo = NULL;
    new->next = *ds;
    *ds = new;
    return(TRUE);
}

/* It looks like DupSheetAdd is used to record compiling galaxies.
 * The moreinfo field stores the domain name of the galaxy, and is
 * NULL for universes
 */
boolean
DupSheetAdd2(ds, item, item2)
DupSheet *ds;
const char *item;
const char *item2;
{
    DupSheetNode *new;
    
    ERR_IF1(!UMalloc((char **)&new, sizeof(DupSheetNode)));
    new->info = item;
    new->moreinfo = item2;
    new->next = *ds;
    *ds = new;
    return(TRUE);
}

/*
 * The way this is used, could probably compare the pointer values
 * rather than the strings.  But for conservatism, we don't do that.
 */
boolean
DupSheetIsDup(ds, item)
DupSheet *ds;
const char *item;
{
    DupSheetNode *p;

    for (p = *ds; p != NULL; p = p->next) {
      if (strcmp(p->info, item) == 0) {
	    return(TRUE);
	  }
    }
    return(FALSE);
}
boolean
DupSheetIsDup2(ds, item, item2)
DupSheet *ds;
const char *item;
const char *item2;
{
    DupSheetNode *p;

    for (p = *ds; p != NULL; p = p->next) {
	if (strcmp(p->info, item) == 0) {
        /* moreinfo is NULL for universes. */
        if (p->moreinfo && strcmp(p->moreinfo, item2) == 0) {
	    return(TRUE);
	  }
	}
    }
    return(FALSE);
}

/* Given the spot that locates the curson in a facet, set
   the KnownBlocks currentDomain to correspond to the domain
   of the facet.  As a side effect, if the domain property of
   the facet has not been defined, it will be set to current domain
   of the KnownBlocks object.
   Returns a pointer to the string identifying the previous domain,
   or NULL if it fails.
        EAL, 9/23/90
*/
const char *
getDomainS(spot)
RPCSpot *spot;
{
    octObject facet = {OCT_UNDEFINED_OBJECT};
    const char *domain;
    const char *oldDomain;

    oldDomain = curDomainName();

    /* get current facet */
    facet.objectId = spot->facet;

    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        return NULL;
    }
    if (!GOCDomainProp(&facet, &domain, oldDomain)) {
        PrintErr(ErrGet());
        return NULL;
    }
    return domain;
}

const char *
setCurDomainS(spot)
RPCSpot *spot;
{
    const char *domain;
    const char *oldDomain;
    domain = getDomainS(spot);
    if (!domain) return NULL;
    oldDomain = curDomainName();
    /* Call Kernel function to set KnownBlock current domain */
    if (! KcSetKBDomain(domain)) {
	PrintErr("Domain error in current facet.");
	return NULL;
    }
    return oldDomain;
}

/* Get domain corresponding to the facet */
const char *
getDomainF(facetPtr)
octObject *facetPtr;
{
    const char *domain;
    const char *oldDomain;
    oldDomain = curDomainName();
    if (!GOCDomainProp(facetPtr, &domain, oldDomain)) {
        PrintErr(ErrGet());
        return NULL;
    }
    return domain;
}

/* Given a facet, set the KnownBlocks currentDomain to correspond
   to the domain of the facet.
   As a side effect, if the facet domain property has not been set,
   it will be set to current domain in the KnownBlocks object.
   Returns a pointer to the string identifying the old domain, or
   NULL if this fails.
        EAL, 9/23/90
*/
const char *
setCurDomainF(facetPtr)
octObject *facetPtr;
{
    const char *domain;
    const char *oldDomain;
    domain = getDomainF(facetPtr);
    if (!domain) return NULL;
    oldDomain = curDomainName();
    /* Call Kernel function to set KnownBlock current domain */
    if (! KcSetKBDomain(domain)) {
	PrintErr("Domain error in current facet.");
	return NULL;
    }
    return oldDomain;
}

/* Get the domain corresponding to an instance. */
const char *
getDomainInst(instPtr)
octObject *instPtr;
{
    octObject mFacet = {OCT_UNDEFINED_OBJECT};
    static char domain[32];
    char srcName[512], *fullName;
    if (IsGal(instPtr) || IsUniv(instPtr) || IsPal(instPtr)) {
	    return setCurDomainF(instPtr);
    }
    if (!MyOpenMaster(&mFacet, instPtr, "interface", "r")) {
	    PrintErr(ErrGet());
	    return NULL;
    }
    octFullName(&mFacet, &fullName);
    if (!IconFileToSourceFile(fullName, srcName, domain)) {
	    PrintErr(ErrGet());
	    return NULL;
    }
    return domain;
}

/* Set the domain to correspond to an instance. */

const char *
setCurDomainInst(instPtr)
octObject *instPtr;
{
    const char* oldDomain;
    const char* domain;
    oldDomain = curDomainName();
    domain = getDomainInst(instPtr);
    if (!domain) return NULL;
    if (!KcSetKBDomain(domain)) {
	PrintErr("Domain error in instance.");
	return NULL;
    }
    return oldDomain;
}
