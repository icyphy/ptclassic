ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 10/23/90

 It measures the time difference of the first and second arrival of the
 same data. If a data arrives at the first time, it is stored into the
 queue. If it is a second arrival of a data, it is matched to the
 first arrival in the queue and generates an output of time difference.
 The data itself is also generated at the second arrival.
**************************************************************************/
}
defstar {
	name { MeasureDelay }
	domain { DE }
	desc {	"Measure the time difference between the first arrival\n"
		"and the second arrival of an event.\n"
		"Output event is generated on the second arrival.\n"
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	output {
		name { timeDiff }
		type { float }
	}
	output {
		name { output }
		type { ANYTYPE }
	}
	hinclude { "PriorityQueue.h" }
	defstate {
		name { queueSize }
		type { int }
		default { "100" }
		desc { "size of the queue" }
	}
	protected {
		PriorityQueue firstArrivalQ;
	}
	start {
		firstArrivalQ.initialize();
	}

	go {
		// get input
		completionTime = arrivalTime;
		Particle& value = input%0;

		// go though the queue to check if it is a second arrival
		int len = firstArrivalQ.length();
		firstArrivalQ.reset();
		while (len > 0) {
			LevelLink* h = firstArrivalQ.next();
			if (!strcmp(value.print(),((Particle*)h->e)->print())){
			   // yes, match! --> output
			   timeDiff.put(completionTime) <<
				   completionTime - h->level;
			   Particle* oldp = (Particle*) h->e;
			   output.put(completionTime) = *oldp;
			   oldp->die();
			   firstArrivalQ.extract(h);
			   return;
			}
			len--;
		}
		// no match, register the event into the queue
		Particle* newp = value.clone();
		*newp = value;
		firstArrivalQ.levelput(newp, arrivalTime);
				
	}
}

