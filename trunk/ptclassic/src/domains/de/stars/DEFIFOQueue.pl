defstar {
	name {FIFOQueue}
	domain {DE}
	derivedFrom { QueueBase }
	desc {
FIFO queue with finite or infinite length.
Events on the "demand" input trigger "outData" if the queue is not empty.
If the queue is empty, then a "demand" event enables the next future
"inData" particle to pass immediately to "outData".
The first particle to arrive at "inData" is always passed directly
to the output, unless "numDemandsPending" is initialized to 0.
If "consolidateDemands" is set to TRUE (the default), then "numDemandsPending"
is not permitted to rise above unity.
The size of the queue is sent to the "size" output whenever an "inData"
or "demand" event is processed.
Input data that doesn't fit in the queue is sent to the "overflow" output.
	}
	version { $Id$}
	author { Soonhoi Ha, E. A. Lee, and Philip Bitar }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	seealso {QueueBase, Stack, PriorityQueue}
	explanation {
.IE "queue"
This star queues inputs in a finite or infinite length FIFO queue.
After the queue has grown to capacity,
any "inData" inputs are sent to the "overflow" output, and not stored.
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
Consider the following example.  The FIFOQueue has capacity 3 and size 2.
Three "inData" particles are waiting
at the input, and all three have the same time stamp.  Two "demand"
particles are waiting, also with the same time stamp.
Assume "consolidateDemands" is set to TRUE (the default).
The "demand" input particles will both be grabbed, and a single
"outData" particle will be sent from the queue, reducing its size to 1.
Then the first two pending "inData" particles will be queued, and the
third will be sent to "overflow".  The queue has reached capacity.
If "consolidateDemands" had been FALSE, two outputs would have been
produced first, reducing the queue size to 0, and making room
for all three pending "inData" particles.
.pp
Consider the next example.  A FIFOQueue star is wired up with the "outData"
fed back to the "demand" input, with no delay, and "numDemandsPending"
is initialized to unity (the default).  The first "inData" particle
will pass directly to the output, and hence appear on the "demand" input
with zero delay.  This enables the second "inData" particle to also pass
through to "outData" immediately, and back to "demand".  The FIFOQueue wired
this way is a no-op, passing input particles directly to the output.
Even if two "inData" particles arrive with the
same time stamp, they will both be passed to the output with the same
time stamp, even though it takes two successive firings to accomplish this.
.pp
As a third example, a FIFOQueue with capacity zero will pass
"inData" particles to the output only if there is a pending demand.
Otherwise, the inData particle will be sent to "overflow".
.pp
Each time a data or demand input arrives, the size of the queue
after processing all inputs is sent to the "size" output.
	}
	ccinclude {
		"DataStruct.h"
	}
	protected {
		Queue queue;
	}
	method {
		name { enqueue }
		access { protected }
		arglist { "()" }
		type { void }
		code {
		    if (Qsize() == 0 && int(numDemandsPending) > 0) {
			outData.put(completionTime) = inData.get();
			numDemandsPending = int(numDemandsPending) - 1;
		    } else if (infinite || Qsize() < int(capacity)) {
			queue.put(inData.get().clone());
		    } else {
			// Data is lost
			overflow.put(completionTime) = inData.get();
		    }
		}
	}
	method {
		name { dequeue }
		access { protected }
		arglist { "()" }
		type { Pointer }
		code {
		    return queue.get();
		}
	}
	method {
		name { handleOverflow }
		access { protected }
		arglist { "()" }
		type { void }
		code {
		    // Nothing to do
		}
	}
	method {
		name { Qsize }
		access { protected }
		arglist { "()" }
		type { int }
		code {
		    return queue.length();
		}
	}
	method {
		name { zapQueue }
		code {
			while (Qsize() > 0) {
				Particle* pp = (Particle*) queue.get();
				pp->die();
			}
			queue.initialize();
		}
	}
	setup {
		DEQueueBase::setup();
		zapQueue();
	}
	destructor {
		zapQueue();
	}
}
