ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/29/90

 This star merges input streams, outputing events in temporal order

**************************************************************************/
}
defstar {
	name {Merge}
	domain {DE}
	desc {
	   "Merge input events, keeping temporal order"
	}
	inmulti {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {anytype}
	}
	constructor {
		input.inheritTypeFrom(output);
	}

	go {
	   completionTime = arrivalTime;
	   InDEMPHIter nextp(input);
	   InDEPort* iport;
	   while ((iport = nextp++) != 0) {
		if(iport->dataNew) {
		    Particle& pp = iport->get();
		    output.put(completionTime) = pp;
		    iport->dataNew = FALSE;
		}
	   }
	}
}
