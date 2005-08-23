defstar {
    name {TkCheckButton}
    domain {CGC}
    desc { A simple tk on/off input source. }
    version { @(#)CGCTkCheckButton.pl	1.3 06 Oct 1996 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    derivedFrom { TclScript }
    location { CGC Tcl/Tk library }
	htmldoc {
This is an asynchronous source star
with a tk graphical user interface.  The star outputs one of two
values as specified by the <i>onValue</i> and <i>offValue</i> parameters.
    }
    state {
	name {identifier}
	type{string}
	default {"Checkbutton"}
	desc {The string to identify the check button in the control panel.}
    }
    state {
	name {onValue}
	type{string}
	default {"1"}
	desc {The value of the output when the button is selected.}
    }
    state {
	name {offValue}
	type{string}
	default {"0"}
	desc {The value of the output when the button is not selected.}
    }
    state {
	name {value}
	type{string}
	desc { The initial value. }
        default { 0 }
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
        tcl_file = "$PTOLEMY/src/domains/cgc/tcltk/stars/tkCheckButton.tcl";
        CGCTclScript::initCode();
    }
}

