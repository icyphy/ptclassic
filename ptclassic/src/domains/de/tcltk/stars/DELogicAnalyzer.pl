defstar {
    name {LogicAnalyzer}
    domain {DE}
    derivedFrom { TclScript }
    desc {
Generate a distinct plot for each porthole with the logic analyzer program.
It uses two parameters to plot each token: Time stamp (arrival time) and 
Token value.
    }
    version { $Id$}
    author { Eduardo N. Spring and Jose Luis Pino }
    copyright {
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
    explanation {
The star reads a tcl/tk source file. It contains three settable parameters as
follows: 
yRange - takes n pairs of numbers in form of (a,b) (c,d) ... where n is the 
number of portholes and a,b,.. are any real or integer numbers. If we enter 
only one pair, it replicates and applies it to all other portholes. There
should be at least one pair entered in order to get a plot.
PlotStyle - determins the plotting style. There are only three different
plotting styles: zero order hold, connect and dot. It takes n number of strings
in form of: zero dot connect ... in any desired order where n is the number of
portholes. If only one string is entered, it replicates and applies to all 
portholes. There should be at least one entry in order to get a plot.
PlotLabels - determins the plot window's title. It takes n number of strings
in form of: string1 string2 ... where n is the number of portholes. By entring
default (only once) it uses the portholes' names as the titles.
The star passes all these parameters to tcl/tk source file which makes all
required widgets. Then upon each fire by star, it calls goTcl procedure which
passes the token's value and arrival time to tcl/tk plot and plots the
corresponding point. Then upon user's request conects the points or draws
zero hold.
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
	tcl_file = "$PTOLEMY/src/domains/de/tcltk/stars/tkLogicAnalyzer.tcl";
	if (PlotLabels.size() < 1 || (strcmp(PlotLabels[0],"default") ==0)) {
	    PlotLabels.resize(input.numberPorts());
	    StringList names;
	    MPHIter next(input);
	    PortHole* port;
	    while ((port = next++) != 0) {
                GenericPort& p = port->far()->realPort();
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





