/* icon.c  edg
Version identification:
$Id$
*/


/* Includes */
#include <stdio.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <pwd.h>
#include "local.h"
#include "rpc.h"
#include "util.h"
#include "err.h"
#include "vemInterface.h"
#include "octMacros.h"
#include "palette.h"
#include "main.h"

#define dmWidth 80  /* dialog entry width */
#define EDIT_ICON_SNAP 5 /* snap size of vem window for edit-icon */


/*  5/9/90
Takes the code directory of a star and returns it's icon directory.
*/
static boolean
CodeDirToIconDir(codeDir, iconDir)
char *codeDir, **iconDir;
{
    char dir[512], buf[512];

    strcpy(dir, codeDir);
    DirName(dir);
    sprintf(buf, "%s/icons", dir);
    ERR_IF1(!StrDup(iconDir, buf));
    return (TRUE);
}

/* 5/8/90
Open a window and run vi on a file.
Does not run in the background.
This routine does not return until vi is exited.
*/
static boolean
EditFile(fileName)
char *fileName;
{
    char buf[612];

    sprintf(buf, "xterm -display %s -name ptolemy_code -e vi %s",
	xDisplay, fileName);
    PrintDebug(buf);
    if (util_csystem(buf)) {
	ErrAdd(sprintf(buf, "Cannot edit Ptolemy code file '%s'", fileName));
    }
    return (TRUE);
}

/* 8/3/90
Open a window and run vi on a file.
Runs in the background and returns immediately.
*/
static boolean
LookAtFile(fileName)
char *fileName;
{
    char buf[612];

    sprintf(buf, "xterm -display %s -name ptolemy_code -e vi %s &",
	xDisplay, fileName);
    PrintDebug(buf);
    if (util_csystem(buf)) {
	ErrAdd(sprintf(buf, "Cannot edit Ptolemy code file '%s'", fileName));
    }
    return (TRUE);
}


/*
Inputs:
    name = name of star
    dir = src directory of star, which contains .cc (and .pt ?) file
*/
static boolean
MkStar(name, domain, dir)
char *name, *domain, *dir;
{
static dmTextItem item = {"Palette", 1, 80, "./user.pal", NULL};
static dmTextItem itTemplate = {"Template", 1, 80, NULL, NULL};
static char *q1 = "Cannot find star definition.  Define a new star?";
    char *iconDir;
    
    if (!KcSetKBDomain(domain)) {
	ErrAdd("Unknown domain: ");
	ErrAdd(domain);
	return FALSE;
    }
    if (!KcIsKnown(name)) {
	ErrAdd("No such star in the given domain");
	return (FALSE);
    }
    ERR_IF2(dmMultiText("Enter pathname of palette", 1, &item) != VEM_OK,
	"Aborted entry");
    ERR_IF1(!CodeDirToIconDir(dir, &iconDir));
    ERR_IF1(!MkStarIconInPal(name, iconDir, item.value));
    return (TRUE);
}

