defstar {
	name {Delay}
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
Delays its input by an amount given by the delay parameter.
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
