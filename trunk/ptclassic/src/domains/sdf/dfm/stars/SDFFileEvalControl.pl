defstar {
    name { FileEvalControl }
    domain { SDF }
    derivedFrom { FileEval }
    version { $Id$ }
    author { E. A. Lee }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { SDF dfm library }
    desc {
This is just like FileEval, except that it supports any number of
integer-valued inputs and outputs in addition to the anytype
inputs and File outputs.  The integers can be referenced in
the "command" parameter using the syntax "intin#1", "intin#2", etc.,
or "intout#1", "intout#2", etc.  If the command does not set
the integer output values (using the Tcl "set" command), then
the value of the outputs will be 0.
    }
    inmulti {
	name {intin}
	type {int}
	desc {Integer inputs used for control.}
    }
    outmulti {
	name {intout}
	type {int}
	desc {Integer outputs used for control.}
    }
    begin {
	if (sourceIfNeeded("fileevalcontrol_editcommand",
	    "$PTOLEMY/src/domains/sdf/dfm/stars/fileevalcontrolsupport.tcl")
	    == 0) {
	        return;
	}

        SDFFileEval::begin();
    }
    code {
extern "C" {
#include "ptk.h"
}
    }
    // Redefine the base class method to first replace all references
    // to the control inputs and outputs (intin and intout).
    method {
	name { evalneeded }
	access { protected }
	arglist { "()" }
	type { int }
	code {
	    int result = SDFFileEval::evalneeded();

	    // Grab the control inputs.
	    MPHIter nexti(intin);
	    PortHole *p;
	    InfString controlin;
	    while ((p = nexti++) != 0) {
		// return a quoted string for Tcl consumption
		controlin += "{";
		controlin += ((*p)%0).print();
		controlin += "} ";
	    }

	    InfString cmd;
	    if (intin.numberPorts() > 0) {
		cmd << "set " << tcl.id() << "(intin) ";
		cmd << controlin << "\n";
	    }
	    cmd << "set " << tcl.id() << 
	    	"(numintout) " << intout.numberPorts() << "\n";
	    cmd << "fileevalcontrol_editcommand ";
	    cmd << tcl.id();
	    if (Tcl_GlobalEval(ptkInterp, (char*)cmd) != TCL_OK) {
		Error::abortRun(*this, ptkInterp->result);
		return 0;
	    }
	    return result;
	}
    }
    go {
	SDFFileEval::go();

	// Produce integer outputs.
	MPHIter nexto(intout);
	PortHole *p;
	int num = 1;
	while ((p = nexto++) != 0) {
	    InfString cmd = "fileevalcontrol_getintout ";
	    cmd << tcl.id() << " " << num;
	    if (Tcl_GlobalEval(ptkInterp, cmd) != TCL_OK) {
		Error::abortRun(*this, ptkInterp->result);
		return;
	    }
	    int value;
	    if (Tcl_GetInt(ptkInterp, ptkInterp->result, &value) != TCL_OK) {
		cmd = "Integer output number ";
		cmd << num << " set to non-integer.\n" << ptkInterp->result;
		Error::abortRun(*this, (const char*) cmd);
		return;
	    }
	    (*p)%0 << value;
	    num++;
	}
    }
}
