defstar {
	name {Poisson}
	domain {DE}
	derivedfrom { RepeatStar }
	version { $Id$}
	author { Soonhoi Ha and E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Generates events according to a Poisson process.
The first event comes out at time zero.
	}
	explanation {
This star generates events according to a Poisson process.
The mean inter-arrival time and magnitude of the events are
given as parameters.
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {meanTime}
		type {float}
		default {"1.0"}
		desc { The mean inter-arrival time. }
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { The magnitude of samples generated. }
	}
	hinclude { <NegExp.h> }
	ccinclude { <ACG.h> }
	protected {
		NegativeExpntl *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG* gen;
	}
	constructor {
		random = NULL;
	}
	destructor {
		if(random) { LOG_DEL; delete random;}
	}
	start {
		if(random) { LOG_DEL; delete random;}
		LOG_NEW; random = new NegativeExpntl(double(meanTime),gen);
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   output.put(completionTime) << double(magnitude);

	   // and schedule the next firing
	   refireAtTime(completionTime);

	   // Generate an exponential random variable.
	   double p = (*random)();

	   // Turn it into an exponential, and add to completionTime
	   completionTime += p;
	}
}
