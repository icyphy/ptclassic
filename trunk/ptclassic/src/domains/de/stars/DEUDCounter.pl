defstar {
	name { UDCounter }
	domain { DE }
	desc {
Increase or decrease the count upon receiving a "countUp" or "countDown" input.
When a "reset" input is received, the count is reset to zero.
An output is generated when a "demand" input is received.
When "demand" and "reset" particles arrive at the same time,
the output is generated before the count is reset.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	explanation {
This is a rather general counter.
Special cases, such as an up only counter, can be realized by connect
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
	start {
		content = int(resetValue);
	}
	go {
		completionTime = arrivalTime;

		// check the countUp/Down input to change the content.
		if (countUp.dataNew) {
			countUp.dataNew = FALSE;
			content++;
		}
		if (countDown.dataNew) {
			countDown.dataNew = FALSE;
			content--;
		}
		if (demand.dataNew) {
			demand.dataNew = FALSE;
			output.put(completionTime) << content;
		}
		if (reset.dataNew) {
			reset.dataNew = FALSE;
			content = int(resetValue);
		}
	}
}
