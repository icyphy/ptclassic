defstar {
	name { StopTimer }
	domain { DE }
	desc {
Generate an output at the stopTime of the
DEScheduler under which this block is running.
This can be used to force actions at the end of a simulation.
Within a wormhole, it can used to force actions at the end
of each invocation of the wormhole.
An input event is required to enable the star.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Once enabled by an input event,
this star generates an output at the stopTime of the DEScheduler controlling 
the star.  It is useful, for example, to clock the
.c Statistics
star in order to observe its final outputs at the end
of a simulation.
It also has interesting uses within a wormhole.
If it enabled during a particular invocation of the wormhole scheduler,
then it will generate an event with time stamp equal to the time
at which this scheduler will return control to the calling system.
Suppose, for example, that a DE subsystem is used within an SDF
system.  The DE subsystem is required to produce output particles,
so that the SDF model of computation is obeyed.
The
.c StopTimer
star can be used to produce the output events,
or to trigger production of those output events, by
clocking the
.c Sampler
star for example.
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

