/* icon.c  edg
Version identification:
$Id$
*/


/* Includes */
#include <stdio.h>
#include <sys/file.h>
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

/*  5/8/90
Takes an absolute icon pathname (facet cellname) and converts it
to it's corresponding Ptolemy source file name.
*/
static boolean
IconToCode(icon, codeFile)
char *icon, **codeFile;
{
    char dir[512], buf[512], *base;

    strcpy(dir, icon);
    DirName(dir);
    base = BaseName(icon);
    sprintf(buf, "%s/../src/%s.cc", dir, base);
    ERR_IF1(!StrDup(codeFile, buf));
    return (TRUE);
}

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
    sprintf(buf, "%s/icon", dir);
    ERR_IF1(!StrDup(iconDir, buf));
    return (TRUE);
}

/* 5/8/90
Open a window and display a code file.
*/
static boolean
EditCode(codeFile)
char *codeFile;
{
    char buf[612];

    sprintf(buf, "xterm -display %s -name ptolemy_code -e vi %s",
	xDisplay, codeFile);
    PrintDebug(buf);
    if (util_csystem(buf)) {
	ErrAdd(sprintf(buf, "Cannot edit Ptolemy code file '%s'", codeFile));
    }
    return (TRUE);
}

/*
Inputs:
    name = name of star
    dir = src directory of star, which contains .cc (and .pt ?) file
*/
static boolean
MkStar(name, dir)
char *name, *dir;
{
static dmTextItem item = {"Palette", 1, 80, "./user", NULL};
static dmTextItem itTemplate = {"Template", 1, 80, NULL, NULL};
static char *q1 = "Cannot find star definition.  Define a new star?";
    char *iconDir;
    
    if (!KcIsKnown(name)) {
	ErrAdd("Unknown star");
	return (FALSE);
/*
The following code is excluded until Ptolemy kernel issues have been
defined.  It's not useful until dynamic loading at least is implemented.
It also needs to be modified for Ptolemy.
*/
#if 0
	char file[512], *expFile;
	sprintf(file, "%s/%s.l", dir, name);
	expFile = util_tilde_expand(file);
	if (access(expFile, F_OK)) {
	    char buf[512], *def;
	    if (itTemplate.value == NULL) {
		sprintf(buf, "%s.StarTemplateFile", UAppName);
		if ((def = RPCXGetDefault("vem", buf)) == NULL) {
		    def = "~ptolemy/lib/starTemplate";
		}
		itTemplate.value = def;
	    }
	    ERR_IF2(dmMultiText(q1, 1, &itTemplate) != VEM_OK,
		"Aborted entry");
	    ERR_IF2(access(util_tilde_expand(dir), W_OK),
		"You do not have write permission in directory.");
	    if (strlen(itTemplate.value) > 0) {
		/* template specified, copy it... */
		ERR_IF2(access(util_tilde_expand(itTemplate.value), F_OK),
		    "Cannot find template file");
		sprintf(buf, "\cp %s %s", itTemplate.value, file);
		PrintDebug(buf);
		ERR_IF2(util_csystem(buf), "Copy failed");
		sprintf(buf, "\chmod u+w %s", file);
		PrintDebug(buf);
		ERR_IF2(util_csystem(buf), "chmod failed");
	    }
	    EditCode(file);
	    PrintDebug(buf);
	    ERR_IF2(util_csystem(buf), "Cannot edit file");
	}
	PrintCon("Loading star definition...");
	/* leave off file suffix so that lisp will load .o before .l */
	sprintf(file, "%s/%s", dir, name);
	ERR_IF1(!KcLoad(file));
	ERR_IF2(!KcIsKnown(name),
	    "MkStar: Load failed.  Load star manually.");
#endif
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
    {"Star directory", 1, dmWidth, NULL, NULL}
};
#define ITEMS_N sizeof(items) / sizeof(dmTextItem)
    struct passwd *pwent;
    char buf[64];

    ViInit("make-star");
    ErrClear();

    if (items[1].value == NULL) {
	if ((pwent = getpwuid(getuid())) == NULL) {
	    PrintErr("Cannot get password entry");
	    ViDone();
	}
	items[1].value = sprintf(buf, "~%s/", pwent->pw_name);
    }
    if (dmMultiText(ViGetName(), ITEMS_N, items) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    if (*items[1].value != '~' || !isalpha(items[1].value[1])) {
	PrintErr("Star directory must begin with '~user'");
	ViDone();
    }
    if (strcmp(BaseName(items[1].value), "src") != 0) {
	PrintErr("Star directory must end with '/src'");
	ViDone();
    }
    if (!MkStar(items[0].value, items[1].value)) {
	PrintErr(ErrGet());
	ViDone();
    }
    ViDone();
#undef ITEMS_N
}

/* 11/18/89
Tries to convert the cell name of the facet into ~user format.
Inputs: tPath = buffer for output.
*/
static boolean
GetTildePath(facetPtr, tPath)
octObject *facetPtr;
char *tPath;
{
    char *fullName, *dir;
    int uid, n;
    struct passwd *pwent;

    octFullName(facetPtr, &fullName);
    uid = getuid();
    ERR_IF2((pwent = getpwuid(uid)) == NULL,
	"GetTildePath: Cannot get password entry");
    dir = pwent->pw_dir;
    n = strlen(dir);
    if (strncmp(fullName, dir, n) == 0) {
	sprintf(tPath, "~%s%s", pwent->pw_name, fullName + n);
	return (TRUE);
    }
    ERR_IF2((pwent = getpwnam(UToolName)) == NULL,
	"GetTildePath: Cannot get password entry");
    dir = pwent->pw_dir;
    n = strlen(dir);
    if (strncmp(fullName, dir, n) == 0) {
	sprintf(tPath, "~%s%s", pwent->pw_name, fullName + n);
	return (TRUE);
    }
    ErrAdd("GetTildePath: cannot convert absolute path to ~user");
    return (FALSE);
}

int 
RpcMkSchemIcon(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
static dmTextItem item = {"Palette", 1, dmWidth, "./user", NULL};
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

int 
RpcLookInside(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet, inst;
    vemStatus status;
    char *fullName, *codeFile;

    ViInit("look-inside");
    ErrClear();
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
	    if (!MyOpenMaster(&mFacet, &inst, "interface", "r")) {
		PrintErr(ErrGet());
		ViDone();
	    }
	    octFullName(&mFacet, &fullName);
	    ERR_IF1(!IconToCode(fullName, &codeFile));
	    ERR_IF1(!EditCode(codeFile));
	    ViDone();
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
    vemOpenWindow(&mFacet, NULL);
    ViDone();
}
