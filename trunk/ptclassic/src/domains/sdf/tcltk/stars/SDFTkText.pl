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

        SDFTclScript::setup();
    }
    constructor {
	put_in_control_panel.clearAttributes(A_SETTABLE);
	label = "Inputs to the TkText star";
    }
}
