defstar {
	name {Impulse}
	derivedFrom { RepeatStar }
	domain {DE}
	desc { Generates a single event at time zero. }
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   output.put(completionTime) << double(magnitude);

	   // and don't schedule any further firings.
	}
}
