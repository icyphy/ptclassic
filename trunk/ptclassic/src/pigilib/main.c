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
#include "utilities.h"
#include "err.h"

/* Global Vars */

char *xDisplay;  /* display name passed by vem */
extern char *pigiVersion;


static void
PrintVersion()
{
    PrintCon(pigiVersion);
}

int
Version(spot, cmdList, userOptionWord) /* ARGSUSED */
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
ClearMarks(spot, cmdList, userOptionWord) /* ARGSUSED */
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
ExitApp(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    CompileEnd();
    RPCExit(0);
}


RPCFunction CommandArray[] = {
    {EditParams, "Edit", "edit-params", "e", 0},
    {DefineParams, "Edit", "define-params", "+", 0},
    {RpcEditDomain, "Edit", "edit-domain", "d", 0},
    {RpcEditTarget, "Edit", "edit-target", "T", 0},
    {RpcEditComment, "Edit", "edit-comment", ";", 0},
    {RpcEditSeed, "Edit", "edit-seed", "#", 0},
    {RpcFindName, "Edit", "find-name"},
    {ClearMarks, "Edit", "clear-marks"},
    {RpcOpenPalette, "Window", "open-palette", "O", 0},
    {RpcOpenFacet, "Window", "open-facet", "F", 0},
    {RpcEditIcon, "Window", "edit-icon", "I", 0},
    {RpcLookInside, "Window", "look-inside", "i", 0},
    {RpcRun, "Exec", "run", "R", 0},
    {RpcReRun, "Exec", "re-run", "!", 0},
    {RpcCompileFacet, "Exec", "compile-facet"},
    {RpcMkSchemIcon, "Extend", "make-schem-icon","@",0},
    {RpcMkStar, "Extend", "make-star","*",0},
    {RpcLoadStar, "Extend", "load-star", "L", 0},
    {ERFilterDesign, "Filter", " equirriple FIR", "<", 0},
    {WFilterDesign, "Filter", " window FIR", ">", 0},
    {RpcPlotSignal, "Utilities", "plot signal", "~", 0},
    {RpcPlotCxSignal, "Utilities", "plot Cx signal", "-", 0},
    {RpcDFT, "Utilities", "DFT", "^", 0},
    {RpcDFTCx, "Utilities", "DFT of Cx signal", "_", 0},
    {Man, "Other", "man"},
    {Profile, "Other", "profile", ",", 0},
    {PrintFacet, "Other", "print facet"},
    {Options, "Other", "options"},
    {Version, "Other", "version"},
    {ExitApp, "Other", "exit-pigi"}
};

long
UserMain(display, spot, cmdList, userOptionWord, array) /* ARGSUSED */
char *display;
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
RPCFunction **array;
{
    char buf[MSG_BUF_MAX], *envVar, *getenv();

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
/* initialize modules: compilation of universes, signal catching, loader*/
    *array = CommandArray;
    PrintVersion();
    welcome_window ();
    PrintConLog("PIGI is running");
    vemPrompt();
    return(sizeof(CommandArray) / sizeof(RPCFunction));
}

void pr_accum_string();

welcome_window ()
{
	open_display (xDisplay);
	accum_string ("Ptolemy Interactive Graphics Interface\n");
	accum_string (pigiVersion);
	accum_string ("\nCopyright 1990, 1991 Regents of the University of California");
	accum_string ("\nAll rights reserved.");
	pr_accum_string ();
}
