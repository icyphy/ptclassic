defstar {
	name {Router}
	domain {DE}
	desc {
Randomly routes an input event to one of its outputs.
Probability is equal for each output. The time delay is zero.
	}
	version { $Id$}
	author { Soonhoi Ha and E. A. Lee}
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
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
	   OutDEMPHIter nextp(output);
	   OutDEPort* pp;
	   for (int i = int(p); i > 0 ; i--) {
		pp = nextp++;
	   }

	   // route the data to the chosen port
	   Particle& ip = input.get();
	   pp->put(completionTime) = ip;
	}
}
