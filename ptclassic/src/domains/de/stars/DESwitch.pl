defstar {
	name {Switch}
	domain {DE}
	desc {
Switches input events to one of two outputs, depending on
the last received control input.
	}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	input {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	output {
		name {true}
		type {anytype}
	}
	output {
		name {false}
		type {anytype}
	}
	constructor {
		true.inheritTypeFrom(false);
		input.inheritTypeFrom(true);
	}

	go {
	   if (input.dataNew) {
	   	completionTime = arrivalTime;
		Particle& pp = input.get();
		int c = int(control%0);
		if(c)
           	   true.put(completionTime) = pp;
		else
           	   false.put(completionTime) = pp;
	   }
	}
}
