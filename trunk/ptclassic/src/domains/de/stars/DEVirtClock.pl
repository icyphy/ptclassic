defstar {
	name		{ VirtClock }
	domain		{ DE }
	version		{ $Id$ }
	author		{ Paul Haskell }
	location	{ DE main library }
	desc {
This star reads in NetworkCells from multiple inputs.
Then, it places these inputs in queues each of length "MaxSize".
(If `MaxSize' is less than or equal to zero then all queues are
infinitely long.) Inputs to full queues are discarded.

The queues are served in virtual-clock fashion, with an output for
every demand input unless all of the queues are empty.
Demand inputs arriving when all queues are empty are ignored.
	}

	inmulti {	name { input }		type { message } }
	input {		name { demand }		type { anytype } }
	output {	name { output }		type { message } }
	output {	name { discard }	type { message } }
	output {	name { length }		type { int } }
	defstate {
		name	{ MaxSize }
		type	{ int }
		default { 50 }
		desc	{ Size of the buffers. }
	}

//////// CODE ////////
	ccinclude { "DataStruct.h", "NetworkCell.h" }

	protected {
		int numInputs;
		int nextInput;
		Queue* queues;
	}

	constructor {
		numInputs = 0;
		queues = (Queue*) NULL;

		input.before(demand);
		input.triggers();
		demand.triggers(output);
		demand.triggers(discard);
		demand.triggers(length);
	}

	setup {
		wrapup();

		nextInput = 0;
		numInputs = input.numberPorts();
		LOG_NEW; queues = new Queue[numInputs];
	}

	wrapup {
		for(int i = 0; i < numInputs; i++) { EmptyQueue(queues[i]); }
		LOG_DEL; delete [] queues; queues = (Queue*) NULL;
		numInputs = 0;
	}

	destructor { wrapup(); }

	inline method { // Increment queue index.
		name { incr }
		type { "int" }
		arglist { "(int& val)" }
		access { protected }
		code {
			val++;
			if (val >= numInputs) val = 0;
			return val;
	}	}

	method {
		name { MaxLength }
		type { "int" }
		arglist { "()" }
		access { private }
		code {
			int retval = 0;
			for(int i = 0; i < numInputs; i++) {
				if (retval < queues[i].size()) {
					retval = queues[i].size();
			}	}
			return (retval);
	}	}

	method {
		name { EmptyQueue }
		access { private }
		arglist { "(Queue& queue)" }
		type { "void" }
		code {
			while(queue.size()) {
				LOG_DEL; delete (Envelope*) queue.get();
			}
	}	}


	go {
		completionTime = arrivalTime;

// Handle inputs.
		InDEMPHIter nextInp(input);
		for(int i = 0; i < numInputs; i++) {
			InDEPort* p = nextInp++;
			while(p->dataNew) {
				LOG_NEW; Envelope* envp = new Envelope;
				p->get().getMessage(*envp);
				TYPE_CHECK(*envp, "NetworkCell");

				if ( (queues[i].size() < int(MaxSize)) ||
						(int(MaxSize) <= 0)) {
					queues[i].put(envp);
				} else {
					discard.put(completionTime) << *envp;
					LOG_DEL; delete envp;
				}
				p->getSimulEvent();
		}	}

// Do outputs.
		while (demand.dataNew) {
// Find a non-empty queue.
			for(int j = 0; (j < numInputs) &&
					!queues[nextInput].size(); j++) { incr(nextInput); }

			if (queues[nextInput].size()) {
				Envelope* envp = (Envelope*) queues[nextInput].get();
				output.put(completionTime) << *envp;
				LOG_DEL; delete envp;

				incr(nextInput);
				length.put(completionTime) << MaxLength();
			}
			demand.getSimulEvent();
		}
	} // end go{}
} // end defstar { VirtClock }
