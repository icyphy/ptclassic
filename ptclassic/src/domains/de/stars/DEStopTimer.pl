defstar {
	name { StopTimer }
	domain { DE }
	desc {
Generates an output at the stopTime of the
DEScheduler under which this block is running.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
This block generates an output at the stopTime of the DEScheduler controlling 
the block.  It is useful for clocking the input of the Sampler
or statistical Blocks in order to observe their final outputs at the end
of the simulation.
	}
	input {
		name { input }
		type { anytype }
	}
	output {
		name { output }
		type { int }
	}
	ccinclude { "DEScheduler.h" }
	go {
		DEScheduler* sched = (DEScheduler*) scheduler();
		completionTime = sched->whenStop();
		output.put(completionTime) << 0;
	}
}

