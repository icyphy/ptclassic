defstar {
	name { GainInt }
	domain { CGC }
	desc { Amplifier: output is input times "gain" (default 1). }
	version { @(#)CGCGainInt.pl	1.3	2/1/96 }
	author { Brian L. Evans }
	acknowledge { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { gain }
		type { int }
		default { 1 }
		desc { Gain of the star. }
	}
	constructor {
		noInternalState();
	}
	go {
		// Check for simple multiple of 2
		int reg = 0x01;
		int found = FALSE;
		int gainValue = gain;
		int i = -1;
		while (++i < 7) {
			if ( gainValue == reg ) {
				found = TRUE;
				break;
			}
			reg <<= 1;
		}

		StringList code = "\t$ref(output) = ";
		if ( found ) {
			if ( i == 0 ) code << "$ref(input)";
			else code << "$ref(input) << " << i;
		}
		else {
			code << "$val(gain) * $ref(input)";
		}
		code << ";\n";
		addCode(code);
	}
	exectime {
		int time = 1;
		if ( int(gain) == 1 ) time = 0;
		return time;
	}
}
