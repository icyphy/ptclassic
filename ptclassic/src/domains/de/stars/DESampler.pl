ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and S. Ha
 Date of creation: 9/30/90

 This star samples its data input at the times given by events on its
 clock input.

**************************************************************************/
}
defstar {
	name {Sampler}
	domain {DE}
	desc {
	   "Samples its data input at the times given by"
	   "events on its clock input."
	}
	input {
		name {input}
		type {anytype}
	}
	input {
		name {clock}
		type {anytype}
	}
	output {
		name {output}
		type {anytype}
	}

	go {
	   // Check to see whether the star was triggered by a clock input
	   if (clock.dataNew) {
		completionTime = arrivalTime;
		output.put(completionTime) = input%0;
		clock.dataNew = FALSE;
	   }
	}
}
