defstar {
	name {TkBreakPt}
	domain {SDF}
	derivedFrom { TclScript }
	desc {
This star is a conditional break point.  If it's condition is true,
it causes the run to pause.
	}
	version { $Id$ }
	author { Alan Kamas }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	hinclude { "ptk.h" }
	defstate {
	        name {condition}
	        type {string}
	        default { "$input(1) < 0" }
	        desc {Condition on which to pause the run}
	}
	setup {
	    if (output.numberPorts() > 0) {
		Error::abortRun(*this, "Outputs not supported");
		return;
	    }
	    // Set parameter values that are not user settable.
	    tcl_file =
		"$PTOLEMY/src/domains/sdf/tcltk/stars/tkBreakPt.tcl";

	    SDFTclScript::setup();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    output.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	}
	go {
	  // call the go method in the parent.
	  SDFTclScript::go();
	  
	  // make sure that the break point is processed NOW
          while (Tk_DoOneEvent(TK_DONT_WAIT|TK_ALL_EVENTS));
	}
}
