defstar {
    name {TkShowBooleans}
    domain {SDF}
    derivedFrom { TclScript }
    desc {
Display input Booleans using color to highlight their value.
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
	default { "Booleans at TkShowBooleans inputs:" }
	desc {Title for the set of Boolean displays}
    }
    defstate {
        name {identifiers}
        type {stringarray}
        default {"BOOL"}
        desc {Individual identifiers to put on each event display}
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
	if (identifiers.size() < input.numberPorts() ) {
	  Error::abortRun(*this, "Need as many identifiers as inputs");
	  return;
	}
    }
    begin {
        tcl_file = "$PTOLEMY/src/domains/sdf/tcltk/stars/tkShowBooleans.tcl";

        SDFTclScript::begin();
    }
    constructor {
        output.setAttributes(P_HIDDEN);
        tcl_file.clearAttributes(A_SETTABLE);
    }
}
