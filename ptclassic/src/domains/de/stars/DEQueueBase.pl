defstar {
	name {QueueBase}
	domain {DE}
	desc {
Base class for FIFO and LIFO queues.
This star is not intended to be used except to derive useful stars from.
All inputs are simply routed to the "overflow" output.  None are stored.
	}
	version { $Id$}
	author { Soonhoi Ha and E. A. Lee }
	acknowledge { Incorporates design ideas from Philip Bitar }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
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
	output {
		name {overflow}
		type {=inData}
		desc {
Arrival data that can not be queued due to capacity limit.
		}
	}
	protected {
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
		inData.triggers(overflow);

		// simultaneous inData events should be available in the
		// same firing with demand events
		inData.before(demand);

		// Following the general rule that if an input can trigger
		// an output with the same time stamp, we should identify
		// a trigger relationship:
		demand.triggers(outData);
	}
	setup {
		infinite = (int(capacity) < 0);
	}

	pure virtual method {
		name { enqueue }
		access { protected }
		arglist { "()" }
		type { void }
	}

	pure virtual method {
		name { dequeue }
		access { protected }
		arglist { "()" }
		type { Pointer }
	}

	pure virtual method {
		name { handleOverflow }
		access { protected }
		arglist { "()" }
		type { void }
	}

	virtual method {
		name { Qsize }
		access { protected }
		arglist { "()" }
		type { int }
		code {
		    return 0;
		}
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
	    while (int(numDemandsPending) > 0 && Qsize() > 0 ) {
		Particle* pp = (Particle*) dequeue();
		outData.put(completionTime) = *pp;
		pp->die();
		numDemandsPending = int(numDemandsPending) - 1;
	    }

	    // If there is new inData, store it
	    while (inData.dataNew) {
		enqueue();
		inData.getSimulEvent();
	    }

	    // Handle overflow
	    handleOverflow();

	    // output the queue size
	    size.put(completionTime) << Qsize();
	}
}
