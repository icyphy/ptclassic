defstar {
	name { XMgraph }
	domain { DE }
	desc {
Generate a plot with the pxgraph program with one point per event.
Any number of event sequences can be plotted simultaneously, up
to the limit determined by the XGraph class.
By default, a straight line is drawn between each pair of events.
	}
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	seealso {pxgraph Xhistogram}
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
		desc { File to save the output to the pxgraph program. }
	}
	defstate {
		name {options}
		type {string}
		default {"-bb -tk -P =800x400"}
		desc { Command line options for the pxgraph program.}
	}
	hinclude { "Display.h" }
	protected {
		XGraph graph;
	}
	setup {
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
