defstar {
	name {Discard}
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
		type {=input}
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
	}
}
