defstar {
	name { LastOfN }
	domain { DDF }
	desc {
Outputs the last token of N input tokens,
where N is the control input.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DDF library }
	input {
		name { input }
		type { ANYTYPE }
		num { 0 }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { ANYTYPE }
	}
	protected {
		int readyToGo;
	}
	constructor {
		output.inheritTypeFrom(input);
	}
	start {
		waitFor(control);
		readyToGo = FALSE;
	}
	go {
		// get control Particles from "control" porthole
		if (!readyToGo) {
			control.grabData();	
			int i = int (control%0);
			waitFor(input, i);
			readyToGo = TRUE;
		} else {
			// skip the N-1 inputs
			int num = int (control%0);
			for (int i = num; i > 0; i--)
				input.grabData();

			// prepare for the next round.
			waitFor(control);
			readyToGo = FALSE;

			// copy last input to output
			if (num < 1) return;
			output%0 = input%0;

			// generate output, and get ready for next firing
			output.sendData();
		}
	}
}


