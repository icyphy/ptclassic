/* 
Copyright (c) 1990-1997 The Regents of the University of California.
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
/*
    icon.c  edg
    Version: @(#)icon.c	2.57	02/22/99
*/


/* Includes */

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

/* Octtools includes */
#include "oct.h"		/* define octObject */
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define RPC data structures */
#include "utility.h"		/* pick up util_csystem() */

/* Pigilib includes */
#include "icon.h"
#include "util.h"
#include "err.h"
#include "vemInterface.h"
#include "palette.h"
#include "main.h"
#include "ptk.h"
#include "octIfc.h"
#include "kernelCalls.h"	/* define callParseClass and KcDomainOf */
#include "pigiLoader.h"
#include "handle.h"
#include "displayFile.h"	/* displayFile(), startTycho() */

#define DM_WIDTH 80		/* dialog entry width */
#define EDIT_ICON_SNAP 5	/* snap size of vem window for edit-icon */

/*  5/9/90
Takes the code directory of a star and returns it's icon directory.
If code directory ends in /stars and ../icons (default icon directory) exists,
then set icon directory to be ../icons. Otherwise, set icon directory
the same as star source code directory.
Warning: it will get rid of the trailing '/' in the string codeDir.
For example, if the user specify the star source code directory (codeDir)
as "~user/work/stars/" with a trailing '/', then codeDir will becomes
"~user/work/stars" upon returning from CodeDirToIconDir.
*/
static boolean
CodeDirToIconDir(codeDir, iconDir)
char *codeDir, **iconDir;
{
    char dir[DM_WIDTH];  /* intermediate string var for icon directory name */
    char dirSave[DM_WIDTH]; /* a copy of dir */
    int len;            /* length of the string dir*/
    struct stat stbuf;  /* used by system call stat */
    char fullName[DM_WIDTH]; /* full name of the default icon directory */

    strcpy(dir, codeDir);
    len = strlen(dir);

    /* remove trailing '/' from dir if there is any */
    if (*(dir+len-1) == '/')
	*(dir+(--len)) = '\0';  

    /* Test if the star source directory name ends in "/stars" */
    if (!strcmp(BaseName(dir), "stars")) {
	strcpy(dirSave, dir);  /* Save a copy of dir */
	DirName(dir); /* Delete "/stars" from dir */

        /* concatenate "/icons" to dir, this is the default icon directory */
	strcat(dir, "/icons");

	/* Must expand ~ before calling system call stat */
	TildeExpand(dir, fullName);

	/* If fullName does not exist or is not a directory then restore dir */
	if (stat(fullName, &stbuf) == -1 || (stbuf.st_mode & S_IFMT) != S_IFDIR)
	    strcpy(dir, dirSave);  
    }

    ERR_IF1(!StrDup(iconDir, dir));
    return(TRUE);
}


/* 8/3/90
Open a window and run tycho on a file.
*/
boolean
LookAtFile(fileName)
const char* fileName;
{
  return displayFile(fileName, PrintDebug, ErrAdd);
}

/*
MkStar() inputs:
    name = name of star
    domain = domain of star
    dir = src directory of star, which contains .pl file
    palette = directory to store the star's icon into
Check to see if the domain exists, then check to see if the star name
is known.  If it not, then try to compile and load it in.

*/
boolean
MkStar(name, domain, dir, palette)
char *name, *domain, *dir, *palette;
{
    char *iconDir;

    /* If the domain is a domain other than NODOMAIN, check the domain
     * and load the star. NODOMAIN is used to create icons for html files.
     */
    if (strcmp(domain,NODOMAIN)) {
        if (!KcSetKBDomain(domain)) {
	    ErrAdd("Unknown domain: ");
	    ErrAdd(domain);
	    return FALSE;
        }

	/* If we don't know about the star, try to load it. */
	/* Get the corresponding class name.  */
	if (!KcIsKnown(name)) {
	   const char *base = callParseClass(name);
	   if (!base) return FALSE;
	   PrintDebug("Star not known, trying to load it");
	   if (!KcCompileAndLink (base, domain, dir, FALSE, NULL))
	       return FALSE;
	   PrintDebug("Load complete");
	   if (!KcIsKnown(name)) {
	       ErrAdd("Load completed, but star is still undefined?!?");
	       return FALSE;
	   }
	}
    }

    ERR_IF1(!CodeDirToIconDir(dir, &iconDir));
    ERR_IF1(!MkStarIconInPal(name, iconDir, domain, palette));
    return (TRUE);
}

/* 11/17/89 3/6/89 8/29/88
*/
int 
RpcMkStar(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("make-star");
    ErrClear();

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkEditStrings ",
			       " \"Make Star\" ",
			       " \"ptkSetMkStar %s \" ",
			       " [ptkGetMkStar]",
			       (char *)NULL) );

    ViDone();
}


