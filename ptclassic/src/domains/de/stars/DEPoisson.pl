ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  S. Ha and E. A. Lee
 Date of creation: 9/29/90

 This star generates events according to a Poisson process.
 The mean inter-arrival time and magnitude of the events are
 given as parameters.

**************************************************************************/
}
defstar {
	name {Poisson}
	domain {DE}
	derivedfrom { RepeatStar }
	desc {
	   "Generates events according to a Poisson process.\n"
	   "The first event comes out at time zero."
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {meanTime}
		type {float}
		default {"1.0"}
		desc { "The mean inter-arrival time" }
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { "The magnitude of samples generated" }
	}
	hinclude { <NegativeExpntl.h> }
	ccinclude { <ACG.h> }
	protected {
		NegativeExpntl *random;
	}
// declare the static random-number generator in the .cc file
	code {
		extern ACG gen;
	}
	constructor {
		random = new NegativeExpntl(double(meanTime),&gen);
	}
	destructor {
		delete random;
	}
	start {
		DERepeatStar :: start();
		random->mean(double(meanTime));
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
