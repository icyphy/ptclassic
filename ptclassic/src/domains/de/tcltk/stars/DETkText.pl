defstar {
    name {TkText}
    domain {DE}
    derivedFrom { TkShowValues }
    desc {
Display the values of the inputs in a separate window,
keeping a specified number of past values in view.
The print method of the input particles is used, so any data
type can be handled.
    }
    version { $Id$ }
    author { Brian Evans and Edward Lee }
    copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { DE tcltk library }
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

        DETclScript::setup();
    }
    constructor {
	put_in_control_panel.clearAttributes(A_SETTABLE);
	label = "Inputs to the TkText star";
    }
}