/**
split this out of GetTildePath -- kennard
**/
static boolean
IsSubPathB( fullPath, subPath, pRemainPath)
    char	*fullPath, *subPath;
    char	**pRemainPath;
{
    char	expanded_path[MAXPATHLEN];
    int		n;
    
    /* Try the subPath as given */
    n = strlen(subPath);
    if (strncmp(fullPath, subPath, n) == 0) {
	if ( pRemainPath )
	    *pRemainPath = fullPath + n;
	return TRUE;
    }

    /* Expand symlinks out of subPath and try that */
    ERR_IF2((AbsPath(subPath, expanded_path) < 0),
        "IsSubPathB: Path expansion failed on subPath.");
    n = strlen(expanded_path);
    if (strncmp(fullPath, expanded_path, n) == 0) {
	if ( pRemainPath )
	    *pRemainPath = fullPath + n;
	return TRUE;
    }
    return FALSE;
}


/* 11/18/89
Tries to convert the cell name of the facet into ~user format.
It does this by comparing the absolute path of the facet with
the path of the user home directory.  It also compares with the
path of the ptolemy user's home directory, since this user typically
contains many shared projects.
Inputs: tPath = buffer for output.
Outputs: tPath = filled with cell name in ~user/... format.
Caveats: The code should support more "shared users" than just ~ptolemy.
     In the extreme, it would search the entire /etc/passwd file.
     But this if probably too costly.
Added handling of $PTOLEMY -- kennard
*/
boolean
GetTildePath(facetPtr, tPath)
    octObject		*facetPtr;
    char		*tPath;
{
    char		*fullName, *envPath, *remainPath;
    int			uid;
    struct passwd	*pwent;

    /*
     * try current user's directory tree: ~user/remainPath
     */
    uid = getuid();
    ERR_IF2((pwent = getpwuid(uid)) == NULL,
	"GetTildePath: Cannot get password entry");

    /* fullName will be malloc'd */
    octFullName(facetPtr, &fullName);

    if ( IsSubPathB( fullName, pwent->pw_dir, &remainPath) ) {
	sprintf(tPath, "~%s%s", pwent->pw_name, remainPath);
    }

    /*
     * try $(PTOLEMY) directory tree: $PTOLEMY/remainPath
     */
    else if ( (envPath = getenv(UToolEnvName))!=NULL
     && IsSubPathB( fullName, envPath, &remainPath) ) {
	sprintf(tPath, "$%s%s", UToolEnvName, remainPath);
    }

    /*
     * try ptolemy user's directory tree: ~ptolemy/remainPath
     * it is not an error if there is no such account.
     */
    else if ((pwent = getpwnam(UToolName)) != 0 &&
	     IsSubPathB( fullName, pwent->pw_dir, &remainPath)) {
	sprintf(tPath, "~%s%s", pwent->pw_name, remainPath);
    }
    else {
	/* Issue a warning that absolute path name is being used */
	PrintErr("Warning: Can't convert absolute path to ~user. The icon will point to its master using the full Unix path.");
	strcpy(tPath,fullName);
    }
    free(fullName);
    return (TRUE);
}

/*
 * Return the expanded, absolute path corresponding to a directory.
 * "path" should be a directory pathname; "abs_path" should be a
 * buffer of MAXPATHLEN characters where the expanded path name will
 * be put.  Returns 0 on success, and one of the following on error:
 *
 *	-1	Couldn't get current directory
 *	-2	Couldn't stat new "path"
 *	-3	"path" isn't a directory
 *	-4	Couldn't cd to "path"
 *	-5	Couldn't get pathname of "path".
 */
int
AbsPath(path, abs_path)
	char		*path;
	char		*abs_path;
{
	char		oldpath[MAXPATHLEN];
	struct stat	s;

#ifndef SYSV
	if (getwd(oldpath) == NULL) {
#else
	if (getcwd(oldpath, MAXPATHLEN) == NULL) {
#endif
		return (-1);
	}

	if (stat(path, &s) < 0) {
		return (-2);
	}

	if ((s.st_mode & S_IFMT) != S_IFDIR) {
		return (-3);
	}

	if (chdir(path) < 0) {
		return (-4);
	}

#ifndef SYSV
	if (getwd(abs_path) == NULL) {
#else
	if (getcwd(abs_path, MAXPATHLEN) == NULL) {
#endif
		return (-5);
	}

	(void) chdir(oldpath);

	return (0);
}

int 
RpcMkSchemIcon(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet = {OCT_UNDEFINED_OBJECT, 0};
    char facetHandle[POCT_FACET_HANDLE_LEN];

    ViInit("make-schem-icon");
    ErrClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
    }
    else {
	ptkOctObj2Handle(&facet,facetHandle);
	TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkEditStrings ",
	  " \"Make Schematic Icon\" ",
          " \"ptkSetMkSchemIcon ", facetHandle, " %s \" ",
          " \" {{Enter pathname of palette} [ptkGetMkSchemIcon]}\" ",
          (char *)NULL) );
    }

    ViDone();
}

