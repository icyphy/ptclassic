defstar {
	name {TkBarGraph}
	domain {DE}
	desc {
Takes any number of inputs and dynamically
displays their values in bar-chart form.
The time stamp is ignored, so the bar chart
only indicates the order of occurrence of events.
If there is more than one input port, then
bars for the two inputs will be plotted together
only if the events are simultaneous.
	}
	version { $Id$ }
	author { B. L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE tcltk library }
	hinclude { "ptk.h" }
	hinclude { "BarGraph.h" }
	inmulti {
		name {input}
		type {anytype}
		desc { Any number of inputs to plot }
	}
	defstate {
		name {label}
		type {string}
		default {"bar chart display"}
		desc {A label to put on the display}
	}
	defstate {
		name {top}
		type {float}
		default { "1.0" }
		desc {Low end of the scale}
	}
	defstate {
		name {bottom}
		type {float}
		default { "-1.0" }
		desc {High end of the scale}
	}
	defstate {
		name {number_of_bars}
		type {int}
		default { 16 }
		desc { Number of bars to display }
	}
	defstate {
		name {bar_graph_height}
		type {float}
		default { 5 }
		desc { Desired height of the bar graph in centimeters }
	}
	defstate {
		name {bar_graph_width}
		type {float}
		default { "10" }
		desc { Desired width of the bar graph in centimeters }
	}
	defstate {
		name {position}
		type {string}
		default { "+0+0" }
		desc { Width of the bar graph in centimeters }
	}
	protected {
		BarGraph bar;
		int count;
	}
	setup {
	    if(double(top) <= double(bottom)) {
		Error::abortRun(*this, "invalid range for the scale");
	    }
	}
	begin {
	    // Make non-const copies of label and position
	    InfString labCopy((const char*)label);
	    InfString posCopy((const char*)position);
	    bar.setup(this,
		(char*) labCopy,
		input.numberPorts(),
		(int) number_of_bars,
		(double) top,
		(double) bottom,
		(char*) posCopy,
		(double) bar_graph_width,
		(double) bar_graph_height);
	    count = 0;
	}
	go {
	    InDEMPHIter nextp(input);
	    InDEPort* p;
	    int i = 0;
	    while ((p = nextp++) != 0) {
		if ( p->dataNew ) {
		  if ( bar.update(i, count, p->get()) == FALSE ) {
		    Error::abortRun(*this, "failed to update bar chart");
		    return;
		  }
		} else {
		  if ( bar.update(i, count, 0.0) == FALSE ) {
		    Error::abortRun(*this, "failed to clear bar");
		    return;
		  }
		}
		i++;
	    }
	    count++;
	    if(count >= int(number_of_bars)) count=0;
	}
}
