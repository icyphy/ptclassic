defcorona {
	name { XMGraph }
	domain { ACS }
	desc {
Generate a multi-signal plot with the pxgraph program.
This program has many options; see the User's manual
of The Almagest.
	}
	version {@(#)ACSXMGraph.pl	1.4 09/08/99}
	author { J. T. Buck and E. A. Lee }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
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
	defstate {
		name {index}
		type {float}
		default {0.0}
		attributes {A_NONSETTABLE|A_NONCONSTANT}
	}
	protected {
		XGraph graph;
	}
	hinclude { "Display.h" }
}
