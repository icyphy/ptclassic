defstar {
	name { DownCounter }
	domain { DDF }
	desc { A down counter from the input value to zero. }
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }
	explanation {
This star generates (int-1), (int-2), ... 1,0 samples for (int) input.
	}
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

