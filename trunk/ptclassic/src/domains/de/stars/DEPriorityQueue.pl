defstar {
	name {PriorityQueue}
	domain {DE}
	desc {
A priority queue.
Inputs have priorities according to the number of the input, with
"inData#1" having highest priority, "inData#2" having the second
highest priority, etc.
When a "demand" is received, outputs are produced by selecting first
based on priority, and then based on the time of arrival, using a
first-in first-out (FIFO) policy.
A finite total capacity of the queue can be specified by setting the
"capacity" parameter to a positive integer.
When the capacity has been reached, further inputs are sent to the "overflow"
output and are not stored.
The "numDemandsPending" and "consolidateDemands" parameters have the same
meaning as in other Queue stars.
The size of the queue is sent to the "size" output whenever an "inData"
or "demand" event is processed.
	}
	version { $Id$ }
	author { Soonhoi Ha and E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
This star queues input particles and produces output particles on demand.
The inputs are assigned priorities so that when an output is demanded,
if any particle from \fIinData#1\fR is available, it
will be sent to the output.  If several particles from
\fIinData#1\fR are available, a FIFO policy is used.
Only if the queue for \fIinData#1\fR is empty
will the queue for \fIinData#2\fR be examined.
This means that later arrivals on \fIinData#1\fR may go to the output before
earlier arrivals on \fIinData#2\fR.
.pp
The first particle to arrive at an input is always passed directly
to the output, unless \fInumDemandsPending\fR is initialized to 0.
If several particles arrive first with the same time stamp, then the highest
priority particle will go to the output.
.pp
If \fIconsolidateDemands\fR is set to TRUE (the default),
then \fInumDemandsPending\fR is not permitted to rise above unity.
This means that
if multiple \fIdemand\fR particles with the same time stamp
are waiting at the \fIdemand\fR input, then they are
consolidated into a single demand, and only one output is produced.
As usual, if a \fIdemand\fR input arrives between the time that the queue
becomes empty and the next arrival of data input, then the star enables
the next \fIinData\fR particle to pass immediately to the output by
setting the state \fInumDemandsPending\fR to unity.
But if more than one \fIdemand\fR event arrives in this time period, then the
effect is the same as if only one such event had arrived.
.pp
If \fIconsolidateDemands\fR is set to FALSE, then every \fIdemand\fR input
will eventually stimulate an output, even if successive demands arrive
when the queue is empty, and even if successive demands arrive with the
same time stamp.
.pp
A total capacity limit can be specified.
This is the limit on the total number of particles that the star
may store from one firing to the next, regardless of which input they came.
Note that storage is allocated dynamically, so a large capacity
does not necessarily imply a large storage usage.
When the capacity has been reached, all further inputs will be routed
to the \fIoverflow\fR output
until a demand input makes space in the star.
To make the capacity infinite, simply enter a negative number for the
"capacity" parameter.
.pp
Like the
.c FIFOQueue
star, this star dequeues particles on demand.
Data outputs are produced when a demand input arrives, if the any of the
queues has particles in it.
If all queues are empty, then no data output is produced
until the next time a data input arrives.
The next data input will be directed to the data output immediately.
Any intervening demand inputs (between the time that the queues
go empty and the next arrival of a data input) are ignored.
.pp
Each time data or demand input arrives, the size of the queue
after processing the input is sent to the \fIsize\fR output.
	}
	seealso {FIFOQueue}
	input {
		name {demand}
		type {anytype}
		desc { Demand an output. }
	}
	inmulti {
		name {inData}
		type {anytype}
		desc { Particles to be queued. }
	}
	output {
		name {outData}
		type {=inData}
		desc { Particles that are de-queued on demand. }
	}
	output {
		name {size}
		type {int}
		desc { The current number of particles stored. }
	}
	output {
		name {overflow}
		type {=inData}
		desc {
Arrival data that cannot be queued due to capacity limit.
		}
	}

	hinclude { "PriorityQueue.h" }
	protected {
		PriorityQueue queue;
		int infinite;
	}
	defstate {
		name {capacity}
		type {int}
		default { "-1" }
		desc {
Maximum total number of particles. If <0, capacity is infinite.
		}
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
		// Hint to the scheduler that both demand and inData trigger
		// size outputs.  And demand triggers an output.
		inData.triggers(size);
		demand.triggers(size);
		demand.triggers(outData);

		// Hint to the scheduler that if there is any chance of
		// simultaneous events on inData and demand, then the inData
		// events should be made available.
		inData.before(demand);
	}
	setup {
		emptyQueue();
		infinite = (int(capacity) < 0);
	}
	destructor {
		emptyQueue();
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

	    // Iterate over the inputs, collecting new input data
	    // All input data should be collected before any outputs are
	    // produced so that priorities of the inputs will be observed.
	    // Note that the queue size limitation will not be observed at
	    // this point.  Only at the end of the firing will the queue
	    // be truncated to capacity.
	    InDEMPHIter nextp(inData);
	    for(int i=0;i<inData.numberPorts(); i++) {
		InDEPort* p = nextp++;
		while (p->dataNew) {
		    queue.levelput(p->get().clone(),i);
		    p->getSimulEvent();
		}
	    }

	    // If the queue has data, and one or more demands are pending,
	    // produce output data.  Higher priority data will be automatically
	    // sent out before lower priority data, because of the order in
	    // which it was enqueued.
	    while (int(numDemandsPending) > 0 && queue.length() > 0 ) {
		Particle* pp = (Particle*) queue.getFirstElem();
		outData.put(completionTime) = *pp;
		pp->die();
		numDemandsPending = int(numDemandsPending) - 1;
	    }

	    // Cut queue down to size by removing lowest priority entries.
	    // Entries removed are sent to the "overflow" output.
	    if (!infinite)
		while (queue.length() > int(capacity)) {
		    Particle* junkp = (Particle*) queue.getLastElem();
		    overflow.put(completionTime) = *junkp;
		    junkp->die();
		}

	    // output the queue size
	    size.put(completionTime) << queue.length();
	}
	// this method empties the queue, returning any Particles to their
	// pools.
	method {
		name { emptyQueue }
		code {
			while (queue.length() > 0) {
				Particle* p = (Particle*) queue.getFirstElem();
				p->die();
			}
		}
	}
}
