ident {
// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.

// This star repeats the input as many times as the condition value.
// Soonhoi Ha
// 
// $Id$
}
defstar {
	name { Repeater }
	domain { DDF }
	desc {
		"This star uses the 'control' input value to decide how many\n"
		"times to repeat the 'input' value on 'output."
	}
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
	constructor {
		output.inheritTypeFrom(input);
	}
	go {
		// get Particles from Geodesic
		control.grabData();
		input.grabData();

		// read control value, and repeat input tokens to output 
		output%0 = input%0;
		int num = int (control%0);
		for (int i = num ; i > 0; i--)
			output.sendData();
	}
}


