defstar {
    name {TkText}
    domain {SDF}
    derivedFrom { TkShowValues }
    desc {
Displays the values of the inputs in a separate window,
keeping a specified number of past values in view.
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
    explanation {
This star uses the "print()" method of the input particles to create
the display.
The default width of the display window is appropriate for integer, float, and
complex particles.
    }
    hinclude { "ptk.h" }
    seealso { TkShowValues }
    defstate {
        name {number_of_past_values}
        type {string}
        default { "100" }
        desc {Specify how many past values you would like saved}
    }
    setup {
        if (output.numberPorts() > 0) {
            Error::abortRun(*this, "Outputs not supported");
            return;
        }
        tcl_file = "$PTOLEMY/src/domains/sdf/tcltk/stars/tkText.tcl";

        // For now, we pass the relevant parameter values to tcl
        // by setting global variables.  We need a better way.
        sprintf(buf,"%d",input.numberPorts());
        if((Tcl_SetVar(ptkInterp,"numInputs",buf,TCL_GLOBAL_ONLY) == NULL)
        || (Tcl_SetVar(ptkInterp,"label",label,TCL_GLOBAL_ONLY) == NULL)) {
            Error::abortRun(*this,"Failed to set parameter values for tcl");
            return;
        }
        if(int(put_in_control_panel)) {
           Tcl_SetVar(ptkInterp,"putInCntrPan","1",TCL_GLOBAL_ONLY);
        } else {
           Tcl_SetVar(ptkInterp,"putInCntrPan","0",TCL_GLOBAL_ONLY);
        }
        if(int(wait_between_outputs)) {
           Tcl_SetVar(ptkInterp,"waitBetweenOutputs","1",TCL_GLOBAL_ONLY);
        } else {
           Tcl_SetVar(ptkInterp,"waitBetweenOutputs","0",TCL_GLOBAL_ONLY);
        }


        if(Tcl_SetVar(ptkInterp,"numberOfValues",number_of_past_values,
			TCL_GLOBAL_ONLY) == NULL) {
            Error::abortRun(*this,"Failed to set parameter values for tcl");
            return;
        }
        SDFTclScript::setup();
    }
    constructor {
	put_in_control_panel.clearAttributes(A_SETTABLE);
	label = "Inputs to the TkText star";
    }
}
