ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 11/8/90

 It is a general resettable up/down counter with demand input.
 Since it is most general form of a counter, we may connect a Null star
 to unused ports to get a simplified version of Counter.  

**************************************************************************/
}
defstar {
	name { UDCounter }
	domain { DE }
	desc {	"In(De)crease the content on each countUp(Down) input.\n"
		"When the reset input comes, reset the content.\n"
		"Output is generated on each demand input.\n"
		"When demand and reset arrives at the same time,\n"
		"output first and reset the content.\n"
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
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { resetValue }
		type { int }
		default { "0" }
		desc { "initial value for the content" }
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
