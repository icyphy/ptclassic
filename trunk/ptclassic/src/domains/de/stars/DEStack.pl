defstar {
	name {Stack}
	domain {DE}
	derivedFrom { QueueBase }
	desc {
A stack with finite or infinite length.
Events on the "demand" input pop data from the stack to "outData" if the
stack is not empty.  If it is empty, then a "demand" event enables the
next future "inData" particle to pass immediately to "outData".
By default, "numDemandsPending" is initialized to 1, so the first
particle to arrive at "inData" is passed directly to the output.
If "consolidateDemands" is set to TRUE (the default), then "numDemandsPending"
is not permitted to rise above unity.
The size of the stack is sent to the "size" output whenever an "inData"
or "demand" event is processed.  Input data that doesn't fit on the stack
is sent to the "overflow" output.
	}
	version { $Id$}
	author { Soonhoi Ha, E. A. Lee, and Philip Bitar }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	seealso {QueueBase, FIFOQueue, PriorityQueue}
	explanation {
This star stacks inputs in a finite or infinite length LIFO queue.
After the stack has grown to capacity,
it sends any further "inData" inputs to the "overflow" output.
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
As usual, if a "demand" input arrives between the time that the stack
goes empty and the next arrival of a data input, it enables
the next "inData" particle to pass immediately to the output by
setting the state "numDemandsPending" to unity.
But if more than one "demand" event arrives in this time period, the
effect is the same as if only one such event had arrived.
.pp
When "demand" inputs and "inData" inputs have the same time
stamp, then they will be pushed and popped without regard to capacity
limitations (since they spend zero time in the stack).
Hence, a collection of simultaneous "inData" particles will be output
in reverse order.
.pp
Consider the following example.  The Stack has capacity 3 and size 2.
Three "inData" particles are waiting
at the input, and all three have the same time stamp.  Two "demand"
particles are waiting, also with the same time stamp.
Assume "consolidateDemands" is set to TRUE (the default).
The "demand" input particles will both be grabbed, and a single
"outData" particle will be popped from the stack, reducing its size to 1.
Then the first two pending "inData" particles will be stacked, and the
third will be sent to "overflow".  The Stack has reached capacity.
If "consolidateDemands" had been FALSE, two outputs would have been
produced first, reducing the stack size to 0, and making room
for all three pending "inData" particles.
.pp
Consider the next example.  A Stack star is wired up with the "outData"
fed back to the "demand" input, with no delay, and "numDemandsPending"
is initialized to unity (the default).  The first "inData" particle
will pass directly to the output, and hence appear on the "demand" input
with zero delay.  This enables the second "inData" particle to also pass
through to "outData" immediately, and back to "demand".  The Stack wired
this way is a no-op, passing input particles directly to the output.
Even if two "inData" particles arrive with the
same time stamp, they will both be passed to the output with the same
time stamp, even though it takes two successive firings to accomplish this.
\fIHowever, they will pass to the output in reverse order.\fR
.pp
As a third example, a Stack with capacity zero will pass
"inData" particles to the output only if there is a pending demand.
Otherwise, the inData particle will be sent to "overflow".
.pp
Each time a data or demand input arrives, the size of the stack
after processing all inputs is sent to the "size" output.
	}
	ccinclude {
		"DataStruct.h"
	}
	protected {
		Stack stack;
	}
	method {
		name { enqueue }
		access { protected }
		arglist { "()" }
		type { void }
		code {
		    // Push new data, ignoring capacity limitations.
		    stack.pushTop(inData.get().clone());
		}
	}
	method {
		name { dequeue }
		access { protected }
		arglist { "()" }
		type { Pointer }
		code {
		    return stack.popTop();
		}
	}
	method {
		name { handleOverflow }
		access { protected }
		arglist { "()" }
		type { void }
		code {
		    // If any demands are pending, deal with them now
		    while (int(numDemandsPending) > 0 && Qsize() > 0 ) {
			Particle* p = (Particle*) dequeue();
			outData.put(completionTime) = *p;
			p->die();
			numDemandsPending = int(numDemandsPending) - 1;
		    }

		    // If capacity is finite, and stack is over full,
		    // pop until capacity
		    if (!infinite) {
			while (Qsize() > int(capacity)) {
			    Particle* pp = (Particle*) stack.popTop();
		    	    overflow.put(completionTime) = *pp;
			    pp->die();
			}
		    }
		}
	}
	method {
		name { Qsize }
		access { protected }
		arglist { "()" }
		type { int }
		code {
		    return stack.size();
		}
	}
	method {
		name { zapStack }
		code {
			while (Qsize() > 0) {
				Particle* pp = (Particle*) stack.popTop();
				pp->die();
			}
			stack.initialize();
		}
	}
	setup {
		DEQueueBase::setup();
		zapStack();
	}
	destructor {
		zapStack();
	}
}
