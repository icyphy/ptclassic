defstar {
	name {Merge}
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc { Merge input events, keeping temporal order.  }
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
		if(iport->dataNew)
		    output.put(completionTime) = iport->get();
	   }
	}
}
