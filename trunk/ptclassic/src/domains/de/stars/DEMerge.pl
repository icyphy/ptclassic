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
	   for(int i=input.numberPorts(); i>0; i--) {
		InDEPort& p = (InDEPort&) input();
		if(p.dataNew) {
		    Particle& pp = p.get();
		    output.put(completionTime) = pp;
		    p.dataNew = FALSE;
		}
	   }
	}
}
