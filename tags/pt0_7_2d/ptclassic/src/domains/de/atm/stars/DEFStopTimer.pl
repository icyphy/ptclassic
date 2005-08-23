defstar {
	name { FStopTimer }
	domain { DE }
	desc {
Generates an output at the stopTime of the
.c DEScheduler
under which this block is running.
	}

	version {@(#)DEFStopTimer.pl	1.5	01 Oct 1996}
	author { Allen Lao }
	copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location { DE ATM library }

	htmldoc {
This block generates an output at the stopTime of the
<tt>DEScheduler</tt>
controlling the block.  It is useful for clocking the input of the
<tt>Sampler</tt>
or statistical Blocks in order to observe their final outputs at the end
of the simulation. It generates a float output instead of an int output
like the 
<tt>DEStopTimer</tt>
in the standard DE star library.
	}
	input {
		name { input }
		type { anytype }
	}
	output {
		name { output }
		type { float }
	}
	ccinclude { "DEScheduler.h" }
	go {
		DEScheduler* sched = (DEScheduler*) scheduler();
		completionTime = sched->whenStop();
		output.put(completionTime) << 0.0;
	}
}

