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
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	defstate {
		name {serviceTime}
		type {float}
		default {"1.0"}
		desc { Service time. }
	}
	constructor {
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
