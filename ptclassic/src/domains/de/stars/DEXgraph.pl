defstar {
	name { Xgraph }
	domain { DE }
	desc { Generate a plot with the xgraph program. }
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	explanation {
Draws a graph with the xgraph function.  It is assumed that "xgraph"
is on your path, or this will not work!!!
	}
	seealso {XMgraph xgraph Xhistogram }
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"X graph"}
		desc { Graph title.}
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc { File to save xgraph input.}
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
		graph.initialize(1, (const char*) options,
				    (const char*) title,
				    (const char*) saveFile);
	}

	go {
		float data = float(input.get());
		graph.addPoint(arrivalTime, data);
	}

	wrapup {
		graph.terminate();
	}
}
