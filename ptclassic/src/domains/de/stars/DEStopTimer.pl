ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 11/8/90

 It generates an output at the stopTime of the DEScheduler controlling 
 this block. This block is useful for clock input of the Sampler
 or statistical Blocks.

**************************************************************************/
}
defstar {
	name { StopTimer }
	domain { DE }
	desc {	"It generates an output at the stopTime of the\n"
		"DEScheduler under which this block is.\n"
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
		DEScheduler* sched = (DEScheduler*) mySched();
		completionTime = sched->whenStop();
		output.put(completionTime) << 0;
	}
}

