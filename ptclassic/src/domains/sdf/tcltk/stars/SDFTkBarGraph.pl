defstar {
	name {TkBarGraph}
	domain {SDF}
	desc {
Dynamically display the value of any number of input signals in bar-chart form.
The first 12 input signals will be assigned distinct colors. After that,
the colors are repeated. The colors can be controlled using X resources.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
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
	    // the cast on the string state label and position must
	    // be (const char *) because a (char *) cast has not been
	    // defined for string states but bar.setup wants (char *)
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
	    MPHIter nexti(input);
	    PortHole *p;
	    int i=0;
	    while ((p = nexti++) != 0) {
		if(bar.update(i++,count,(*p)%0) == FALSE) {
		    Error::abortRun(*this, "failed to update bar chart");
		    return;
		}
	    }
	    count++;
	    if(count >= int(number_of_bars)) count=0;
	}
}
