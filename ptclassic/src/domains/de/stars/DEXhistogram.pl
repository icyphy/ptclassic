defstar {
	name { Xhistogram }
	domain { DE }
	desc {
Generate a histogram with the pxgraph program.  The parameter "binWidth" determines the
width of a bin in the histogram.  The number of bins will depend on the range of
values in the events that arrive. The time of arrival of events is ignored.
This star is identical to the SDF Xhistogram, but is used often enough in the
DE domain that it is provided here for convenience.
	}
	version { @(#)DEXhistogram.pl	2.12	11/26/97}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
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
		name {showPercent}
		type {int}
		default {FALSE}
		desc {Show Y-axis as percentages rather than counts.}
	}
	defstate {
		name {options}
		type {string}
		default {"-bb =800x400"}
		desc { Command line options for pxgraph.}
	}
	protected {
		// The XHistogram class does all the work.
		XHistogram his;
	}
	hinclude { "Histogram.h" }
	setup {
		his.initialize(this,binWidth,options,title,saveFile);
		his.setPercentageDisplay(int(showPercent));
	}
	go {
		his.addPoint(double(input.get()));
	}
	wrapup {
		his.terminate();
	}
}
