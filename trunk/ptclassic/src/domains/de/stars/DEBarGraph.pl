defstar {
	name { BarGraph }
	domain { DE }
	desc {
Generate a plot with the pxgraph program that uses
a zero-order hold to interpolate between event values.
Two points are plotted for each event, one when the event
first occurs, and the second when the event is supplanted
by a new event.  A horizontal line then connects the two points.
If "draw_line_to_base" is YES then a vertical line to the base
of the bar graph is also drawn for each event occurrence.
	}
	version { $Id$ }
	author { Brian Evans and Edward Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	seealso {XMgraph pxgraph Xhistogram }
	inmulti {
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
		desc { File to save pxgraph input.}
	}
	defstate {
		name { draw_line_to_base }
		type { int }
		default { "NO" }
		desc { Specifies vertical line to base. }
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
		desc { Command line options for a bar graph in pxgraph.}
	}
	hinclude { "Display.h" }
	protected {
		XGraph graph;
		float *lastDataValues;
		int *firstPointFlags;
		int numports;
	}
	constructor {
		lastDataValues = 0;
		firstPointFlags = 0;
	}
	setup {
		LOG_DEL; delete [] lastDataValues;
		LOG_DEL; delete [] firstPointFlags;
		numports = input.numberPorts();
		LOG_NEW; lastDataValues = new float[ numports + 1 ];
		LOG_NEW; firstPointFlags = new int[ numports + 1];
		for ( int port = 1; port <= numports; port++ ) {
	 	  lastDataValues[port] = base;
		  firstPointFlags[port] = TRUE;
		}
		graph.initialize(this,
				 input.numberPorts(),
				 (const char*) options,
				 (const char*) title,
				 (const char*) saveFile);
	}
	go {
		InDEMPHIter nextp(input);
		for ( int port = 1; port <= numports; port++ ) {
		  InDEPort *iportp = nextp++;
		  if ( iportp->dataNew ) {
		    float data = float(iportp->get());
		    if ( firstPointFlags[port] )
		      firstPointFlags[port] = FALSE;
		    else
		      graph.addPoint(port, arrivalTime, lastDataValues[port]);
		    if (int(draw_line_to_base))
		      graph.addPoint(port, arrivalTime, base);
		    graph.addPoint(port, arrivalTime, data);
		    lastDataValues[port] = data;
		  }
		  iportp->dataNew = FALSE;
		}
	}
	wrapup {
		for ( int port = 1; port <= numports; port++ )
		  graph.addPoint(port, arrivalTime, lastDataValues[port]);
		graph.terminate();
	}
	destructor {
		LOG_DEL; delete [] lastDataValues;
		LOG_DEL; delete [] firstPointFlags;
	}
}
