defstar {
	name {Poisson}
	domain {DE}
	derivedfrom { RepeatStar }
	version { $Id$}
	author { Soonhoi Ha and E. A. Lee }
	copyright { 1991 The Regents of the University of California }
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
		if(random) delete random;
	}
	start {
		if(random) delete random;
		random = new NegativeExpntl(double(meanTime),gen);
		DERepeatStar :: start();
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   // (The double cast is because of a gnu compiler bug)
	   output.put(completionTime) << float(double(magnitude));

	   // and schedule the next firing
	   refireAtTime(completionTime);

	   // Generate an exponential random variable.
	   double p = (*random)();

	   // Turn it into an exponential, and add to completionTime
	   completionTime += p;
	}
}
