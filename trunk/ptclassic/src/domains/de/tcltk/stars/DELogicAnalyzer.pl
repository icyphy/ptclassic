defstar {
    name {LogicAnalyzer}
    domain {DE}
    derivedFrom { TclScript }
    desc {
Generate a plot with the logic analyzer program that uses two parameter
for each token: Time stamp and Token value.
    }
    version { $Id$}
    author { Eduardo N. Spring }
    copyright {
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
    location { DE tcltk library }
    defstate {
	name { WindowName }
	type {string}
	default {"Logic Analyzer Display"}
	desc {A label to put on the display}
    }
    defstate {
	name { yRange }
	type { ComplexArray }
	default { "(-1,1)" }
	desc {range of particle's value}
    }	
    defstate {
	name { yRangeList}
	type {String}
	desc { A list of the yRange values }
        attributes { A_NONSETTABLE|A_CONSTANT }
    }
    defstate {
	name {PlotStyle}
	type {StringArray}
	default { "zero" }
	desc {the style to draw particles, zero = zero-order-hole, connect = connect-the-particles, dot = display-only-dots }
    }
    defstate {
	name {PlotLabels}
	type {StringArray}
	default { "default" }
	desc {the labels for each plot, if default, use port names }
    }

    setup {
        int i; 
	if (output.numberPorts() > 0) {
	    Error::abortRun(*this, "Outputs not supported");
	    return;
	}
	tcl_file = "~spring/proj1/tkLogicAnalyzer.tcl";
	if (PlotLabels.size() < 1 || (strcmp(PlotLabels[0],"default") ==0)) {
	    PlotLabels.resize(input.numberPorts());
	    StringList names;
	    MPHIter next(input);
	    PortHole* port;
	    while ((port = next++) != 0) {
                GenericPort& p = port->far().realPort();
		names << p.fullName() << " ";
            }
	    PlotLabels.setCurrentValue(names);
	}
	else if (PlotLabels.size() != input.numberPorts()) {
	    Error::abortRun(*this,"Number of PlotLabels not equal to the number of input ports");
	    return;
	}
        for (i = 0 ; i < PlotStyle.size() ; i++ ) {
            if (strcmp(PlotStyle[i],"zero") &&
                strcmp(PlotStyle[i],"dot")  &&
                strcmp(PlotStyle[i],"connect")) {
                Error::abortRun(*this,"PlotStyle must be one of zero, dot, or connect");
                return;
            }
        }
	if (PlotStyle.size() != input.numberPorts()) {
	    if (PlotStyle.size() != 1) {
		Error::abortRun(*this,"Number of PlotStyles not equal to the number of input ports");
		return;
	    }
	    StringList style = PlotStyle[0];
	    style << "[" << input.numberPorts() << "]";
	    PlotStyle.setCurrentValue(style);
	}
	if (yRange.size() != input.numberPorts()) {
	    if (yRange.size() != 1) {
		Error::abortRun(*this,"Number of PlotStyles not equal to the number of input ports");
		return;
	    }
	    StringList style;
 	    style << yRange.currentValue();
	    style << "[" << input.numberPorts() << "]";
	    yRange.setCurrentValue(style);
	}
        StringList range;
        for (i = 0; i < yRange.size() ; i++)
            range << yRange[i].real() << " " << yRange[i].imag() << " ";
        yRangeList.setCurrentValue(range);
	DETclScript::setup();
    }
    constructor {
	// Hide irrelevant outputs and states.
	output.setAttributes(P_HIDDEN);
	tcl_file.clearAttributes(A_SETTABLE);
    }
}





