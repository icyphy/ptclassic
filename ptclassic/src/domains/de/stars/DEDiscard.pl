ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/21/90

 This star discards input events that occur before the threshold time.
 Events after the threshold time are passed to the output.
 It is useful for removing transients and studying steady-state effects.

**************************************************************************/
}
defstar {
	name {Discard}
	domain {DE}
	desc {
	   "Discards input events that occur before the threshold time."
	}
	input {
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
	defstate {
		name {threshold}
		type {float}
		default {"0.0"}
		desc { "All events before this time are discarded" }
	}
	go {
	   completionTime = arrivalTime;
	   Particle& pp = input.get();
	   if (arrivalTime >= double(threshold)) {
           	output.put(completionTime) = pp;
	   }
	   input.dataNew = FALSE;
	}
}
