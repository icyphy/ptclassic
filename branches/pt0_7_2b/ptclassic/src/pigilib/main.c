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
/*  main.c  edg  5/9/90
Version identification:
@(#)main.c	2.44	02/05/97
Initialization code for pigiRpc.
*/

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>
#include <stdlib.h>

/* Octtools includes */
#include "oct.h"
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define remote procedure calls */

/* Pigilib includes */
#include "main.h"
#include "vemInterface.h"
#include "utilities.h"
#include "compile.h"
#include "edit.h"
#include "exec.h"
#include "util.h"
#include "icon.h"
#include "misc.h"
#include "ganttIfc.h"
#include "options.h"
#include "err.h"
#include "xfunctions.h"
#include "pigiLoader.h"

/* Global Vars */

char *xDisplay;		/* display name passed by vem */

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
    return 0;
}


RPCFunction CommandArray[] = {
    {EditParams, "Edit", "edit-params", "e", 0},
    {RpcEditDomain, "Edit", "edit-domain", "d", 0},
    {RpcEditTarget, "Edit", "edit-target", "T", 0},
    {RpcEditComment, "Edit", "edit-comment", ";", 0},
    {EditPragmas, "Edit", "edit-pragmas", "a", 0},
    {RpcEditSeed, "Edit", "set-seed", "#", 0},
    {RpcFindName, "Edit", "find-name"},
    {ClearMarks, "Edit", "clear-marks"},
    {RpcOpenPalette, "Window", "open-palette", "O", 0},
    {RpcOpenFacet, "Window", "open-facet", "F", 0},
    {RpcEditIcon, "Window", "edit-icon", "I", 0},
    {RpcLookInside, "Window", "look-inside", "i", 0},
    {RpcTycho, "Window", "Tycho", "y", 0},
    {RpcRun, "Exec", "run", "R", 0},
    {RpcRunAllDemos, "Exec", "run-all-demos", "!", 0},
    {RpcCompileFacet, "Exec", "compile-facet"},
    {RpcDisplaySchedule, "Exec", "display-schedule"},
    {RpcMkSchemIcon, "Extend", "make-schem-icon", "@", 0},
    {RpcMkStar, "Extend", "make-star", "*", 0},
    {RpcLoadStar, "Extend", "load-star", "L", 0},
    {RpcLoadStarPerm, "Extend", "load-star-perm", "K", 0},
    {ERFilterDesign, "Filter", " equirriple FIR", "<", 0},
    {WFilterDesign, "Filter", " window FIR", ">", 0},
    {RpcPlotSignal, "Utilities", "plot signal", "~", 0},
    {RpcPlotCxSignal, "Utilities", "plot Cx signal", "-", 0},
    {RpcDFT, "Utilities", "DFT", "^", 0},
    {RpcDFTCx, "Utilities", "DFT of Cx signal", "_", 0},
    {ShowFacetNum, "Other", "facet number", "H", 0},
    {Man, "Other", "man", "M", 0},
    {Profile, "Other", "profile", ",", 0},
    {PrintFacet, "Other", "print facet", "", 0},
    {RpcShowName, "Other", "show-name", "n", 0},
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
    /* Set the DISPLAY environment variable for ptolemy stars that use it,
       if it does not already exist
     */
    if (getenv("DISPLAY") == 0) {
	sprintf(buf, "DISPLAY=%s", display);
	if(!StrDup(&envVar, buf)) {
	    PrintErr(ErrGet());
	    RPCExit(-1);
	}
	putenv(envVar);
    }
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
    KcDoStartupLinking();
    PrintConLog("PIGI is running");
    vemPrompt();
    *array = CommandArray;
    return(sizeof(CommandArray) / sizeof(RPCFunction));
}
