defstar {
	name { DownCounter }
	domain { DDF }
	desc { A down counter from the input value to zero. }
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
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

