defstar {
	name { Thresh }
	domain { DDF }
	desc {
This star has only a single input, and it is dynamic.
It behaves like an SDF star, except that it has a threshold
in addition to the number of tokens consumed.  The number of
inputs available on the input arc must equal the threshold.
THIS STAR IS MEANT FOR TESTING SCHEDULERS ONLY. OTHERWISE,
IT SERVES NO USEFUL FUNCTION.
	}
	version { @(#)DDFThresh.pl	1.5	3/6/96 }
	author { Edward A. Lee, Richard Stevens, and Christopher Scannell }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.

Richard Stevens and Christopher Scannell are employees of the
U.S. Government whose contributions to this computer program fall
within the scope of 17 U.S.C. A7 105.
	}
	location { DDF library }
	input {
		name { input }
		type { ANYTYPE }
		num { 0 }
	}
	output {
		name { output }
		type { ANYTYPE }
	}
	state {
		name { threshold }
		type { int }
		default { 1 }
		descriptor { number of tokens needed at the input to fire. }
	}
	state {
		name { numconsumed }
		type { int }
		default { 1 }
		descriptor { number of tokens consumed when star fires. }
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
	setup {
		if ( int(numconsumed) <= 0 ) {
			StringList msg = "Number of tokens consumed must be ";
			msg << "positive, instead of " << int(numconsumed);
			Error::abortRun(*this, msg);
		}
		if ( int(threshold) <= 0 ) {
			StringList msg = "Threshold must be ";
			msg << "positive, instead of " << int(threshold);
			Error::abortRun(*this, msg);
		}
	}
	begin {
		waitFor(input, int(threshold));
	}
	go {
		for (int i = int(numconsumed); i > 0; i--) {
			input.receiveData();
		}

		// copy last input to output
		output%0 = input%0;

		// generate output, and get ready for next firing
		output.sendData();

		waitFor(input, int(threshold));
	}
}
