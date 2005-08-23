defstar {
	name {TkLogicAnalyzer}
	domain {DE}
	derivedFrom { TclScript }
	desc {
Display discrete dataflow over time, recording the entire history.
The values displayed are only 1 and 0.
	}
	version { @(#)DETkLogicAnalyzer.pl	1.7	07 Oct 1996 }
	author { Matthew T. Tavis }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
This star is derived from DETclScript, and does little more than
document the functionality of the script and specify the script file
to be loaded.
<p>
The <i>title</i> parameter specifies the title to be used for the plot.
Keep in mind that the star name associated with the plot will be attached
automatically to the given title.
<p>
The <i>signalLabels</i> parameter specifies labels for each plot.
It can take any of three forms.  If it is empty, or has just the single 
string "default", then it uses the name of the porthole connected to the
corresponding input.  Alternatively, it can specify one label for each 
input, with the labels separated by spaces.
<p>
The <i>TimeWindow</i> parameter specifies the amount of simulated time
that can be viewed at one time in the window.  In other words, it is
the time represented by the width of the plot window.
<p>
The <i>position</i> parameter is a string which specifies the size and
location of the window, in standard geometry flag format.  The format
is <i>width</i>x<i>height</i>[+-]<i>x-position</i>[+-]<i>y-position</i>.
The default leaves out the size specification and simply indicates the
initial position should be +0+0.
<p>
The <i>width_cm</i> parameter specifies the horizontal width of each plot 
in cm.
<p>
The <i>height_cm</i> parameter specifies the vertical height of each plot 
in cm.
<p>
	}
	location { DE Tcl/Tk library }
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
    setup {
	if (output.numberPorts() > 0) {
	    Error::abortRun(*this, "Outputs are not supported");
	    return;
	}
    }
    // The following must be "begin" not "setup" so that the number
    // of portholes has been fixed (in case there is a HOF star on the input).
    begin {
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
