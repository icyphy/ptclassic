/* util.c  edg
Version identification:
$Id$
Utility functions.
*/

/* Includes */
#include <stdio.h>
#include <strings.h>
#include <pwd.h>
#include <search.h>
#include "local.h"
#include "err.h"
#include "util.h"
#include "rpc.h"
#include "octIfc.h"
#include "vemInterface.h"


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
    extern char *malloc();

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
    extern char *malloc();

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


/* TildeExpand  4/28/88
Copied from OCT.  Modified a little.
Inputs: input = string "~xxx..."
    buffer = buffer of chars
Outputs: buffer = expanded string
Caveats: does not skip white space at beginning of string.
*/
void
TildeExpand(input, buffer)
char *input;
char *buffer;
{
    char *end;
    int name_length;
    struct passwd *entry;

    if (input[0] != '~') {
	strcpy(buffer, input);
	return;
    }
    
    end = index(input,'/');
    if (end == (char *) 0) {
	end = input + strlen(input);
    }

    name_length = end - input - 1;

    strncpy(buffer, input+1, name_length);
    buffer[name_length] = '\0';

    if (name_length == 0) {
	entry = getpwuid(getuid());
    } else {
	entry = getpwnam(buffer);
    }
    
    if (entry == (struct passwd *) 0) {
	return;
    }

    strcpy(buffer, entry->pw_dir);
    strcat(buffer, end);
    return;
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

    if ((base = rindex(name, '/')) != NULL) {
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
	*outPtr = sprintf(buf, "%s/%s", dir, baseName);
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
	    "UGetFullTechDir: can not read .Xdefaults 'vem.technology'");
	ERR_IF1(!UMalloc(&dir, strlen(tmp) + 32));
	sprintf(dir, "%s/ptolemy", tmp);
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


/***** UniqName routines  11/30/89
Uses <search.h> package for hash table.
*/
void hdestroy();

static boolean UniqNameInitDone = FALSE;

/*
Clears all names
*/
boolean
UniqNameInit()
{
    if (UniqNameInitDone) {
	hdestroy();
    }
    ERR_IF2(!hcreate(100), "UniqNameInit: cannot create hash table");
    UniqNameInitDone = TRUE;
    return TRUE;
}

/*
Outputs: return = str with unique name or NULL if error.  Can free() str.
Updates: 3/8/90 = fix for uvax: ++ lvalue
    3/20/90 = check for full hash table
*/
char *
UniqNameGet(s)
char *s;
{
    char buf[100];
    int *ip;

    ENTRY item, *found, *hsearch();
    item.key = s;
    found = hsearch(item, FIND);
    if (found == NULL) {
	item.data = 0;
	if (!(found = hsearch(item, ENTER))) {
	    ErrAdd("UniqNameGet: hash table is full!");
	    return NULL;
	}
    }
    ip = (int *) &found->data;
    return(DupString(sprintf(buf, "%s%d", s, ++ (*ip))));
}
/***** End UniqName routines */

/***** DupSheet routines  1/24/89
Updates: 4/13/90 = allow multiple DupSheets
*/

boolean
DupSheetInit(ds)
DupSheet *ds;
{
    *ds = NULL;
}

boolean
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
    return(TRUE);
}

boolean
DupSheetAdd(ds, item)
DupSheet *ds;
char *item;
{
    DupSheetNode *new;
    
    ERR_IF1(!UMalloc(&new, sizeof(DupSheetNode)));
    new->info = item;
    new->next = *ds;
    *ds = new;
    return(TRUE);
}

boolean
DupSheetIsDup(ds, item)
DupSheet *ds;
char *item;
{
    DupSheetNode *p;

    for (p = *ds; p != NULL; p = p->next) {
	if (strcmp(p->info, item) == 0) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

extern char* curDomainName();

/* Given the spot that locates the curson in a facet, set
   the KnownBlocks currentDomain to correspond to the domain
   of the facet.  As a side effect, if the domain property of
   the facet has not been defined, it will be set to current domain
   of the KnownBlocks object.
   Returns a pointer to the string identifying the previous domain,
   or NULL if it fails.
        EAL, 9/23/90
*/
char *
setCurDomainS(spot)
RPCSpot *spot;
{
    octObject facet;
    char *domain, *oldDomain;

    oldDomain = curDomainName();

    /* get current facet */
    facet.objectId = spot->facet;

    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        return FALSE;
    }
    if (!GOCDomainProp(&facet, &domain, oldDomain)) {
        PrintErr(ErrGet());
        return NULL;
    }
    /* Call Kernel function to set KnownBlock current domain */
    if (! KcSetKBDomain(domain)) {
	PrintErr("Domain error in current facet.");
	return NULL;
    }
    return oldDomain;
}

/* Given a facet, set the KnownBlocks currentDomain to correspond
   to the domain of the facet.
   As a side effect, if the facet domain property has not been set,
   it will be set to current domain in the KnownBlocks object.
   Returns a pointer to the string identifying the old domain, or
   NULL if this fails.
        EAL, 9/23/90
*/
char *
setCurDomainF(facetPtr)
octObject *facetPtr;
{
    char *domain, *oldDomain;

    oldDomain = curDomainName();

    if (!GOCDomainProp(facetPtr, &domain, oldDomain)) {
        PrintErr(ErrGet());
        return NULL;
    }
    /* Call Kernel function to set KnownBlock current domain */
    if (! KcSetKBDomain(domain)) {
	PrintErr("Domain error in current facet.");
	return NULL;
    }
    return oldDomain;
}

/* Set the domain to correspond to an instance. */

char *
setCurDomainInst(instPtr)
octObject *instPtr;
{
    octObject mFacet;
    char *oldDomain = curDomainName();
    char domain[64], srcName[512], *fullName;
    if (IsGal(instPtr) || IsUniv(instPtr) || IsPal(instPtr))
	    return setCurDomainF(instPtr);
    if (!MyOpenMaster(&mFacet, instPtr, "interface", "r")) {
	    PrintErr(ErrGet());
	    return NULL;
    }
    octFullName(&mFacet, &fullName);
    if (!IconFileToSourceFile(fullName, srcName, domain)) {
	    PrintErr(ErrGet());
	    return NULL;
    }
    if (!KcSetKBDomain(domain)) {
	PrintErr("Domain error in instance.");
	return NULL;
    }
    return oldDomain;
}
