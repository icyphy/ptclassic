ident {
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  E. A. Lee
Date of creation: 10/9/90

This star queues inputs in a finite length FIFO queue.
Note that in the DE domain, ordinary arcs also function as a FIFO queue.
One difference here is that this star refuses to accept any new inputs
after the queue has grown to a specified capacity.
(Note that storage is allocated dynamically, so a large capacity
does not necessarily imply a large storage usage.)
.pp
Another difference is that this star de-queues particles on demand.
Data outputs are produced when a demand input arrives, if the queue
is not empty.  If the queue is empty, no data output is produced
until the next time a data input arrives.
That next data input will be directed to the data output immediately.
Any intervening demand inputs (between the time that the queue
goes empty and the next arrival of a data input) are ignored.
.pp
Each time a data or demand input arrives, the size of the queue
after processing the input is sent to the ``size'' output.
**************************************************************************/
}
defstar {
	name {Queue}
	domain {DE}
	desc {
	   "FIFO queue with finite length."
	}
	input {
		name {demand}
		type {anytype}
	}
	input {
		name {inData}
		type {anytype}
	}
	output {
		name {outData}
		type {anytype}
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
	}
	constructor {
		inData.inheritTypeFrom(outData);
	}
	defstate {
		name {capacity}
		type {int}
		default {"10"}
		desc { "Maximum size of the queue." }
	}

	go {
	   // If the data input is new, and the queue is not full, store it
	   completionTime = arrivalTime;
	   if (inData.dataNew) {
		if (queue.length() < int(capacity)) {
		   Particle& pp = inData.get();
		   Particle* newp = pp.clone();
		   *newp = pp;
		   queue.put(newp);
		}

		inData.dataNew = FALSE;
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
