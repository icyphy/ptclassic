defstar {
    name {TkRadioButton}
    domain {CGC}
    desc { Graphical one-of-many input source. }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    derivedFrom { TclScript }
    location { CGC Tcl/Tk library }
    explanation {
This is an asynchronous source star
with a particular graphical user interface.
The star always outputs one of a finite number of values: the output is
controlled by the user selecting one of several buttons.
Exactly one button in the group is on.
.LP
The \fIpairs\fR parameter defines a set of pairs.
Each pair should be enclosed in backslash-escaped curly braces.
Each pair consists of two words: a one word label and a value.
The value is not checked at compile time: it is literally passed to Tk.
    }
    state {
	name {identifier}
	type{string}
	default {"Select one of:"}
	desc {The string to identify the slider in the control panel.}
    }
    state {
	name {pairs}
	type{string}
	default {"\\{One 1\\} \\{Two 2\\}"}
	desc {Name and value pairs.}
    }
    state {
	name {initialChoice}
	type{string}
	desc {Name of button to start at.}
        default { Two }
    }    
    constructor {
        // Hide irrelevant outputs and states.
        input.setAttributes(P_HIDDEN);
        tcl_file.clearAttributes(A_SETTABLE);
        synchronous.clearAttributes(A_SETTABLE);
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
	CGCTclScript::setup();
    }        

    initCode {
        synchronous = FALSE;
        tcl_file = "$PTOLEMY/src/domains/cgc/tcltk/stars/tkRadioButton.tcl";
        CGCTclScript::initCode();
    }
}

