defstar {
	name {Discard}
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
Discards input events that occur before the threshold time.
	}
	explanation {
This star discards input events that occur before the threshold time.
Events after the threshold time are passed to the output.
It is useful for removing transients and studying steady-state effects.
	}
	seealso { Gate }
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
		desc { All events before this time are discarded. }
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
