defstar {
	name { PassGate }
	domain { DE }
	version { $Id$ }
	author { Soonhoi Ha and Edward Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
If the gate is open, particles pass from "input" to "output".
When the gate is closed, no outputs are produced.
If "input" particles arrive while the gate is closed, the most
recent one will be passed to "output" when the gate is re-opened.
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
		desc { Opens the gate on TRUE and closes it on FALSE. }
	}
	output {
		name { output }
		type { = input }
	}
	defstate {
		name { gateOpen }
		type { int }
		default { TRUE }
		desc { Gate position. }
	}
	constructor {
		control.triggers();
		// Hint to the scheduler that given a choice, stars feeding
		// the control input should fire before stars feeding the
		// "input" input.
		control.before(input);
	}
	go {
		if (control.dataNew)
		    // Set gateOpen state equal to control input
		    gateOpen = int(control.get());

		// receive the normal input
		if (input.dataNew && int(gateOpen))
		    output.put(arrivalTime) = input.get();
	}
}
