ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/30/90

 This star generates events at regular intervals, starting at time zero.

**************************************************************************/
}
defstar {
	name {Clock}
	derivedFrom { RepeatStar }
	domain {DE}
	desc {
	   "Generates events at regular intervals, starting at time zero."
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {interval}
		type {float}
		default {"1.0"}
		desc { "The interval of events" }
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { "The magnitude of samples generated" }
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   // (The double cast is because of a gnu compiler bug)
	   output.put(completionTime) << float(double(magnitude));

	   // and schedule the next firing
	   refireAtTime(completionTime);

	   // Increment the completion time
	   completionTime += double(interval);
	}
}
