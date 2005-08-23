defstar {
	name { GainCx }
	domain { SDF }
	desc {
This is an amplifier; the complex output is the complex input
multiplied by the complex "gain" (default 1.0).
	}
	version {@(#)SDFGainCx.pl	1.9	09/01/97}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { input }
		type { complex }
	}
	output {
		name { output }
		type { complex }
	}
	defstate {
		name { gain }
		type { complex }
		default { "(1,0)" }
		desc { Gain of the star. }
	}
	go {
		// We use a temporary variable to avoid gcc2.7.2/2.8 problems
		Complex tmpgain = (input%0);
		Complex tmpgain2 = gain;
		output%0 << tmpgain2 * tmpgain;
	}
}

