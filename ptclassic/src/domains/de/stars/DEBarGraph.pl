defstar {
	name { BarGraph }
	domain { DE }
	desc { Generate bar chart plots with the xgraph program. }
	version { $Id$ }
	author { Brian Evans and Edward Lee }
	copyright {
Copyright (c) 1993 and 1994 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Draws one or more bar graphs with the xgraph function.
It is assumed that "xgraph" is on your path, or this will not work!!!
	}
	seealso {Xgraph XMgraph xgraph Xhistogram }
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
	protected {
		XGraph graph;
		float *lastDataValues;
		int *firstPointFlags;
		int numports;
	}
	setup {
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
		LOG_DEL; delete [] lastDataValues;
		LOG_DEL; delete [] firstPointFlags;
	}
}
