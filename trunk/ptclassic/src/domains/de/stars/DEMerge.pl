defstar {
	name {Merge}
	domain {DE}
	version { $Id$}
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Merge input events, keeping temporal order.  Simultaneous events are
merged in the order of the port number on which they appear, with port #1
being processed first.
	}
	inmulti {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	go {
	   completionTime = arrivalTime;
	   InDEMPHIter nextp(input);
	   InDEPort* iport;
	   while ((iport = nextp++) != 0) {
		if (iport->dataNew)
		    output.put(completionTime) = iport->get();
	   }
	}
}