defstar {
	name { MeasureInterval }
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
The value of each output event is the simulated time since the last input
event (or since zero, for the first input event). The time stamp of each 
output event is the time stamp of the input event that triggers it.
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
		output.put(arrivalTime) << (arrivalTime - completionTime);
		completionTime = arrivalTime;
	}
}

