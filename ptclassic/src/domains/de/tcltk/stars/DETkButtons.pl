defstar {
	name {TkButtons}
	domain {DE}
	derivedFrom { TclScript }
	desc {
Output the specified value when buttons are pushed.
If "allow_simultaneous_events" is yes, then output events are produced only when
the button labeled "PUSH TO PRODUCE EVENTS" is pushed.
Note that the time stamps of the outputs are rather arbitrary.
The current time of the scheduler, whatever that happens to be
when the buttons are pushed, is used.
	}
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE tcltk library }
	hinclude { "ptk.h" }
	defstate {
	        name {label}
	        type {string}
	        default { "Buttons you can push:" }
	        desc {Title for the set of buttons}
	}
	defstate {
		name {identifiers}
		type{stringarray}
		default {"BUTTON1"}
		desc { A list of strings that identify each button }
	}
	defstate {
	        name {value}
		type {float}
		default {0.0}
		desc { The value produced when a button is pushed }
	}
	defstate {
	        name {allow_simultaneous_events}
		type {int}
		default {"NO"}
		desc { If YES, allow buttons to produce simultaneous events }
	}
	defstate {
		name {put_in_control_panel}
		type {int}
		default { "YES" }
		desc {Specify to put slider in the control panel (vs. its own window)}
	}
	setup {
	    if (input.numberPorts() > 0) {
		Error::abortRun(*this, "Inputs not supported");
		return;
	    }
	    if (output.numberPorts() > identifiers.size()) {
		Error::abortRun(*this, "Need as many identifiers as outputs");
		return;
	    }
	}
	begin {
	    // Set parameter values that are not user settable.
	    tcl_file =
		"$PTOLEMY/src/domains/de/tcltk/stars/tkButtons.tcl";

	    DETclScript::begin();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    input.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	}
}
