defstar {
	name {TkBarGraph}
	domain {DE}
	desc {
Takes any number of inputs and dynamically
displays their values in bar-chart form.
	}
	version { $Id$ }
	author { B. L. Evans }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
	    if(float(top) <= float(bottom)) {
		Error::abortRun(*this, "invalid range for the scale");
	    }
	    bar.setup(this,
		(const char *) label,
		input.numberPorts(),
		(int) number_of_bars,
		(float) top,
		(float) bottom,
		(const char *) position,
		(float) bar_graph_width,
		(float) bar_graph_height);
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
		}
		i++;
	    }
	    count++;
	    if(count >= int(number_of_bars)) count=0;
	}
}
