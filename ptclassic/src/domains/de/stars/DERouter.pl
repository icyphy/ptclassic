ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer: S. Ha and E. A. Lee
Date of creation: 9/29/90

 Randomly routes an input event to one of its outputs.
 Probability is equal for each output.

************************************************************************/
}
defstar {
	name {Router}
	domain {DE}
	desc {
	   "Randomly routes an input event to one of its outputs."
	   "Probability is equal for each output. Delay is zero."
	}
	input {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
		name{output}
		type{ANYTYPE}
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	go {
           completionTime = arrivalTime;
	   // choose a output port randomly
	   double p = drand48() * double(output.numberPorts()) + 1.0;
	   output.reset();
	   OutDEPort* pp;
	   for (int i = int(p); i > 0 ; i--) {
		pp = (OutDEPort*) &(output());
	   }

	   // route the data to the chosen port
	   Particle& ip = input.get();
	   pp->put(completionTime) = ip;
	}
}
