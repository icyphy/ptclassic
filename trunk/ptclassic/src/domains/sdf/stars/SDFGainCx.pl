defstar {
	name { GainCx }
	domain { SDF }
	desc {
This is an amplifier; the complex output is the complex input
multiplied by the complex "gain" (default 1.0).
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		output%0 << Complex(gain) * Complex(input%0);
	}
}

