defstar {
	name { XMgraph }
	domain { DE }
	desc { Generate a plot with the xgraph program. }
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Draws a graph with the xgraph function.  It is assumed that "xgraph"
is on your path, or this will not work!!!
Any number of distinct DE signals can be plotted together, up to a
maximum number of inputs determined in the definition of the XGraph class.
	}
	seealso {xgraph Xgraph Xhistogram}
	inmulti {
		name { input }
		type { float }
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
		desc { File to save the output to the xgraph program. }
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc { Command line options for the xgraph program.}
	}
	hinclude { "Display.h" }
	protected {
		XGraph graph;
	}
	start {
		graph.initialize(this, input.numberPorts(),
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
