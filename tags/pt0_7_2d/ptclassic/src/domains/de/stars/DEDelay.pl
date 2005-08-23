defstar {
	name {Delay}
	domain {DE}
	version { @(#)DEDelay.pl	2.5	3/2/95}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Each input event is sent to the output with its time stamp
incremented by an amount given by the "delay" parameter.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	defstate {
		name {delay}
		type {float}
		default {"1.0"}
		desc { Amount of time delay. }
	}
	constructor {
		delayType = TRUE;
	}
	go {
	   completionTime = arrivalTime + double(delay);
	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