int 
RpcShowName(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject inst = {OCT_UNDEFINED_OBJECT, 0},
	      facet = {OCT_UNDEFINED_OBJECT, 0};
    vemStatus status;
    static char buf[512];

    ViInit("show-name");
    ErrClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
    }
    else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
    }
    else {
	switch (inst.type) {
	case OCT_FACET:
	    (void) sprintf(buf, "Facet: '%s'", inst.contents.facet.facet);
	    break;
	case OCT_TERM:
	    (void) sprintf(buf, "Terminal named: '%s'",
		inst.contents.term.name);
	    break;
	case OCT_NET:
	    (void) sprintf(buf, "Net");
	    break;
	case OCT_INSTANCE:
	    (void) sprintf(buf, "Instance named '%s'\nFrom master '%s'",
		inst.contents.instance.name,
		inst.contents.instance.master);
	    break;
	default:
	    (void) sprintf(buf, "No name for this object");
	    break;
	}
	Tcl_VarEval(ptkInterp,"ptkMessage {", buf, "}", (char *)NULL);
	FreeOctMembers(&inst);
    }
    FreeOctMembers(&facet);
    ViDone();
}

int
RpcTycho(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("Tycho");
    ErrClear();

    startTycho();

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,
	"::tycho::view TclShell -geometry -0+0 -text {Welcome to Tycho in Pigi\n}\n",
	(char *)NULL) );

    ViDone();
}

int 
RpcLookInside(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet = {OCT_UNDEFINED_OBJECT, 0},
	      inst = {OCT_UNDEFINED_OBJECT, 0},
	      facet = {OCT_UNDEFINED_OBJECT, 0};
    vemStatus status;
    char *fullName, codeFile[512], domain[64];

    ViInit("look-inside");
    ErrClear();

/* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
    }
    else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
    }
    else {
	if (IsStar(&inst)) {
	    if (!MyOpenMaster(&mFacet, &inst, "interface", "r"))
		PrintErr(ErrGet());
	    else {
		octFullName(&mFacet, &fullName);
		/* Figure out file names */
		if (!IconFileToSourceFile(fullName, codeFile, domain) ||
		    !LookAtFile(codeFile))
		    PrintErr(ErrGet());
		free(fullName);
		FreeOctMembers(&mFacet);
	    }
	}
	else if (IsGal(&inst) || IsUniv(&inst) || IsPal(&inst)) {
	    if (!MyOpenMaster(&mFacet, &inst, "contents", "r")) {
		PrintErr(ErrGet());
	    }
	    else {
		/* Change to full name because VEM re-opens facet */
		octFullName(&mFacet, &fullName);
		/* fullName is on the heap, will be freed by FreeOctMembers */
		free(mFacet.contents.facet.cell);
		mFacet.contents.facet.cell = fullName;
		vemOpenWindow(&mFacet, NULL);
		FreeOctMembers(&mFacet);
	    }
	}
	else 
	    PrintErr("The icon instance is not a universe, galaxy, palette, or star.");
	FreeOctMembers(&inst);
    }
    FreeOctMembers(&facet);
    ViDone();
}

/* This function takes an icon file name and finds the matching source
   file.  If there is no matching source file, FALSE is returned.
   the string "sourceFile" gets the full source file name and
   the string "domain" gets the domain.
*/

int
IconFileToSourceFile (iconFile, sourceFile, domain)
char* iconFile, *sourceFile, *domain;
{
	char dir[512];
	const char *dom = 0;
	const char *base;
	int i, n;

	strcpy (dir, iconFile);
	DirName (dir);
/* See if dir ends in /icons */
	if (!strcmp(BaseName(dir), "icons")) {
		DirName (dir);
		strcat(dir, "/stars");
	}
	base = callParseClass(BaseName(iconFile));

	/* Search for source file */
	n = numberOfDomains ();
	for (i = 0; i < n; i++) {
		dom = nthDomainName(i);
		if (FindStarSourceFile(dir, dom, base, sourceFile) >= 0)
			break;
		dom = 0;
	}
	if (!dom) {
		sprintf (sourceFile,
			 "Can't find source file for %s, perhaps that domain is not built in to this image?",
			 base);
		ErrAdd (sourceFile);
		return FALSE;
	}
	strcpy (domain, dom);
	return TRUE;
}

int 
RpcEditIcon(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet = {OCT_UNDEFINED_OBJECT, 0},
	      inst = {OCT_UNDEFINED_OBJECT, 0};
    vemStatus status;
    Window iconWindow; /* the vem window to edit icon in */
    wnOpts options; /* need this to set snap size of iconWindow */

    ViInit("edit-icon");
    ErrClear();
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
    }
    else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
    }
    else if (!MyOpenMaster(&mFacet, &inst, "interface", "r")) {
	PrintErr(ErrGet());
    }
    else {
	iconWindow = vemOpenWindow(&mFacet, NULL);
	/* When editing icon it's convenient to have a smaller snap size */
	vemWnGetOptions(iconWindow, &options);
	options.snap = EDIT_ICON_SNAP;
	vemWnSetOptions(iconWindow, &options);
    }
    ViDone();
}
