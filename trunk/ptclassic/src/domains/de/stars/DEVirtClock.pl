defstar {
	name { VirtClock }
	domain { DE }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { Richard Han, Paul Haskell }
	location { DE main library }
	desc {
This star reads in a NetworkCell. It identifies which virtual circuit
number the cell belongs to and then computes the virtual time stamp for
the cell by applying the (special!) virtual clock algorithm (see the full
manual page). It then outputs all cells in order of increasing
virtual time stamp.

Upon receiving a 'demand' input, the cell with the smallest timestamp is
output. An output packet is generated for every demand input unless
all of the queues are empty. Demand inputs arriving when all queues
are empty are ignored.

The number of stored cells is output after the receipt of each 'input'
or 'demand'.

When a cell arrives and the number of stored cells equals 'MaxSize'
then the cell with the biggest virtual timestamp is discarded. This
cell may or may not be the new arrival. If 'MaxSize' is zero or
negative, then infinitely many cells can be stored.
	} // end description

	explanation {
The (new, improved) virtual clock buffer service discipline used by
this star works like this:
.pp
Each connection through a buffer is assigned a rate Ri.
Let Di = (1/Ri), the minimum allowable inter-cell time duration.
.pp
The virtual "current time" is the timestamp of the most recently
served cell.
.pp
The first cell from the first established connection through a
buffer is given virtual timestamp 0.0.
The first cells from other connections are stamped with the
virtual "current time".
Cells from a connection i other than the first cell are stamped with
the later (i.e. maximum) of
.ti 5
(1) the current time
.ti 5
(2) Di + the timestamp of the most recent enqueued OR served cell
from stream i.
.pp
Note that the timestamp for each new cell is AT LEAST Di time units
later than the timestamps of all other cells on the same virtual
circuit.
.pp
When a cell arrives at a full buffer, it is assigned a timestamp as
usual.
The cell with the biggest timestamp is discarded.
If the discarded cell is not the just-arrived cell, the just-arrived
cell is enqueued using the just-freed memory.
.pp
This buffer control policy (i.e. buffer service and buffer access
policies) is fair, work-conserving, and buffer-conserving.
Fairness is guaranteed because, in the presence of competing traffic,
every source gets served at exactly its average rate.
The control is work-conserving because even if only one source
is active, the buffer never remains idle when there are
cells available to serve.
The controls is buffer-conserving because cells are discarded only
when the buffer is full.
.pp
This buffer control strategy simplifies Zhang's proposed
Virtual Clock control discipline [1].
Zhang's discipline requires that network users and networks agree
on a "monitoring interval" in addition to an average service rate.
If a source is found to violate the agreed data rate over any monitoring
interval, some unspecified action must be taken.
.pp
The discipline presented here does not use a monitoring interval;
it only performs buffer admission control when the buffer is full.
Also, this discipline makes explicit the action taken when a
buffer fills.
It discards the cell with the "most future" virtual timestamp.
This control penalizes the virtual circuit that deserves it the
most, but it still guarantees that penalized virtual circuits
receive
.ul
at least
their requested bandwidth allocation.
This buffer control strategy thus eliminates the "flooding" problem
inherent in Zhang's Virtual Clock discipline.
With Zhang's discipline, one source flooding a single buffer can
prevent other sources from becoming enqueued.
With this discipline, a misbehaving source's cells receive timestamps
much larger than the current virtual time.
Thus, arrivals from other sources
can bump out cells from the misbehaving source.
Every source is
.ul
guaranteed
service at its average rate
.ul
without loss
assuming:
.ti 5
(1) the buffer size is at least as large as the number of sources
.ti 5
(2) the buffer output rate is at least as large as the sum of the
source input rates
.pp
This discipline ends up being a flavor of Earliest Due Date
scheduling with
.ti 5
(1) a specific algorithm for picking due dates at each buffer
.ti 5
(2) a specific cell discard mechanism that ensures loss fairness
.UH Reference
.ip [1]
Lixia Zhang, "VirtualClock: A New Traffic Control Algorithm for
Packet Switching Networks," \fISIGCOMM'90 Symposium\fR,
Sept. 1990, Philadelphia, \fIComputer Communications Review\fR,
vol. 20:4, Sept. 1990.
	} // end explanation

	inmulti {	name { input }		type { message } }
	input {		name { demand }		type { anytype } }
	output {	name { output }		type { message } }
	output {	name { discard }	type { message } }
	output {	name { length }		type { int } }

	defstate {
		name { MaxSize }
		type { int }
		default { 50 }
		desc { Maximum number of stored cells. }
	}

	defstate {
		name { AvgRate }
		type { floatarray }
		default { "1.0 1.0" }
		desc { Average rate for each input virtual circuit. }
	}

//////// CODE ////////
	ccinclude { "Error.h", <minmax.h> }
	hinclude { "NetworkCell.h", "DataStruct.h", "PriorityQueue.h" }

	header {
// BufCell instances store an arriving NetworkCell along with the
// number of arrivals on that virtual circuit.
		class BufCell {
		public:
			BufCell(const int i, const Envelope& e): counter(i)
					{ myEnvp = e; }
			Envelope& envp() { return myEnvp; }
			const int count() { return counter; }
		private:
			Envelope myEnvp;
			int counter;
		};

// Tag instances store the virtual circuit i.d. of an input cell and
// the virtual timestamp assigned to that cell.
		class Tag {
		public:
			Tag(const int i, const float f): port(i), tStamp(f) { }
			const int output() { return port; }
			const float vTime() { return tStamp; }
		private:
			int port;
			float tStamp;
		};
	}

	protected {
// This star implements storage with TWO mechanisms: a sorted priority
// queue that contains 'Tag' instances, and a COLLECTION of FIFO
// queues (one per virtual circuit) that store 'BufCell' instances
// (and thus store input cells). The sorted priority queue is useful
// in that it determines the order in which cells from different
// virtual circuits should be served. The collection of FIFO queues
// are useful in that they simplify the implementation of different
// cell discard disciplines.

// The 'cellCount' variable counts the number of cells on each
// destination address.

		int numStreams; // Number of virtual circuits being served;
		float vTime; // virtual time for the entire buffer.
		float* vTimeStamp; // Next timestamp value for each virt. ckt.
		int* cellCount; // Number of cells so far from each virt. ckt.
		PriorityQueue* priList; // Holds all (sorted) Tag's.
		Queue* queues; // Array of Queue's that store BufCell's.
	}

	constructor {
		delayType = TRUE;

		// initialize variables
		numStreams = 0;
		vTime = 0.0;

		// initialize pointers
		vTimeStamp = 0;
		cellCount = 0;
		priList = 0;
		queues = 0;

		input.before(demand);
		input.triggers(discard);
		demand.triggers(output);
		demand.triggers(length);
	}

	setup {
		if (priList) {
			while(priList->length()) {
				LOG_DEL; delete (Tag*) priList->getFirstElem();
			}
			LOG_DEL; delete priList;
		}

		if (queues) {
			for(int i = 0; i < numStreams; i++) {
				while((queues[i]).size()) {
					LOG_DEL; delete (BufCell*) (queues[i]).get();
			}	}
			LOG_DEL; delete [] queues;
		}

		LOG_DEL; delete [] vTimeStamp;
		LOG_DEL; delete [] cellCount;

		vTime = 0.0;
		numStreams = AvgRate.size();

		LOG_NEW; priList = new PriorityQueue;
		LOG_NEW; queues = new Queue[numStreams];

		LOG_NEW; vTimeStamp = new float[numStreams];
		LOG_NEW; cellCount = new int[numStreams];
		for (int i = 0; i < numStreams; i++) {
			vTimeStamp[i] = 0.0;
			cellCount[i] = 0;
		}
	}

	destructor {
		LOG_DEL; delete [] vTimeStamp;
		LOG_DEL; delete [] cellCount;

		if (priList) {
			while(priList->length()) {
				LOG_DEL; delete (Tag*) priList->getFirstElem();
			}
			LOG_DEL; delete priList;
		}

		if (queues) {
			for(int i = 0; i < numStreams; i++) {
				while((queues[i]).size()) {
					LOG_DEL; delete (BufCell*) (queues[i]).get();
			}	}
			LOG_DEL; delete [] queues;
		}
	}

// The LoseOne() method discards one cell from the given queue.
// Other stars can override this method. Those stars can refer to the
// "count()" and "priority()" methods of class "BufCell".
	virtual method {
		name { LoseOne }
		type { void }
		access { protected }
		arglist { "(Queue& queue)" }
		code {
			if (queue.size() <= 0) { return; }
			BufCell* loss = (BufCell*) queue.getTail();
			discard.put(completionTime) << loss->envp();
			LOG_DEL; delete loss;
		}
	} // end LoseOne{}


	go {
		completionTime = arrivalTime;

// Handle inputs: for each input...
		InDEMPHIter nextInp(input);
		InDEPort* p;
		while ( (p = nextInp++) ) {
			while (p->dataNew) {

// Get the input Message.
				Envelope envp;
				p->get().getMessage(envp);

// Check its type and get its destination address.
				TYPE_CHECK(envp, "NetworkCell");
				const NetworkCell* ncp = (const NetworkCell*)
						envp.myData();
				const int destAddr = ncp->dest();
				if ((destAddr < 0) || (destAddr >= AvgRate.size())) {
					Error::abortRun(*this, "Illegal dest. addr.");
					return;
				}

// Timestamp for this arrival is the more recent of "current virtual
// time" or timestamp of last arrival + interarrival time.
				vTimeStamp[destAddr] = max(vTime,
						float(vTimeStamp[destAddr] +
						1.0/AvgRate[destAddr]) );

// BUFFER SERVICE CONTROL:
// Put new arrival into the two appropriate queues.
				LOG_NEW;
				Tag* tag = new Tag(destAddr, vTimeStamp[destAddr]);
				priList->levelput(tag, vTimeStamp[destAddr]);

				LOG_NEW;
				BufCell* cell = new
						BufCell(cellCount[destAddr]++, envp);
				(queues[destAddr]).put(cell);

// BUFFER ACCESS CONTROL:
// If global queue is too long, find the source "most in violation."
// That is, the source with the virtual timestamp most in the future.
// Delete some element from that queue (with the LoseOne() fcn). Modify
// the vTimeStamp[] entry (for inputs with the same destination address
// as the deleted element) so that they are not delayed unfairly by the
// deleted cell.
				if ((MaxSize > 0) && (priList->length() > MaxSize)) {
					Tag* loss = (Tag*) priList->getLastElem();
					const int la = loss->output();
					LOG_DEL; delete loss;
					vTimeStamp[la] -= 1.0/AvgRate[la];
					LoseOne(queues[la]);
				}
				length.put(completionTime) << priList->length();
				p->getSimulEvent();
		}	} // end handle inputs.

// Handle outputs.
		while (demand.dataNew) {
			if (priList->length() > 0) {
				Tag* outTag = (Tag*) priList->getFirstElem();
				vTime = outTag->vTime();
				BufCell* outCell = (BufCell*)
						(queues[outTag->output()]).get();
				output.put(completionTime) << outCell->envp();
				LOG_DEL; delete outTag;
				LOG_DEL; delete outCell;
			}
			length.put(completionTime) << priList->length();
			demand.getSimulEvent();
		}
	} // end go{}
} // end defstar { VirtClock }
