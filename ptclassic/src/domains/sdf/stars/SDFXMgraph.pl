ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/19/90
 Revised (from Xgraph) by E. A. Lee to accept multiple inputs, 9/18/90.
 Revised 10/3/90 to work under the preprocessor.
 Revised 10/20/90 to use XGraph class

 Draws a graph with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!

 This version is for the SDF domain.
**************************************************************************/

}

defstar {
	name { XMgraph }
	domain { SDF }
	desc { "Generate a multi-signal plot with the xgraph program." }
	inmulti {
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
	protected {
		XGraph graph;
		int index;
	}
	hinclude { "Display.h" }

	start {
		graph.initialize(input.numberPorts(),
			(const char*) options,
			(const char*) title,
			(const char*) saveFile);
		index = 0;
	}

	go {
		MPHIter nexti(input);
		for (int i = 1; i <= input.numberPorts(); i++) {
			graph.addPoint(i, float(index), float((*nexti++)%0));
		}
		index++;
	}
	wrapup {
		graph.terminate();
	}
}
