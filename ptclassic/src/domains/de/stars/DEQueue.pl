defstar {
	name {Queue}
	domain {DE}
	desc { FIFO queue with finite length.  }
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	seealso {PrQueue}
	explanation {
This star queues inputs in a finite length FIFO queue.
It refuses to accept any new inputs
after the queue has grown to a specified capacity.
(Note that storage is allocated dynamically, so a large capacity
does not necessarily imply a large storage usage.)
.pp
This star de-queues particles on demand.
Data outputs are produced when a demand input arrives, if the queue
is not empty.  If the queue is empty, no data output is produced
until the next time a data input arrives.
That next data input will be directed to the data output immediately.
Initially, the queue is empty and the first data input is directed
to the data output immediately as if there were a past demand input.
Any intervening demand inputs (between the time that the queue
goes empty and the next arrival of a data input) are ignored.
.pp
Each time a data or demand input arrives, the size of the queue
after processing the input is sent to the ``size'' output.
	}
	input {
		name {demand}
		type {anytype}
		desc { Triggers an output. }
	}
	input {
		name {inData}
		type {anytype}
	}
	output {
		name {outData}
		type {=inData}
	}
	output {
		name {size}
		type {int}
	}
	ccinclude {
		"DataStruct.h"
	}
	protected {
		Queue queue;
		int infinite;
	}
	defstate {
		name {capacity}
		type {int}
		default {"10"}
		desc { Maximum size of the queue (if -1, infinite). }
	}
	constructor {
		inData.triggers(size);
		demand.triggers(outData);
	}
	start {
		demand.dataNew = TRUE;
		infinite = (int(capacity) < 0);
		queue.initialize();
	}

	go {
	   // If the data input is new, and the queue is not full, store it
	   completionTime = arrivalTime;
	   if (inData.dataNew) {
		if (infinite || queue.length() < int(capacity)) {
		   queue.put(inData.get().clone());
		}
		else inData.dataNew = FALSE;
	   }
	   // Produce outData only if the demand input is new.
	   if (demand.dataNew) {
		if (queue.length() > 0) {
		   Particle* pp = (Particle*) queue.get();
		   outData.put(completionTime) = *pp;
		   demand.dataNew = FALSE;
		}
	   }
	   // output the queue size
	   size.put(completionTime) << int(queue.length());
	}
}
