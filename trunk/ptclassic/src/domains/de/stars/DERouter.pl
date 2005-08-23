defstar {
	name {Router}
	domain {DE}
	desc {
Randomly route an input event to one of its outputs.
The probability is equal for each output. The time delay is zero.
	}
	version { @(#)DERouter.pl	2.13	6/20/95}
	author { Soonhoi Ha and E. A. Lee}
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	input {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
		name{output}
		type{=input}
	}
	hinclude { <Uniform.h> }
	ccinclude { <ACG.h> }
	protected {
		Uniform *random;
	}
// declare the extern random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = NULL;
	}
	destructor {
		LOG_DEL; delete random;
	}
	setup {
		LOG_DEL; delete random;
		LOG_NEW; random = new Uniform(0,double(output.numberPorts()),gen);
	}

	go {
           completionTime = arrivalTime;
	   // choose a output port randomly
	   double p = (*random)();
	   OutDEMPHIter nextp(output);
	   OutDEPort* pp = nextp++;
	   for (int i = int(p); i > 0 ; i--) {
		pp = nextp++;
	   }

	   // route the data to the chosen port
	   Particle& ip = input.get();
	   pp->put(completionTime) = ip;
	}
}
