defstar {
    name {TkStripChart}
    domain {DE}
    derivedFrom { TclScript }
    desc {
Display events in time, recording the entire history.
The supported styles are 'hold' for zero-order hold,
'connect' for connected dots, and 'dot' for unconnected dots.
An interactive help window describes other options for the plot.
    }
    version { $Id$ }
    author { Eduardo N. Spring, Jose Luis Pino, and Edward A. Lee }
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
The \fIyRange\fR parameter specifies the vertical range of each plot.
Each range is specified as a pair of number in the form (low,high).
If only one range is specified, then it applied to all plots.  If more
than one range is specified, then one range should be specified for each 
input.  I.e., then number of ranges specified should equal the number of 
inputs.
.pp
The \fITimeWindow\fR parameter specifies the amount of simulated time
that can be viewed at one time in the window.  In other words, it is
the time represented by the width of the plot window.
.pp
The \fIPlotStyle\fR parameter determines the plotting style.
There are currently three styles: zero order hold,
connected lines, and unconnected dots.
These are specified by the strings "hold", "dot", or "connect".
The PlotStyle can be different for each input porthole.
.pp
The \fIPlotLabels\fR parameter specifies labels for each plot.
It can take any of three forms.  If it is empty, or has just the single 
string "default", then it uses the name of the porthole connected to the
corresponding input.  Alternatively, it can specify one label for each 
input, with the labels separated by spaces.
    }
    location { DE tcltk library }
    defstate {
	name { title }
	type { string }
	default { "Ptolemy Strip Chart" }
	desc { A title to put on the display }
    }
    defstate {
	name {signalLabels}
	type {StringArray}
	default { "default" }
	desc {Label(s) for each plot. If 'default', use port names. }
    }
    defstate {
	name { yRange }
	type { ComplexArray }
	default { "(-1,1)" }
	desc {The vertical range(s) of the plot}
    }
    defstate {
	name { TimeWindow }
	type { float }
	default { "25" }
	desc {The time represented by one window width}
    }
    // The following hidden state is used to store the yRange list in a
    // form that is easier to Tcl.
    defstate {
	name { yRangeList}
	type {String}
        attributes { A_NONSETTABLE|A_CONSTANT }
    }
    defstate {
	name {style}
	type {StringArray}
	default { "hold" }
	desc { Plot styles are hold, connect, or dot. }
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
	default { "25" }
	desc { Width of each plot in centimeters }
    }
    defstate {
	name { height_cm }
	type{string}
	default { "5" }
	desc { Height of each plot in centimeters }
    }
    // The following must be "begin" not "setup" so that the number
    // of portholes has been fixed (in case there is a HOF star on the input).
    begin {
        int i; 
	if (output.numberPorts() > 0) {
	    Error::abortRun(*this, "Outputs are not supported");
	    return;
	}
// FIXME: The following path is TEMPORARY for testing

//	tcl_file = "$PTOLEMY/src/domains/de/tcltk/stars/tkStripChart.tcl";
	tcl_file = "/users/eal/StripChart/tkStripChart.tcl";

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
        for (i = 0 ; i < style.size() ; i++ ) {
            if (strcmp(style[i],"hold") &&
                strcmp(style[i],"dot")  &&
                strcmp(style[i],"connect")) {
                Error::abortRun(*this,"style must be one of hold, dot, or connect");
                return;
            }
        }
	if (style.size() != input.numberPorts()) {
	    if (style.size() != 1) {
		Error::abortRun(*this,"Number of styles is not equal to the number of input ports");
		return;
	    }
	    StringList st = style[0];
	    st << "[" << input.numberPorts() << "]";
	    style.setCurrentValue(st);
	}
	if (yRange.size() != input.numberPorts()) {
	    if (yRange.size() != 1) {
		Error::abortRun(*this,"Number of yRange values is not equal to the number of input ports");
		return;
	    }
	    StringList st;
 	    st << yRange.currentValue();
	    st << "[" << input.numberPorts() << "]";
	    yRange.setCurrentValue(st);
	}
	// Parse the complex numbers into a form that is easier to Tcl.
        StringList range;
        for (i = 0; i < yRange.size() ; i++)
            range << yRange[i].real() << " " << yRange[i].imag() << " ";
        yRangeList.setCurrentValue(range);

	DETclScript::begin();
    }
    constructor {
	// Hide irrelevant outputs and states.
	output.setAttributes(P_HIDDEN);
	tcl_file.clearAttributes(A_SETTABLE);
    }
}





