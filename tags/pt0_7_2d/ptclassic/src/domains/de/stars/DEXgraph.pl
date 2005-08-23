defstar {
	name { Xgraph }
	domain { DE }
	desc { Generate a plot with the xgraph program. }
	version { @(#)DEXgraph.pl	1.13	10/06/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	htmldoc {
Draws a graph with the pxgraph program.  It is assumed that "pxgraph"
is on your path, or this will not work!!!
	}
	seealso {XMgraph $PTOLEMY/src/pxgraph/pxgraph.htm Xhistogram }
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
		default {"-bb -tk -P =800x400"}
		desc { Command line options for xgraph.}
	}
	hinclude { "Display.h" }
	protected {
		XGraph graph;
	}
	setup {
		graph.initialize(this, 1, (const char*) options,
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
