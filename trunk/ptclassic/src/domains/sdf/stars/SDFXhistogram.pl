defstar {
	name { Xhistogram }
	domain { SDF }
	desc {
Generate a histogram with the xgraph program.  'binWidth' determines the
bin width.  'options' passes extra options to xgraph.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
Creates a histogram with the xgraph program.  It is assumed that "xgraph"
is on your path, or this will not work!
The \fIbinWidth\fR parameter specifies how wide histogram
bin will be.  The number of bins is determined automatically from
the input data.
.pp
By default, the xgraph program gets the options ``-bar -nl -brw \fIhalfw\fR''
where \fIhalfw\fR is half the bin width.
.Ir "xgraph program"
.Id "histogram, X window"
	}
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"Xhistogram"}
		desc { Title for the plot. }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {File to save input.}
	}
	defstate {
		name {binWidth}
		type {float}
		default {"1.0"}
		desc {Width of bins for histogram.}
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc {Extra Command line options for xgraph.}
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
		his.addPoint(float(input%0));
	}
	wrapup {
		his.terminate();
	}
}
