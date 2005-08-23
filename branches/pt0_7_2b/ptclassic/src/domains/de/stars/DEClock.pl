defstar {
	name {Clock}
	derivedFrom { RepeatStar }
	domain {DE}
	version { @(#)DEClock.pl	1.10	3/2/95}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Generate events at regular intervals, starting at time zero.
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
