defstar {
    name {TkShowEvents}
    domain {DE}
    derivedFrom { TclScript }
    desc {
Display input event values together with the time stamp
at which they occur. The print method of the input
particles is used to show the value, so any data type
can be handled, although the space allocated on the
screen may need to be adjusted.
    }
    version { $Id$ }
    author { Edward Lee }
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
	default { "Events at TkShowEvents inputs:" }
	desc {Title for the set of event displays}
    }
    defstate {
        name {identifiers}
        type {stringarray}
        default {"EVENT"}
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
        tcl_file = "$PTOLEMY/src/domains/de/tcltk/stars/tkShowEvents.tcl";

        DETclScript::setup();
    }
    constructor {
        output.setAttributes(P_HIDDEN);
        tcl_file.clearAttributes(A_SETTABLE);
    }
}
