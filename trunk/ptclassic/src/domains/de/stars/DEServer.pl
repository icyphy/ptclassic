defstar {
	name {Server}
	domain {DE}
	desc {
This star emulates a server.
If input events arrive when it is not busy,
it delays them by the service time (a constant parameter).
If they arrive when it is busy, it delays them by more.
It must become free, and then serve them.
	}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {anytype}
	}
	defstate {
		name {serviceTime}
		type {float}
		default {"1.0"}
		desc { Service time. }
	}
	constructor {
		input.inheritTypeFrom(output);
		delayType = TRUE;
	}
	go {
	   // No overlapped execution. set the time.
	   if (arrivalTime > completionTime)
		completionTime = arrivalTime + double(serviceTime);
	   else
		completionTime += double(serviceTime);

	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
