defstar {
	name {TkMeter}
	domain {SDF}
	derivedFrom { TclScript }
	desc {
Takes any number of inputs and dynamically
displays their value on a set of sliding scales
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	hinclude { "ptk.h" }
	defstate {
		name {label}
		type {string}
		default {"sliding scale display"}
		desc {A label to put on the display}
	}
	// Use string states below because they will be passed
	// to tcl, where the validity will be checked.
	defstate {
		name {low}
		type {string}
		default { "-1.0" }
		desc {Low end of the scale}
	}
	defstate {
		name {high}
		type {string}
		default { "1.0" }
		desc {High end of the scale}
	}
	defstate {
		name {put_in_control_panel}
		type {int}
		default { "YES" }
		desc {Specify to put bars in the control panel (vs. their own window)}
	}
	setup {
	    if (output.numberPorts() > 0) {
		Error::abortRun(*this, "Outputs not supported");
		return;
	    }
	    tcl_file =
		"$PTOLEMY/src/domains/sdf/tcltk/stars/tkMeter.tcl";
	    // For now, we pass the relevant parameter values to tcl
	    // by setting global variables.  We need a better way.
	    sprintf(buf,"%d",input.numberPorts());
	    if((Tcl_SetVar(ptkInterp, "label", label, TCL_GLOBAL_ONLY) == NULL)
	    || (Tcl_SetVar(ptkInterp, "low", low, TCL_GLOBAL_ONLY) == NULL)
	    || (Tcl_SetVar(ptkInterp, "high", high, TCL_GLOBAL_ONLY) == NULL)
	    || (Tcl_SetVar(ptkInterp, "numInputs", buf, TCL_GLOBAL_ONLY)
		== NULL)) {
                Error::abortRun(*this,"Failed to set parameter values for tcl");
                return;
            }
	    if(int(put_in_control_panel)) {
	       Tcl_SetVar(ptkInterp,"putInCntrPan","1",TCL_GLOBAL_ONLY);
	    } else {
	       Tcl_SetVar(ptkInterp,"putInCntrPan","0",TCL_GLOBAL_ONLY);
	    }
	    SDFTclScript::setup();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    output.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	    synchronous.clearAttributes(A_SETTABLE);
	}
}
