defstar {
	name {Impulse}
	derivedFrom { RepeatStar }
	domain {DE}
	desc { Generates a single event at time zero. }
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	output {
		name {output}
		type {float}
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { The value of sample generated. }
	}
	constructor {
		delayType = TRUE;
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   // (The double cast is because of a gnu compiler bug)
	   output.put(completionTime) << float(double(magnitude));

	   // and don't schedule any further firings.
	}
}
