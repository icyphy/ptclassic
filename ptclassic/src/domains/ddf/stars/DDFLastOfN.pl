defstar {
	name { LastOfN }
	domain { DDF }
	desc {
Outputs the last token of N input tokens,
where N is the control input.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "DownSample"; }
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
			control.receiveData();	
		}
		int num = int (control%0);
		if (input.numTokens() >= num) {
			// skip the num-1 inputs
			for (int i = num; i > 0; i--)
				input.receiveData();

			// prepare for the next round.
			waitFor(control);
			readyToGo = FALSE;

			// copy last input to output
			if (num < 1) return;
			output%0 = input%0;

			// generate output, and get ready for next firing
			output.sendData();
		} else {
			waitFor(input, num);
			readyToGo = TRUE;
		} 
	}
}


