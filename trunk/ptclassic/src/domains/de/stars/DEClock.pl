defstar {
	name {Clock}
	derivedFrom { RepeatStar }
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
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
