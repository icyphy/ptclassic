defstar {
	name { UDCounter }
	domain { DE }
	desc {
This is a general counter which can count up or down.
The processing order of the ports is:
countUp/Down -> demand -> reset.
Simultaneous demand and reset events are ignored.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	explanation {
This is a rather general counter.
Increase or decrease the count upon receiving a "countUp" or "countDown" input.
Special cases, such as an up only counter, can be realized by connect
a Null star to the unneeded inputs.
.pp
When a "reset" input is received, the count is reset to zero.
An output is generated when a "demand" input is received.
When "demand" and "reset" particles arrive at the same time,
the output is generated before the count is reset.
The "countUp(or Down)" input is processed before the "demand" input.
The simultaneous demand inputs or reset inputs are ignored; if there
are three "demand" input at the same time, we generate only one output.
.pp
This star operates in the phase-based mode, in which all simultaneous
events in the global event queue are fed into the star at each firing.
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
		countUp.before(demand);
		countDown.before(demand);
		reset.before(demand);

		setMode(PHASE);

	}
	start {
		content = int(resetValue);
	}
	go {
		completionTime = arrivalTime;

		// check the countUp/Down input to change the content.
		if (countUp.dataNew) {
			content += countUp.numSimulEvents();
			countUp.dataNew = FALSE;
		}
		if (countDown.dataNew) {
			content -= countDown.numSimulEvents();
			countDown.dataNew = FALSE;
		}
		if (demand.dataNew) {
			output.put(completionTime) << content;
			demand.dataNew = FALSE;
		}
		if (reset.dataNew) {
			reset.dataNew = FALSE;
			content = int(resetValue);
		}
	}
}
