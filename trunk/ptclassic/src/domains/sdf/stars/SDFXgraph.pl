defstar {
	name { Xgraph }
	domain { SDF }
	desc { Generate a plot with the xgraph program. }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
The input signal is plotted using the \fIxgraph\fR program.
This program must be in your path, or this star will not work!
The \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
	}
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"X graph"}
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
		default {""}
		desc { Command line options for xgraph.}
	}
	hinclude { "Display.h" }
	protected {
		XGraph graph;
	}

	start {
		graph.initialize(this, 1, options, title, saveFile);
	}

	go {
		graph.addPoint(float(input%0));
	}
	wrapup {
		graph.terminate();
	}
}