/* 11/17/89 3/6/89 8/29/88
*/
int 
RpcMkStar(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
static dmTextItem items[] = {
    {"Star name", 1, dmWidth, NULL, NULL},
    {"Domain", 1, 80, DEFAULT_DOMAIN, NULL},
    {"Star src directory", 1, dmWidth, NULL, NULL}
};
#define ITEMS_N sizeof(items) / sizeof(dmTextItem)
    struct passwd *pwent;
    char buf[64];

    ViInit("make-star");
    ErrClear();

    if(setCurDomainS(spot) == NULL) {
	PrintErr("Domain error in facet.");
	ViDone();
    }

    if (items[2].value == NULL) {
	if ((pwent = getpwuid(getuid())) == NULL) {
	    PrintErr("Cannot get password entry");
	    ViDone();
	}
	items[2].value = sprintf(buf, "~%s/", pwent->pw_name);
    }
    if (dmMultiText(ViGetName(), ITEMS_N, items) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    if (*items[2].value != '~' || !isalpha(items[1].value[1])) {
	PrintErr("Star src directory must begin with '~user'");
	ViDone();
    }
    if (!MkStar(items[0].value, items[1].value, items[2].value)) {
	PrintErr(ErrGet());
	ViDone();
    }
    ViDone();
#undef ITEMS_N
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
*/
static boolean
GetTildePath(facetPtr, tPath)
octObject *facetPtr;
char *tPath;
{
    char *fullName, expanded_path[MAXPATHLEN], *dir, *edir;
    int uid, n;
    struct passwd *pwent;
    char *kludge;

    octFullName(facetPtr, &fullName);

    /* check if in current user's directory tree */
    uid = getuid();
    ERR_IF2((pwent = getpwuid(uid)) == NULL,
	"GetTildePath: Cannot get password entry");

    n = strlen(pwent->pw_dir);

    /* If the fullName begins with the absolute path name of the user's
       home directory, convert to ~user format. */
    if (strncmp(fullName, pwent->pw_dir, n) == 0) {
	sprintf(tPath, "~%s%s", pwent->pw_name, fullName + n);
	return (TRUE);
    }

    /* In case pw entry is not a real directory, but rather is connected
       via symbolic links to a real directory, expand it.  */
    ERR_IF2((AbsPath(pwent->pw_dir, expanded_path) < 0),
        "GetTildePath: Path expansion failed on /etc/passwd entry.");

    n = strlen(expanded_path);

    /* If the fullName begins with the absolute path name of the user's
       home directory, convert to ~user format. */
    if (strncmp(fullName, expanded_path, n) == 0) {
	sprintf(tPath, "~%s%s", pwent->pw_name, fullName + n);
	return (TRUE);
    }

    /* If the fullName begins with the absolute path name of ptolemy's
       home directory, convert to ~ptolemy format. */
    ERR_IF2((pwent = getpwnam(UToolName)) == NULL,
        "GetTildePath: Cannot get password entry for 'ptolemy'");

    n = strlen(pwent->pw_dir);

    if (strncmp(fullName, pwent->pw_dir, n) == 0) {
	sprintf(tPath, "~%s%s", pwent->pw_name, fullName + n);
	return (TRUE);
    }

    /* In case entry is not a real directory, but rather is connected
       via symbolic links to a real directory, expand it.  */
    ERR_IF2((AbsPath(pwent->pw_dir, expanded_path) < 0),
        "GetTildePath: Path expansion failed on /etc/passwd entry.");

    n = strlen(expanded_path);

    if (strncmp(fullName, expanded_path, n) == 0) {
	sprintf(tPath, "~%s%s", pwent->pw_name, fullName + n);
	return (TRUE);
    }

    /* Issue a warning that absolute path name is being used */
    PrintErr("Warning: Can't convert absolute path to ~user. Icon master should not henceforth be moved.");
    strcpy(tPath,fullName);
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

	if (getwd(oldpath) == NULL) {
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

	if (getwd(abs_path) == NULL) {
		return (-5);
	}

	(void) chdir(oldpath);

	return (0);
}

int 
RpcMkSchemIcon(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
static dmTextItem item = {"Palette", 1, dmWidth, "./user.pal", NULL};
    octObject facet;
    char buf[512];

    ViInit("make-schem-icon");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    if (dmMultiText("Enter pathname of palette", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    if (!GetTildePath(&facet, buf)) {
	PrintErr(ErrGet());
	ViDone();
    }
    DirName(buf);
    if (IsGalFacet(&facet)) {
	if (!MkGalIconInPal(&facet, buf, item.value)) {
	    PrintErr(ErrGet());
	    ViDone();
	}
    } else if (IsPalFacet(&facet)) {
	if (!MkPalIconInPal(&facet, buf, item.value)) {
	    PrintErr(ErrGet());
	    ViDone();
	}
    } else {
	if (!MkUnivIconInPal(&facet, buf, item.value)) {
	    PrintErr(ErrGet());
	    ViDone();
	}
    }
    ViDone();
}

char* callParseClass();
char* KcDomainOf();

int 
RpcLookInside(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet, inst, facet;
    vemStatus status;
    char *fullName, dir[512], codeFile[512], *base, *domain;

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
        ViDone();
    } else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
	ViDone();
    } else {
	if (IsGal(&inst) || IsUniv(&inst) || IsPal(&inst)) {
	    if (!MyOpenMaster(&mFacet, &inst, "contents", "r")) {
		PrintErr(ErrGet());
		ViDone();
	    }
	    /* Change to full name because VEM re-opens facet */
	    octFullName(&mFacet, &fullName);
	    mFacet.contents.facet.cell = fullName;
	    vemOpenWindow(&mFacet, NULL);
	} else {
	    if (IsStar(&inst)) {
	    	if (!MyOpenMaster(&mFacet, &inst, "interface", "r")) {
		    PrintErr(ErrGet());
		    ViDone();
	        }
	        octFullName(&mFacet, &fullName);

	        /* Figure out file names */
	        strcpy(dir, fullName);
	        DirName(dir);

		/* The following supports names like Fork.output=2
		   using only the "Fork" part.  */
	        base = callParseClass(BaseName(fullName));
		/* determine the domain */
		setCurDomainF(&facet);
		domain = KcDomainOf(base);

	        /* Form the filename */
	        sprintf(codeFile, "%s/../stars/%s%s.pl", dir, domain, base);
	        ERR_IF1(!LookAtFile(codeFile));

		ViDone();
	    } else {
		PrintErr("The icon instance is not a universe, galaxy, palette, or star.");
                ViDone();
            }
	}
    }
    ViDone();
}

int 
RpcEditIcon(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet, inst;
    vemStatus status;
    char *fullName;
    Window iconWindow; /* the vem window to edit icon in */
    wnOpts options; /* need this to set snap size of iconWindow */

    ViInit("edit-icon");
    ErrClear();
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
	ViDone();
    }
    if (!MyOpenMaster(&mFacet, &inst, "interface", "r")) {
	PrintErr(ErrGet());
	ViDone();
    }
    iconWindow = vemOpenWindow(&mFacet, NULL);
    /* When editing icon it's convenient to have a smaller snap size */
    vemWnGetOptions(iconWindow, &options);
    options.snap = EDIT_ICON_SNAP;
    vemWnSetOptions(iconWindow, &options);
    ViDone();
}
