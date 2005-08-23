defstar {
    name {TkShowValues}
    domain {MDSDF}
    derivedFrom { TclScript }
    desc {
Displays the values of the inputs.
    }
    version { @(#)MDSDFTkShowValues.pl	1.4 01 Oct 1996 }
    author { E. A. Lee, modified for MDSDF by Mike Chen }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { MDSDF tcltk library }
	htmldoc {
This star uses the "print()" method of the input particles to create
the display.
The width of the display window is appropriate for integer, float, and
complex particles.  Particles that require more space to display may
create problems.
    }
    hinclude { "ptk.h" }
    defstate {
        name {label}
        type {string}
        default {"Inputs to the TkShowValues star:"}
        desc {A label to put on the display}
    }
    defstate {
        name {put_in_control_panel}
        type {int}
        default { "YES" }
        desc {Specify to put bars in the control panel (vs. their own window)}
    }
    defstate {
        name {wait_between_outputs}
        type {int}
        default { "NO" }
        desc {Specify whether to wait for user input between output values}
    }
    setup {
        if (output.numberPorts() > 0) {
            Error::abortRun(*this, "Outputs not supported");
            return;
        }
    }
    begin {
        tcl_file = "$PTOLEMY/src/domains/sdf/tcltk/stars/tkShowValues.tcl";

        MDSDFTclScript::begin();
    }
    constructor {
        output.setAttributes(P_HIDDEN);
        tcl_file.clearAttributes(A_SETTABLE);
    }
}
