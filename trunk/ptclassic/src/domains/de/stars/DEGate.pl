defstar {
	name { Gate }
	domain { DE }
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
If the gate is open, particles pass from "input" to "output".
When the gate is closed, "input" particles are ignored.
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
		desc { Opens and closes the gate. }
	}
	output {
		name { output }
		type { = input }
	}
	defstate {
		name { initPosition }
		type { string }
		default { "open" }
		desc { Initial gate position:  open (o,O) or closed (c,C). }
	}
	defstate {
		name { controlLogic }
		type { int }
		default { FALSE }
		desc { Control value which opens the gate. }
	}
	private {
		int open;
	}
	constructor {
		control.triggers();
		control.before(input);
	}
	start {
		// set initial position of the gate
		switch(*initPosition.getInitValue()) {
		    case 'o':
		    case 'O':
			open = TRUE;
			break;
		    case 'c':
		    case 'C':
			open = FALSE;
			break;
		    default:
			Error::abortRun(*this, "invalid initial position.");
			break;
		}
	}

	go {
		// open gate if control input matches controlLogic
		if (control.dataNew)
		    open = ( (int(control.get()) && int(controlLogic))
			|| (!int(control.get()) && !int(controlLogic)) );

		// receive the normal input
		if (input.dataNew && open)
		    output.put(arrivalTime) = input.get();
	}
}
