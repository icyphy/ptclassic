defstar {
	name { Repeater }
	domain { DDF }
	desc {
This star uses the "control" input value to specify how many
times to repeat the "input" value on "output".
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DDF library }
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { ANYTYPE }
		num { 0 }
	}
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "UpSample"; }
	}
	constructor {
		output.inheritTypeFrom(input);
	}
	go {
		// get Particles from Geodesic
		control.receiveData();
		input.receiveData();

		// read control value, and repeat input tokens to output 
		int num = int (control%0);
		for (int i = num ; i > 0; i--) {
			output%0 = input%0;
			output.sendData();
		}
	}
}


