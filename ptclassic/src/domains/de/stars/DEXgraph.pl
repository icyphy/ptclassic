ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck and E. A. Lee
 Date of creation: 3/19/90
 Revised 10/2/90 to work under the preprocessor.
 Revised 10/21/90 to use XGraph class

 Draws a graph with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!

 This version is for the DE domain.
**************************************************************************/
}

defstar {
	name { Xgraph }
	domain { DE }
	desc { "Generate a plot with the xgraph program." }
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"X graph"}
		desc {"graph title"}
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {"file to save xgraph input"}
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc {"command line options for xgraph"}
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
