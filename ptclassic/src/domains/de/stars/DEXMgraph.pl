ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/10/90
 Modified: 10/21/90 to use XGraph class.

 Draws a graph with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!
 Any number of distinct DE signals can be plotted together, up to a
 maximum number of inputs determined in the definition of the XGraph class.
**************************************************************************/
}

defstar {
	name { XMgraph }
	domain { DE }
	desc { "Generate a plot with the xgraph program." }
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
	hinclude { "Display.h" }
	protected {
		XGraph graph;
	}
	start {
		graph.initialize(input.numberPorts(),
				(const char*) options,
				(const char*) title,
				(const char*) saveFile);
	}

	go {
	    InDEMPHIter nextp(input);
	    for (int i = 1; i <= input.numberPorts(); i++) {
		InDEPort* p = nextp++;
	    	if(p->dataNew) {
		    graph.addPoint(i, arrivalTime, float(p->get()));
		}
		p->dataNew = FALSE;
	    }
	}

	wrapup {
	    graph.terminate();
	}
}
