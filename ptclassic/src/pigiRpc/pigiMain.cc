/*******************************************************************
SCCS version identification
@(#)pigiMain.cc	2.15	10/28/99

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

 Programmer: J. T. Buck
 Modified by Alan Kamas to use a Tcl interpreter in the main loop

 Main program for pigiRpc.

********************************************************************/

#include <string.h>

#ifdef linux
#include <fpu_control.h>
#endif

/* C++ function prototypes not defined in any include files */
extern void ptkConsoleWindow();

extern "C" {
#include "kernelCalls.h"	/* define KcInitLog and KcCatchSignals */
#include "pigiLoader.h"		/* define KcLoadInit */
#include "xfunctions.h"		/* define global variable pigiFilename */

/* Prototypes for functions defined in pigilib includes that depend on Oct */
extern void CompileInit();	/* defined in compile.h */
extern void ptkMainLoop();	/* defined in ptkTkSetup.h */

/* Prototypes not defined in any include files */
int ptkRPCInit(int argc, char **argv);
};

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int main(int argc, char** argv)
{
	pigiFilename = argv[0];

#if TK_MAJOR_VERSION >= 8
        Tcl_FindExecutable(argv[0]);
#endif

	KcLoadInit(argv[0]);
#if defined(PTLINUX) && defined(__i386__)
	// Fix for DECalendarQueue SIGFPE under x86 linux.
	// Note by W. Reimer: Glibc 2.1 does not support __setfpucw() any
	// longer. Instead there is a macro _FPU_SETCW. Actually, the whole
	// fix is not required any longer because in the default FPU control
	// word of libc 5.3.12 and newer (glibc) the interrupt mask bit for
	// invalid operation (_FPU_MASK_IM) is already set.
#if (_FPU_DEFAULT & _FPU_MASK_IM) == 0
#ifdef _FPU_SETCW
	{ fpu_control_t cw = (_FPU_DEFAULT | _FPU_MASK_IM); _FPU_SETCW(cw); }
#else
	__setfpucw(_FPU_DEFAULT | _FPU_MASK_IM);
#endif /* _FPU_SETCW */
#endif /* (_FPU_DEFAULT & _FPU_MASK_IM) == 0 */
#endif /* defined(PTLINUX) && defined(__i386__) */
	KcInitLog("pigiLog.pt");
	CompileInit();
	KcCatchSignals();

	/* Strip off end of argv, not front */
	int doConsole = FALSE;
	if (argc >= 2 && strcmp(argv[argc-1], "-console") == 0) {
		doConsole = TRUE;
		--argc;
	}

	ptkRPCInit(argc, argv);
	if (doConsole) ptkConsoleWindow();
	ptkMainLoop();
	return 0;
}
