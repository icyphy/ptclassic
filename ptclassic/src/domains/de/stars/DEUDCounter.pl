defstar {
	name { UDCounter }
	domain { DE }
	desc {
This is a general counter that can count up or down.
The processing order of the ports is:
countUp -> countDown -> demand -> reset.
If multiple countUp or countDown events with the same time stamp
are pending, all will be processed.  If multiple demand events with
the same time stamp are pending, they will be consolidated into a
single demand, so only one output will be produced.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	explanation {
This is an up/down counter.
Receiving events on the "countUp" or "countDown" inputs causes the
internal state of the counter to increment or decrement.
Special cases, such as an up only counter, can be realized by connect
a Null star to the unneeded inputs.
.pp
When a "reset" input is received, the count is reset to "resetValue".
An output is generated when a "demand" input is received.
When "demand" and "reset" events arrive with the same time stamp,
the output is produced before the count is reset.
The "countUp" and "countDown" inputs are processed before the "demand" input,
so if the time stamps are identical, the output will reflect the
pending increment(s) and/or decrement(s).
Simultaneous "demand" inputs are consolidated; if there
are multiple "demand" input events with the same time stamp,
the star generates only one output.
.pp
This star operates in the phase-based mode, in which all simultaneous
events in the global event queue are fed into the star at each firing.
	}
	input {
		name { countUp }
		type { anytype }
		desc { Increment internal state. }
	}
	input {
		name { countDown }
		type { anytype }
		desc { Decrement internal state. }
	}
	input {
		name { reset }
		type { anytype }
		desc { Reset internal state to resetValue. }
	}
	input {
		name { demand }
		type { anytype }
		desc { Read internal state and send to output. }
	}
	output {
		name { output }
		type { int }
		desc { Internal count value at the time of the demand input. }
	}
	defstate {
		name { resetValue }
		type { int }
		default { "0" }
		desc {  Initial and reset value for the counter. }
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
