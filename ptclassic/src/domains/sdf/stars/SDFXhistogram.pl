defstar {
	name { Xhistogram }
	domain { SDF }
	desc {
Generate a histogram with the xgraph program.  The parameter
"binWidth" determines the bin width.
	}
	version {@(#)SDFXhistogram.pl	2.16 11/26/97}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
Creates a histogram with the xgraph program.  It is assumed that "xgraph"
is on your path, or this will not work!
The <i>binWidth</i> parameter specifies how wide histogram
bin will be.  The number of bins is determined automatically from
the input data.
<p>
By default, the xgraph program gets the options ``-bar -nl -brw <i>halfw</i>''
where <i>halfw</i> is half the bin width.
<a name="xgraph program"></a>
<a name="histogram, X window"></a>
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
		name {showPercent}
		type {int}
		default {FALSE}
		desc {Show Y-axis as percentages rather than counts.}
	}
	defstate {
		name {options}
		type {string}
		default {"-bb =800x400"}
		desc {Extra Command line options for xgraph.}
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
		his.addPoint(double(input%0));
	}
	wrapup {
		his.terminate();
	}
}
