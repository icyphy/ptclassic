defstar {
	name {TkButtons}
	domain {SDF}
	derivedFrom { TclScript }
	desc {
This star outputs the value 0.0 on all outputs unless the correspoding
button is pushed.  When the button is pused, the output takes the value
given by the parameter "value".  If "synchronous" is YES, then outputs
are produced only when some button is pushed.  I.e., the star waits for
a button to be pushed before its go method returns.
If "allow_simultaneous_events" is yes, then buttons pushed are registered
only when the button labeled "PUSH TO PRODUCE OUTPUTS" is pushed.
Note that if "synchronous" is NO, this star is nondeterminate.
	}
	version { $Id$ }
	author { Edward A. Lee }
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
		default {1.0}
		desc { The value produced when a button is pushed }
	}
	defstate {
	        name {allow_simultaneous_events}
		type {int}
		default {"NO"}
		desc { If YES, allow buttons to produce simultaneous output values }
	}
	defstate {
	        name {synchronous}
		type {int}
		default {"NO"}
		desc { If YES, wait until a button is pushed before returning from the go method }
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
		"$PTOLEMY/src/domains/sdf/tcltk/stars/tkButtons.tcl";

	    SDFTclScript::begin();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    input.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	}
	go {
	  // First, call the go method in the parent.
	  // This produces all the outputs with the current value of the
	  // output array.
	  SDFTclScript::go();

	  // Next, reset the output array so that the values are not reused
	  // next time the star fires.
	  for (int i=0; i < output.numberPorts(); i++) {
	    tcl.setOneOutput(i, 0.0);
	  }
	}
}
