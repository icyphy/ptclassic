/**************************************************************************
Version identification:
$Id$

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
		       
 Programmer:  J. T. Buck
 Date of creation: 3/4/92

This is the main program for the ptcl interpreter.  Most of the code
here handles I/O and is copied from the "wish" Tcl/Tk interpreter by
John Ousterhout.

Eventually we will use streams instead and move this stuff into the PTcl
class.

**************************************************************************/
static const char file_id[] = "ptclMain.cc";
#include "PTcl.h"
#include "Linker.h"
#include "SimControl.h"
#include "Error.h"
#include "StringList.h"
#include <stdio.h>

extern "C" int isatty(int);

static void prompt (int tty) {
	if (tty) {
		printf("ptcl: ");
		fflush(stdout);
	}
}

static void loadStartup(Tcl_Interp* interp);

main (int argc, char** argv) {
	if (argc > 2) {
		fprintf (stderr, "Usage: ptcl [file]\n");
		return 1;
	}
	Linker::init(argv[0]);	// initialize incremental link module
	int tty = isatty(0);
	if (tty) {		// set up interrupt handler
		SimControl::catchInt();
	}
	// all this stuff should be moved into the PTcl class.
	// streams would be better than stdio.

	char* cmd;
	int result;
	char line[200];
	Tcl_Interp* interp = Tcl_CreateInterp();
	PTcl ptcl(interp);

// Load in [info library]/init.tcl.
	loadStartup(interp);

	int gotPartial = 0;
	int status = 0;		// return status.
	if (argc == 2) {
		result = Tcl_EvalFile(interp, argv[1]);
		if (result != TCL_OK) {
			// print error info if it exists
			char* info = Tcl_GetVar(interp, "errorInfo", 0);
			if (info) fprintf (stderr, "%s\n", info);
			status = 1;
		}
	}
	else {
		Tcl_CmdBuf buffer = Tcl_CreateCmdBuf();
		prompt(tty);
		while (fgets (line, 200, stdin) != NULL) {
			cmd = Tcl_AssembleCmd(buffer, line);
			if (cmd) {
				gotPartial = 0;
				result = Tcl_Eval(interp, cmd, 0, 0);
				if (interp->result[0])
					printf ("%s\n", interp->result);
				prompt(tty);
			}
			else {
				gotPartial = 1;
			}
		}
		if (gotPartial) {
			line[0] = 0;
			cmd = Tcl_AssembleCmd(buffer, line);
			if (cmd) {
				result = Tcl_Eval(interp, cmd, 0, 0);
				printf ("%s\n", interp->result);
			}
		}
		Tcl_DeleteCmdBuf(buffer);
	}
	Tcl_DeleteInterp(interp);
	return status;
}

// the following stub is here until we support the Gantt chart
// under the interpreter.

extern "C" int displayGanttChart(const char* file) {
	StringList cmd;
	cmd << "gantt " << file;
	system(cmd);
	return 0;
}

// this file loads in the startup Tcl code.
// The environment variable TCL_LIBRARY is also set to a standard position.

static void loadStartup(Tcl_Interp* interp) {
	const char *pt = getenv("PTOLEMY");
	if (!pt) pt = expandPathName("~ptolemy");
	StringList dir = pt;
	dir << "/tcl/lib";
	StringList f = dir;
	f << "/init.tcl";
	FILE* fd;
	if ((fd = fopen(f,"r")) != 0) {
		fclose(fd);
		StringList putenvArg;
		putenvArg << "TCL_LIBRARY=" << dir;
		putenv(hashstring(putenvArg));
		char * fname = f.newCopy();
		if (Tcl_EvalFile(interp, fname) == TCL_ERROR)
			fprintf(stderr, "ptcl: error in startup file");
		LOG_DEL; delete fname;
	}
	else fprintf(stderr, "ptcl: cannot locate Tcl startup file\n");
}
