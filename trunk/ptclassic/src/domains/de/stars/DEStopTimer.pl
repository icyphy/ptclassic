defstar {
	name { StopTimer }
	domain { DE }
	desc {
Generates an output at the stopTime of the
DEScheduler under which this block is running.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
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

