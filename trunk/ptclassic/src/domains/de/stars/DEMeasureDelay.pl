defstar {
	name { MeasureDelay }
	domain { DE }
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Measure the time difference between the first arrival
and the second arrival of an event with the same value.
The second arrival and the time difference are each sent to outputs.
	}
	explanation {
This star measures the time difference of the first and second arrival of
particles with the same value on the same input.
When the first particle with a particular data value
arrives, it is stored.
When the second particle with a particular data value arrives,
it is matched to the first arrival in storage, and
a \fItimeDiff\fR
output is generated with value equal to the arrival time difference.
The data value itself is also
sent to the \fIoutput\fR port upon the second arrival.
.pp
Packets are only considered to have the "same value" if they are copied
from a common source (so that each packet shares the same common PacketData
structure).  This suffices for common uses of MeasureDelay where the
same packet is routed through a delay-free path and also a path with delay.
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
		type { =input }
	}
	hinclude { "PriorityQueue.h" }
	protected {
		PriorityQueue firstArrivalQ;
	}
	constructor {
		input.triggers(output);
	}
	setup {
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
			if (value == *(Particle*)(h->e)) {
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
		firstArrivalQ.levelput(newp, arrivalTime);

	}
	// remove leftover particles from queue
	wrapup {
		while (firstArrivalQ.length() > 0) {
			Particle* p = (Particle*)firstArrivalQ.getFirstElem();
			p->die();
		}
	}
	destructor {
		wrapup();	// in case it was not called before
	}
}

