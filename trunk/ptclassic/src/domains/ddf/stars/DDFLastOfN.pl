ident {
// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.

// This star outputs the last input tokens from "N" tokens.
// (N = condition value)
// Soonhoi Ha
// 
// $Id$
}
defstar {
	name { LastOfN }
	domain { DDF }
	desc { "A LastOfN Star outputs the last tokens from N inputs" }
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


