defstar {
	name {TkSlider}
	domain {SDF}
	derivedFrom { TclScript }
	desc { Output a value determined by a Tk scale slider. }
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
		name {low}
		type{float}
		default {"0.0"}
		desc {The low end of the scale.}
	}
	defstate {
		name {high}
		type{float}
		default {"1.0"}
		desc {The high end of the scale.}
	}
	defstate {
		name {value}
		type{float}
		default {"0.0"}
		desc {The starting value to send to the output.}
	}
	defstate {
		name {identifier}
		type{string}
		default {"Scale"}
		desc {The string to identify the slider in the control panel.}
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
	    if (output.numberPorts() != 1) {
		Error::abortRun(*this, "Only a single output is supported");
		return;
	    }
	    // Set parameter values that are not user settable.
	    tcl_file =
		"$PTOLEMY/src/domains/sdf/tcltk/stars/tkSlider.tcl";
	    synchronous = 0;
	    // For now, we pass the relevant parameter values to tcl
	    // by setting global variables.  We need a better way.
	    sprintf(buf,"%d",100*(double(value)/(double(high)-double(low))));
	    if((Tcl_SetVar(ptkInterp, "identifier", identifier,
			TCL_GLOBAL_ONLY) == NULL)
	    || (Tcl_SetVar(ptkInterp, "low", low.initValue(),
		TCL_GLOBAL_ONLY) == NULL)
	    || (Tcl_SetVar(ptkInterp, "high", high.initValue(),
		TCL_GLOBAL_ONLY) == NULL)
	    || (Tcl_SetVar(ptkInterp, "value", value.initValue(),
		TCL_GLOBAL_ONLY) == NULL)) {
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
	    input.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	    synchronous.clearAttributes(A_SETTABLE);
	}
}