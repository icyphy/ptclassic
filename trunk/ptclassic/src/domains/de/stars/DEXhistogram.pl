defstar {
	name { Xhistogram }
	domain { DE }
	desc {
Generate a histogram with the xgraph program.  'binWidth' determines the
bin width.  'options' passes extra options to xgraph.
	}
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Creates a histogram with the xgraph function.  It is assumed that "xgraph"
is on your path, or this will not work!!!
The time of arrival is ignored.  This star is identical to the SDF
Xhistogram, but is used often enough in the DE domain that it is provided
here for convenience.
	}
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"Xhistogram"}
		desc { Graph title.}
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc { File to save Xhistogram input.}
	}
	defstate {
		name {binWidth}
		type {float}
		default {"1.0"}
		desc { Width of bins for histogram.}
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc { Command line options for xgraph.}
	}
	protected {
		// The XHistogram class does all the work.
		XHistogram his;
	}
	hinclude { "Histogram.h" }
	start {
		his.initialize(this,binWidth,options,title,saveFile);
	}
	go {
		float data = float(input%0);
		his.addPoint(data);
	}
	wrapup {
		his.terminate();
	}
}
