ident {
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  E. A. Lee and S. Ha
Date of creation: 10/10/90

This star queues input particles and produces output particles on demand.
The inputs are assigned priorities so that when an output is demanded,
if any particles from input#1 are available, the next one in the queue
will be sent to the output.  Only if the queue for input#1 is empty
will the queue for input#2 be examined.
This means that later arrivals on input#1 may go to the output before
earlier arrivals on input#2.
There is currently a limit of 10 distinct inputs, and hence 10 priorities.
.pp
A total capacity limit is specified.
This is the limit on the total number of particles that the star
may store in its queues at any one time, regardless of which
input they came from.
Note that storage is allocated dynamically, so a large capacity
does not necessarily imply a large storage usage.
When the capacity is reached, all further inputs will be ignored
until a demand input makes space in the star.
To make the capacity infinite, simply enter a negative number.
.pp
Like the
.c Queue
star, this star de-queues particles on demand.
Data outputs are produced when a demand input arrives, if the any of the
queues has particles in it.
If all queues are empty, no data output is produced
until the next time a data input arrives.
That next data input will be directed to the data output immediately.
Any intervening demand inputs (between the time that the queues
go empty and the next arrival of a data input) are ignored.
.pp
Each time a data or demand input arrives, the size of the queue
after processing the input is sent to the ``size'' output.

 TEMPORARY:  THE MAXIMUM NUMBER OF INPUTS IS HARDWIRED BECAUSE I DON'T
 KNOW HOW TO A #define USING THE PREPROCESSOR.

**************************************************************************/
}
defstar {
	name {PrQueue}
	domain {DE}
	desc {
	   "Priority queue with finite total capacity."
	}
	input {
		name {demand}
		type {anytype}
		// desc { "Demand an ouput" }
	}
	inmulti {
		name {inData}
		type {anytype}
		// desc { "Particles to be queued" }
	}
	output {
		name {outData}
		type {anytype}
		// desc { "Particles that are de-queued on demand" }
	}
	output {
		name {size}
		type {int}
		// desc { "The current number of particles stored" }
	}
	ccinclude {
		"DataStruct.h"
	}
	protected {
		Queue* queue[10];	// maximum of 10 inputs
	}
	constructor {
		inData.inheritTypeFrom(outData);
	}
	defstate {
		name {curSize}
		type {int}
		default {"0"}
		desc { "Current number of particles stored." }
	}
	defstate {
		name {capacity}
		type {int}
		default {"10"}
		desc { "Maximum total number of particles stored." }
	}

	start {
	   if (inData.numberPorts() > 10) {
		Error::abortRun(*this, "Too many input ports.");
	   } else {
		// Create or initialize the Queues
		for(int i=(inData.numberPorts()-1); i>=0; i--) {
		    if (queue[i]) queue[i]->initialize();
		    else queue[i] = new Queue;
		}
	   }
	   demand.dataNew = TRUE;

	}

	go {
	   // If there is new data input, and the queue is not full, store it
	   completionTime = arrivalTime;
	   InDEMPHIter nextp(inData);
	   for(int i=0;i<inData.numberPorts(); i++) {
		InDEPort* p = nextp++;
		if (p->dataNew) {
		    if (int(curSize) < int(capacity)) {
			curSize += 1;
			Particle& pp = p->get();
			Particle* newp = pp.clone();
			*newp = pp;
			queue[i]->put(newp);
		    }
		    p->dataNew = FALSE;
		}
	   }
	   // Produce outData only if the demand input is new.
	   if (demand.dataNew) {
		// If a queue has data...
		if ((int)curSize > 0) {
		   // ...figure out which one
		   int qNum = 0;
		   while ((qNum < inData.numberPorts()) &
			(queue[qNum]->length() <= 0)) {
		      qNum += 1;
		   }
		   Particle* pp = (Particle*) queue[qNum]->get();
		   outData.put(completionTime) = *pp;
		   demand.dataNew = FALSE;
		   curSize -= 1;
		}
	   }
	   // output the queue size
	   size.put(completionTime) << int(curSize);
	}
}
