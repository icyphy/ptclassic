ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/9/90

 This star generates a single event at the time origin.

**************************************************************************/
}
defstar {
	name {Impulse}
	derivedFrom { RepeatStar }
	domain {DE}
	desc {
	   "Generates a single event at time zero."
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {magnitude}
		type {float}
		default {"1.0"}
		desc { "The magnitude of sample generated" }
	}
	go {
	   // Generate the output event
	   // (Recall that the first event comes out at time 0).
	   // (The double cast is because of a gnu compiler bug)
	   output.put(completionTime) << float(double(magnitude));

	   // and don't schedule any further firings.
	}
}
