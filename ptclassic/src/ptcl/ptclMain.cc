/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.
		       
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
#include <stdio.h>

extern "C" int isatty(int);

void prompt (int tty) {
	if (tty) {
		printf("ptcl: ");
		fflush(stdout);
	}
}

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

extern "C" int displayGanttChart(const char*) {
	Error::error("Gantt chart display not implemented in ptcl");
	return 0;
}
