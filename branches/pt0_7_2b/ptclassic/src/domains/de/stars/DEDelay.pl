ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/29/90

 This star delays its input by an amount given by the delay parameter.

**************************************************************************/
}
defstar {
	name {Delay}
	domain {DE}
	desc {
	   "Delays its input by an amount given by the delay parameter."
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
		desc { "Amount of time delay" }
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	go {
	   completionTime = arrivalTime + double(delay);
	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
}
