defstar{
	name		{ PriorityCheck }
	domain		{ DE }
	version		{ $Id$ }
	author		{ Paul Haskell }
	location	{ DE main library }
	desc {
This star reads in input "NetworkCell" packets
from multiple input sources. If the priority of an input NetworkCell
is less than the most recent "priority" input, then the cell is
sent to the "discard" output. Otherwise it is sent to the "output"
output.
	}

	input {		name { input }		type { message } }
	input {		name { priority }	type { int } }
	output {	name { output }		type { message } }
	output {	name { discard }	type { message } }

	ccinclude { "NetworkCell.h" }

	constructor {
		input.before(priority);
		priority.triggers();
		input.triggers(output);
		input.triggers(discard);
	}

	protected { int cutLevel; }
	setup { cutLevel = 0; }

	go {
		completionTime = arrivalTime;

		while (priority.dataNew) {
			cutLevel = priority.get();
			priority.getSimulEvent();
		}

		while (input.dataNew) {
			Envelope envp;
			input.get().getMessage(envp);
			TYPE_CHECK(envp, "NetworkCell");
			const NetworkCell* cell = (const NetworkCell*)
					envp.myData();
			if (int(*cell) >= cutLevel)
				output.put(completionTime) << envp;
			else
				discard.put(completionTime) << envp;
			input.getSimulEvent();
		}
	}
} // end defstar { PriorityCheck }
