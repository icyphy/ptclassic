ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/19/90
 Revised 10/3/90 to work under the preprocessor.
 Revised 10/20/90 to use XGraph class

 Draws a graph with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!

 This version is for the SDF domain.
**************************************************************************/
}

defstar {
	name { Xgraph }
	domain { SDF }
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
		graph.addPoint(float(input%0));
	}
	wrapup {
		graph.terminate();
	}
}
