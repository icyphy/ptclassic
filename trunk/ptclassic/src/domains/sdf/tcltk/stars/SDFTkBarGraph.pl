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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
	defstate {
	        name {updateSize}
	        type {int}
	        default {1}
	        desc { To speed up the display, this many bars are updated at once }
	}
	protected {
		BarGraph bar;
		int count,batchCount;
	}
	setup {
	    if(double(top) <= double(bottom)) {
		Error::abortRun(*this, "invalid range for the scale");
	    }
	    input.setSDFParams(1,(int)updateSize-1);
	    // Need to make non-const copies of "position" and "label"
	    InfString posCopy((const char*)position);
	    InfString labCopy((const char*)label);
	    bar.setup(this,
		(char*) labCopy,
		input.numberPorts(),
		(int) number_of_bars,
		(double) top,
		(double) bottom,
		(char*) posCopy,
		(double) bar_graph_width,
		(double) bar_graph_height);
	    count = batchCount = 0;
	}
	go {
	  if (++batchCount == (int)updateSize) {
	    batchCount = 0;
	    MPHIter nexti(input);
	    PortHole *p;
	    // Iterate over the number of points to be plotted at once.
	    for (int j = (int)updateSize-1; j >= 0; j--) {
	      int i=0;
	      while ((p = nexti++) != 0) {
		if(bar.update(i++,count,(*p)%j) == FALSE) {
		  Error::abortRun(*this, "failed to update bar chart");
		  return;
		}
	      }
	      nexti.reset();
	      count++;
	      if(count >= int(number_of_bars)) count=0;
	    }
	  }
	}
}
