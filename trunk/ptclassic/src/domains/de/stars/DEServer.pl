ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/29/90

 This star emulates a server.  If input events arrive when it is not busy,
 it delays them by the service time.  If they arrive when it is busy,
 it delays them by more.  It must become free, and then serve them.

**************************************************************************/
}
defstar {
	name {Server}
	domain {DE}
	desc {
	   "This star emulates a server.\n"
	   "If input events arrive when it is not busy,\n"
 	   "it delays them by the service time (constant).\n"
	   "If they arrive when it is busy,\n"
 	   "it delays them by more.\n"
	   "It must become free, and then serve them."
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
		name {serviceTime}
		type {float}
		default {"1.0"}
		desc { "Service time" }
	}
	constructor {
		input.inheritTypeFrom(output);
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
