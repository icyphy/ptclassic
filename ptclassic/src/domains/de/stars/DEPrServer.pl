defstar {
	name { PrServer }
	domain { DE }
	derivedFrom { PriorityStar }
	desc {
This Star is a server with ranked input ports.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
This star is a simple model of DEStar with ranked input.
If input event comes in during the execution, it is queued.
When the server is available, it serves an event with highest priority
first ignoring the event arrival time.
Among events of highest priority, earlier event gets first.
If a queue is overflowed, it generates warning and continues.
	}
	input {
		name {input1}
		type {ANYTYPE}
	}
	input {
		name {input2}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {ANYTYPE}
	}
	constructor {
		priority(input1, 1);
		priority(input2, 2);
		delayType = TRUE;
	}
	defstate {
		name {serviceTime}
		type {float}
		default {"1.0"}
		desc {"Service time"}
	}
	go {
		// incoming event during execution is just queued up.
		if (!isItRunnable()) return;

		// add execution time to completionTime
		completionTime += double(serviceTime);

		// define what functions to do in order of priority
		Particle* p;
		if ((p = fetchData(input1))) ;		// priority 1
		else if ((p = fetchData(input2))) ;	// priority 2
		else {
			Error::abortRun (*this, ": no data fetched");
			return;
		}
		
		// do something
		output.put(completionTime) = *p;

		// go to the Next Event
		goToNext();
	}
}
