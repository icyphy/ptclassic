defstar {
	name { UDCounter }
	domain { DE }
	desc {
This is up/down counter.
The processing order of the ports is: countUp -> countDown -> demand -> reset.
Specifically, all simultaneous "countUp" inputs are processed.
Then all simultaneous "countDown" inputs are processed.
If there are multiple simultaneous "demand" inputs, all but the first
are ignored.  Only one output will be produced.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Upon receiving a "countUp" or "countDown" input, an internal counter is
incremented or decremented.
When a "reset" input is received, the count is reset to "resetValue".
A "count" output is generated when a "demand" input is received.
When "demand" and "reset" particles arrive at the same time,
the output is generated before the count is reset.
The "countUp(or Down)" input is processed before the "demand" input,
so if there are identical time stamps on these inputs, the increment
and/or decrement will occur before the output is produced.
If successive inputs arrive with the same time stamp on any input path,
they are all processed.
Special cases, such as an up only counter, can be realized by connecting
a Null star to the unneeded inputs.
	}
	input {
		name { countUp }
		type { anytype }
	}
	input {
		name { countDown }
		type { anytype }
	}
	input {
		name { reset }
		type { anytype }
	}
	input {
		name { demand }
		type { anytype }
		desc { Stimulate an output. }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { resetValue }
		type { int }
		default { "0" }
		desc {  Initial value for the counter. }
	}
	private {
		int content;
	}
	constructor {
		countUp.triggers();
		countDown.triggers();
		reset.triggers();

		// The following statements hint to the scheduler that
		// if given a choice, stars feeding data to the countUp,
		// countDown, and reset inputs should fire before stars
		// feeding data to the demand input.
		countUp.before(demand);
		countDown.before(demand);
		reset.before(demand);
	}
	setup {
		content = int(resetValue);
	}
	go {
		completionTime = arrivalTime;

		// check the countUp/Down input to change the content.
		while (countUp.dataNew) {
			content++;
			countUp.getSimulEvent();
		}
		while (countDown.dataNew) {
			content--;
			countDown.getSimulEvent();
		}
		if (demand.dataNew) {
			output.put(completionTime) << content;
		}
		// swallow all demand inputs
		while (demand.dataNew) demand.getSimulEvent();

		while (reset.dataNew) {
			content = int(resetValue);
			reset.getSimulEvent();
		}
	}
}
