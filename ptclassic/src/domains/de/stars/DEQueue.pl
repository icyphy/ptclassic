defstar {
	name {Queue}
	domain {DE}
	desc {
THIS STAR IS OBSOLETE.  USE FIFOQueue.
FIFO queue with finite or infinite length.
Events on the "demand" input trigger "outData" if the Queue is not empty.
If the queue is empty, then a "demand" event enables the next future
"inData" particle to pass immediately to "outData".
The first particle to arrive at "inData" is always passed directly
to the output, unless "numDemandsPending" is initialized to 0.
If "consolidateDemands" is set to TRUE (the default), then "numDemandsPending"
is not permitted to rise above unity.
The size of the queue is sent to the "size" output whenever an "inData"
or "demand" event is processed.
	}
	version { $Id$}
	author { Soonhoi Ha and E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	seealso {PrQueue}
	explanation {
This star queues inputs in a finite or infinite length FIFO queue.
It discards any "inData" inputs after the queue has grown to capacity.
If the "capacity" parameter is a negative number,
then the capacity is taken to be infinite.
Note that storage is allocated dynamically, so a large capacity
does not necessarily imply a large storage usage.
.pp
If "consolidateDemands" is set to FALSE, then every "demand" input
will eventually stimulate an output, even if successive demands arrive
when the queue is empty, and even if successive demands arrive with the
same time stamp.
.pp
If "consolidateDemands" is set to TRUE (the default), then "numDemandsPending"
is not permitted to rise above unity.
This means that
if multiple "demand" particles with the same time stamp
are waiting at the "demand" input, they are
consolidated into a single demand, and only one output is produced.
As usual, if a "demand" input arrives between the time that the queue
goes empty and the next arrival of a data input, it enables
the next "inData" particle to pass immediately to the output by
setting the state "numDemandsPending" to unity.
But if more than one "demand" event arrives in this time period, the
effect is the same as if only one such event had arrived.
.pp
When "demand" inputs and "inData" inputs have the same time
stamp, then if the queue is not empty, the star produces as many
outputs as possible before collecting the inData.
Hence, if the queue is at capacity, the dequeueing makes
room for the new inData.  If the queue is empty when "demand" and "inData"
arrive with the same time stamp, then the "inData"
particle will pass directly to the "outData", even if the capacity
of the queue is zero.  If there is more than one
"inData" particle with the same time stamp, the remaining "inData" particles
will be collected after the output has been produced.
.pp
Consider the following example.  The Queue has capacity 3 and size 2.
Three "inData" particles are waiting
at the input, and all three have the same time stamp.  Two "demand"
particles are waiting, also with the same time stamp.
Assume "consolidateDemands" is set to TRUE (the default).
The "demand" input particles will both be grabbed, and a single
"outData" particle will be sent from the queue, reducing its size to 1.
Then the first two pending "inData" particles will be queued, and the
third will be discarded.  The queue has reached capacity.
If "consolidateDemands" had been FALSE, two outputs would have been
produced first, reducing the queue size to 0, and making room
for all three pending "inData" particles.
.pp
Consider the next example.  A Queue star is wired up with the "outData"
fed back to the "demand" input, with no delay, and "numDemandsPending"
is initialized to unity (the default).  The first "inData" particle
will pass directly to the output, and hence appear on the "demand" input
with zero delay.  This enables the second "inData" particle to also pass
through to "outData" immediately, and back to "demand".  The Queue wired
this way is a no-op, passing input particles directly to the output.
Even if two "inData" particles arrive with the
same time stamp, they will both be passed to the output with the same
time stamp, even though it takes two successive firings to accomplish this.
.pp
As a third example, a Queue with capacity zero will pass
"inData" particles to the output only if there is a pending demand.
Otherwise, the inData particle will be discarded.
.pp
Each time a data or demand input arrives, the size of the queue
after processing all inputs is sent to the "size" output.
	}
	input {
		name {demand}
		type {anytype}
		desc { Triggers an immediate output unless Queue is empty.  }
	}
	input {
		name {inData}
		type {anytype}
		desc { Particles to be queued. }
	}
	output {
		name {outData}
		type {=inData}
	}
	output {
		name {size}
		type {int}
		desc { Size of Queue. Triggered by inData or demand inputs. }
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
		default {"-1"}
		desc { Maximum size of the queue. If <0, capacity is infinite. }
	}
	defstate {
		name {numDemandsPending}
		type {int}
		default { "1" }
		desc { Number of demands that have not yet stimulated outputs. }
	}
	defstate {
		name { consolidateDemands }
		type {int}
		default { TRUE }
		desc { If FALSE, every demand eventually stimulates an output. }
	}
	constructor {
		// Indicate to the scheduler that size outputs are triggered
		// by inData or demand inputs.
		inData.triggers(size);
		demand.triggers(size);

		// simultaneous inData events should be available in the
		// same firing with demand events
		inData.before(demand);

		demand.triggers(outData);
		// Note that although demand triggers outData if the Queue
		// has data, it does not trigger an immediate output if the
		// Queue is empty.  Hence, if the Queue is empty, there is
		// a delay in this path.  Although this might imply we do not
		// want to declare that demand triggers outData, actually, we
		// do.  Consider for instance a star feeding data to both
		// the demand input of the Queue and a downstream star that
		// also gets data from the demand input of the star. We want
		// the Queue to fire before the downstream star. Delay free
		// loops involving a path through demand to outData are fine,
		// but an infinitessimal delay should be put in the feedback
		// path.
	}
	setup {
		infinite = (int(capacity) < 0);
		queue.initialize();
	}

	go {
	    completionTime = arrivalTime;

	    while (demand.dataNew) {
		if (int(consolidateDemands))
			numDemandsPending = 1;
		else
			numDemandsPending = int(numDemandsPending) + 1;
		// get any pending simultaneous demand inputs.
		demand.getSimulEvent();
	    }

	    // If the queue has data, and one or more demands are pending,
	    // produce output data
	    while (int(numDemandsPending) > 0 && queue.length() > 0 ) {
		Particle* pp = (Particle*) queue.get();
		outData.put(completionTime) = *pp;
		pp->die();
		numDemandsPending = int(numDemandsPending) - 1;
	    }

	    // If there is new inData, and the queue is not full, store it
	    while (inData.dataNew) {
		if (queue.length() == 0 && int(numDemandsPending) > 0) {
		    outData.put(completionTime) = inData.get();
		    numDemandsPending = int(numDemandsPending) - 1;
		} else if (infinite || queue.length() < int(capacity)) {
		    queue.put(inData.get().clone());
		}
		inData.getSimulEvent();
	    }

	    // output the queue size
	    size.put(completionTime) << int(queue.length());
	}
}
