defstar {
	name { GainCx }
	domain { SDF }
	desc {
Amplifier: output is input times "gain" (default 1.0).  Input, output,
and gain factor are all complex numbers.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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

