defstar {
	name {Clock}
	derivedFrom { RepeatStar }
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Generates events at regular intervals, starting at time zero.
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {interval}
		type {float}
		default {"1.0"}
		desc { The interval of events. }
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { The value of the output particles generated. }
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   output.put(completionTime) << double(magnitude);

	   // and schedule the next firing
	   refireAtTime(completionTime);

	   // Increment the completion time
	   completionTime += double(interval);
	}
}
