defstar {
	name {TkLogicAnalyzer}
	domain {DE}
	derivedFrom { TclScript }
	desc {
Display discrete dataflow over time, recording the entire history.
The values displayed are only 1 and 0.
	}
	version { $Id$ }
	author { Matthew T. Tavis }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	explanation {
This star is derived from DETclScript, and does little more than
document the functionality of the script and specify the script file
to be loaded.
.pp
# FIXME: variable descriptions go here !!
	}
	location { DE tcltk library }
	defstate {
		name { title }
		type { string }
		default { "Ptolemy Logic Analyzer" }
		desc { A title to put on the display }
	}
	defstate {
		name { signalLabels }
		type { StringArray }
		default { "default" }
		desc { Label(s) for each plot.  If 'default', use port names }
	}
	defstate {
		name { TimeWindow }
		type { float }
		default { "20" }
		desc {The time represented by one window width}
 	}
	defstate {
		name { position }
		type{string}
		default { "+0+0" }
		desc { Location of the window in standard X format }
	}
 	defstate {
		name { width_cm }
		type{string}
		default { "20" }
		desc { Width of each plot in centimeters }
  	}
	defstate {
		name { height_cm }
		type{string}
		default { "1" }
		desc { Height of each plot in centimeters }
	}
    // The following must be "begin" not "setup" so that the number
    // of portholes has been fixed (in case there is a HOF star on the input).
    begin {
	if (output.numberPorts() > 0) {
	    Error::abortRun(*this, "Outputs are not supported");
	    return;
	}
	tcl_file = "$PTOLEMY/src/domains/de/tcltk/stars/tkLogicAnalyzer.itcl";

	if (signalLabels.size() < 1 ||
	    (strcmp(signalLabels[0],"default") ==0)) {
	    signalLabels.resize(input.numberPorts());
	    StringList names;
	    MPHIter next(input);
	    PortHole* port;
	    while ((port = next++) != 0) {
                GenericPort& p = port->far()->realPort();
		names << p.fullName() << " ";
            }
	    signalLabels.setCurrentValue(names);
	} else if (signalLabels.size() != input.numberPorts()) {
	    Error::abortRun(*this,"Number of signalLabels is not equal to the number of input ports");
	    return;
	}
	DETclScript::begin();
    }
    constructor {
	// Hide irrelevant outputs and states.
	output.setAttributes(P_HIDDEN);
	tcl_file.clearAttributes(A_SETTABLE);
    }
}