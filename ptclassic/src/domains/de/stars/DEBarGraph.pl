defstar {
	name { BarGraph }
	domain { DE }
	desc { Generate a plot with the xgraph program. }
	version { $Id$ }
	author { Brian Evans and Edward Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Draws a bar graph with the xgraph function.
It is assumed that "xgraph" is on your path, or this will not work!!!
	}
	seealso {Xgraph XMgraph xgraph Xhistogram }
	input {
		name { input }
		type { anytype }
	}
	defstate {
		name {title}
		type {string}
		default {"X Graph Bar Chart"}
		desc { Graph title.}
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc { File to save xgraph input.}
	}
	defstate {
		name {base}
		type {float}
		default {0.0}
		desc { Base of the bar graph. }
	}
	defstate {
		name {options}
		type {string}
		default {"-bb -lw 3 -zg black -x time -0 input"}
		desc { Command line options for a bar graph in xgraph.}
	}
	hinclude { "Display.h" }
	public {
		float lastDataValue;
		int firstPoint; 
	}
	protected {
		XGraph graph;
	}
	setup {
	 	lastDataValue = base;
		firstPoint = TRUE;
		graph.initialize(this, 1, (const char*) options,
				    (const char*) title,
				    (const char*) saveFile);
	}
	go {
		float data = float(input.get());
		if ( firstPoint )
		  firstPoint = FALSE;
		else
		  graph.addPoint(arrivalTime, lastDataValue);
		graph.addPoint(arrivalTime, base);
		graph.addPoint(arrivalTime, data);
		lastDataValue = data;
	}
	wrapup {
		graph.addPoint(arrivalTime, lastDataValue);
		graph.terminate();
	}
}
