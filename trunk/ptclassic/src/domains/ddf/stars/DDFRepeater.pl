defstar {
	name { Repeater }
	domain { DDF }
	desc {
Given a control input with integer value N,
and a single input data particle, produce N
copies of the data particle on the output.
	}
	version { @(#)DDFRepeater.pl	2.12	10/29/95 }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	constructor {
		output.inheritTypeFrom(input);
	}
	go {
		// get Particles from Geodesic
		control.receiveData();
		input.receiveData();

		// read control value, and repeat input tokens to output 
		int num = int(control%0);
		for (int i = num; i > 0; i--) {
			output%0 = input%0;
			output.sendData();
		}
	}
}
