defstar {
	name { XMgraph }
	domain { SDF }
	desc {
Generate a multi-signal plot with the pxgraph program.
This program has many options; see the User's manual
of The Almagest.
	}
	version {@(#)SDFXMgraph.pl	2.18 10/06/96}
	author { J. T. Buck and E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
The input signal is plotted using the <i>pxgraph</i> program.
This program must be in your path, or this star will not work!
The <i>title</i> parameter specifies a title for the plot.
The <i>saveFile</i> parameter optionally specifies a file for
storing the data in a syntax acceptable to pxgraph.
A null string prevents any such storage.
The <i>options</i> string is passed directly to the pxgraph program
as command-line options.  See the manual section describing pxgraph
for a complete explanation of the options.
<a name="graph, X window, multi-signal"></a>
<a name="pxgraph program"></a>
<a name="xgraph program"></a>
	}
	seealso { Xgraph $PTOLEMY/src/pxgraph/pxgraph.htm XYgraph Xhistogram }
	inmulti {
		name { input }
		type { float }
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
		desc { File name for saving plottable data. }
	}
	defstate {
		name {options}
		type {string}
		default {"-bb -tk =800x400"}
		desc {Command line options for the xgraph program.}
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
	protected {
		XGraph graph;
		double index;
	}
	hinclude { "Display.h" }

	setup {
		graph.initialize(this, input.numberPorts(),
			options, title, saveFile, ignore);
		index = xInit;
	}

	go {
		MPHIter nexti(input);
		for (int i = 1; i <= input.numberPorts(); i++) {
			graph.addPoint(i, index, float((*nexti++)%0));
		}
		index += double(xUnits);
	}
	wrapup {
		graph.terminate();
	}
}
