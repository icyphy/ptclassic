/*  main.c  edg  5/9/90
Version identification:
$Id$
Initialization code for pigiRpc.
*/

/* Includes */
#include <stdio.h>
#include "local.h"
#include "rpc.h"
#include "vemInterface.h"
#include "compile.h"
#include "edit.h"
#include "exec.h"
#include "util.h"
#include "icon.h"
#include "misc.h"
#include "ganttIfc.h"
#include "options.h"

/* Global Vars */

char *xDisplay;  /* display name passed by vem */
char *version = "Pigi Version: 0.0";


static void
PrintVersion()
{
    PrintCon(version);
}

int
Version(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("version");
    ErrClear();
    PrintVersion();
    ViDone();
}

int 
ClearMarks(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("clear-marks");
    ErrClear();
    if (!EssClear()) {
	PrintErr(ErrGet());
    }
    FindClear();
    ViDone();
}

int
ExitApp(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    CompileEnd();
    RPCExit(0);
}


RPCFunction CommandArray[] = {
    {EditParams, "Edit", "edit-params", "e", 0},
    {RpcEditComment, "Edit", "edit-comment", ";", 0},
    {RpcFindName, "Edit", "find-name"},
    {ClearMarks, "Edit", "clear-marks"},
    {RpcOpenPalette, "Window", "open-palette", "O", 0},
    {RpcOpenFacet, "Window", "open-facet", "F", 0},
    {RpcEditIcon, "Window", "edit-icon"},
    {RpcLookInside, "Window", "look-inside", "i", 0},
    {RpcRun, "Exec", "run", "R", 0},
    {RpcReRun, "Exec", "re-run", "!", 0},
    {RpcCompileFacet, "Exec", "compile-facet"},
    {RpcMkSchemIcon, "Extend", "make-schem-icon"},
    {RpcMkStar, "Extend", "make-star"},
    {RpcLoadStars, "Extend", "load-stars"},
    {RpcLoad, "Extend", "load"},
    {Man, "Other", "man"},
    {Profile, "Other", "profile"},
    {Options, "Other", "options"},
    {Version, "Other", "version"},
    {ExitApp, "Other", "exit-pigi"}
};

int
UserMain(display, spot, cmdList, userOptionWord, array)
char *display;
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
RPCFunction **array;
{
    char buf[MSG_BUF_MAX], *envVar, *getenv();

    __main();  /* for c++ initialization */

    xDisplay = display;
    /* Set the DISPLAY environment variable for ptolemy stars that use it */
    sprintf(buf, "DISPLAY=%s", display);
    if(!StrDup(&envVar, buf)) {
	PrintErr(ErrGet());
	RPCExit(-1);
    }
    putenv(envVar);

    if (!OptInit()) {
	ErrAdd("UserMain: OptInit() failed");
	PrintErr(ErrGet());
	RPCExit(-1);
    }
    if (!OpenPaletteInit()) {
	ErrAdd("UserMain: OpenPaletteInit() failed");
	PrintErr(ErrGet());
	RPCExit(-1);
    }
    CompileInit();
    *array = CommandArray;
    PrintVersion();
    PrintConLog("PIGI is running");
    vemPrompt();
    return(sizeof(CommandArray) / sizeof(RPCFunction));
}
