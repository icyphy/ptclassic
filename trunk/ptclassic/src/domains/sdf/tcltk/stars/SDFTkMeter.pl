defstar {
	name {TkMeter}
	domain {SDF}
	derivedFrom { TclScript }
	desc {
Dynamically display the value of any number of input signals on a set of bar meters.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
	}
	begin {
	    tcl_file =
		"$PTOLEMY/src/domains/sdf/tcltk/stars/tkMeter.tcl";
	    SDFTclScript::begin();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    output.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	}
}
