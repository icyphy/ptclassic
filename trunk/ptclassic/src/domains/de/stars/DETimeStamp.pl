defstar {
	name { TimeStamp }
	domain { DE }
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
The value of the output events is the time stamp of the input events.
The time stamp of the output events is also the time stamp of the input
events.
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	output {
		name { output }
		type { float }
	}
	constructor {
		input.triggers(output);
	}
	go {
		completionTime = arrivalTime;
		output.put(completionTime) << completionTime;
	}
}

