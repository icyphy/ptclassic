ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/29/90

 This star switches input events to one of two outputs, depending on
 the last received control input.

**************************************************************************/
}
defstar {
	name {Switch}
	domain {DE}
	desc {
	   "Switches input events to one of two outputs, depending on\n"
	   "the last received control input."
	}
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
