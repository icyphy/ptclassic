ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer: S. Ha and E. A. Lee
Date of creation: 9/29/90
Revision: use gnu random number generator (S. Ha, 10/11/90)

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
	hinclude { <Uniform.h> }
	ccinclude { <ACG.h> }
	protected {
		Uniform *random;
	}
// declare the extern random-number generator in the .cc file
	code {
		extern ACG gen;
	}
	constructor {
		input.inheritTypeFrom(output);
		random = new Uniform(0,1,&gen);
	}
	destructor {
		delete random;
	}
	start {
		random->high(double(output.numberPorts()));
	}

	go {
           completionTime = arrivalTime;
	   // choose a output port randomly
	   double p = (*random)() + 1.0;
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
