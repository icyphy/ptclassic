defstar {
	name {TkMeter}
	domain {DE}
	derivedFrom { TclScript }
	desc {
Dynamically display the value of any number of input signals on a set of
bar meters.
	}
	version { $Id$  }
	author { Brian Evans and Edward Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE tcltk library }
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
	    DETclScript::setup();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    output.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	}
}