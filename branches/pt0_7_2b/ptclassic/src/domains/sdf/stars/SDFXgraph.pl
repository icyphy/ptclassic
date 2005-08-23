defstar {
	name { Xgraph }
	domain { SDF }
	desc {
Generate a plot of a single signal with the xgraph program.
	}
	version {@(#)SDFXgraph.pl	1.19 10/06/96}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
The input signal is plotted using the <i>pxgraph</i> program which
is provided with the Ptolemy distribution.
Note that pxgraph is a specially modified version of xgraph.
The <i>title</i> parameter specifies a title for the plot.
The <i>saveFile</i> parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The <i>options</i> string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
<a name="xgraph program"></a>
<a name="graph, X window"></a>
	}
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"Ptolemy Xgraph"}
		desc { Title for the plot. }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {File to save the input to the xgraph program.}
	}
	defstate {
		name {options}
		type {string}
		default {"-bb -tk =800x400"}
		desc { Command line options for xgraph.}
	}
	defstate {
		name {ignore}
		type {int}
		default { 0 }
		desc { Number of initial values to ignore.}
	}
	defstate {
		name {xUnits}
		type {float}
		default { 1.0 }
		desc { For labeling, horizontal increment between samples. }
	}
	defstate {
		name {xInit}
		type {float}
		default { 0.0 }
		desc { For labeling, horizontal value of the first sample. }
	}
	hinclude { "Display.h" }
	protected {
		XGraph graph;
		double index;
	}

	setup {
		graph.initialize(this, 1, options, title, saveFile, ignore);
		index = xInit;
	}

	go {
		graph.addPoint(1,index,float(input%0));
		index += double(xUnits);
	}
	wrapup {
		graph.terminate();
	}
}
