defstar {
	name { DownCounter }
	domain { DDF }
	desc {
Given an integer input with value N, produce a sequence of output
integers with values (N-1), (N-2), ... 1, 0.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "UpSample"; }
	}
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
		num { 0 }
	}
	go {
		// get input token from Geodesic
		input.receiveData();

		// generates output
		int in = int (input%0);
		for (int i = in - 1 ; i >= 0; i--) {
			output%0 << i ;
			output.sendData();
		}
	}
}

