defstar {
	name { XMgraph }
	domain { SDF }
	desc { Generate a multi-signal plot with the xgraph program. }
	version {$Id$}
	author { J. T. Buck and E. A. Lee }
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
.Id "graph, multi-signal"
.Ir "xgraph program"
	}
	seealso { Xgraph xgraph XYgraph Xhistogram }
	inmulti {
		name { input }
		type { float }
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
		desc { File name for saving plottable data. }
	}
	defstate {
		name {options}
		type {string}
		default {""}
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

	start {
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
