defstar {
	name {Delay}
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
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
		type {anytype}
	}
	defstate {
		name {delay}
		type {float}
		default {"1.0"}
		desc { Amount of time delay. }
	}
	constructor {
		input.inheritTypeFrom(output);
		delayType = TRUE;
	}
	go {
	   completionTime = arrivalTime + double(delay);
	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
